typedef struct irc_channel_t irc_channel_t;
typedef struct irc_user_t irc_user_t;

struct irc_channel_t {
    char *name;
    char *topic;
    size_t nusers;
    char **users;

    size_t nmodes;
    struct {
        char c;
        size_t nparam;
        char **users;
    } *modes;
};

struct irc_user_t
{
    char *nick;
    char *user;
    char *host;
    char *gecos;

    size_t nchans;
    char **chans;
};


