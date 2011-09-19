#include "sigyn.h"

mowgli_heap_t *recvq_heap;
mowgli_list_t recvq;

mowgli_heap_t *sendq_heap;
mowgli_list_t sendq;

void queue_init(void)
{
    recvq_heap = mowgli_heap_create(sizeof(queue_t), 1024, BH_NOW);

    if (!recvq_heap)
        sigyn_fatal("queue_init(): block allocator failed.");

    sendq_heap = mowgli_heap_create(sizeof(queue_t), 256, BH_NOW);

    if (!sendq_heap)
        sigyn_fatal("queue_init(): block allocator failed.");
}

queue_t *recvq_add(socket_t sock, char *string, bool complete)
{
    mowgli_node_t *n;
    queue_t *q, *tail;
    char tmp[1024];

    if (strlen(string) == 0)
        return NULL;

    n = recvq.tail;
    if (n != NULL)
    {
        tail = (queue_t *)n->data;
        if (tail->completed == false)
        {
            snprintf(tmp, BUFSIZE, "%s%s", tail->string, string);
            tail->string = strdup(tmp);
            tail->completed = complete;
            return tail;
        }
    }

    q = mowgli_heap_alloc(sendq_heap);
    q->sock = sock;
    q->string = strdup(string);
    if (complete)
    {
        q->len = strlen(q->string);
        q->completed = true;
    }
    else
        q->completed = false;
    mowgli_node_add(q, mowgli_node_create(), &recvq);
    return q;
}

void recvq_dump(socket_t sock)
{
    mowgli_node_t *n, *tn;
    queue_t *q;

    MOWGLI_LIST_FOREACH_SAFE(n, tn, recvq.head)
    {
        q = (queue_t *)n->data;

        if ((q->sock == sock) && (q->completed == true))
        {
            printf(">> %s\n", q->string);
            parse(q->string);
            mowgli_node_delete(n, &recvq);
        }
    }
}

void sendq_add(socket_t sock, char *string, size_t len)
{
    queue_t *q;
    q = mowgli_heap_alloc(sendq_heap);
    q->len = len;
    q->sock = sock;
    q->string = strdup(string);
    q->completed = true;
    mowgli_node_add(q, mowgli_node_create(), &sendq);
}

void sendq_dump(socket_t sock)
{
    mowgli_node_t *n, *tn;
    queue_t *q;

    MOWGLI_LIST_FOREACH_SAFE(n, tn, sendq.head)
    {
        q = (queue_t *)n->data;
        if (q->sock == sock)
        {
            me.stats.outB += write(q->sock, q->string, q->len);
            mowgli_node_delete(n, &sendq);
        }
    }
}

void sendq_flush(socket_t sock)
{
    mowgli_node_t *n, *tn;
    queue_t *q;

    MOWGLI_LIST_FOREACH_SAFE(n, tn, sendq.head)
    {
        q = (queue_t *)n->data;
        if (q->sock == sock)
            mowgli_node_delete(n, &sendq);
    }
}
