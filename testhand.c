/* testing the rank_hand function in hand.h
 * 
 * test vectors gathered from the "poker hand rankings" card in a deck
 * of Bicycle cards */

#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "card.h"
#include "deck.h"
#include "hand.h"

double get_time()
{
    struct timeval t;
    struct timezone tz;
    gettimeofday(&t, &tz);
    return t.tv_sec + t.tv_usec*1e-6;
}

void print_hand(card_t hand[])
{
    int i;

    switch (rank_hand(hand)) {
        case HIGHCARD:
            printf("High Card:       ");
            break;
        case ONEPAIR:
            printf("One Pair:        ");
            break;
        case TWOPAIR:
            printf("Two Pair:        ");
            break;
        case THREEKIND:
            printf("Three of a Kind: ");
            break;
        case STRAIGHT:
            printf("Straight:        ");
            break;
        case FLUSH:
            printf("Flush:           ");
            break;
        case FULLHOUSE:
            printf("Full House:      ");
            break;
        case FOURKIND:
            printf("Four of a Kind:  ");
            break;
        case STRAIGHTFLUSH:
            printf("Straight Flush:  ");
            break;
        case ROYALFLUSH:
            printf("Royal Flush:     ");
            break;
    }
    
    for (i = 0; i < 5; i++) {
        switch (hand[i].value) {
            case 1:
                printf(" A");
                break;
            case 10:
                printf("10");
                break;
            case 11:
                printf(" J");
                break;
            case 12:
                printf(" Q");
                break;
            case 13:
                printf(" K");
                break;
            default:
                printf(" %d", hand[i].value);
        }
        switch(hand[i].suit) {
            case CLUBS:
                putchar('C');
                break;
            case DIAMONDS:
                putchar('D');
                break;
            case HEARTS:
                putchar('H');
                break;
            case SPADES:
                putchar('S');
                break;
        }
        putchar(' ');
    }
    puts("");
}

int main() {
    int i;
    double start_time;
    card_t high_card[5];
    card_t one_pair[5];
    card_t two_pair[5];
    card_t three_kind[5];
    card_t straight[5];
    card_t flush[5];
    card_t full_house[5];
    card_t four_kind[5];
    card_t straight_flush[5];
    card_t royal_flush[5];
    
    init_deck();

    /* construct high card */
    high_card[0] = deck[26];        /* Ace of Hearts */
    high_card[1] = deck[5];         /* 6 of Clubs */
    high_card[2] = deck[28];        /* 3 of Hearts */
    high_card[3] = deck[19];        /* 7 of Diamonds */
    high_card[4] = deck[48];        /* 10 of Spades */

    /* one pair */
    one_pair[0] = deck[43];         /* 5 of Spades */
    one_pair[1] = deck[30];         /* 5 of Hearts */
    one_pair[2] = deck[14];         /* 2 of Diamonds */
    one_pair[3] = deck[6];          /* 7 of Clubs */
    one_pair[4] = deck[3];          /* 4 of Clubs */

    /* two pair */
    two_pair[0] = deck[26];         /* Ace of Hearts */
    two_pair[1] = deck[0];          /* Ace of Clubs */
    two_pair[2] = deck[20];         /* 8 of Diamonds */
    two_pair[3] = deck[46];         /* 8 of Spades */
    two_pair[4] = deck[28];         /* 3 of Hearts */

    /* three of a kind */
    three_kind[0] = deck[26];       /* Ace of Hearts */
    three_kind[1] = deck[0];        /* Ace of Clubs */
    three_kind[2] = deck[39];       /* Ace of Spades */
    three_kind[3] = deck[19];       /* 7 of Diamonds */
    three_kind[4] = deck[50];       /* Queen of Spades */

    /* straight */
    straight[0] = deck[43];         /* 5 of Spades */
    straight[1] = deck[5];          /* 6 of Clubs */
    straight[2] = deck[32];         /* 7 of Hearts */
    straight[3] = deck[7];          /* 8 of Clubs */
    straight[4] = deck[21];         /* 9 of Diamonds */

    /* flush */
    flush[0] = deck[14];            /* 2 of Diamonds */
    flush[1] = deck[20];            /* 8 of Diamonds */
    flush[2] = deck[17];            /* 5 of Diamonds */
    flush[3] = deck[13];            /* Ace of Diamonds */
    flush[4] = deck[18];            /* 6 of Diamonds */

    /* full house */
    full_house[0] = deck[19];       /* 7 of Diamonds */
    full_house[1] = deck[45];       /* 7 of Spades */
    full_house[2] = deck[6];        /* 7 of Clubs */
    full_house[3] = deck[1];        /* 2 of Clubs */
    full_house[4] = deck[27];       /* 2 of Hearts */

    /* four of a kind */
    four_kind[0] = deck[24];        /* Queen of Diamonds */
    four_kind[1] = deck[37];        /* Queen of Hearts */
    four_kind[2] = deck[50];        /* Queen of Spades */
    four_kind[3] = deck[11];        /* Queen of Clubs */
    four_kind[4] = deck[43];        /* 5 of Spades */

    /* straight flush */
    straight_flush[0] = deck[43];   /* 5 of Spades */
    straight_flush[1] = deck[44];   /* 6 of Spades */
    straight_flush[2] = deck[45];   /* 7 of Spades */
    straight_flush[3] = deck[46];   /* 8 of Spades */
    straight_flush[4] = deck[47];   /* 9 of Spades */

    /* royal flush */
    royal_flush[0] = deck[35];      /* 10 of Hearts */
    royal_flush[1] = deck[36];      /* Jack of Hearts */
    royal_flush[2] = deck[37];      /* Queen of Hearts */
    royal_flush[3] = deck[38];      /* King of Hearts */
    royal_flush[4] = deck[26];      /* Ace of Hearts */

    /* print each of the above hands and pass them to rank_hand() */
    print_hand(high_card);
    print_hand(one_pair);
    print_hand(two_pair);
    print_hand(three_kind);
    print_hand(straight);
    print_hand(flush);
    print_hand(full_house);
    print_hand(four_kind);
    print_hand(straight_flush);
    print_hand(royal_flush);

    start_time = get_time();
    for (i = 0; i < 5000000; i++) {
        rank_hand(royal_flush);
    }
    printf("\nProcessed 5,000,000 Royal Flushes in %f\n", get_time() - start_time);

    return 0;
}

