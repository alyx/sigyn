#ifndef SIGYN_ME_H
#define SIGYN_ME_H
#include "mowgli.h"
#include "irc.h"
#include "platform.h"

struct me {
    irc_user_t *client;
    mowgli_config_file_t * config;
    mowgli_eventloop_t *ev;
    struct uplink {
        bool connected;
        mowgli_linebuf_t * line;
        char *port;
        char *hostname;
        bool ssl;
        char *vhost;
    } uplink;
    struct stats {
        time_t start;
        int inB;
        int outB;
        int channels;
    } stats;
    struct isupport {
        char * casemapping;
        struct len {
            size_t channel;
            size_t nick;
            size_t topic;
        } len;
    } isupport;
    int maxfd;
    int debug;
} me;

#endif
