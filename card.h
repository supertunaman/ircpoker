#ifndef CARD_H
#define CARD_H

#define COMMUNITY 10 /* if a card_t's cardholder is this, then it's a community card */

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

#endif

