# Copyright (c) 2003-2004 E. Will et al.
# Rights to this code are documented in doc/LICENSE.
#
# This file contains build instructions.
#
# $Id: Makefile.in 8375 2007-06-03 20:03:26Z pippijn $
#

MODULE = scripting

SRCS =		\
		python.c

include ../../extra.mk
include ../../buildsys.mk
include ../../buildsys.module.mk

CPPFLAGS	+= -I../../include -I../../libmowgli-2/src/libmowgli -I/usr/local/include/python2.7 -I/usr/local/include/python2.7
CFLAGS      += -DNDEBUG -fno-strict-aliasing
LDFLAGS     += -L/usr/local/lib/python2.7/config -lutil -lm -lpython2.7 -Wl,--export-dynamic
