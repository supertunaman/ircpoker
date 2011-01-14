#ifndef HAND_H
#define HAND_H

#include "card.h"
#include "game.h"
#include "player.h"

typedef enum ranks {
    HIGHCARD,
    ONEPAIR,
    TWOPAIR,
    THREEKIND,
    STRAIGHT,
    FLUSH,
    FULLHOUSE,
    FOURKIND,
    STRAIGHTFLUSH,
    ROYALFLUSH
} ranks_t;

typedef struct hand {
    card_t cards[5];
    int cardholder;
    ranks_t rank;
    int advantage;
} hand_t;

void sort(int a[], int n);
ranks_t rank_hand(card_t hand[]);

#endif

