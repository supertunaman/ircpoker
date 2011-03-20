#include "player.h"

void bet(int player_id, int amount)
{
    if (amount >= players[player_id].chips) {
        players[player_id].allin = 1;
        players[player_id].bet = players[player_id].chips;
    } else {
        players[player_id].bet = amount;
    }
}

void fold(player_id)
{
    players[player_id].folded = 1;
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

