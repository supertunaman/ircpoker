/* Copyright 2011 Tom Jollans <t@jollybox.de>
 * This code is under the Chicken Dance License v0.1 */

#include <stdio.h>
#include "card.h"

char *irc_print_card (card_t card, int color, int utf8)
{
    static const char *red_start = "\x03" "2";
    static const char *red_end = "\x03";

    static char result[16];
    char *resp = result;
    int red = 0;

    switch (card.suit) {
        case DIAMONDS:
        case HEARTS:
            red = color;
        default:;
    }

    if (red) {
        resp += sprintf(resp, "%s", red_start);
    }

    switch (card.value) {
        case 1:
            resp += sprintf(resp, " A");
            break;;
        case 11:
            resp += sprintf(resp, " J");
            break;;
        case 12:
            resp += sprintf(resp, " Q");
            break;;
        case 13:
            resp += sprintf(resp, " K");
            break;;
        default:
            resp += sprintf(resp, "%2d", card.value);
    }

    switch (card.suit) {
        case CLUBS:
            if (utf8) resp += sprintf(resp, "\xe2\x99\xa3");
            else      resp += sprintf(resp, " Clubs");
            break;;
        case DIAMONDS:
            if (utf8) resp += sprintf(resp, "\xe2\x99\xa6");
            else      resp += sprintf(resp, " Diamonds");
            break;;
        case HEARTS:
            if (utf8) resp += sprintf(resp, "\xe2\x99\xa5");
            else      resp += sprintf(resp, " Hearts");
            break;;
        case SPADES:
            if (utf8) resp += sprintf(resp, "\xe2\x99\xa0");
            else      resp += sprintf(resp, " Spades");
            break;;
    }

    if (red) {
        resp += sprintf(resp, "%s", red_end);
    }

    *resp = '\0';

    return result;
}

