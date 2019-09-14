#include "sigyn.h"
#include <curl/curl.h>

const char *API_ROOT;

static void cmd_bug(const irc_event_t *event, int parc, char **parv);

size_t (*web_write_to_buffer)(void *buf, size_t size, size_t nmemb, mowgli_string_t *input_buf);

DECLARE_MODULE("web/bugzilla", MODULE_UNLOAD_CAPABILITY_OK,
    _modinit, _moddeinit, "1.0", "Alyx Wolcott <alyx@malkier.net>");

void _modinit(module_t *m)
{
  mowgli_config_file_entry_t *entry;

  MODULE_TRY_REQUEST_SYMBOL(m, web_write_to_buffer, "web/core", "web_write_to_buffer");

  command_add("bug", cmd_bug, 1, AC_NONE, "Retrieve summary from Bugzilla",
      "<bug-id>");

  entry = config_find_entry(me.config->entries, "bugzilla");
  if (entry == NULL)
  {
    m->mflags = MODTYPE_FAIL;
    return;
  }

  API_ROOT = entry->vardata;
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
  command_del("bug", cmd_bug);
}

static void check_tag(int i)
{
  switch(i) {
    case MOWGLI_JSON_TAG_NULL:
      printf("json: null\n");
      break;
   case MOWGLI_JSON_TAG_BOOLEAN:
      printf("json: bool\n");
      break;
  case MOWGLI_JSON_TAG_INTEGER:
      printf("json: int\n");
      break;
  case MOWGLI_JSON_TAG_FLOAT:
      printf("json: float\n");
      break;
  case MOWGLI_JSON_TAG_STRING:
      printf("json: str\n");
      break;
  case MOWGLI_JSON_TAG_ARRAY:
      printf("json: arr\n");
      break;
  case MOWGLI_JSON_TAG_OBJECT:
      printf("json: obj\n");
      break;
  }
}

static int print_bug_details(const char *key, void *data, void *privdata)
{
  mowgli_json_t *n, *m;
  irc_event_t *event;

  n = (mowgli_json_t *)data;
  event = (irc_event_t *)privdata;

  if (strcmp(key, "summary") == 0)
  {
    command_reply_safe(event->target, "%s", n->v.v_string->str);
  }

  return 1;
}

static void
cmd_bug(const irc_event_t *event, int parc, char **parv)
{
  char uri[BUFSIZ];
  CURL *curl;
  CURLcode res;
  mowgli_string_t *buf;
  mowgli_json_t *json, *curr;

  curl = curl_easy_init();

  snprintf(uri, BUFSIZ, "%s/bug/%s", API_ROOT, parv[1]);
  logger(LOG_DEBUG, "[web/bugzilla]: evaluating %s", uri);

  if (curl == NULL)
  {
    logger(LOG_DEBUG, "[web/bugzilla] curl_easy_init() failed");
    command_reply(event->target, "Internal error. Try again later.");
    return;
  }

  buf = mowgli_string_create();

  curl_easy_setopt(curl, CURLOPT_URL, uri);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, web_write_to_buffer);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, buf);

  res = curl_easy_perform(curl);

  if (res != CURLE_OK)
  {
    logger(LOG_DEBUG, "[web/bugzilla]: curl_easy_perform() failed: %s", curl_easy_strerror(res));
    command_reply(event->target, "Bug lookup error. Try again later.");
    mowgli_string_destroy(buf);
    curl_easy_cleanup(curl);
    return;
  }

  json = mowgli_json_parse_string(buf->str);

  if (json == NULL || json->tag != MOWGLI_JSON_TAG_OBJECT)
  {
    logger(LOG_DEBUG, "[web/bugzilla] lookup failed: Invalid JSON");
    command_reply(event->target, "Bug lookup error, try again later.");
    goto cleanup;
  }

  if ((curr = mowgli_json_object_retrieve(json, "bugs")) == NULL)
  {
    logger(LOG_DEBUG,
        "[web/bugzilla] unhandled exception: missing required field 'summary'");
    command_reply(event->target, "Bug lookup error, try again later.");
    goto cleanup;
  }

  mowgli_patricia_foreach(curr->v.v_object, print_bug_details, event);

cleanup:
  mowgli_json_decref(json);
  mowgli_string_destroy(buf);
  curl_easy_cleanup(curl);
  return;
}
