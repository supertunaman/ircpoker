/* Copyright 2011 Tuna <tuna@supertunaman.com
 * This code is under the Chicken Dance License v0.1 */
#ifndef GAME_H
#define GAME_H

typedef struct game *game_tp;

#include "card.h"
#include "deck.h"
#include "player.h"

typedef struct pot {
    int content;
    int bet;
    player_t **players;
    int n_players;
} pot_t;

enum game_phase {
    PHASE_PRE_DEAL,
    PHASE_PRE_FLOP,
    PHASE_FLOP,
    PHASE_TURN,
    PHASE_RIVER
};

struct game {
    /* infrastructure */
    card_t deck[52];
    card_t community[5];    /* the community cards */
    player_t *players;
    int n_players;
    pot_t *pots;
    int n_pots;

    /* rules */
    int small_blind, big_blind;
    int betting_unit;
    int base_stock;
    /* TODO: limits */

    player_t *house; /* overlord */

    /* situation */
    enum game_phase phase;
    int turn;
    int button;
};


void deal_community(game_tp);

game_tp new_game(int n_players);
void free_game(game_tp);

int next_player(game_tp, int playeridx);

#endif

