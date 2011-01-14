#ifndef PLAYER_H
#define PLAYER_H

#include "card.h"

#define NICK_LEN 32

typedef struct player {
    card_t hand[2];
    char nick[NICK_LEN];
    int chips;
    int active;
} player_t;

player_t players[10];

#endif
