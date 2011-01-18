#include "hand.h"

int handcmp(card_t hand1[], card_t hand2[])
{
    ranks_t rank1 = rank_hand(hand1);
    ranks_t rank2 = rank_hand(hand2);
    int handvals1[5];
    int handvals2[5];
    int i;
    
    for (i = 0; i < 5; i++) {
        handvals1[i] = hand1[i].value;
        handvals2[i] = hand2[i].value;
    }
    sort(handvals1, 5);
    sort(handvals2, 5);

    if (rank1 != rank2)
        return (rank1 > rank2);

    switch (rank1)
    {
        case ROYALFLUSH:
            return 2;
        case STRAIGHT:
        case STRAIGHTFLUSH:
            if (handvals1[4] != handvals2[4])
                return (handvals1[4] > handvals2[4]);
            else
                return 2;
        case FLUSH:
            for (i = 0; i < 5; i++) {
                if (handvals1[i] == 1)
                    handvals1[i] = 14;
                if (handvals2[i] == 1)
                    handvals2[i] = 14;
            }
            sort(handvals1, 5);
            sort(handvals2, 5);
            for (i = 4; i >= 0; i--) {
                if (handvals1[i] != handvals2[i])
                    return (handvals1[i] > handvals2[i]);
            }
            return 2;
        default:
            return 2;
    }
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

