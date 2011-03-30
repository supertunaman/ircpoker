/* Copyright 2011 Tuna <tuna@supertunaman.com
 * This code is under the Chicken Dance License v0.1 */

/* testing the rank_hand function in hand.h
 * 
 * test vectors gathered from the "poker hand rankings" card in a deck
 * of Bicycle cards and wikipedia */

#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "card.h"
#include "deck.h"
#include "hand.h"

double get_time()
{
    /* used for benchmarking */
    struct timeval t;
    struct timezone tz;
    gettimeofday(&t, &tz);
    return t.tv_sec + t.tv_usec*1e-6;
}

void print_hand(card_t hand[])
{
    /* pretty prints the given hand */
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
    /* puts(""); */
}

int main() {
    int i;
    double start_time;
    card_t high_card1[5];   /* now we're going to generate several hands and test the ranking stuff */
    card_t high_card2[5];
    card_t one_pair1[5];
    card_t one_pair2[5];
    card_t two_pair1[5];
    card_t two_pair2[5];
    card_t three_kind1[5];
    card_t three_kind2[5];
    card_t straight1[5];
    card_t straight2[5];
    card_t flush1[5];
    card_t flush2[5];
    card_t full_house1[5];
    card_t full_house2[5];
    card_t four_kind1[5];
    card_t four_kind2[5];
    card_t straight_flush1[5];
    card_t straight_flush2[5];
    card_t royal_flush1[5];
    card_t royal_flush2[5];
    
    init_deck();

    /* construct high card */
    high_card1[0] = deck[26];       /* Ace of Hearts */
    high_card1[1] = deck[5];        /* 6 of Clubs */
    high_card1[2] = deck[28];       /* 3 of Hearts */
    high_card1[3] = deck[19];       /* 7 of Diamonds */
    high_card1[4] = deck[48];       /* 10 of Spades */

    high_card2[0] = deck[9];        /* 10 of Clubs */
    high_card2[1] = deck[20];       /* 8 of Diamonds */
    high_card2[2] = deck[18];       /* 6 of Diamonds */
    high_card2[3] = deck[16];       /* 4 of Diamonds */
    high_card2[4] = deck[40];       /* 2 of Spades */

    /* one pair */
    one_pair1[0] = deck[43];        /* 5 of Spades */
    one_pair1[1] = deck[30];        /* 5 of Hearts */
    one_pair1[2] = deck[14];        /* 2 of Diamonds */
    one_pair1[3] = deck[6];         /* 7 of Clubs */
    one_pair1[4] = deck[3];         /* 4 of Clubs */

    one_pair2[0] = deck[43];        /* 5 of Spades */
    one_pair2[1] = deck[4];         /* 5 of Clubs */
    one_pair2[2] = deck[21];        /* 9 of Diamonds */
    one_pair2[3] = deck[44];        /* 6 of Spades */
    one_pair2[4] = deck[33];        /* 8 of Hearts */

    /* two pair */
    two_pair1[0] = deck[26];        /* Ace of Hearts */
    two_pair1[1] = deck[0];         /* Ace of Clubs */
    two_pair1[2] = deck[20];        /* 8 of Diamonds */
    two_pair1[3] = deck[46];        /* 8 of Spades */
    two_pair1[4] = deck[28];        /* 3 of Hearts */

    two_pair2[0] = deck[9];         /* 10 of Clubs */
    two_pair2[1] = deck[22];        /* 10 of Diamonds */
    two_pair2[2] = deck[14];        /* 2 of Diamonds */
    two_pair2[3] = deck[40];        /* 2 of Spades */
    two_pair2[4] = deck[46];        /* 8 of Spades */

    /* three of a kind */
    three_kind1[0] = deck[26];      /* Ace of Hearts */
    three_kind1[1] = deck[0];       /* Ace of Clubs */
    three_kind1[2] = deck[39];      /* Ace of Spades */
    three_kind1[3] = deck[19];      /* 7 of Diamonds */
    three_kind1[4] = deck[50];      /* Queen of Spades */

    three_kind2[0] = deck[21];      /* 9 of Diamonds */
    three_kind2[1] = deck[47];      /* 9 of Spades */
    three_kind2[2] = deck[8];       /* 9 of Clubs */
    three_kind2[3] = deck[6];       /* 7 of Clubs */
    three_kind2[4] = deck[13];      /* Ace of Diamonds */

    /* straight */
    straight1[0] = deck[43];        /* 5 of Spades */
    straight1[1] = deck[5];         /* 6 of Clubs */
    straight1[2] = deck[32];        /* 7 of Hearts */
    straight1[3] = deck[7];         /* 8 of Clubs */
    straight1[4] = deck[21];        /* 9 of Diamonds */

    straight2[0] = deck[40];        /* 2 of Spades */
    straight2[1] = deck[2];         /* 3 of Clubs */
    straight2[2] = deck[29];        /* 4 of Hearts */
    straight2[3] = deck[43];        /* 5 of Spades */
    straight2[4] = deck[31];        /* 6 of Hearts */

    /* flush */
    flush1[0] = deck[14];           /* 2 of Diamonds */
    flush1[1] = deck[20];           /* 8 of Diamonds */
    flush1[2] = deck[17];           /* 5 of Diamonds */
    flush1[3] = deck[13];           /* Ace of Diamonds */
    flush1[4] = deck[18];           /* 6 of Diamonds */

    flush2[0] = deck[14];           /* 2 of Diamonds */
    flush2[1] = deck[16];           /* 4 of Diamonds */
    flush2[2] = deck[18];           /* 6 of Diamonds */
    flush2[3] = deck[21];           /* 9 of Diamonds */
    flush2[4] = deck[22];           /* 10 of Diamonds */

    /* full house */
    full_house1[0] = deck[19];      /* 7 of Diamonds */
    full_house1[1] = deck[45];      /* 7 of Spades */
    full_house1[2] = deck[6];       /* 7 of Clubs */
    full_house1[3] = deck[1];       /* 2 of Clubs */
    full_house1[4] = deck[27];      /* 2 of Hearts */

    full_house2[0] = deck[22];      /* 10 of Diamonds */
    full_house2[1] = deck[9];       /* 10 of Clubs */
    full_house2[2] = deck[35];      /* 10 of Hearts */
    full_house2[3] = deck[6];       /* 7 of Clubs */
    full_house2[4] = deck[19];      /* 7 of Diamonds */

    /* four of a kind */
    four_kind1[0] = deck[24];       /* Queen of Diamonds */
    four_kind1[1] = deck[37];       /* Queen of Hearts */
    four_kind1[2] = deck[50];       /* Queen of Spades */
    four_kind1[3] = deck[11];       /* Queen of Clubs */
    four_kind1[4] = deck[43];       /* 5 of Spades */

    four_kind2[0] = deck[21];       /* 9 of Diamonds */
    four_kind2[1] = deck[34];       /* 9 of Hearts */
    four_kind2[2] = deck[47];       /* 9 of Spades */
    four_kind2[3] = deck[8];        /* 9 of Clubs */
    four_kind2[4] = deck[19];       /* 7 of Diamonds */

    /* straight flush */
    straight_flush1[0] = deck[43];  /* 5 of Spades */
    straight_flush1[1] = deck[44];  /* 6 of Spades */
    straight_flush1[2] = deck[45];  /* 7 of Spades */
    straight_flush1[3] = deck[46];  /* 8 of Spades */
    straight_flush1[4] = deck[47];  /* 9 of Spades */

    straight_flush2[0] = deck[5];   /* 6 of Clubs */
    straight_flush2[1] = deck[6];   /* 7 of Clubs */
    straight_flush2[2] = deck[7];   /* 8 of Clubs */
    straight_flush2[3] = deck[8];   /* 9 of Clubs */
    straight_flush2[4] = deck[9];   /* 10 of Clubs */

    /* royal flush */
    royal_flush1[0] = deck[35];     /* 10 of Hearts */
    royal_flush1[1] = deck[36];     /* Jack of Hearts */
    royal_flush1[2] = deck[37];     /* Queen of Hearts */
    royal_flush1[3] = deck[38];     /* King of Hearts */
    royal_flush1[4] = deck[26];     /* Ace of Hearts */

    royal_flush2[0] = deck[9];      /* 10 of Clubs */
    royal_flush2[1] = deck[10];     /* Jack of Clubs */
    royal_flush2[2] = deck[11];     /* Queen of Clubs */
    royal_flush2[3] = deck[12];     /* King of Clubs */
    royal_flush2[4] = deck[0];      /* Ace of Clubs */

    /* print each of the above hands and pass them to rank_hand() 
     * A star is printed next to the one that's greater, or both if they're equal */
    printf("1: ");
    print_hand(high_card1);
    if (handcmp(high_card1, high_card2) > -1) 
        printf(" *");
    puts("");
    printf("2: ");
    print_hand(high_card2);
    if (handcmp(high_card2, high_card1) > -1)
        printf(" *");
    puts("");
    puts("-----");
    
    printf("1: ");
    print_hand(one_pair1);
    if (handcmp(one_pair1, one_pair2) > -1)
        printf(" *");
    puts("");
    printf("2: ");
    print_hand(one_pair2);
    if (handcmp(one_pair2, one_pair1) > -1)
        printf(" *");
    puts("");
    puts("-----");

    printf("1: ");
    print_hand(two_pair1);
    if (handcmp(two_pair1, two_pair2) > -1)
        printf(" *");
    puts("");
    printf("2: ");
    print_hand(two_pair2);
    if (handcmp(two_pair2, two_pair1) > -1)
        printf(" *");
    puts("");
    puts("-----");
    
    printf("1: ");
    print_hand(three_kind1);
    if (handcmp(three_kind1, three_kind2) > -1)
        printf(" *");
    puts("");
    printf("2: ");
    print_hand(three_kind2);
    if (handcmp(three_kind2, three_kind1) > -1)
        printf(" *");
    puts("");
    puts("-----");

    printf("1: ");
    print_hand(straight1);
    if (handcmp(straight1, straight2) > -1)
        printf(" *");
    puts("");
    printf("2: ");
    print_hand(straight2);
    if (handcmp(straight2, straight1) > -1)
        printf(" *");
    puts("");
    puts("-----");

    printf("1: ");
    print_hand(flush1);
    if (handcmp(flush1, flush2) > -1)
        printf(" *");
    puts("");
    printf("2: ");
    print_hand(flush2);
    if (handcmp(flush2, flush1) > -1)
        printf(" *");
    puts("");
    puts("-----");

    printf("1: ");
    print_hand(full_house1);
    if (handcmp(full_house1, full_house2) > -1)
        printf(" *");
    puts("");
    printf("2: ");
    print_hand(full_house2);
    if (handcmp(full_house2, full_house1) > -1)
        printf(" *");
    puts("");
    puts("-----");

    printf("1: ");
    print_hand(four_kind1);
    if (handcmp(four_kind1, four_kind2) > -1)
        printf(" *");
    puts("");
    printf("2: ");
    print_hand(four_kind2);
    if (handcmp(four_kind2, four_kind1) > -1)
        printf(" *");
    puts("");
    puts("-----");

    printf("1: ");
    print_hand(straight_flush1);
    if (handcmp(straight_flush1, straight_flush2) > -1)
        printf(" *");
    puts("");
    printf("2: ");
    print_hand(straight_flush2);
    if (handcmp(straight_flush2, straight_flush1) > -1)
        printf(" *");
    puts("");
    puts("-----");

    printf("1: ");
    print_hand(royal_flush1);
    if (handcmp(royal_flush1, royal_flush2) > -1)
        printf(" *");
    puts("");
    printf("2: ");
    print_hand(royal_flush2);
    if (handcmp(royal_flush2, royal_flush1) > -1)
        printf(" *");
    puts("");

    /* testing comparations using simple ranks */
    printf("\n    Royal Flush ");
    if (handcmp(royal_flush1, high_card1) > -1)
        putchar('>');
    else
        putchar('<');
    puts(" High Card");

    printf("       One Pair ");
    if (handcmp(one_pair1, straight_flush1) > -1)
        putchar('>');
    else
        putchar('<');
    puts(" Straight Flush");

    printf(" Four of a Kind ");
    if (handcmp(four_kind1, two_pair1) > -1)
        putchar('>');
    else
        putchar('<');
    puts(" Two Pair");

    printf("Three of a Kind ");
    if (handcmp(three_kind1, full_house1) > -1)
        putchar('>');
    else
        putchar('<');
    puts(" Full House");

    printf("          Flush ");
    if (handcmp(flush1, straight1) > -1)
        putchar('>');
    else
        putchar('<');
    puts(" Straight");

    /* and now, benchmarking! */
    start_time = get_time();
    for (i = 0; i < 5000000; i++) {
        rank_hand(royal_flush1);
    }
    printf("\nProcessed 5,000,000 Royal Flushes in %f\n", get_time() - start_time);

    start_time = get_time();
    for (i = 0; i < 5000000; i++) {
        handcmp(one_pair1, one_pair2);
    }
    printf("Compared 5,000,000 sets of One Pair in %f\n", get_time() - start_time);

    return 0;
}


