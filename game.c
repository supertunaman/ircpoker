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
    g->pot = 0;
    /* calloc overwrites with zeroes. All players will be:
     * active = 0
     * folded = 0
     * allin = 0
     *  &c.
     */
    g->players = calloc(n_players, sizeof (player_t));
    g->n_players = n_players;

    init_deck(g->deck);

    return g;
}

void free_game(game_tp g)
{
    free(g->players);
    free(g);
}

