/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */
#ifndef __SIGYN_PARSER_H
#define __SIGYN_PARSER_H
//#pragma once

#include "irc.h"

extern irc_user_t *parse_user(char *hostmask);
extern irc_event_t *parse(char *text);
#endif
