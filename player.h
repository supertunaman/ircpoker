/* Copyright 2011 Tuna <tuna@supertunaman.com
 * This code is under the Chicken Dance License v0.1 */
#ifndef PLAYER_H
#define PLAYER_H

#include "card.h"

#define NICK_LEN 32

typedef struct player {
    card_t hand[2];
    card_t best_hand[5];
    char nick[NICK_LEN];
    int chips;
    int bet;
    int active;
    int allin;
    int folded;
} player_t;

#include "deck.h"
#include "game.h"
#include "hand.h"

/* sounds like a local variable? we'll see */
/* int player_ranks[10]; */ /* used for sorting players to find best hand(s) */

void bet(game_tp, int player_id, int amount);
void fold(game_tp, int player_id);
void get_best_player_hand(game_tp, int player_id);
/* pass array long enough for all players */
void get_player_ranks(game_tp, int ranking_array[]);
/* free result with free() when done. */
int *alloc_player_ranks(game_tp);

#endif
