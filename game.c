#include "game.h"

void deal_community()
{
    /* Gives 5 cards to COMMUNITY */
    int i;
    int c = 0;
    for (i = 0; i < 52; i++)
    {
        if (!deck[i].dealt) {
            deck[i].dealt = 1;
            deck[i].cardholder = COMMUNITY;
            community[c] = deck[i];
            c++;
        }
        if (c > 4)
            break;
    }
}

