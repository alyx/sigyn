import _sigyn

def action(target, message):
    _sigyn.irc_action(target, message)

def password(password):
    _sigyn.irc_pass(password)

def nick(nick):
    _sigyn.irc_nick(nick)

def user(user, host, server, real):
    _sigyn.irc_user(user, host, server, real)

def oper(user, password):
    _sigyn.irc_oper(user, password)

def quit(message=None):
    _sigyn.irc_quit(message)

def squit(server, message):
    _sigyn.irc_squit(server, message)

def join(channel, key=None):
    _sigyn.irc_join(channel, key)

def part(channel, message=None):
    _sigyn.irc_part(channel, message)

def mode(target, modestring):
    _sigyn.irc_mode(target, modestring)

def topic(channel, topic=None):
    _sigyn.irc_topic(channel, topic)

def names(channel):
    _sigyn.irc_names(channel)

def invite(user, channel):
    _sigyn.irc_invite(user, channel)

def kick(channel, user, comment=None):
    _sigyn.irc_kick(channel, user, comment)

def version(server=None):
    _sigyn.irc_version(server)

def stats(query, server=None):
    _sigyn.irc_stats(query, server)

def links(remote=None, mask=None):
    _sigyn.irc_links(remote, mask)

def connect(target, port=None, remote=None):
    _sigyn.irc_connect(target, port, remote)

def trace(server=None):
    _sigyn.irc_trace(server)

def admin(server=None):
    _sigyn.irc_admin(server)

def info(server=None):
    _sigyn.irc_info(server)

def privmsg(reciever, string):
    _sigyn.irc_privmsg(reciever, string)

def notice(reciever, string):
    _sigyn.irc_notice(reciever, string)

def who(name, o=None):
    _sigyn.irc_who(name, o)

def whois(nickmask, server=None):
    _sigyn.irc_whois(nickmask, server)

def whowas(nickname, count=None, server=None):
    _sigyn.irc_whowas(nickname, count, server)

def kill(nickname, reason):
    _sigyn.irc_kill(nickname, reason)

def ping(server1, server2=None):
    _sigyn.irc_ping(server1, server2)

def pong(daemon):
    _sigyn.irc_pong(daemon)

def away(message=None):
    _sigyn.irc_away(message)

def users(server=None):
    _sigyn.irc_users(server)

def userhost(nickname):
    _sigyn.irc_userhost(nickname)

def sendctcp(target, args):
    _sigyn.ctcp_send(target, args)

def ctcpreply(target, args):
    _sigyn.ctcp_send(target, args)

