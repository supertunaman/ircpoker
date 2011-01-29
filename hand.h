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

int handcmp(card_t hand1[], card_t hand2[]);
void get_best_player_hand(int player_id);
void promote_aces(int handvals[]);
void sort(int a[], int n);
ranks_t rank_hand(card_t hand[]);

#endif

