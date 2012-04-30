PLUGIN=python$(PLUGIN_SUFFIX)

SRCS = main.c

include ../../../extra.mk
include ../../../buildsys.mk

plugindir = $(MODDIR)/modules/scripting

CPPFLAGS += -I../../../include -I../../../libmowgli-2/src/libmowgli -I/usr/local/include/python2.7 -I/usr/local/include/python2.7 
CFLAGS += $(PLUGIN_CFLAGS) -DNDEBUG -fno-strict-aliasing
LDFLAGS += $(PLUGIN_LDFLAGS) -L/usr/local/lib/python2.7/config -lutil -lm -lpython2.7 -Wl,--export-dynamic
