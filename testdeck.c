/* Copyright 2011 Tuna <tuna@supertunaman.com
 * This code is under the Chicken Dance License v0.1 */
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "deck.h"
#include "game.h"
#include "hand.h"

static game_tp testgame;

void deal_cards()
{
    int i;
    for (i=0; i<10; i++)
    {
        deal(testgame, i);
    }
}

double get_time()
{
    /* used for benchmarking */
    struct timeval t;
    struct timezone tz;
    gettimeofday(&t, &tz);
    return t.tv_sec + t.tv_usec*1e-6;
}

void print_community()
{
    /* pretty prints the community hand */
    int i;

    printf("Community has: ");
    for (i = 0; i < 5; i++)
    {
        switch (testgame->community[i].suit) {
            case CLUBS:
                printf("%d of Clubs", testgame->community[i].value);
                break;
            case DIAMONDS:
                printf("%d of Diamonds", testgame->community[i].value);
                break;
            case HEARTS:
                printf("%d of Hearts", testgame->community[i].value);
                break;
            case SPADES:
                printf("%d of Spades", testgame->community[i].value);
                break;
        }
        if (i < 4)
            printf(", ");
        else
            puts("");
    }
    
    printf("Community cards form a ");
    switch (rank_hand(testgame->community)) {
        case ROYALFLUSH:
            puts("Royal Flush");
            break;
        case STRAIGHTFLUSH:
            puts("Straight Flush");
            break;
        case FOURKIND:
            puts("Four of a Kind");
            break;
        case FULLHOUSE:
            puts("Full House");
            break;
        case FLUSH:
            puts("Flush");
            break;
        case STRAIGHT:
            puts("Straight");
            break;
        case THREEKIND:
            puts("Three of a Kind");
            break;
        case TWOPAIR:
            puts("Two Pair");
            break;
        case ONEPAIR:
            puts("One Pair");
            break;
        case HIGHCARD:
            puts("High Card");
            break;
    }
}

void print_deck()
{
    /* pretty prints the entire deck */
    int i;
    for (i=0; i<52; i++)
    {
        switch (testgame->deck[i].suit) {
            case CLUBS:
                printf("%d of Clubs\n", testgame->deck[i].value);
                break;
            case DIAMONDS:
                printf("%d of Diamonds\n", testgame->deck[i].value);
                break;
            case HEARTS:
                printf("%d of Hearts\n", testgame->deck[i].value);
                break;
            case SPADES:
                printf("%d of Spades\n", testgame->deck[i].value);
                break;
        }
    }
}

void print_hands()
{
    /* prints player hands and their ranks nicely */
    int i;
    int c;
    for (i = 0; i < 10; i++)
    {
        printf("Player %d has ", i);
        for (c = 0; c < 2; c++)
        {
            switch (testgame->players[i].hand[c].suit) {
                case CLUBS:
                    printf("%02d of Clubs ", testgame->players[i].hand[c].value);
                    break;
                case DIAMONDS:
                    printf("%02d of Diamonds ", testgame->players[i].hand[c].value);
                    break;
                case HEARTS:
                    printf("%02d of Hearts ", testgame->players[i].hand[c].value);
                    break;
                case SPADES:
                    printf("%02d of Spades ", testgame->players[i].hand[c].value);
                    break;
            }
            if (c < 1) { printf("and "); }
        }
        printf("\n");
        get_best_player_hand(testgame, i);
        switch (rank_hand(testgame->players[i].best_hand)) {
            case ROYALFLUSH:
                puts("...Royal Flush");
                break;
            case STRAIGHTFLUSH:
                puts("...Straight Flush");
                break;
            case FOURKIND:
                puts("...Four of a Kind");
                break;
            case FULLHOUSE:
                puts("...Full House");
                break;
            case FLUSH:
                puts("...Flush");
                break;
            case STRAIGHT:
                puts("...Straight");
                break;
            case THREEKIND:
                puts("...Three of a Kind");
                break;
            case TWOPAIR:
                puts("...Two Pair");
                break;
            case ONEPAIR:
                puts("...One Pair");
                break;
            case HIGHCARD:
                puts("...High Card");
                break;
        }
    }
}

int main()
{
    double start_time[10];
    int i, j;
    int sorted_players[10];
    
    testgame = new_game(10);
    for (i = 0; i < 10; i++) {
        testgame->players[i].active = 1;
    }
    print_deck();
    puts("*** AFTER SHUFFLING ***");
    shuffle_deck(testgame);
    print_deck();
    puts("*** COMMUNITY CARDS ***");
    deal_community(testgame);
    print_community();
    puts("*** DEALING CARDS ***");
    deal_cards();
    print_hands();

    puts("*** SORTING PLAYERS ***");
    get_player_ranks(testgame, sorted_players);
    puts("In order of card ranking (low to high):");
    for (i = 0; i < 10; i++)
        printf("Player %d\n", sorted_players[i]);

    /*
    puts("*** BENCHMARKING ***");
    start_time[0] = get_time();
    for (i = 0; i < 5000; i++)
        shuffle_deck();
    printf("Shuffled deck 5,000 times in %f\n", get_time() - start_time[0]);

    for (i = 0; i < 10; i++) {
        start_time[i] = get_time();
        for (j = 0; j < 100000; j++)
            get_best_player_hand(i);
        printf("Calculated best hand for Player %d 100,000 times in %f\n", i, get_time() - start_time[i]);
    }
    */

    return 0;
}
