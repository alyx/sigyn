import _sigyn

__all__ = ['cmd', 'irc', 'log', 'timer']

def config(name):
    try:
        return _sigyn.config(name)
    except:
        return None

