/* Copyright 2011 Tuna <tuna@supertunaman.com
 * This code is under the Chicken Dance License v0.1 */

#ifndef CARD_H
#define CARD_H

#define COMMUNITY -1 /* if a card_t's cardholder is this, then it's a community card */

typedef enum suit {
    CLUBS,
    DIAMONDS,
    HEARTS,
    SPADES
} suit_t;

typedef struct card {
    int idx;
    int value;
    suit_t suit;
    int dealt;
    int cardholder; 
} card_t;

char *irc_print_card (card_t card, int color, int utf8);

#endif

