import _sigyn
import sigyn.cmd, sigyn.irc, sigyn.log, sigyn.timer

__all__ = ['cmd', 'irc', 'log', 'timer']

def config(name):
    try:
        return _sigyn.config(name)
    except:
        return None

