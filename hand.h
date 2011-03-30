/* Copyright 2011 Tuna <tuna@supertunaman.com
 * This code is under the Chicken Dance License v0.1 */
#ifndef HAND_H
#define HAND_H

#include <stdlib.h>
#include <string.h>
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
    int equal_to[10];
} hand_t;

int handcmp(card_t hand1[], card_t hand2[]);
void promote_aces(int handvals[]);
void sort(int a[], int n);
ranks_t rank_hand(card_t hand[]);

#endif

