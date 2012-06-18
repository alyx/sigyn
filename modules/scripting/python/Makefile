PLUGIN=python$(PLUGIN_SUFFIX)

SRCS = main.c

include ../../../extra.mk
include ../../../buildsys.mk

plugindir = $(MODDIR)/modules/scripting

CFLAGS += -I../../../include -I../../../libmowgli-2/src/libmowgli $(PLUGIN_CFLAGS) $(PYTHON_CFLAGS) -DLIBDIR="\"$(libdir)\""
LDFLAGS += $(PLUGIN_LDFLAGS) $(PYTHON_LIBS)

install-extra:
	-$(INSTALL) -m 755 -d $(libdir)/python
	-$(INSTALL) -m 755 -d ./sigyn $(libdir)/python/sigyn
	-$(INSTALL) -m 600 -c ./sigyn/__init__.py $(libdir)/python/sigyn 
	-$(INSTALL) -m 600 -c ./sigyn/cmd.py $(libdir)/python/sigyn 
	-$(INSTALL) -m 600 -c ./sigyn/config.py $(libdir)/python/sigyn 
	-$(INSTALL) -m 600 -c ./sigyn/irc.py $(libdir)/python/sigyn 
	-$(INSTALL) -m 600 -c ./sigyn/log.py $(libdir)/python/sigyn 
	-$(INSTALL) -m 600 -c ./sigyn/timer.py $(libdir)/python/sigyn 
		
