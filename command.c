/* Copyright 2011 Tom Jollans <t@jollybox.de>
 * This code is under the Chicken Dance License v0.1 */

#include "command.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *
first_word (const char *txt)
{
    int i;

    for (i=0; txt[i] && !isspace(txt[i]); ++i);

    return strndup(txt, i);
}

void
separate_first_word (char **txt, char **restp)
{
    char *p;

    while (isspace(**txt)) (*txt)++;
    for (p = *txt; *p && !isspace(*p); ++p);
    
    if (*p && *(p+1))
        *restp = p+1;
    else /* end of string */
        *restp = NULL;

    *p = '\0';
}

static void advance_game (irc_session_t *, game_tp, const char *);

static void
set_turn (irc_session_t *session,
          game_tp game,
          const char *channel,
          int player_id,
          int new_round)
{
    char *s;

    int original_player = player_id;
    int bet;
    goto do_player;
skip_player:
    new_round = 0;
    player_id = next_player(game, player_id);
    if (player_id == original_player || player_id == -1) {
        advance_game(session, game, channel);
        return;
    }
do_player:
    bet = game->pots[game->n_pots-1].bet;
    player_t *player = & game->players[player_id];

    if (player->folded) {
        fprintf(stderr, "WARNING: Folded player passed to set_turn(). This should never happen.\n");
        goto skip_player;
    }

    if (bet < player->bet) {
        fprintf(stderr, "ERROR: Anomalous bets.\n");
        irc_cmd_msg(session, channel, "ERROR: Anomalous bets.");
        goto skip_player;
    }

    if (player->allin) {
        if (asprintf(&s, "%s is already all in.", player->nick) != -1) {
            irc_cmd_msg(session, channel, s);
            free(s);
        }
        goto skip_player;
    }

    if (!new_round && bet == 0 && player_id == next_player(game, game->button)) {
        /* everybody hath checked. How boring. */
        advance_game(session, game, channel);
        return;
    }

    if (bet == 0) {
        game->turn = player_id;
        if (asprintf(&s, "No bet. %s, you may check or raise.",
                         player->nick) != -1) {
            irc_cmd_msg(session, channel, s);
            free(s);
        }
        goto check_active;
    }

    if (bet > player->bet) {
        game->turn = player_id;
        if (asprintf(&s, "The bet is %d. %s, you may call or raise the bet.",
                         bet, player->nick) != -1) {
            irc_cmd_msg(session, channel, s);
            free(s);
        }
        goto check_active;
    }

    if (bet == player->bet) {
        /* everybody agrees. */
        if (game->phase == PHASE_PRE_FLOP &&
            bet == game->big_blind &&
            player_id == next_player(game, next_player(game, game->button))) {
            /* pre-flop. Big-blindist may raise. */
            game->turn = player_id;
            if (asprintf(&s, "The bet is still %d. %s, you may call or raise.",
                             bet, player->nick) != -1) {
                irc_cmd_msg(session, channel, s);
                free(s);
            }
            goto check_active;
        } else {
            advance_game(session, game, channel);
            return;
        }
    }

    fprintf(stderr, "ERROR: set_turn improperly concluded.\n");
    return;

check_active:
    if (!player->active) {
        if (asprintf(&s, "%s is inactive and folds by default.",
                         player->nick) != -1) {
            irc_cmd_msg(session, channel, s);
            free(s);
        }
        fold(game, player_id);
        goto skip_player;
    }
    else return;
}

static void
deal_round (irc_session_t *session,
            game_tp game,
            const char *channel)
{
    char *s;

    if (!game->phase == PHASE_PRE_DEAL) return;

    undeal(game);
    int i;
    for (i=0; i<game->n_players; ++i) {
        game->players[i].folded = (!game->players[i].active)
                                || game->players[i].chips == 0;
        game->players[i].bet = 0;
        game->players[i].allin = 0;
    }
    /* get rid of all pots. */
    for (i=0; i<game->n_pots; ++i) {
        free(game->pots[i].players);
    }
    game->n_pots = 1;
    game->pots[0].players = NULL;
    game->pots[0].n_players = 0;
    game->pots[0].content = 0;
    game->pots[0].bet = 0;
    shuffle_deck(game);

    int small = next_player(game, game->button);
    if (small < 0) {
        irc_cmd_msg(session, channel, "Two active players required. Aborting.");
        return;
    }
    int big = next_player(game, small);
    if (big < 0) {
        irc_cmd_msg(session, channel, "Cannot find player. Aborting.");
        return;
    }

    bet (game, small, game->small_blind);
    if (asprintf(&s, "%s pays small blind of %d.", game->players[small].nick,
                                                   game->small_blind) != -1) {
        irc_cmd_msg(session, channel, s);
        free(s);
    }

    bet (game, big, game->big_blind);
    if (asprintf(&s, "%s pays big blind of %d.", game->players[big].nick,
                                                 game->big_blind) != -1) {
        irc_cmd_msg(session, channel, s);
        free(s);
    }


    int pidx = small;
    do {
        deal(game, pidx);
        char *card1 = strdup(irc_print_card(game->players[pidx].hand[0], USE_COLOR, USE_UTF8));
        char *card2 = strdup(irc_print_card(game->players[pidx].hand[1], USE_COLOR, USE_UTF8));
        if (asprintf(&s, "Your cards: %s %s", card1, card2) != -1) {
            irc_cmd_msg(session, game->players[pidx].nick, s);
            free(s);
        }
        free(card1);
        free(card2);
    } while ((pidx = next_player(game, pidx)) != small);

    game->phase = PHASE_PRE_FLOP;

    irc_cmd_msg(session, channel, "Hands dealt.");
    set_turn(session, game, channel, next_player(game, big), 0);
}

static void
advance_game (irc_session_t *session,
              game_tp game,
              const char *channel)
{
    int i;
    char *s;
    char *card1, *card2, *card3, *card4, *card5;
    int active_players = 0;
    int last_active = -1;

    /* new betting round. Zero bets. */
    for (i = 0; i < game->n_players; ++i) {
        game->players[i].bet = 0;
        if (!game->players[i].folded) {
            active_players++;
            last_active = i;
        }
    }
    for (i = 0; i < game->n_pots; ++i) {
        game->pots[i].bet = 0;
    }

    if (active_players == 0) {
        irc_cmd_msg(session, channel, "No players left? This may be an error, "
                    "but I'll just go out for a beer with all that cash.");
        return;
    }
    if (active_players == 1) {
        if (asprintf(&s, "Everybody else has folded. %s wins by default.",
                          game->players[last_active].nick) != -1) {
            irc_cmd_msg(session, channel, s);
            free(s);
        }
        /* Only one player left => nobody else all-in => one non-empty pot. */
        if (game->n_pots > 1 && game->pots[1].content != 0) {
            irc_cmd_msg(session, channel, "Error: There is more than one pot.");
            fprintf(stderr, "ERROR: There is more than one pot.\n");
        }
        game->players[last_active].chips += game->pots[0].content;
        if (asprintf(&s, "%s is awarded %d. They now have a total of %d.",
                          game->players[last_active].nick,
                          game->pots[0].content,
                          game->players[last_active].chips) != -1) {
            irc_cmd_msg(session, channel, s);
            free(s);
        }

        irc_cmd_msg(session, channel, "TODO: next round.");
        return;
    }

    switch (game->phase) {
        case PHASE_PRE_DEAL:
            /* This should not occur. */
            fprintf(stderr, "WARNING: Confusion over when to deal, apparently.\n");
            deal_round(session, game, channel);
            return;
        case PHASE_PRE_FLOP:
            deal_community(game);
            game->phase = PHASE_FLOP;
            card1 = strdup(irc_print_card(game->community[0], USE_COLOR, USE_UTF8));
            card2 = strdup(irc_print_card(game->community[1], USE_COLOR, USE_UTF8));
            card3 = strdup(irc_print_card(game->community[2], USE_COLOR, USE_UTF8));
            if (asprintf(&s, "Flop. Community cards: %s %s %s",
                             card1, card2, card3) != -1) {
                irc_cmd_msg(session, channel, s);
                free(s);
            }
            free(card1);
            free(card2);
            free(card3);
            break;
        case PHASE_FLOP:
            game->phase = PHASE_TURN;
            card1 = strdup(irc_print_card(game->community[0], USE_COLOR, USE_UTF8));
            card2 = strdup(irc_print_card(game->community[1], USE_COLOR, USE_UTF8));
            card3 = strdup(irc_print_card(game->community[2], USE_COLOR, USE_UTF8));
            card4 = strdup(irc_print_card(game->community[3], USE_COLOR, USE_UTF8));
            if (asprintf(&s, "Turn. Community cards: %s %s %s %s",
                             card1, card2, card3, card4) != -1) {
                irc_cmd_msg(session, channel, s);
                free(s);
            }
            free(card1);
            free(card2);
            free(card3);
            free(card4);
            break;
        case PHASE_TURN:
            game->phase = PHASE_RIVER;
            card1 = strdup(irc_print_card(game->community[0], USE_COLOR, USE_UTF8));
            card2 = strdup(irc_print_card(game->community[1], USE_COLOR, USE_UTF8));
            card3 = strdup(irc_print_card(game->community[2], USE_COLOR, USE_UTF8));
            card4 = strdup(irc_print_card(game->community[3], USE_COLOR, USE_UTF8));
            card5 = strdup(irc_print_card(game->community[4], USE_COLOR, USE_UTF8));
            if (asprintf(&s, "River. Community cards: %s %s %s %s %s",
                             card1, card2, card3, card4, card5) != -1) {
                irc_cmd_msg(session, channel, s);
                free(s);
            }
            free(card1);
            free(card2);
            free(card3);
            free(card4);
            free(card5);
            break;
        case PHASE_RIVER:
            irc_cmd_msg(session, channel, "TODO: showdown");
            return;
    }
    set_turn(session, game, channel, next_player(game, game->button), 1);
}

void
process_cmd (irc_session_t *session,
             const char *from, const char *channel,
             char *cmd)
{
    char *rest;
    char *resp;
    char from_nick[32];
    game_tp game;
    
    irc_target_get_nick(from, from_nick, 32);

    separate_first_word(&cmd, &rest);
    
    if (strcasecmp(cmd, "quit") == 0) {
        irc_cmd_msg(session, channel ? channel : from_nick, "Live long and prosper. Good-bye!");
        irc_cmd_quit(session, "Fold.");
    } else if (strcasecmp(cmd, "help") == 0) {
        const char *to = channel ? channel : from_nick;
        irc_cmd_msg(session, to, "This is ircpoker."); usleep(100);
        irc_cmd_msg(session, to, "List of available bot commands: quit, init, game, set, deal, help"); usleep(100);
        irc_cmd_msg(session, to, "List of in-game to-the-table declarations: what's the game?, join game, afk, leave game, re, back");
    } else if (strcasecmp(cmd, "init") == 0) {
        /* new game */
        struct channel_game *cgame;

        if (!channel) {
            irc_cmd_msg(session, from_nick, "Games can only be created in channels");
            return;
        }
        if (get_channel_game(session, channel)) {
            char *resp;
            if (asprintf(&resp, "%s: There is already a game in this channel.", from_nick) != -1) {
                irc_cmd_msg(session, channel, resp);
                free(resp);
            }
            return;
        }

        game = new_game(1);
        strncpy(game->players[0].nick, from_nick, NICK_LEN);
        game->players[0].active = 1;
        game->house = &game->players[0];
        cgame = malloc(sizeof(struct channel_game));
        cgame->session = session;
        cgame->channel = strdup(channel);
        cgame->game = game;
        cgame->next = channel_games_list;
        channel_games_list = cgame;

        list_game_info(session, game, channel);
    } else if (strcasecmp(cmd, "game") == 0) {
        const char *chan = channel;
        if (!(chan = channel))
            chan = rest;

        if ((game = get_channel_game(session, chan))) {
            list_game_info(session, game, channel ? channel: from_nick);
        } else {
            irc_cmd_msg(session, channel ? channel : from_nick, "No game.");
        }
    } else if (strcasecmp(cmd, "set") == 0) {
        if ((game = get_channel_game(session, channel))) {
            int val, i;
            if (strcmp(from_nick, game->house->nick) != 0) {
                irc_cmd_msg(session, channel, "Only The House may change the rules.");
                return;
            }
            if (!rest) {
                irc_cmd_msg(session, channel, "Usage: set [base stock|small blind|big blind] [|=|to] {value}");
                return;
            }
            if        (   sscanf(rest, "base stock %d", &val) > 0
                       || sscanf(rest, "base stock = %d", &val) > 0
                       || sscanf(rest, "base stock to %d", &val) > 0 ) {
                game->base_stock = val;
                for (i=0; i<game->n_players; ++i) {
                    if (game->players[i].chips < val) {
                        int delta = val - game->players[i].chips;
                        game->players[i].chips = val;
                        if (asprintf(&resp, "%s given %d in chips.", 
                                     game->players[i].nick, delta) != -1) {
                            irc_cmd_msg(session, channel, resp); usleep(100);
                            free(resp);
                        }
                    }
                }
            } else if (   sscanf(rest, "small blind %d", &val) > 0
                       || sscanf(rest, "small blind = %d", &val) > 0
                       || sscanf(rest, "small blind to %d", &val) > 0 ) {
                game->small_blind = val;
            } else if (   sscanf(rest, "big blind %d", &val) > 0
                       || sscanf(rest, "big blind = %d", &val) > 0
                       || sscanf(rest, "big blind to %d", &val) > 0 ) {
                game->big_blind = val;
            } else {
                irc_cmd_msg(session, channel, "Unknown setting."); usleep(100);
                irc_cmd_msg(session, channel, "Usage: set [base stock|small blind|big blind] [|=|to] {value}");
            }
        } else {
            irc_cmd_msg(session, channel ? channel : from_nick, "No game.");
        }
    } else if (strcasecmp(cmd, "deal") == 0) {
        if ((game = get_channel_game(session, channel))) {
            if (strcmp(from_nick, game->house->nick) != 0) {
                irc_cmd_msg(session, channel, "Only The House may call for first deal.");
            }
            deal_round(session, game, channel);
        } else {
            irc_cmd_msg(session, channel ? channel : from_nick, "No game.");
        }
    } else {
        if (asprintf(&resp, "No such command: %s.", cmd) != -1) {
            irc_cmd_msg(session, channel ? channel : from_nick, resp);
            free(resp);
        }
    }
}

void
process_bet_cmd (irc_session_t *session,
                 const char *from, const char *channel, game_tp game,
                 char *cmd)
{
    char *resp;
    char from_nick[32];
    int i, val;
    irc_target_get_nick(from, from_nick, 32);

    int player_id = -1;

    for (i=0; i<game->n_players; ++i) {
        if (strcmp(game->players[i].nick, from_nick) == 0) {
            player_id = i;
        }
    }

    if (strcasecmp(cmd, "what's the game?") == 0) {
        list_game_info(session, game, from_nick);
    } else if (strcasecmp(cmd, "join game") == 0) {
        if (player_id != -1) {
            game->players[player_id].active = 1;
            if (asprintf(&resp, "Player %s is active.", from_nick) != -1) {
                irc_cmd_msg(session, channel, resp);
                free(resp);
            }
            return;
        }
        game->players = realloc(game->players,
                                (game->n_players+1) * sizeof(player_t));
        game->players[game->n_players].chips = game->base_stock;
        game->players[game->n_players].bet = 0;
        game->players[game->n_players].active = 1;
        game->players[game->n_players].folded = 1;
        game->players[game->n_players].allin = 0;
        strncpy(game->players[game->n_players].nick, from_nick, NICK_LEN);
        game->n_players ++;
        if (asprintf(&resp, "Draw up a chair, %s!", from_nick) != -1) {
            irc_cmd_msg(session, channel, resp);
            free(resp);
        }
    } else if (strcasecmp(cmd, "leave game") == 0
            || strcasecmp(cmd, "afk") == 0) {
        if (player_id != -1) {
            game->players[player_id].active = 0;
            if (asprintf(&resp, "Player %s is inactive.", from_nick) != -1) {
                irc_cmd_msg(session, channel, resp);
                free(resp);
            }
        }
    } else if (strcasecmp(cmd, "re") == 0
            || strcasecmp(cmd, "back") == 0) {
        if (player_id != -1) {
            game->players[player_id].active = 1;
            if (asprintf(&resp, "Player %s is active.", from_nick) != -1) {
                irc_cmd_msg(session, channel, resp);
                free(resp);
            }
        }

    } else if (strcasecmp(cmd, "check") == 0) {
        if (game->turn != player_id) goto wrong_player;
        if (game->pots[game->n_pots-1].bet == 0) {
            if (asprintf(&resp, "%s checks.",
                             game->players[player_id].nick) != -1) {
                irc_cmd_msg(session, channel, resp);
                free(resp);
            }
            set_turn(session, game, channel, next_player(game, player_id), 0);
        } else {
            irc_cmd_msg(session, channel,
                        "The bet is not 0. You cannot check. To call, say 'call'.");
        }

    } else if (strcasecmp(cmd, "call") == 0) {
        if (game->turn != player_id) goto wrong_player;
        if (game->pots[game->n_pots-1].bet == 0) {
            if (asprintf(&resp, "%s checks.",
                             game->players[player_id].nick) != -1) {
                irc_cmd_msg(session, channel, resp);
                free(resp);
            }
        } else {
            bet(game, player_id, game->pots[game->n_pots-1].bet);
            if (game->players[player_id].allin) {
                if (asprintf(&resp, "%s goes all in.",
                                 game->players[player_id].nick) != -1) {
                    irc_cmd_msg(session, channel, resp);
                    free(resp);
                }
            } else {
                if (asprintf(&resp, "%s calls.",
                                 game->players[player_id].nick) != -1) {
                    irc_cmd_msg(session, channel, resp);
                    free(resp);
                }
            }
        }
        set_turn(session, game, channel, next_player(game, player_id), 0);

    } else if (sscanf(cmd, "raise %d", &val) == 1) {
        if (game->turn != player_id) goto wrong_player;
        int betval = game->pots[game->n_pots-1].bet + val;
        bet(game, player_id, betval);
        if (game->players[player_id].allin) {
            if (asprintf(&resp, "%s goes all in.",
                             game->players[player_id].nick) != -1) {
                irc_cmd_msg(session, channel, resp);
                free(resp);
            }
        } else {
            if (asprintf(&resp, "%s raises by %d to %d.",
                             game->players[player_id].nick, val, betval) != -1) {
                irc_cmd_msg(session, channel, resp);
                free(resp);
            }
        }
        set_turn(session, game, channel, next_player(game, player_id), 0);

    } else if (strcasecmp(cmd, "raise") == 0) {
        /* TODO: betting unit */
    } else if (sscanf(cmd, "bet %d", &val) == 1) {
        if (game->turn != player_id) goto wrong_player;
        if (game->pots[game->n_pots-1].bet > val) {
            irc_cmd_msg(session, channel, "You must at least match the bet or fold.");
        } else {
            int raise = val - game->pots[game->n_pots-1].bet;
            bet(game, player_id, val);
            if (game->players[player_id].allin) {
                if (asprintf(&resp, "%s goes all in.",
                                 game->players[player_id].nick) != -1) {
                    irc_cmd_msg(session, channel, resp);
                    free(resp);
                }
            } else if (raise == 0) {
                if (asprintf(&resp, "%s calls.",
                                 game->players[player_id].nick) != -1) {
                    irc_cmd_msg(session, channel, resp);
                    free(resp);
                }
            } else {
                if (asprintf(&resp, "%s raises by %d to %d.",
                                 game->players[player_id].nick, raise, val) != -1) {
                    irc_cmd_msg(session, channel, resp);
                    free(resp);
                }
            }
            set_turn(session, game, channel, next_player(game, player_id), 0);
        }
    } else if (strcasecmp(cmd, "all in") == 0) {
        if (game->turn != player_id) goto wrong_player;
        bet(game, player_id, game->players[player_id].chips);
        if (game->players[player_id].allin) {
            if (asprintf(&resp, "%s goes all in.",
                             game->players[player_id].nick) != -1) {
                irc_cmd_msg(session, channel, resp);
                free(resp);
            }
        } else {
            if (asprintf(&resp, "%s should have been all in by now.",
                             game->players[player_id].nick) != -1) {
                irc_cmd_msg(session, channel, resp);
                free(resp);
            }
        }
        set_turn(session, game, channel, next_player(game, player_id), 0);

    } else if (strcasecmp(cmd, "fold") == 0) {
        if (game->turn != player_id) goto wrong_player;
        fold (game, player_id);
        if (asprintf(&resp, "%s folds.",
                         game->players[player_id].nick) != -1) {
            irc_cmd_msg(session, channel, resp);
            free(resp);
        }
        set_turn(session, game, channel, next_player(game, player_id), 0);

    }

    return;
wrong_player:
    if (asprintf(&resp, "%s, it is not your turn!",
                     game->players[player_id].nick) != -1) {
        irc_cmd_msg(session, channel, resp);
        free(resp);
    }
}

void
list_game_info (irc_session_t *session, game_tp game, const char *dest)
{
    int i;
    int past_first = 0;
    char *s;

    s = malloc(sizeof(char) * (game->n_players * (NICK_LEN+2) + 42));

    irc_cmd_msg(session, dest, "The game is Texas Hold'em, nothing wild."); usleep(100);
    if (game->small_blind || game->big_blind) {
        if (sprintf(s, "Small/big blinds are %d / %d.", game->small_blind,
                                                        game->big_blind) != -1)
            irc_cmd_msg(session, dest, s); usleep(100);
    } else {
        irc_cmd_msg(session, dest, "No blinds."); usleep(100);
    }

    if (sprintf(s, "Base chip stock is %d.", game->base_stock) != -1)
        irc_cmd_msg(session, dest, s); usleep(100);

    if (sprintf(s, "The House is represented by %s.", game->house->nick) != -1)
        irc_cmd_msg(session, dest, s); usleep(100);

    strcpy(s, "Active players: ");
    for (i=0; i<game->n_players; ++i) {
        if (game->players[i].active) {
            if (past_first) {
                strcat(s, ", ");
            }
            past_first = 1;
            strcat(s, game->players[i].nick);
        }
    }
    irc_cmd_msg(session, dest, s); usleep(100);

    free(s);
}


