/* Copyright 2011 Tom Jollans <t@jollybox.de>
 * This code is under the Chicken Dance License v0.1 */

#ifndef IRC_H
#define IRC_H

#include <libircclient/libircclient.h>

#include "game.h"

const char *get_irc_nick (irc_session_t *session);


extern
struct channel_game {
    irc_session_t *session;
    char *channel;
    game_tp game;
    struct channel_game *next;
} 
    * channel_games_list;

game_tp get_channel_game (irc_session_t *session, const char *channel);

#endif

