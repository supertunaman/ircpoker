#include "deck.h"
#include "player.h"

void deal(player_t *player, int playeridx)
{
    int i;
    int c = 0;
    for (i = 0; i < 52; i++)
    {
        if (!deck[i].dealt) {
            deck[i].dealt = 1;
            deck[i].cardholder = playeridx;
            (*player).hand[c] = deck[i];
            c++;
        }
        if (c > 1)
            break;
    }
}

void init_deck()
{
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

void shuffle_deck()
{
    int i;
    int j;
    struct card tmp;

    /* read in a few bytes from /dev/urandom */
    FILE *urandom = fopen("/dev/urandom", "rb");
    char key_buf[KEY_LEN + 1];
    fgets(key_buf, KEY_LEN + 1, urandom);
    fclose(urandom);
    
    /* initialize RC4 KSA */
    rc4_init(key_buf, KEY_LEN);
    
    /* discard 256 bytes */
    for (i = 0; i < 256; i++) { rc4_output(); }

    /* Fisher-Yates shuffle using RC4 as an RNG */
    for (i = 52; i > 1; i--)
    {
        j = rc4_output() % i;
        tmp = deck[j];
        deck[j] = deck[i - 1];
        deck[i - 1] = tmp;
    }
}