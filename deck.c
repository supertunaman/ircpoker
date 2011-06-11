/* Copyright 2011 Tuna <tuna@supertunaman.com
 * This code is under the Chicken Dance License v0.1 */
#include "deck.h"

void rc4_init(unsigned char *key, unsigned int key_length);
unsigned char rc4_output();


void deal(game_tp g, int playeridx)
{
    /* finds next available card and "gives" it to the player */
    int i;
    int c = 0;
    for (i = 0; i < 52; i++)
    {
        if (!g->deck[i].dealt) {
            g->deck[i].dealt = 1;
            g->deck[i].cardholder = playeridx;
            g->players[playeridx].hand[c] = g->deck[i];
            c++;
        }
        if (c > 1)
            break;
    }
}

void init_deck(card_t deck[])
{
    /* intializes deck, assigns values, suits and sets dealt to 0 */
    int i;
    for (i = 0; i < 52; i++)
    {
        deck[i].idx = i;

        if ((i + 1) % 13 != 0)
            deck[i].value = (i + 1) % 13;
        else
            deck[i].value = 13;
        
        deck[i].suit = i / 13;
        deck[i].dealt = 0;
    }
}

void shuffle_deck(game_tp g)
{
    /* uses RC4 as an RNG to fisher-yates shuffle the deck */
    int i;
    int j;
    struct card tmp;

    /* read in a few bytes from /dev/urandom */
    /* TODO: clean this up a bit, no need to read a file and initialize RC4 every time the deck is shuffled */
    FILE *urandom = fopen("/dev/urandom", "rb");
    unsigned char key_buf[KEY_LEN + 1];
    fgets((char *)key_buf, KEY_LEN + 1, urandom);
    fclose(urandom);
    
    /* initialize RC4 KSA */
    rc4_init(key_buf, KEY_LEN);
    
    /* discard 256 bytes, like a good boy */
    for (i = 0; i < 256; i++) 
        rc4_output();

    /* Fisher-Yates shuffle using RC4 as an RNG */
    for (i = 52; i > 1; i--)
    {
        j = rc4_output() % i;
        tmp = g->deck[j];
        g->deck[j] = g->deck[i - 1];
        g->deck[i - 1] = tmp;
    }
}

void undeal(game_tp g)
{
    int i;

    for (i=0; i<52; ++i) {
        g->deck[i].dealt = 0;
    }
}

