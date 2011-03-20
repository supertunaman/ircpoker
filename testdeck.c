#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "deck.h"
#include "game.h"
#include "hand.h"

void deal_cards()
{
    int i;
    for (i=0; i<10; i++)
    {
        deal(i);
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
        switch (community[i].suit) {
            case CLUBS:
                printf("%d of Clubs", community[i].value);
                break;
            case DIAMONDS:
                printf("%d of Diamonds", community[i].value);
                break;
            case HEARTS:
                printf("%d of Hearts", community[i].value);
                break;
            case SPADES:
                printf("%d of Spades", community[i].value);
                break;
        }
        if (i < 4)
            printf(", ");
        else
            puts("");
    }
    
    printf("Community cards form a ");
    switch (rank_hand(community)) {
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
        switch (deck[i].suit) {
            case CLUBS:
                printf("%d of Clubs\n", deck[i].value);
                break;
            case DIAMONDS:
                printf("%d of Diamonds\n", deck[i].value);
                break;
            case HEARTS:
                printf("%d of Hearts\n", deck[i].value);
                break;
            case SPADES:
                printf("%d of Spades\n", deck[i].value);
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
            switch (players[i].hand[c].suit) {
                case CLUBS:
                    printf("%02d of Clubs ", players[i].hand[c].value);
                    break;
                case DIAMONDS:
                    printf("%02d of Diamonds ", players[i].hand[c].value);
                    break;
                case HEARTS:
                    printf("%02d of Hearts ", players[i].hand[c].value);
                    break;
                case SPADES:
                    printf("%02d of Spades ", players[i].hand[c].value);
                    break;
            }
            if (c < 1) { printf("and "); }
        }
        printf("\n");
        get_best_player_hand(i);
        switch (rank_hand(players[i].best_hand)) {
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
    
    for (i = 0; i < 10; i++) {
        players[i].active = 1;
        players[i].folded = 0;
    }

    init_deck();
    print_deck();
    puts("*** AFTER SHUFFLING ***");
    shuffle_deck();
    print_deck();
    puts("*** COMMUNITY CARDS ***");
    deal_community();
    print_community();
    puts("*** DEALING CARDS ***");
    deal_cards();
    print_hands();

    puts("*** SORTING PLAYERS ***");
    player_sort();
    puts("In order of card ranking (low to high):");
    for (i = 0; i < 10; i++)
        printf("Player %d\n", player_ranks[i]);

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
