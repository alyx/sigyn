#include "sigyn.h"
#include <curl/curl.h>

#define API_ROOT "https://cve.circl.lu/api/cve/%s"

static void cmd_cve(const irc_event_t *event, int parc, char **parv);

size_t (*web_write_to_buffer)(void *buf, size_t size, size_t nmemb, mowgli_string_t *input_buf);

DECLARE_MODULE("web/cve", MODULE_UNLOAD_CAPABILITY_OK,
    _modinit, _moddeinit, "1.0", "Alyx Wolcott <alyx@malkier.net>");

void _modinit(module_t *m)
{
    MODULE_TRY_REQUEST_SYMBOL(m, web_write_to_buffer, "web/core", "web_write_to_buffer");
    
    command_add("cve", cmd_cve, 1, AC_NONE, "Retrieve information on a CVE",
        "<cve-id>");
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    command_del("cve", cmd_cve);
}

static void
cmd_cve(const irc_event_t *event, int parc, char **parv)
{
    char uri[BUFSIZ];
    CURL *curl;
    CURLcode res;
    mowgli_string_t *buf;
    mowgli_json_t *json, *curr;

    curl = curl_easy_init();

    snprintf(uri, BUFSIZ, API_ROOT, parv[1]);

    if (curl == NULL)
    {
        command_reply(event->target, "Internal error. Try again later.");
        logger(LOG_DEBUG, "[web/cve] curl_easy_init() failed");
        return;
    }

    buf = mowgli_string_create();

    curl_easy_setopt(curl, CURLOPT_URL, uri);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, web_write_to_buffer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buf);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        command_reply(event->target, "CVE lookup error. Try again later.");
        logger(LOG_DEBUG, "[web/cve]: curl_easy_perform() failed: %s", curl_easy_strerror(res));
        mowgli_string_destroy(buf);
        curl_easy_cleanup(curl);
        return;
    }

    json = mowgli_json_parse_string(buf->str);

    if (json == NULL || json->tag != MOWGLI_JSON_TAG_OBJECT)
    {
        logger(LOG_DEBUG, "[web/cve] lookup failed: Invalid JSON");
        command_reply(event->target, "CVE lookup error, try again later.");
        goto cleanup;
    }

    if ((curr = mowgli_json_object_retrieve(json, "summary")) == NULL)
    {
        logger(LOG_DEBUG, 
            "[web/cve] unhandled exception: missing required field 'summary'");
        command_reply(event->target, "CVE lookup error, try again later.");
        goto cleanup;
    }

    command_reply(event->target, "%s: %s", parv[1], curr->v.v_string->str);

cleanup:
    mowgli_json_decref(json);
    mowgli_string_destroy(buf);
    curl_easy_cleanup(curl);
    return;
}