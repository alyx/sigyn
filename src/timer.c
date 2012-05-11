/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"

mowgli_heap_t *timer_heap;
mowgli_list_t timers;

static time_t next_event;


void timer_init(void)
{
    timer_heap = mowgli_heap_create(sizeof(timer_event_t), 1024, BH_NOW);

    if (!timer_heap)
        sigyn_fatal("timer_init(): block allocator failed.");
}

/*
 * Routine Description:
 * This routine provides a function to create a new timed event, appending it to the event queue.
 *
 * Arguments:
 *     name     - A string specifying the name of the event.
 *     function - A pointer to the function to call when the timer is activated.
 *     arg      - A pointer to the argument sent to the function.
 *     wait     - The amount of time to wait before calling the timer.
 *
 * Return:
 *     runtime  - A time_t object containing the time the event will run.
 */

time_t timer_add(const char *name, timer_function_t function, void *arg, 
        unsigned int wait)
{
    time_t runtime;
    timer_event_t *event;

    runtime = time(NULL);
    runtime += wait;

    event = mowgli_heap_alloc(timer_heap);
    event->name = mowgli_strdup(name);
    event->function = function;
    event->arguments = arg;
    event->wait = wait;
    event->time = runtime;

    mowgli_node_add(event, mowgli_node_create(), &timers);

    if ((runtime < next_event) || (next_event == 0))
        next_event = runtime;

    return runtime;
}

void timer_del(const char *name, void *function)
{
    timer_event_t *event;
    mowgli_node_t *n, *tn;

    MOWGLI_ITER_FOREACH_SAFE(n, tn, timers.head)
    {
        event = (timer_event_t *)n->data;
        if ((event->function == (timer_function_t)function) && ((strcasecmp(event->name, name)) == 0))
        {
            mowgli_free(event->name);
            mowgli_node_delete(n, &timers);
            mowgli_heap_free(timer_heap, event);
        }
    }
}

void run_timers(time_t currtime)
{
    bool repeat;
    timer_event_t *event;
    mowgli_node_t *n;
    MOWGLI_ITER_FOREACH(n, timers.head)
    {
        event = (timer_event_t *)n->data;


        if (next_event == event->time)
            next_event = 0;


        if (currtime >= event->time)
        {
            repeat = event->function(event->arguments);
            if (repeat == false)
                timer_del(event->name, event->function);
            else
                event->time = (time(NULL) + event->wait);
        }
        else
        {
            if ((next_event > event->time) || (next_event == 0))
                next_event = event->time;
        }
    }
}

time_t get_next_timer(void)
{
    if (next_event >= time(NULL))
        return next_event;

    return 0;
}
