#include <stdio.h>
#include "deck.h"
#include "game.h"
#include "hand.h"

void deal_cards()
{
    int i;
    for (i=0; i<10; i++)
    {
        deal(&players[i], i);
    }
}

void print_community()
{
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
        case FOURKIND:
            puts("Four of a Kind");
            break;
        case FULLHOUSE:
            puts("Full House");
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
        if (i != 9)
            puts("---");
    }
}

int main()
{
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

    return 0;
}
