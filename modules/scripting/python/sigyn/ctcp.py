import _sigyn

def send(target, args):
    _sigyn.ctcp_send(target, args)

def reply(target, args):
    _sigyn.ctcp_send(target, args)
