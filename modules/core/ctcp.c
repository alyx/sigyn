#include "sigyn.h"

DECLARE_MODULE("core/ctcp", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "Justin Crawford <Justasic@gmail.com>");

static void handle_ctcp(void *data, UNUSED void *udata);

void _modinit(UNUSED module_t *m)
{
    mowgli_hook_associate("PRIVMSG", handle_ctcp, NULL);
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    mowgli_hook_dissociate("PRIVMSG", handle_ctcp);
}

static void handle_ctcp(void *data, UNUSED void *udata)
{
    irc_event_t *event;
    event = (irc_event_t *)data;

    char *parv[MAXPARC + 1];
    INIT_PARV(parv, MAXPARC + 1);
    char message[BUFSIZE+1];
    strcpy(message, event->data);
    size_t len = strlen(message);
    message[len] == '\0';

    if(*message == '\001' && message[len-1] == '\001')
    {
	// Doesn't strip all chars in one call??
	rmchar(message, "\001");
	rmchar(message, "\001");
	int parc = tokenize(message, parv);

	if(parc > 0 && (strcmp(parv[1], "ACTION")) == 1)
	  return; // Ignore /me's
	else
	{
	    char *command = mowgli_alloc(BUFSIZE);
	    mowgli_strlcat(command, "CTCP ", BUFSIZE); // Prefix with "CTCP "
	    mowgli_strlcat(command, message, BUFSIZE);
	    mowgli_hook_call(command, event);
	    mowgli_free(command);
	}
    }
}
