/* Copyright 2011 Tuna <tuna@supertunaman.com
 * This code is under the Chicken Dance License v0.1 */
#include "player.h"

#include <stdlib.h>

void bet(game_tp g, int player_id, int amount)
{
    /* sets player's bet to amount */
    /* TODO: implement pots, and handle that here as well */
    int old_bet = g->players[player_id].bet;
    int raise = amount - old_bet;
    int D, i, j;

    if (raise >= g->players[player_id].chips) {
        g->players[player_id].allin = 1;
        g->players[player_id].bet += g->players[player_id].chips;
        raise = g->players[player_id].bet- old_bet;
        g->players[player_id].chips = 0;
    } else {
        g->players[player_id].bet += raise;
        g->players[player_id].chips -= raise;
    }

    for (i = 0; i < g->n_pots; ++i) {
        int hasme = 0;
        /* register interest in pot. */
        for (j = 0; j < g->pots[i].n_players; ++j) {
            if (g->pots[i].players[j] == &g->players[player_id]) {
                hasme = 1;
                break;
            }
        }
        if (!hasme) {
            g->pots[i].players = realloc(g->pots[i].players, ++g->pots[i].n_players);
            g->pots[i].players[g->pots[i].n_players-1] = & g->players[player_id];
        }
        /* do the math. */
        if ((D = (g->pots[i].bet - old_bet)) > 0) {
            /* match the bet, if possible. */
            if (D <= raise) {
                g->pots[i].content += D;
                raise -= D;
                old_bet = g->pots[i].bet;
            } else {
                /* new side pot. */
                pot_t sidepot;
                sidepot.content = 0;
                sidepot.bet = g->pots[i].bet;
                sidepot.players = NULL;
                sidepot.n_players = 0;
                g->pots[i].bet = g->players[player_id].bet;
                int DD;
                /* transfer higher bets */
                for (j = 0; j < g->pots[i].n_players; ++j) {
                    if ((DD = g->pots[i].players[j]->bet - g->pots[i].bet) > 0) {
                        g->pots[i].content -= DD;
                        sidepot.content += DD;
                    }
                }
                /* move higher pots along and insert side pot into array. */
                g->pots = realloc (g->pots, ++g->n_pots);
                for (j = i; j < g->n_pots-1; ++j)
                    g->pots[j+1] = g->pots[j];
                g->pots[i+1] = sidepot;

                /* all in. Ignore higher pots. */
                break;
            }
        }
        if (i == g->n_pots-1 && raise > 0) {
            /* current pot, I can raise. */
            g->pots[i].content += raise;
            g->pots[i].bet = g->players[player_id].bet;
        }
    }
}

void fold(game_tp g, int player_id)
{
    /* removes player from the current hand */
    g->players[player_id].folded = 1;
}

void get_best_player_hand(game_tp g, int player_id)
{
    /* finds the best possible 5-card hand for players[player_id] */
    card_t last_hand[5];
    card_t cur_hand[5];
    int i;
    int j;
    int k;

    /* compare to the community cards first */
    for (i = 0; i < 5; i++)
        last_hand[i] = g->community[i];

    /* replace two of the community cards at a time with the pocket cards */
    for (i = 0; i < 5; i++) {
         for (j = 0; j < i; j++) {
             for (k = 0; k < 5; k++)
                 cur_hand[k] = g->community[k];
             cur_hand[i] = g->players[player_id].hand[0];
             cur_hand[j] = g->players[player_id].hand[1];

             /* if cur_hand beats last_hand, set last_hand's values to cur_hand's */
             if (handcmp(cur_hand, last_hand) > 0)
                 for (k = 0; k < 5; k++)
                     last_hand[k] = cur_hand[k];
         }
    }

    /* now replace one card at a time and compare */
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 5; j++)
            cur_hand[j] = g->community[j];
        cur_hand[i] = g->players[player_id].hand[0];

        if (handcmp(cur_hand, last_hand) > 0)
            for (j = 0; j < 5; j++)
                last_hand[j] = cur_hand[j];

        for (j = 0; j < 5; j++)
            cur_hand[j] = g->community[j];
        cur_hand[i] = g->players[player_id].hand[1];

        if (handcmp(cur_hand, last_hand) > 0)
            for (j = 0; j < 5; j++)
                last_hand[j] = cur_hand[j];
    }

    /* by now, last_hand should be the best possible hand */
    for (i = 0; i < 5; i++)
        g->players[player_id].best_hand[i] = last_hand[i];
}

/* TODO: stop using qsort and get rid of this evil.
 * We might want thread safety some day. */
static game_tp _current_game;

int playercmp(const void * player1, const void * player2)
{
    /* compares two players in a way that is usable by qsort() */
    if (_current_game->players[*(int *)player1].active == 0 ||
            _current_game->players[*(int *)player1].folded == 1)
        return -1;
    if (_current_game->players[*(int *)player2].active == 0 ||
            _current_game->players[*(int *)player2].folded == 1)
        return 1;

    return handcmp( _current_game->players[ *(int *)player1 ].best_hand,
                    _current_game->players[ *(int *)player2 ].best_hand);
}

/* pass array long enough for all players */
void get_player_ranks(game_tp g, int ranking_array[])
{
    int i;

    for (i = 0; i < g->n_players; i++) {
        ranking_array[i] = i;
        get_best_player_hand(g, i);
    }

    _current_game = g;

    qsort(ranking_array, g->n_players, sizeof(int), playercmp);
}

/* free result with free() when done. */
int *alloc_player_ranks(game_tp g)
{
    int *ranking_array = malloc(sizeof (int) * g->n_players);
    get_player_ranks(g, ranking_array);
    return ranking_array;
}


