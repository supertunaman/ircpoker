/* Copyright 2011 Tuna <tuna@supertunaman.com
 * This code is under the Chicken Dance License v0.1 */
#ifndef DECK_H
#define DECK_H

#include <stdio.h>
#include <stdlib.h>
#include "card.h"
#include "game.h"
#include "player.h"

#define KEY_LEN 10 /* used to key RC4 for the shuffle. loloverkill */

/* card_t deck[52]; */

void deal(game_tp, int playeridx);
void init_deck(card_t deck[]);
void shuffle_deck(game_tp);
void undeal(game_tp g);

#endif

