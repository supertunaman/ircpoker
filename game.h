/* Copyright 2011 Tuna <tuna@supertunaman.com
 * This code is under the Chicken Dance License v0.1 */
#ifndef GAME_H
#define GAME_H

typedef struct game *game_tp;

#include "card.h"
#include "deck.h"
#include "player.h"

struct game {
    card_t deck[52];
    int pot;
    card_t community[5];    /* the community cards */
    player_t *players;
    int n_players;
};

void deal_community(game_tp);

game_tp new_game(int n_players);
void free_game(game_tp);

#endif

