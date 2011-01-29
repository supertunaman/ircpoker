#ifndef PLAYER_H
#define PLAYER_H

#include "card.h"
#include "deck.h"
#include "game.h"

#define NICK_LEN 32

typedef struct player {
    card_t hand[2];
    card_t best_hand[5];
    char nick[NICK_LEN];
    int chips;
    int active;
    int folded;
} player_t;

player_t players[10];

#endif
