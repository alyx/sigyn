PLUGIN=voicebot$(PLUGIN_SUFFIX)

SRCS = main.c

include ../../../extra.mk
include ../../../buildsys.mk

plugindir = $(MODDIR)/modules/contrib

CPPFLAGS += -I../../../include -I..
CFLAGS += $(PLUGIN_CFLAGS)
LDFLAGS += $(PLUGIN_LDFLAGS)

