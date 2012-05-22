PROG		= sigyn${PROG_SUFFIX}

BASE_SRCS = \
	base64.c \
	config.c \
	daemonise.c \
	glob.c \
	irc.c \
	parser.c \
	signal.c \
	string.c \
	tokenize.c \
	cap.c     \
	command.c \
	io.c      \
	interface.c \
	logger.c \
	module.c \
	queue.c \
	sigyn.c \
	$(STACKTRACE_ENABLE) \
    channel.c

SRCS = ${BASE_SRCS}

include ../extra.mk
include ../buildsys.mk

CPPFLAGS	+= $(MOWGLI_CFLAGS) -I../include -DBINDIR=\"$(bindir)\"
LIBS		+= $(MOWGLI_LIBS) $(RPATH) $(PROG_IMPLIB_LDFLAGS)

build: all

install-extra:
	-$(INSTALL) -m 755 -d $(DESTDIR)$(prefix)
	-$(INSTALL) -m 755 -d $(DESTDIR)$(bindir)
	-$(INSTALL) -m 755 -d $(DESTDIR)$(sysconfdir)
	-$(INSTALL) -m 755 -d $(DESTDIR)$(localstatedir)
	-$(INSTALL) -m 755 -d $(DESTDIR)$(DOCDIR)
	-$(INSTALL) -m 755 -d $(DESTDIR)$(LOGDIR)
	-$(INSTALL) -m 755 -d $(DESTDIR)$(RUNDIR)
	-$(INSTALL) -m 755 -d $(DESTDIR)$(DATADIR)
	-$(INSTALL) -m 755 -d $(DESTDIR)$(SHAREDIR)/help
	$(INSTALL) -m 600 -c ../dist/conf.example $(DESTDIR)$(sysconfdir)

include .deps
