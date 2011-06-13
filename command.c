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
    int big = next_player(game, small);

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
        char *card1 = strdup(irc_print_card(game->players[pidx].hand[0], 1, 1));
        char *card2 = strdup(irc_print_card(game->players[pidx].hand[1], 1, 1));
        if (asprintf(&s, "Your cards: %s %s", card1, card2) != -1) {
            irc_cmd_msg(session, game->players[pidx].nick, s);
            free(s);
        }
        free(card1);
        free(card2);
    } while ((pidx = next_player(game, pidx)) != small);

    game->turn = next_player(game, big);
    game->phase = PHASE_PRE_FLOP;

    if (asprintf(&s, "Cards dealt. %s, the bet is %d. Do you call?",
                                     game->players[game->turn].nick,
                                     game->big_blind) != -1) {
        irc_cmd_msg(session, channel, s);
        free(s);
    }
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
    int i;
    irc_target_get_nick(from, from_nick, 32);

    if (strcasecmp(cmd, "what's the game?") == 0) {
        list_game_info(session, game, from_nick);
    } else if (strcasecmp(cmd, "join game") == 0) {
        for (i=0; i<game->n_players; ++i) {
            if (strcmp(game->players[i].nick, from_nick) == 0) {
                game->players[i].active = 1;
                if (asprintf(&resp, "Player %s is active.", from_nick) != -1) {
                    irc_cmd_msg(session, channel, resp);
                    free(resp);
                }
                goto _joined;
            }
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
_joined:
        ;
    } else if (strcasecmp(cmd, "leave game") == 0
            || strcasecmp(cmd, "afk") == 0) {
        for (i=0; i<game->n_players; ++i) {
            if (strcmp(game->players[i].nick, from_nick) == 0) {
                game->players[i].active = 0;
                if (asprintf(&resp, "Player %s is inactive.", from_nick) != -1) {
                    irc_cmd_msg(session, channel, resp);
                    free(resp);
                }
                break;
            }
        }
    } else if (strcasecmp(cmd, "re") == 0
            || strcasecmp(cmd, "back") == 0) {
        for (i=0; i<game->n_players; ++i) {
            if (strcmp(game->players[i].nick, from_nick) == 0) {
                game->players[i].active = 1;
                if (asprintf(&resp, "Player %s is active.", from_nick) != -1) {
                    irc_cmd_msg(session, channel, resp);
                    free(resp);
                }
                break;
            }
        }
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


