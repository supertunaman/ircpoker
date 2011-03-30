/* Copyright 2011 Tuna <tuna@supertunaman.com
 * This code is under the Chicken Dance License v0.1 */
#ifndef GAME_H
#define GAME_H

#include "card.h"
#include "deck.h"
#include "player.h"

int allin[10];          /* list of players that are all in */
card_t community[5];    /* the community cards */
int pot;

void deal_community();

#endif

