#include <Python.h>
#include <dlfcn.h>
#include "sigyn.h"

#define py_return_err_if_null(x) \
    if (x == NULL) \
    { \
        fprintf(stderr, "Argument %s is undefined.", #x); \
        PyErr_Format(PyExc_TypeError, "Argument %s is undefined.", #x); \
    }

#define py_return_err_if_zero(x) \
    if (x == 0) \
    { \
        fprintf(stderr, "Argument %s is undefined.", #x); \
        PyErr_Format(PyExc_TypeError, "Argument %s is undefined.", #x); \
    }

#define py_check_err \
    if (PyErr_Occurred()) \
        PyErr_PrintEx(1)

struct module_state {
  PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
# define PyString_FromString PyUnicode_FromString
# define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else
# define GETSTATE(m) (&_state)
static struct module_state _state;
#endif

DECLARE_MODULE("scripting/python", MODULE_UNLOAD_CAPABILITY_OK,
        _modinit, _moddeinit, "1.0", "Alyx <alyx@malkier.net>");

static void cmd_loadpy(const irc_event_t * event, int parc, char ** parv);
static void cmd_runpy (const irc_event_t * event, int parc, char ** parv);

static mowgli_patricia_t * py_cmd_list;
static mowgli_patricia_t * py_timer_list;

struct timer_data
{
    PyObject *cb;
    PyObject *args;
    const char *name;
    bool repeats;
};

/*
 * Python shim section.
 * All PyObject functions are for the Python<->C API
 * and should not be used by anything not in a Python file.
 */

static void py_cmd_cb(const irc_event_t * event, int parc, char ** parv)
{
    PyObject * o;

    o = mowgli_patricia_retrieve(py_cmd_list, event->command);
    if (o == NULL)
        return;
    PyObject_CallFunction(o, "sssssss", event->command, event->target,
            event->data, event->origin->nick, event->origin->user,
            event->origin->host, event->origin->hostmask);
    py_check_err;
}

static void py_timer_cb(void * data)
{
    struct timer_data * timerdata;
    timerdata = (struct timer_data *)data;
    PyObject_CallFunction(timerdata->cb, "sO", timerdata->name, timerdata->args);
    if (!timerdata->repeats)
        mowgli_free(timerdata);
}


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

    PyModule_AddIntMacro(m, AC_NONE);
    PyModule_AddIntMacro(m, AC_ADMIN);
}

static PyObject * sigyn_config(PyObject * self, PyObject * args)
{
    const char * name;
    mowgli_config_file_entry_t * value;
    PyObject * val;

    PyArg_ParseTuple(args, "s", &name);
    py_return_err_if_null(name);
    value = config_find_entry(me.config->entries, name);
    if (value && value->vardata)
    {
        val = PyString_FromString(value->vardata);
        Py_INCREF(val);
        return val;
    }

    PyErr_Format(PyExc_LookupError, "Could not find config entry %s", name);
    return NULL;
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

static PyObject * sigyn_isupport(PyObject * self, PyObject * args)
{
    const char * key;
    PyObject * value;

    PyArg_ParseTuple(args, "s", &key);
    py_return_err_if_null(key);
    value = Py_BuildValue("s", mowgli_patricia_retrieve(isupport_table, key));
    Py_INCREF(value);
    return value;
}

static PyObject * sigyn_cmd_add(PyObject * self, PyObject * args)
{
    const char * cmd, * help, * syntax;
    unsigned int argc, perm;
    PyObject * cb;

    PyArg_ParseTuple(args, "sOIIss", &cmd, &cb, &argc, &perm, &help, &syntax);
    py_return_err_if_null(cmd);
    py_return_err_if_zero(args);
    py_return_err_if_zero(perm);
    py_return_err_if_null(help);
    py_return_err_if_null(syntax);
    mowgli_patricia_add(py_cmd_list, cmd, cb);
    command_add(cmd, py_cmd_cb, argc, perm, help, syntax);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * sigyn_cmd_del(PyObject * self, PyObject * args)
{
    const char * cmd;
    PyObject * cb;

    PyArg_ParseTuple(args, "sO", &cmd, &cb);
    py_return_err_if_null(cb);
    py_return_err_if_null(cmd);
    command_del(cmd, py_cmd_cb);
    mowgli_patricia_delete(py_cmd_list, cmd);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * sigyn_timer_add(PyObject * self, PyObject * args)
{
    const char * name;
    unsigned int when;
    PyObject * cb, * repeat, * arg;
    struct timer_data * tdata;
    mowgli_eventloop_timer_t * timer;
    tdata = mowgli_alloc(sizeof(struct timer_data));
    PyArg_ParseTuple(args, "sOOIO", &name, &cb, &arg, &when, &repeat);
    py_return_err_if_null(name);
    py_return_err_if_null(cb);
    py_return_err_if_zero(when);
    tdata->cb = cb;
    tdata->args = arg;
    tdata->name = name;
    bool continuous;
    if (repeat == NULL || repeat == Py_False)
        continuous = false;
    if (repeat == Py_True)
        continuous = true;
    tdata->repeats = continuous;
    timer = timer_add(name, py_timer_cb, tdata, (time_t)when, continuous);
    mowgli_patricia_add(py_timer_list, name, timer);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * sigyn_timer_del(PyObject * self, PyObject *args)
{
    const char * name;
    mowgli_eventloop_timer_t * timer;
    PyArg_ParseTuple(args, "s", &name);
    py_return_err_if_null(name);
    timer = mowgli_patricia_retrieve(py_timer_list, name);
    timer_del(timer);
    mowgli_patricia_delete(py_timer_list, name);
    Py_INCREF(Py_None);
    return Py_None;
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
    {"config", sigyn_config, METH_VARARGS, "Retrieves an entry from sigyn's config."},
    {"isupport", sigyn_isupport, METH_VARARGS, "Finds value of an ISUPPORT entry."},
    {"cmd_add", sigyn_cmd_add, METH_VARARGS, "Adds a command to the command tree."},
    {"cmd_del", sigyn_cmd_del, METH_VARARGS, "Deletes a command from the command tree."},
    {"timer_add", sigyn_timer_add, METH_VARARGS, "Adds a timer."},
    {"timer_del", sigyn_timer_del, METH_VARARGS, "Deletes a timer."},
    {NULL, NULL, 0, NULL}
};

#if PY_MAJOR_VERSION >= 3

static PyModuleDef SigynModule = {
  PyModuleDef_HEAD_INIT, "_sigyn", NULL, -1, SigynMethods,
  NULL, NULL, NULL, NULL
};

static PyObject * PyInit_sigyn(void)
{
  PyObject * module;
  module = PyModule_Create(&SigynModule);
  add_constants(module);
  return module;
}

#endif

/*
 * END PYTHON SHIM FUNCTIONS
 */

static int ends_with_py(const char *str)
{
  char *suffix = ".py";
  size_t lenstr, lensuffix;

  lensuffix = 3;

  if (!str)
    return 0;
  lenstr = strlen(str);
  if (lensuffix > lenstr)
    return 0;
  return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

static void loadscript_py(mowgli_config_file_entry_t * entry)
{
  FILE * fp;

  if (entry == NULL)
    logger(LOG_DEBUG, "loadscript_py(): Initial entry is NULL");

  while (entry != NULL)
  {
    logger(LOG_DEBUG, "loadscript_py(): Entry name, %s", entry->varname);
    if (!strcmp(entry->varname, "loadscript"))
    {
      if (ends_with_py(entry->vardata))
      {
        if ((fp = fopen(entry->vardata, "r")) == NULL)
        {
          logger(LOG_GENERAL, "[Scripting] Failed to load script %s\n", entry->vardata);
        }
        else
        {
          PyRun_AnyFileExFlags(fp, entry->vardata, 0, NULL);
          py_check_err;
          logger(LOG_GENERAL, "[Scripting] Loaded module %s\n", entry->vardata);
        }
      }
    }
    if (entry->entries != NULL)
      loadscript_py(entry->entries);
    entry = entry->next;
  }
}

void _modinit(UNUSED module_t * m)
{
    PyObject * module, * path;
#if PY_MAJOR_VERSION >= 3
    PyImport_AppendInittab("_sigyn", &PyInit_sigyn);
#endif

    dlopen("libpython3.5.so", RTLD_NOW | RTLD_GLOBAL);

    Py_Initialize();
    PyEval_InitThreads();

    py_cmd_list = mowgli_patricia_create(strcasecanon);
    py_timer_list = mowgli_patricia_create(strcasecanon);
#if PY_MAJOR_VERSION <  3
    module = Py_InitModule("_sigyn", SigynMethods);
    add_constants(module);
#endif
    path = PySys_GetObject("path");
    PyList_Append(path, PyString_FromString(LIBDIR "/python"));

    command_add("loadpy", cmd_loadpy, 1, AC_ADMIN, "Loads and executes a Python file", "<file>");
    command_add("runpy", cmd_runpy, 1, AC_ADMIN, "Runs a string of Python code", "<string>");
    loadscript_py(me.config->entries);
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    Py_Finalize();
    /*mowgli_patricia_destory(py_cmd_list);*/
    /*mowgli_patricia_destroy(py_timer_list);*/
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
    py_check_err;
}

static void cmd_runpy(const irc_event_t * event, int parc, char ** parv)
{
    char * buf;
    logger(LOG_DEBUG, "cmd_runpy called");
    buf = (strchr(event->data, ' ')+1);
    PyRun_SimpleStringFlags(buf, NULL);
    py_check_err;
}

