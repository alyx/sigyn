/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alyx Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#ifndef SIGYN_TOOLS_H
#define SIGYN_TOOLS_H

#include "mowgli.h"

/* base64.c functions */
unsigned char * base64_encode(const unsigned char *str, int length);
unsigned char * base64_decode(const unsigned char *str, int length, int *ret);

/* config.c functions */
extern void config_check(mowgli_config_file_t * config);
extern mowgli_config_file_entry_t * config_fatal_find_entry(
        mowgli_config_file_entry_t * entries, char * name);
extern mowgli_config_file_entry_t * config_find_entry(mowgli_config_file_entry_t * start,
        const char * name);
extern void config_print(mowgli_config_file_entry_t * entries, int level);
/* string.c functions */
extern void strcasecanon(char *str);
extern void strip(char *line, char *strippers);
extern void rmchar(char *line, char *delims);
extern const char *strip_colour_codes(const char *message);

/* glob.c function */
bool glob(const char * match, const char * string);

#define INIT_PARV(x, y) \
int __i; \
for(__i = 0; __i < y; ++__i) \
  x[__i] = '\0';
  
/* parse.c functions */
mowgli_patricia_t * isupport_table;

extern irc_user_t *parse_user(char hostmask[]);
extern void parse(char line[]);

/* sigyn.c functions */
extern void initialise_sigyn(char *nick, char *ident, char *gecos, 
        char *uplink, char *port, bool use_ssl, char *vhost);
extern void sigyn_fatal(char *format, ...);
extern void sigyn_cleanup(void);

/* io.c */
extern mowgli_linebuf_t * new_conn(const char * host, const char * port,
        bool use_ssl, mowgli_linebuf_readline_cb_t *cb, void * udata);

static inline int sigyn_hostname(char * host, int len)
{
    return gethostname(host, len);
}

/* signal.c functions */
extern void signals_init(void);

/* stack.c functions */
extern void sigyn_log_backtrace(void);

/* tokenize.c functions */
extern unsigned int tokenize(char *message, char **parv);

#endif /* SIGYN_TOOLS_H */
