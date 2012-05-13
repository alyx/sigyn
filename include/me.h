#ifndef SIGYN_ME_H
#define SIGYN_ME_H
#include "mowgli.h"
#include "irc.h"
#include "platform.h"

struct me {
    irc_user_t *client;
    mowgli_config_file_t * config;
    struct uplink {
        bool connected;
        mowgli_linebuf_t * line;
        mowgli_eventloop_t *ev;
        char *port;
        char *hostname;
        char *vhost;
    } uplink;
    struct stats {
        time_t start;
        int inB;
        int outB;
        int channels;
    } stats;
    int maxfd;
} me;

#endif
