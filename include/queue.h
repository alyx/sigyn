/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */
#ifndef __SIGYN_QUEUE_H
#define __SIGYN_QUEUE_H

#include <sys/types.h>
#include "config.h"
#include "platform.h"

typedef struct queue_ queue_t;

struct queue_ {
    char *string;
    socket_t sock;
    size_t len;
    bool completed;
};

extern mowgli_list_t sendq;

extern void queue_init(void);
extern queue_t *recvq_add(socket_t sock, char *string, bool complete);
extern void recvq_dump(socket_t sock);
extern void sendq_add(socket_t sock, char *string, size_t len);
extern void sendq_dump(socket_t sock);
extern void sendq_flush(socket_t sock);

#endif /* __SIGYN_QUEUE_H */
