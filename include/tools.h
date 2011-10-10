/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#ifndef __SIGYN_TOOLS_H
#define __SIGYN_TOOLS_H

/* config.c functions */
extern char *config_get_string(const char *section, const char *key);
extern bool config_get_bool(const char *section, const char *key);
extern int config_get_int(const char *section, const char *key);

/* string.c functions */
extern void strip(char *line, char *strippers);
#ifndef HAVE_STRLCAT
extern size_t strlcat(char *dest, const char *src, size_t count);
#endif
#ifndef HAVE_STRLCPY
extern size_t strlcpy(char *dest, const char *src, size_t count);
#endif

/* parse.c functions */
extern irc_user_t *parse_user(char hostmask[]);
extern irc_event_t *parse(char line[]);
extern void preparse(char line[]);

/* sigyn.c functions */
extern void sigyn_fatal(char *format, ...);
extern void sigyn_cleanup(void);

/* connect.c functions */
extern int StartWSA(void);
extern int sigyn_hostname(char *host, int len);
extern socket_t uplink_connect(char *uplink, uint16_t port, char *vhost);
extern void uplink_disconnect(void);

/* tokenize.c functions */
extern int tokenize(char *message, char **parv);

#endif /* __SIGYN_TOOLS_H */
