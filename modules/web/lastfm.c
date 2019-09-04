#include "sigyn.h"
#include <curl/curl.h>

#define API_ROOT "https://ws.audioscrobbler.com/2.0/?method=user.getrecenttracks&user=%s&api_key=%s&format=json&limit=1"

static void cmd_np(const irc_event_t *event, int parc, char **parv);

size_t (*web_write_to_buffer)(void *buf, size_t size, size_t nmemb, mowgli_string_t *input_buf);

DECLARE_MODULE("web/lastfm",
               MODULE_UNLOAD_CAPABILITY_OK,
               _modinit,
               _moddeinit,
               "1.0",
               "Alyx Wolcott <alyx@malkier.net>");

const char *api_key;

struct track
{
    char *title;
    char *artist;
    char *album;
    bool np;
};

void _modinit(module_t *m)
{
    mowgli_config_file_entry_t *root, *entry;

    MODULE_TRY_REQUEST_SYMBOL(m, web_write_to_buffer, "web/core", "web_write_to_buffer");

    command_add("np", cmd_np, 1, AC_NONE,
                "Retrieve Now Playing information for a Last.FM account",
                "<username>");

    root = config_find_entry(me.config->entries, "lastfm");
    if (root == NULL)
    {
        m->mflags = MODTYPE_FAIL;
        return;
    }
    entry = config_find_entry(root->entries, "api_key");
    if (entry == NULL)
    {
        m->mflags = MODTYPE_FAIL;
        return;
    }

    api_key = entry->vardata;
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    command_del("np", cmd_np);
}

static int get_first_track(const char *key, void *data, void *privdata)
{
    mowgli_json_t *n, *m;
    struct track *t;

    n = (mowgli_json_t *)data;
    t = (struct track *)privdata;

    if (strcmp(key, "name") == 0)
    {
        t->title = n->v.v_string->str;
    }
    else if (strcmp(key, "@attr") == 0)
    {
        m = mowgli_patricia_retrieve(n->v.v_object, "nowplaying");
        if (m != NULL)
        {
            if (strcmp(m->v.v_string->str, "0") == 0)
            {
                t->np = false;
            }
            else
            {
                t->np = true;
            }
        }
    }
    else if (strcmp(key, "artist") == 0)
    {
        m = mowgli_patricia_retrieve(n->v.v_object, "#text");
        if (m != NULL)
            t->artist = m->v.v_string->str;
    }
    else if (strcmp(key, "album") == 0)
    {
        m = mowgli_patricia_retrieve(n->v.v_object, "#text");
        if (m != NULL)
            t->album = m->v.v_string->str;
    }

    return 1;
}

static void
cmd_np(const irc_event_t *event, int parc, char **parv)
{
    char uri[BUFSIZE];
    CURL *curl;
    CURLcode res;
    mowgli_string_t *input_buf;
    mowgli_json_t *json, *curr;
    struct track *t;

    curl = curl_easy_init();

    snprintf(uri, BUFSIZE, API_ROOT, parv[1], api_key);

    if (curl == NULL)
    {
        command_reply(event->target, "Internal error. Try again later.");
        logger(LOG_DEBUG, "[web/lastfm] curl_easy_init() failed");
        return;
    }

    input_buf = mowgli_string_create();

    curl_easy_setopt(curl, CURLOPT_URL, uri);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, web_write_to_buffer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, input_buf);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        command_reply(event->target, "Last.FM error. Try again later.");
        logger(LOG_DEBUG, "[web/lastfm]: curl_easy_perform() failed: %s", curl_easy_strerror(res));
        mowgli_string_destroy(input_buf);
        curl_easy_cleanup(curl);
        return;
    }

    t = mowgli_alloc(sizeof(struct track));

    t->title = t->artist = t->album = NULL;
    t->np = false;

    json = mowgli_json_parse_string(input_buf->str);

    if (json == NULL || json->tag != MOWGLI_JSON_TAG_OBJECT)
    {
        logger(LOG_DEBUG, "[web/lastfm] lookup failed: Invalid JSON");
        command_reply(event->target, "Last.FM error, try again later.");
        goto cleanup;
    }

    if ((curr = mowgli_json_object_retrieve(json, "recenttracks")) == NULL)
    {
        logger(LOG_DEBUG,
               "[web/lastfm] unhandled exception: missing required field 'recenttracks'");
        command_reply(event->target, "Last.FM error, try again later.");
        goto cleanup;
    }

    if ((curr = mowgli_json_object_retrieve(curr, "track")) == NULL)
    {
        logger(LOG_DEBUG,
               "[web/lastfm] unhandled exception: missing required field 'track'");
        command_reply(event->target, "Last.FM error, try again later.");
        goto cleanup;
    }

    mowgli_patricia_foreach(curr->v.v_object, get_first_track, t);

    if (t->album)
    {
        command_reply(event->target, "%s %s %s by %s (%s)",
                      parv[1], (t->np ? "is now playing" : "last played"),
                      t->title, t->artist, t->album);
    }
    else
    {
        command_reply(event->target, "%s %s %s by %s", parv[1],
                      (t->np ? "is now playing" : "last played"), t->title, t->artist);
    }

cleanup:
    mowgli_json_decref(json);
    mowgli_string_destroy(input_buf);
    curl_easy_cleanup(curl);
    return;
}
