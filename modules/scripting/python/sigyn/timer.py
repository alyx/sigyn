import _sigyn

def add(name, callback, args, timer, repeats=False):
    _sigyn.timer_add(name, callback, args, timer, repeats)

def remove(name):
    _sigyn.timer_del(name)
