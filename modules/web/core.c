#include "sigyn.h"
#include <curl/curl.h>

DECLARE_MODULE("web/core",
    MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
    "1.0", "Alyx Wolcott <alyx@malkier.net>");

void _modinit(module_t *m)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    curl_global_cleanup();
}

size_t web_write_to_buffer(void *buf, size_t size, size_t nmemb, mowgli_string_t *input_buf)
{
    size_t r;

    r = size * nmemb;
    mowgli_string_append(input_buf, (char *)buf, r);

    return r;
}