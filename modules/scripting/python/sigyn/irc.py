import _sigyn

def pass(password):
    _sigyn.irc_pass(password)

def nick(nick):
    _sigyn.irc_nick(nick)

def user(user, host, server, real):
    _sigyn.irc_user(user, host, server, real)

def oper(user, password):
    _sigyn.irc_oper(user, password)

def quit(message=null):
    _sigyn.irc_quit(message)

def squit(server, message):
    _sigyn.irc_squit(server, message)

def join(channel, key=null):
    _sigyn.irc_join(channel, key)

def part(channel, message=null):
    _sigyn.irc_part(channel, message)

def mode(target, modestring):
    _sigyn.irc_mode(target, modestring)

def topic(channel, topic=null):
    _sigyn.irc_topic(channel, topic)

def names(channel):
    _sigyn.irc_names(channel)

def invite(user, channel):
    _sigyn.irc_invite(user, channel)

def kick(channel, user, comment=null):
    _sigyn.irc_kick(channel, user, comment)

def version(server=null):
    _sigyn.irc_version(server)

def stats(query, server=null):
    _sigyn.irc_stats(query, server)

def links(remote=null, mask=null):
    _sigyn.irc_links(remote, mask)

def connect(target, port=null, remote=null):
    _sigyn.irc_connect(target, port, remote)

def trace(server=null):
    _sigyn.irc_trace(server)

def admin(server=null):
    _sigyn.irc_admin(server)

def info(server=null):
    _sigyn.irc_info(server)

def privmsg(reciever, string):
    _sigyn.irc_privmsg(reciever, string)

def notice(reciever, string):
    _sigyn.irc_notice(reciever, string)

def who(name, o=null):
    _sigyn.irc_who(name, o)

def whois(nickmask, server=null):
    _sigyn.irc_whois(nickmask, server)

def whowas(nickname, count=null, server=null):
    _sigyn.irc_whowas(nickname, count, server)

def kill(nickname, reason):
    _sigyn.irc_kill(nickname, reason)

def ping(server1, server2=null):
    _sigyn.irc_ping(server1, server2)

def pong(daemon):
    _sigyn.irc_pong(daemon)

def away(message=null):
    _sigyn.irc_away(message)

def users(server=null):
    _sigyn.irc_users(server)

def userhost(nickname):
    _sigyn.irc_userhost(nickname)


