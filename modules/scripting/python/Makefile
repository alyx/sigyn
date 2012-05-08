PLUGIN=python$(PLUGIN_SUFFIX)

SRCS = main.c

include ../../../extra.mk
include ../../../buildsys.mk

plugindir = $(MODDIR)/modules/scripting

CFLAGS += -I../../../include -I../../../libmowgli-2/src/libmowgli $(PLUGIN_CFLAGS) $(PYTHON_CFLAGS)
LDFLAGS += $(PLUGIN_LDFLAGS) $(PYTHON_LIBS)
