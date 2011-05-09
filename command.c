/* Copyright 2011 Tom Jollans <t@jollybox.de>
 * This code is under the Chicken Dance License v0.1 */

#include "command.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *
first_word (const char *txt)
{
    int i;

    for (i=0; txt[i] && !isspace(txt[i]); ++i);

    return strndup(txt, i);
}

void
separate_first_word (char **txt, char **restp)
{
    char *p;

    while (isspace(**txt)) (*txt)++;
    for (p = *txt; *p && !isspace(*p); ++p);
    
    if (*p && *(p+1))
        *restp = p+1;
    else /* end of string */
        *restp = NULL;

    *p = '\0';
}

void
process_cmd (irc_session_t *session,
             const char *from, const char *channel,
             char *cmd)
{
    char *rest;
    char from_nick[32];
    
    irc_target_get_nick(from, from_nick, 32);

    separate_first_word(&cmd, &rest);
    
    if (strcasecmp(cmd, "quit") == 0) {
        irc_cmd_msg(session, channel ? channel : from_nick, "Live long and prosper. Good-bye!");
        irc_cmd_quit(session, "Fold.");
    } else {
        char *resp;
        if (asprintf(&resp, "No such command: %s.", cmd) != -1) {
            irc_cmd_msg(session, channel ? channel : from_nick, resp);
            free(resp);
        }
    }
}

