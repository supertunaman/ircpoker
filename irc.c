/* Copyright 2011 Tom Jollans <t@jollybox.de>
 * This code is under the Chicken Dance License v0.1 */

#include "command.h"

#include <libircclient/libircclient.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void on_connect (irc_session_t *session, const char *event, const char *origin, const char **params, unsigned count);
void on_privmsg (irc_session_t *session, const char *event, const char *origin, const char **params, unsigned count);
void on_chanmsg (irc_session_t *session, const char *event, const char *origin, const char **params, unsigned count);
void on_invite  (irc_session_t *session, const char *event, const char *origin, const char **params, unsigned count);

void on_generic (irc_session_t *session, const char *event, const char *origin, const char **params, unsigned count);
void on_numeric (irc_session_t *session, unsigned event, const char *origin, const char **params, unsigned count);

static char *irc_nick;
struct channel_game *channel_games_list = NULL;

/* might want different nicks on different servers some day. */
const char *
get_irc_nick (irc_session_t *session)
{
    return irc_nick;
}

int
main (int argc, char **argv)
{
    /* Basic single-server single-thread IRC bot. */
    irc_callbacks_t callbacks = {
        .event_nick = &on_generic,
        .event_quit = &on_generic,
        .event_join = &on_generic,
        .event_part = &on_generic,
        .event_mode = &on_generic,
        .event_umode = &on_generic,
        .event_topic = &on_generic,
        .event_kick = &on_generic,
        .event_notice = &on_generic,
        /* .event_channel_notice = &on_generic, */
        .event_ctcp_req = &on_generic,
        .event_ctcp_rep = &on_generic,
        .event_ctcp_action = &on_generic,
        .event_unknown = &on_generic,
        .event_numeric = &on_numeric,

        .event_connect = &on_connect,
        .event_privmsg = &on_privmsg,
        .event_channel = &on_chanmsg,
        .event_invite  = &on_invite
    };

    irc_session_t *session;
    char *server, *nick;
    int port;

    if (argc != 4) {
        fprintf(stderr, "usage: ircpoker [server] [port] [nick]\n");
        return 2;
    }
    /* TODO: configuration file */
    server = argv[1];
    port = atoi(argv[2]); /* TODO: value checking */
    nick = argv[3];
    irc_nick = strdup(nick);

    session = irc_create_session(&callbacks);
    if (!session) {
        fprintf(stderr, "ERROR creating session handle. Quitting.\n");
        return 1;
    }

    printf("nick: '%s'\n", nick);
    if (irc_connect(session, server, port, NULL,
                    nick, nick, "IRC Poker")
            != 0) {
        fprintf(stderr, "ERROR connecting to server %s on port %d: %s\n",
                        server, port, irc_strerror(irc_errno(session)));
        return 1;
    }

    /* error checking might be nice */
    irc_run(session);
    irc_destroy_session(session);
    return 0;
}

void
on_connect (irc_session_t *session, const char *event,
            const char *origin, const char **params, unsigned count)
{
    printf("*** CONNECTION ESTABLISHED. ***\n");
}

void
on_privmsg (irc_session_t *session, const char *event,
            const char *origin, const char **params, unsigned count)
{
    const char *dest = params[0];
    /* process_cmd needs to be able to modify the cmd string */
    char *msg = strdup(params[1]);

    printf("privmsg: %s --> %s: \"%s\"\n", origin, dest, msg);

    /* private messages are always seen as commands. */
    process_cmd(session, origin, NULL, msg);
    free(msg);
}

void
on_chanmsg (irc_session_t *session, const char *event,
            const char *origin, const char **params, unsigned count)
{
    int len;
    const char *dest = params[0];
    /* process_cmd needs to be able to modify the cmd string */
    char *msg = strdup(params[1]);
    char *firstword;

    if (strcmp(dest, get_irc_nick(session)) == 0) {
        /* the lib can get confused when the server silently changes the nick) */
        on_privmsg(session, event, origin, params, count);
        free(msg);
        return;
    }

    printf("%s in %s: \"%s\"\n", origin, dest, msg);

    /* treat messages prefixed with my name as commands: */
    firstword = first_word(msg);
    len = strlen(firstword);
    /* name may be postfixed with colon or comma */
    switch(firstword[len-1]) {
        case ':':
        case ',':
            firstword[len-1] = '\0';
    }
    if (strcmp(firstword, get_irc_nick(session)) == 0) {
        process_cmd(session, origin, dest, &msg[len+1]);
    } else {
        /* is there a game on the channel? If so, check all messages for 
         * betting-round commands */
        game_tp game;
        
        if ((game = get_channel_game(session, dest))) {
            process_bet_cmd(session, origin, dest, game, msg);
        }
    }
    free(firstword);


    free(msg);
}

void
on_invite (irc_session_t *session, const char *event,
            const char *origin, const char **params, unsigned count)
{
    /* possibly check if this is me? probably not necessary. */
    const char *invitee = params[0];
    const char *channel = params[1];

    printf("invitation for %s to join %s, from %s. Vamos!\n", invitee, channel, origin);
    irc_cmd_join(session, channel, NULL);
}


void
on_generic (irc_session_t *session, const char *event, const char *origin, const char **params, unsigned count)
{
    int i;

    printf("-- [%s] -- (%s) -- ", event, origin);
    for (i=0; i<count; ++i)
        printf("%s -- ", params[i]);
    printf("\n");
}

void
on_numeric (irc_session_t *session, unsigned event, const char *origin, const char **params, unsigned count)
{
    int i;

    printf("-- [%d] -- (%s) -- ", event, origin);
    for (i=0; i<count; ++i)
        printf("%s -- ", params[i]);
    printf("\n");

    if (event == LIBIRC_RFC_RPL_WELCOME && count == 2) {
        /* get the actual nick (in case the server changed it) */
        char *old_irc_nick = irc_nick;
        irc_nick = strdup(params[0]);
        free(old_irc_nick);
    }
}

game_tp
get_channel_game (irc_session_t *session, const char *channel)
{
    struct channel_game *cg;

    if (!channel) return NULL;
    for (cg = channel_games_list; cg; cg = cg->next)
        if (cg->session == session && !strcasecmp(cg->channel, channel))
            return cg->game;

    return NULL;
}

