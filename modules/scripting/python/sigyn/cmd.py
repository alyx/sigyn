import _sigyn

def add(name, callback, args, perm, help, syntax):
    _sigyn.cmd_add(name, callback, args, perm, help, syntax)

def del(name, callback):
    _sigyn.cmd_del(name, callback)
