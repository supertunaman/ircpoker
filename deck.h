#ifndef DECK_H
#define DECK_H

#include <stdio.h>
#include <stdlib.h>
#include "card.h"
#include "player.h"

#define KEY_LEN 10

card_t deck[52];

void deal(int playeridx);
void init_deck();
void shuffle_deck();

#endif

