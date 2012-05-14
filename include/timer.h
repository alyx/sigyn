/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alyx Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#ifndef SIGYN_TIMER_H
#define SIGYN_TIMER_H

#include "mowgli.h"

#define timer_del(x) mowgli_timer_destroy(me.ev, x);

static inline mowgli_eventloop_timer_t * timer_add(const char * name, mowgli_event_dispatch_func_t * func, void * arg, time_t when, bool repeat)
{
    if (repeat)
        return mowgli_timer_add(me.ev, name, func, arg, when);
    else
        return mowgli_timer_add_once(me.ev, name, func, arg, when);
}

#endif /* SIGYN_TIMER_H */
