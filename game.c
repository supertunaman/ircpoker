/* Copyright 2011 Tuna <tuna@supertunaman.com
 * This code is under the Chicken Dance License v0.1 */
#include "game.h"

#include <stdlib.h>

void deal_community(game_tp g)
{
    /* Gives 5 cards to COMMUNITY */
    int i;
    int c = 0;
    for (i = 0; i < 52; i++)
    {
        if (!g->deck[i].dealt) {
            g->deck[i].dealt = 1;
            g->deck[i].cardholder = COMMUNITY;
            g->community[c] = g->deck[i];
            c++;
        }
        if (c > 4)
            break;
    }
}

struct game *
new_game (int n_players)
{
    struct game *g;

    g = (struct game *) malloc(sizeof (struct game));
    /* calloc overwrites with zeroes. All players will be:
     * active = 0
     * folded = 0
     * allin = 0
     *  &c.
     */
    g->players = calloc(n_players, sizeof (player_t));
    g->n_players = n_players;
    g->pots = calloc(1, sizeof (pot_t));
    g->n_pots = 1;
    /* players are added to the pot by betting. */
    g->pots[0].players = NULL;
    g->pots[0].n_players = 0;
    g->pots[0].content = 0;
    g->pots[0].bet = 0;

    /* defaults */
    g->small_blind = 1;
    g->big_blind = 2;
    g->betting_unit = 0;
    g->base_stock = 0;

    g->phase = PHASE_PRE_DEAL;
    g->turn = 0;
    g->button = 0;

    init_deck(g->deck);

    return g;
}

void free_game (game_tp g)
{
    int i;
    for (i=0; i<g->n_pots; ++i)
        free(g->pots[i].players);
    free(g->pots);
    free(g->players);
    free(g);
}

int
next_player (game_tp g, int playeridx)
{
    int res = playeridx;
    do {
        res++;
        if (res >= g->n_players) res = 0;
        if (res == playeridx) return -1;
    } while (g->players[res].folded);
    return res;
}

