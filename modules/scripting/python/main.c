#include <Python.h>
#include "sigyn.h"

#define py_return_err_if_null(x) if (x == NULL) { fprintf(stderr, "Argument %s is undefined.", #x); PyErr_Format(PyExc_TypeError, "Argument %s is undefined.", #x); } 

DECLARE_MODULE("scripting/python", MODULE_UNLOAD_CAPABILITY_OK,
        _modinit, _moddeinit, "1.0", "Alyx <alyx@malkier.net>");

static void cmd_loadpy(const irc_event_t * event, int parc, char ** parv);
static void cmd_runpy (const irc_event_t * event, int parc, char ** parv);

/*
 * Python shim section.
 * All PyObject functions are for the Python<->C API
 * and should not be used by anything not in a Python file.
 */

static void add_constants(PyObject * m)
{
    PyModule_AddIntConstant(m, "CMD_BADPARAM", 0);
    PyModule_AddIntConstant(m, "CMD_NEEDSPARAM", 1);
    PyModule_AddIntConstant(m, "CMD_NOAUTH", 2);

    PyModule_AddIntMacro(m, LOG_ALL);
    PyModule_AddIntMacro(m, LOG_RAW);
    PyModule_AddIntMacro(m, LOG_DEBUG);
    PyModule_AddIntMacro(m, LOG_CRITICAL);
    PyModule_AddIntMacro(m, LOG_ERROR);
    PyModule_AddIntMacro(m, LOG_WARNING);
    PyModule_AddIntMacro(m, LOG_GENERAL);
}

static PyObject * sigyn_config(PyObject * self, PyObject * args)
{
    const char * name;
    PyArg_ParseTuple(args, "s", &name);
    py_return_err_if_null(name);
    mowgli_config_file_entry_t * value = config_find_entry(me.config->entries, name);
    PyObject *val = (value == NULL ? Py_None : PyString_FromString(value->vardata));
    Py_INCREF(val);
    return val;
}

static PyObject * sigyn_logger(PyObject * self, PyObject * args)
{
    const unsigned int level;
    char * msg;
    PyArg_ParseTuple(args, "is", &level, &msg);
    py_return_err_if_null(msg);
    logger(level, msg);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * sigyn_irc_pass(PyObject * self, PyObject * args) 
{ 
    const char * password; 
    PyArg_ParseTuple(args, "s", &password); 
    py_return_err_if_null(password);
    irc_pass(password);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_nick(PyObject * self, PyObject * args) 
{ 
    const char * nick;
    PyArg_ParseTuple(args, "s", &nick);
    py_return_err_if_null(nick);
    irc_nick(nick);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_user(PyObject * self, PyObject * args)
{
    const char * user, * host, * server, * real;
    PyArg_ParseTuple(args, "ssss", &user, &host, &server, &real);
    py_return_err_if_null(user);
    py_return_err_if_null(host);
    py_return_err_if_null(server);
    py_return_err_if_null(real);
    irc_user(user, host, server, real);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_oper(PyObject * self, PyObject * args)
{
    const char * user, * password;
    PyArg_ParseTuple(args, "ss", &user, &password);
    py_return_err_if_null(user);
    py_return_err_if_null(password);
    irc_oper(user, password);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_quit(PyObject * self, PyObject * args)
{ 
    const char * message;
    PyArg_ParseTuple(args, "|s", &message);
    irc_quit(message);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_squit(PyObject * self, PyObject * args)
{
    const char * server, * message;
    PyArg_ParseTuple(args, "ss", &server, &message);
    py_return_err_if_null(server);
    py_return_err_if_null(message);
    irc_squit(server, message);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_join(PyObject * self, PyObject * args)
{
    const char * channel, * key;
    PyArg_ParseTuple(args, "s|s", &channel, &key);
    py_return_err_if_null(channel);
    irc_join(channel, key);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_part(PyObject * self, PyObject * args) 
{ 
    const char * channel, * message;
    PyArg_ParseTuple(args, "s|s", &channel, &message);
    py_return_err_if_null(channel);
    irc_part(channel, message);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_mode(PyObject * self, PyObject * args)
{
    const char * target, * modestring;
    PyArg_ParseTuple(args, "ss", &target, &modestring);
    py_return_err_if_null(target);
    py_return_err_if_null(modestring);
    irc_mode(target, modestring);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_topic(PyObject * self, PyObject * args)
{
    const char * channel, * topic;
    PyArg_ParseTuple(args, "s|s", &channel, &topic);
    py_return_err_if_null(channel);
    irc_topic(channel, topic);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_names(PyObject * self, PyObject * args) 
{
    const char * channel;
    PyArg_ParseTuple(args, "s", &channel);
    py_return_err_if_null(channel);
    irc_names(channel);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_invite(PyObject * self, PyObject * args)
{
    const char * user, * channel;
    PyArg_ParseTuple(args, "ss", &user, &channel);
    py_return_err_if_null(user);
    py_return_err_if_null(channel);
    irc_invite(user, channel);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_kick(PyObject * self, PyObject * args)
{
    const char * channel, * user, * comment;
    PyArg_ParseTuple(args, "ss|s", &channel, &user, &comment);
    py_return_err_if_null(channel);
    py_return_err_if_null(user);
    irc_kick(channel, user, comment);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_version (PyObject * self, PyObject * args) 
{
    const char * server;
    PyArg_ParseTuple(args, "|s", &server);
    irc_version(server);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_stats(PyObject * self, PyObject * args)
{
    const char * query, * server;
    PyArg_ParseTuple(args, "s|s", &query, &server);
    py_return_err_if_null(query);
    irc_stats(query, server);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_links(PyObject * self, PyObject * args)
{
    const char * remote, * mask;
    PyArg_ParseTuple(args, "|ss", &remote, &mask);
    irc_links(remote, mask);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_time(PyObject * self, PyObject * args) 
{
    const char * server;
    PyArg_ParseTuple(args, "|s", &server);
    irc_time(server);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_connect(PyObject * self, PyObject * args)
{
    const char * target, * remote;
    const int port;
    PyArg_ParseTuple(args, "s|is", &target, &port, &remote);
    py_return_err_if_null(target);
    irc_connect(target, port, remote);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_trace(PyObject * self, PyObject * args)
{
    const char * server;
    PyArg_ParseTuple(args, "|s", &server);
    irc_trace(server);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_admin(PyObject * self, PyObject * args)
{
    const char * server;
    PyArg_ParseTuple(args, "|s", &server);
    irc_admin(server);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_info(PyObject * self, PyObject * args)
{
    const char * server;
    PyArg_ParseTuple(args, "|s", &server);
    irc_info(server);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_privmsg(PyObject * self, PyObject * args)
{
    const char * reciever, * string;
    PyArg_ParseTuple(args, "ss", &reciever, &string);
    py_return_err_if_null(reciever);
    py_return_err_if_null(string);
    irc_privmsg(reciever, string);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_notice(PyObject * self, PyObject * args)
{
    const char * reciever, * string;
    PyArg_ParseTuple(args, "ss", &reciever, &string);
    py_return_err_if_null(reciever);
    py_return_err_if_null(string);
    irc_notice(reciever, string);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_who(PyObject * self, PyObject * args)
{
    const char * name, * o;
    PyArg_ParseTuple(args, "s|s", &name, &o);
    py_return_err_if_null(name);
    irc_who(name, o);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_whois(PyObject * self, PyObject * args)
{
    const char * server, * nickmask;
    PyArg_ParseTuple(args, "s|s", &nickmask, &server);
    py_return_err_if_null(nickmask);
    irc_whois(server, nickmask);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_whowas(PyObject * self, PyObject * args)
{
    const char * nickname, * server;
    const int    count;
    PyArg_ParseTuple(args, "s|is", &nickname, &count, &server);
    irc_whowas(nickname, (int)count, server);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_kill(PyObject * self, PyObject * args)
{
    const char * nickname, * reason;
    PyArg_ParseTuple(args, "ss", &nickname, &reason);
    py_return_err_if_null(nickname);
    py_return_err_if_null(reason);
    irc_kill(nickname, reason);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_ping(PyObject * self, PyObject * args)
{
    const char * server1, * server2;
    PyArg_ParseTuple(args, "s|s", &server1, &server2);
    py_return_err_if_null(server1);
    irc_ping(server1, server2);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_pong(PyObject * self, PyObject * args)
{
    const char * daemon;
    PyArg_ParseTuple(args, "s", &daemon);
    py_return_err_if_null(daemon);
    irc_pong(daemon);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_away(PyObject * self, PyObject * args)
{
    const char * message;
    PyArg_ParseTuple(args, "|s", &message);
    irc_away(message);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_users(PyObject * self, PyObject * args)
{ 
    const char * server;
    PyArg_ParseTuple(args, "|s", &server);
    irc_users(server);
    Py_INCREF(Py_None); return Py_None; 
}

static PyObject * sigyn_irc_userhost(PyObject * self, PyObject * args)
{
    const char * nickname;
    PyArg_ParseTuple(args, "s", &nickname);
    py_return_err_if_null(nickname);
    irc_userhost(nickname);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_irc_action(PyObject * self, PyObject * args)
{
    const char * target, * message;
    PyArg_ParseTuple(args, "ss", &target, &message);
    py_return_err_if_null(target);
    py_return_err_if_null(message);
    irc_action(target, message);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_ctcp_send(PyObject * self, PyObject * args)
{
    const char * target, * contents;
    PyArg_ParseTuple(args, "ss", &target, &contents);
    py_return_err_if_null(target);
    py_return_err_if_null(contents);
    ctcp_send(target, contents);
    Py_INCREF(Py_None); return Py_None;
}

static PyObject * sigyn_ctcp_reply(PyObject * self, PyObject * args)
{
    const char * target, * contents;
    PyArg_ParseTuple(args, "ss", &target, &contents);
    py_return_err_if_null(target);
    py_return_err_if_null(contents);
    ctcp_reply(target, contents);
    Py_INCREF(Py_None); return Py_None;
}


static PyMethodDef SigynMethods[] = {
    {"irc_pass", sigyn_irc_pass, METH_VARARGS, "Send PASS to the server."},
    {"irc_nick", sigyn_irc_nick, METH_VARARGS, "Send NICK to the server."},
    {"irc_user", sigyn_irc_user, METH_VARARGS, "Send USER to the server."},
    {"irc_oper", sigyn_irc_oper, METH_VARARGS, "Send OPER to the server."},
    {"irc_quit", sigyn_irc_quit, METH_VARARGS, "Send QUIT to the server."},
    {"irc_squit", sigyn_irc_squit, METH_VARARGS, "Send SQUIT to the server."},
    {"irc_join", sigyn_irc_join, METH_VARARGS, "Send JOIN to the server."},
    {"irc_part", sigyn_irc_part, METH_VARARGS, "Send PART to the server."},
    {"irc_mode", sigyn_irc_mode, METH_VARARGS, "Send MODE to the server."},
    {"irc_topic", sigyn_irc_topic, METH_VARARGS, "Send TOPIC to the server."},
    {"irc_names", sigyn_irc_names, METH_VARARGS, "Send NAMES to the server."},
    {"irc_invite", sigyn_irc_invite, METH_VARARGS, "Send INVITE to the server."},
    {"irc_kick", sigyn_irc_kick, METH_VARARGS, "Send KICK to the server."},
    {"irc_version", sigyn_irc_version, METH_VARARGS, "Send VERSION to the server."},
    {"irc_stats", sigyn_irc_stats, METH_VARARGS, "Send STATS to the server."},
    {"irc_links", sigyn_irc_links, METH_VARARGS, "Send LINKS to the server."},
    {"irc_time", sigyn_irc_time, METH_VARARGS, "Send TIME to the server."},
    {"irc_connect", sigyn_irc_connect, METH_VARARGS, "Send CONNECT to the server."},
    {"irc_trace", sigyn_irc_trace, METH_VARARGS, "Send TRACE to the server."},
    {"irc_admin", sigyn_irc_admin, METH_VARARGS, "Send ADMIN to the server."},
    {"irc_info", sigyn_irc_info, METH_VARARGS, "Send INFO to the server."},
    {"irc_privmsg", sigyn_irc_privmsg, METH_VARARGS, "Send PRIVMSG to the server."},
    {"irc_notice", sigyn_irc_notice, METH_VARARGS, "Send NOTICE to the server."},
    {"irc_who", sigyn_irc_who, METH_VARARGS, "Send WHO to the server."},
    {"irc_whois", sigyn_irc_whois, METH_VARARGS, "Send WHOIS to the server."},
    {"irc_whowas", sigyn_irc_whowas, METH_VARARGS, "Send WHOWAS to the server."},
    {"irc_kill", sigyn_irc_kill, METH_VARARGS, "Send KILL to the server."},
    {"irc_ping", sigyn_irc_ping, METH_VARARGS, "Send PING to the server."},
    {"irc_pong", sigyn_irc_pong, METH_VARARGS, "Send PONG to the server."},
    {"irc_away", sigyn_irc_away, METH_VARARGS, "Send AWAY to the server."},
    {"irc_users", sigyn_irc_users, METH_VARARGS, "Send USERS to the server."},
    {"irc_userhost", sigyn_irc_userhost, METH_VARARGS, "Send USERHOST to the server."},
    {"irc_action", sigyn_irc_action, METH_VARARGS, "Sends a CTCP ACTION to a target."},
    {"ctcp_send", sigyn_ctcp_send, METH_VARARGS, "Sends a CTCP to a target."},
    {"ctcp_reply", sigyn_ctcp_reply, METH_VARARGS, "Replies to a CTCP."},
    {"log", sigyn_logger, METH_VARARGS, "Logs the specified message to the locations set in sigyn config."},
    {"config", sigyn_config, METH_VARARGS, "Retrieves an entry from sigyn's config."}
};

/*
 * END PYTHON SHIM FUNCTIONS
 */

void _modinit(UNUSED module_t * m)
{
    PyObject * module;

    Py_Initialize();
    module = Py_InitModule("_sigyn", SigynMethods);
    add_constants(module);
    command_add("loadpy", cmd_loadpy, 1, AC_ADMIN, "Loads and executes a Python file", "<file>");
    command_add("runpy", cmd_runpy, 1, AC_ADMIN, "Runs a string of Python code", "<string>");
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    Py_Finalize();
    command_del("loadpy", cmd_loadpy);
    command_del("runpy", cmd_runpy);
}
    
static void cmd_loadpy(const irc_event_t * event, int parc, char ** parv)
{
    FILE * fp;
    logger(LOG_DEBUG, "cmd_loadpy called");
    if ((fp = fopen(parv[1], "r")) == NULL)
    {
        irc_notice(event->origin->nick, "Opening the file %s failed.", parv[1]);
        return;
    }

    PyRun_AnyFileExFlags(fp, parv[1], 0, NULL);
}

static void cmd_runpy(const irc_event_t * event, int parc, char ** parv)
{
    char * buf;
    PyObject * main, * result;
    logger(LOG_DEBUG, "cmd_runpy called");
    buf = strdup(event->data+7);
    PyRun_SimpleStringFlags(buf, NULL);
    free(buf);
}
