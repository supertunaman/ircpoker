#include "hand.h"

int playercmp(const void * player1, const void * player2)
{
    if (players[*(int *)player1].active == 0 ||
            players[*(int *)player1].folded == 1)
        return -1;
    if (players[*(int *)player2].active == 0 ||
            players[*(int *)player2].folded == 1)
        return 1;

    get_best_player_hand(*(int *)player1);
    get_best_player_hand(*(int *)player2);

    return handcmp( players[ *(int *)player1 ].best_hand,
                    players[ *(int *)player2 ].best_hand);
}

void player_sort()
{
    int i;

    for (i = 0; i < 10; i++)
        player_ranks[i] = i;

    qsort(player_ranks, 10, sizeof(int), playercmp);
}

void get_best_player_hand(player_id)
{
    card_t last_hand[5];
    card_t cur_hand[5];
    int i;
    int j;
    int k;

    /* compare to the community cards first */
    for (i = 0; i < 5; i++)
        last_hand[i] = community[i];

    /* replace two of the community cards at a time with the pocket cards */
    for (i = 0; i < 5; i++) {
         for (j = 0; j < i; j++) {
             for (k = 0; k < 5; k++)
                 cur_hand[k] = community[k];
             cur_hand[i] = players[player_id].hand[0];
             cur_hand[j] = players[player_id].hand[1];

             /* if cur_hand beats last_hand, set last_hand's values to cur_hand's */
             if (handcmp(cur_hand, last_hand) > 0)
                 for (k = 0; k < 5; k++)
                     last_hand[k] = cur_hand[k];
         }
    }

    /* now replace one card at a time and compare */
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 5; j++)
            cur_hand[j] = community[j];
        cur_hand[i] = players[player_id].hand[0];

        if (handcmp(cur_hand, last_hand) > 0)
            for (j = 0; j < 5; j++)
                last_hand[j] = cur_hand[j];

        for (j = 0; j < 5; j++)
            cur_hand[j] = community[j];
        cur_hand[i] = players[player_id].hand[1];

        if (handcmp(cur_hand, last_hand) > 0)
            for (j = 0; j < 5; j++)
                last_hand[j] = cur_hand[j];
    }

    /* by now, last_hand should be the best possible hand */
    for (i = 0; i < 5; i++)
        players[player_id].best_hand[i] = last_hand[i];
}

int handcmp(card_t hand1[], card_t hand2[])
{
    ranks_t rank1 = rank_hand(hand1);
    ranks_t rank2 = rank_hand(hand2);
    int handvals1[5];
    int handvals2[5];
    int i;
    int first_pair1 = 0;
    int first_pair2 = 0;
    int second_pair1 = 0;
    int second_pair2 = 0;
    int third_card1;
    int third_card2;
    int fourth_card1;
    int fourth_card2;
    int fifth_card1;
    int fifth_card2;
    int full_of1;
    int full_of2;
    int histogram1[15];
    int histogram2[15];
    int tmp;

    /* set all histogram values to 0 */
    for (i = 0; i < 15; i++) { histogram1[i] = 0; }
    for (i = 0; i < 15; i++) { histogram2[i] = 0; }
    
    /* put values of cards into handvals arrays and sort */
    for (i = 0; i < 5; i++) {
        handvals1[i] = hand1[i].value;
        handvals2[i] = hand2[i].value;
    }
    sort(handvals1, 5);
    sort(handvals2, 5);

    /* return result if ranks differ */
    if (rank1 < rank2)
        return -1;
    if (rank1 > rank2)
        return 1;

    /* Ranks are the same, so now we get to work */
    switch (rank1)
    {
        case ROYALFLUSH:
            return 0;   /* A royal flush is a royal flush */

        /* same process for straight and straight flush */
        case STRAIGHT:
        case STRAIGHTFLUSH:
            /* compare the last (highest) element of each array */
            if (handvals1[4] < handvals2[4])
                return -1;
            if (handvals1[4] > handvals2[4])
                return 1;
            return 0;

        /* test which hand has the higher card(s) */
        case FLUSH:
        case HIGHCARD:
            promote_aces(handvals1);    /* turn all values of 1 to 14 */
            promote_aces(handvals2);

            /* loop down both lists and try to find a higher card */
            for (i = 4; i >= 0; i--) {
                if (handvals1[i] < handvals2[i])
                    return -1;
                if (handvals1[i] > handvals2[i])
                    return 1;
            }
            return 0;

        case FOURKIND:
            promote_aces(handvals1);
            promote_aces(handvals2);

            /* find out which majority is greater */
            if (handvals1[2] < handvals2[2])
                return -1;
            if (handvals1[2] > handvals2[2])
                return 1;

            /* if the four of a kind is all in the community, test fifth card */
            /* the fifth card will either be [0] or [4] after sorting */
            if (handvals1[0] != handvals1[2])
                fifth_card1 = handvals1[0];
            else
                fifth_card1 = handvals1[4];

            if (handvals2[0] != handvals2[2])
                fifth_card2 = handvals2[0];
            else
                fifth_card2 = handvals2[4];

            if (fifth_card1 < fifth_card2)
                return -1;
            if (fifth_card1 > fifth_card2)
                return 1;
            
            return 0;

        case FULLHOUSE:
            promote_aces(handvals1);
            promote_aces(handvals2);

            /* compare the three of a kind */
            if (handvals1[2] < handvals2[2])
                return -1;
            if (handvals1[2] > handvals2[2])
                return 1;
            
            /* find out where the "full of" pair is */
            if (handvals1[1] != handvals1[2])
                full_of1 = handvals1[1];
            else
                full_of1 = handvals1[3];
            
            if (handvals2[1] != handvals2[2])
                full_of2 = handvals2[1];
            else
                full_of2 = handvals2[3];

            /* compare the "full of" cards */
            if (full_of1 < full_of2)
                return -1;
            if (full_of1 > full_of2)
                return 1;

            return 0;

        case THREEKIND:
            promote_aces(handvals1);
            promote_aces(handvals2);

            /* compare the three of a kind */
            if (handvals1[2] < handvals2[2])
                return -1;
            if (handvals1[2] > handvals2[2])
                return 1;
            
            /* find the fourth and fifth cards and compare them */
            if (handvals1[0] == handvals1[2]) {
                fourth_card1 = handvals1[4];
                fifth_card1 = handvals1[3];
            } else {
                fifth_card1 = handvals1[0];
            }
            if (handvals1[1] == handvals1[2])
                fourth_card1 = handvals1[4];
            else
                fourth_card1 = handvals1[1];

            if (handvals2[0] == handvals2[2]) {
                fourth_card2 = handvals2[4];
                fifth_card2 = handvals2[3];
            } else {
                fifth_card2 = handvals2[0];
            }
            if (handvals2[1] == handvals2[2])
                fourth_card2 = handvals2[4];
            else
                fourth_card2 = handvals2[1];

            if (fourth_card1 < fourth_card2)
                return -1;
            if (fourth_card1 > fourth_card2)
                return 1;

            if (fifth_card1 < fifth_card2)
                return -1;
            if (fifth_card1 > fifth_card2)
                return 1;

            return 0;

        case TWOPAIR:
            promote_aces(handvals1);
            promote_aces(handvals2);

            /* put values into the histograms */
            for (i = 0; i < 5; i++) {
                histogram1[handvals1[i]]++;
                histogram2[handvals2[i]]++;
            }

            /* go through both histograms and figure out the pairs and fifth card */
            for (i = 2; i < 15; i++) {
                if (histogram1[i] == 1) 
                    fifth_card1 = i;
                if (histogram1[i] == 2) {
                    if (first_pair1)
                        second_pair1 = i;
                    else
                        first_pair1 = i;
                }
                
                if (histogram2[i] == 1) 
                    fifth_card2 = i;
                if (histogram2[i] == 2) {
                    if (first_pair2)
                        second_pair2 = i;
                    else
                        first_pair2 = i;
                }
            }

            /* make sure the first pair is higher than the second pair */
            if (first_pair1 < second_pair1) {
                tmp = first_pair1;
                first_pair1 = second_pair1;
                second_pair1 = tmp;
            }
            if (first_pair2 < second_pair2) {
                tmp = first_pair2;
                first_pair2 = second_pair2;
                second_pair2 = tmp;
            }

            /* finally compare the pairs and the fifth card */
            if (first_pair1 < first_pair2)
                return -1;
            if (first_pair1 > first_pair2)
                return 1;

            if (second_pair1 < second_pair2)
                return -1;
            if (second_pair1 > second_pair2)
                return 1;

            if (fifth_card1 < fifth_card2)
                return -1;
            if (fifth_card1 > fifth_card2)
                return 1;
            
            return 0;

        case ONEPAIR:
            promote_aces(handvals1);
            promote_aces(handvals2);

            for (i = 0; i < 5; i++) {
                histogram1[handvals1[i]]++;
                histogram2[handvals2[i]]++;
            }

            /* find both pairs and then break */
            for (i = 2; i < 15; i++) {
                if (histogram1[i] == 2) 
                    first_pair1 = i;
                if (histogram2[i] == 2)
                    first_pair2 = i;
                if (first_pair1 && first_pair2)
                    break;
            }
            
            /* compare the pairs */
            if (first_pair1 != first_pair2)
                return (first_pair1 > first_pair2);

            /* find third, fourth, and fifth cards */
            for (i = 0; i < 5; i++) {
                if (handvals1[i] != first_pair1) {
                    if (!fifth_card1)
                        fifth_card1 = handvals1[i];
                    else if (!fourth_card1)
                        fourth_card1 = handvals1[i];
                    else
                        third_card1 = handvals1[i];
                }
                if (handvals2[i] != first_pair2) {
                    if (!fifth_card2)
                        fifth_card2 = handvals2[i];
                    else if (!fourth_card2)
                        fourth_card2 = handvals2[i];
                    else
                        third_card2 = handvals2[i];
                }
            }

            /* compare third fourth and fifth cards */
            if (third_card1 < third_card2)
                return -1;
            if (third_card1 > third_card2)
                return 1;

            if (fourth_card1 < fourth_card2)
                return -1;
            if (fourth_card1 > fourth_card2)
                return 1;

            if (fifth_card1 < fifth_card2)
                return -1;
            if (fifth_card1 > fifth_card2)
                return 1;

            return 0;

        default:    /* shouldn't happen */
            return 0;
    }
}

void promote_aces(int handvals[])
{
    int i;
    
    for (i = 0; i < 5; i++) {
        if (handvals[i] == 1)
            handvals[i] = 14;
    }
    sort(handvals, 5);
}

void sort(int a[], int n)
{
    int i, j, value;
    
    for (i = 1; i < n; i++)
    {
        value = a[i];
        for (j = i - 1; j >= 0 && a[j] > value; j--) {
            a[j + 1] = a[j];
        }
        a[j + 1] = value;
    }
}

ranks_t rank_hand(card_t hand[])
{
    int handvals[5];
    int histogram[13];
    int i;
    int flush = 1;
    
    /* set all histogram values to 0 */
    for (i = 0; i < 13; i++) { histogram[i] = 0; }

    /* generate histogram of multiples */
    for (i = 0; i < 5; i++)
    {
        histogram[hand[i].value - 1]++;
    }
    sort(histogram, 13);
    
    /* evaluate sorted histogram */
    switch (histogram[12]) {
        case 4:
            return FOURKIND;
        case 3:
            if (histogram[11] == 2)
                return FULLHOUSE;
            else
                return THREEKIND;
        case 2:
            if (histogram[11] == 2)
                return TWOPAIR;
            else
                return ONEPAIR;
    }
    
    /* test for a flush */
    for (i = 1; i < 5; i++) {
        if (hand[i].suit == hand[0].suit)
            flush++;    /* will amount to 5 if it's a flush */
    }

    /* test for straight and straight flush */
    for (i = 0; i < 5; i++) { handvals[i] = hand[i].value; }
    sort(handvals, 5);

    if (handvals[4] - handvals[0] == 4 && flush == 5) 
        return STRAIGHTFLUSH;
    if (handvals[4] - handvals[0] == 4)
        return STRAIGHT;
    
    /* test for flush and royal flush */
    handvals[0] = 14;
    sort(handvals, 5);
    if (handvals[4] - handvals[0] == 4 && flush == 5)
        return ROYALFLUSH; /* OMG :O */
    if (flush == 5)
        return FLUSH;

    return HIGHCARD;
}

