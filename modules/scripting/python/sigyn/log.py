import _sigyn

def debug(msg):
    _sigyn.log(_sigyn.LOG_DEBUG, msg)

def critical(msg):
    _sigyn.log(_sigyn.LOG_CRITICAL, msg)

def error(msg):
    _sigyn.log(_sigyn.LOG_ERROR, msg)

def warning(msg):
    _sigyn.log(_sigyn.LOG_WARNING, msg)

def info(msg):
    _sigyn.log(_sigyn.LOG_GENERAL, msg)
