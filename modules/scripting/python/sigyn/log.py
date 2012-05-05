import _sigyn

def raw(msg):
    _sigyn.log(_sigyn.LOG_RAW, msg)

def debug(msg):
    _sigyn.log(_sigyn.LOG_DEBUG, msg)

def error(msg):
    _sigyn.log(_sigyn.LOG_ERROR, msg)

def general(msg):
    _sigyn.log(_sigyn.LOG_GENERAL, msg)
