/* Copyright 2011 Tuna <tuna@supertunaman.com
 * This code is under the Chicken Dance License v0.1 */
#ifndef PLAYER_H
#define PLAYER_H

#include "card.h"
#include "deck.h"
#include "game.h"
#include "hand.h"

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

player_t players[10];
int player_ranks[10]; /* used for sorting players to find best hand(s) */

void bet(int player_id, int amount);
void fold(int player_id);
void get_best_player_hand(int player_id);
int playercmp(const void * player1, const void * p2);
void player_sort();

#endif
