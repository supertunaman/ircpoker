/* Copyright 2011 Tom Jollans <t@jollybox.de>
 * This code is under the Chicken Dance License v0.1 */

#ifndef COMMAND_H
#define COMMAND_H

#include "irc.h"

char *first_word (const char *txt);

void process_cmd (irc_session_t *session, const char *from,
                  const char *channel, char *cmd);

void process_bet_cmd (irc_session_t *session, const char *from,
                      const char *channel, game_tp game, char *cmd);

void list_game_info (irc_session_t *session, game_tp game, const char *dest);

#endif

