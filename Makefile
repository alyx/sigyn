#CFLAGS=-c -Wall -Wextra -Werror -Iinclude -std=c99 -ggdb3
CFLAGS=-c -Wall -ggdb3 -Iinclude -Ibuild/object/include -D_MK_HOST
LDFLAGS=-lmowgli
#SOURCES=src/logger.c src/parser.c src/hostname.c tests/parse.c tests/log.c tests/hostname.c tests/test.c
#SOURCES=src/logger.c src/parser.c tests/parse_self.c
#atheme_string.c  connect.c        hostname.c       hostname.o       irc.c            logger.c         logger.o         parser.c         parser.o         platform.c       sigyn.c
#SOURCES=src/atheme_string.c src/connect.c src/irc.c src/logger.c src/parser.c src/sigyn.c
SOURCES=src/atheme_string.c src/connect.c src/irc.c src/logger.c src/parser.c src/module.c src/sigyn.c
OBJECTS=$(SOURCES:.c=.o)
EXE=build/sigyn

all: $(SOURCES) $(EXE)

$(EXE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm $(OBJECTS) $(EXE)
