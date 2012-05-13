/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alyx Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#ifndef __SIGYN_TOOLS_H
#define __SIGYN_TOOLS_H

#include "mowgli.h"

/* config.c functions */
extern void config_check(mowgli_config_file_t * config);
extern mowgli_config_file_entry_t * config_fatal_find_entry(
        mowgli_config_file_entry_t * entries, char * name);
extern mowgli_config_file_entry_t * config_find_entry(mowgli_config_file_entry_t * start,
        const char * name);
extern void config_print(mowgli_config_file_entry_t * entries, int level);
/* string.c functions */
extern void strip(char *line, char *strippers);
extern const char *strip_colour_codes(const char *message);

/* glob.c function */
bool glob(const char * match, const char * string);

#define INIT_PARV(x, y) \
int __i; \
for(__i = 0; __i < y; ++__i) \
  x[__i] = '\0';
  
/* parse.c functions */
extern irc_user_t *parse_user(char hostmask[]);
extern void parse(char line[]);
extern void preparse(char line[]);

/* sigyn.c functions */
extern void initialise_sigyn(char *nick, char *ident, char *gecos, 
        char *uplink, uint16_t port, char *vhost);
extern void sigyn_fatal(char *format, ...);
extern void sigyn_cleanup(void);

/* connect.c functions */
extern int StartWSA(void);
extern int sigyn_hostname(char *host, int len);
extern socket_t uplink_connect(char *uplink, uint16_t port, char *vhost);
extern void uplink_disconnect(void);

/* signal.c functions */
extern void signals_init(void);

/* stack.c functions */
#ifdef ENABLE_STACK
extern void sigyn_log_backtrace(void);
#endif

/* tokenize.c functions */
extern int tokenize(char *message, char **parv);

#endif /* __SIGYN_TOOLS_H */
