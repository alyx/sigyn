CFLAGS=-c -Wall -Wextra -Werror -Iinclude -std=c99 -pedantic-errors -ggdb3
LDFLAGS=-lmowgli
SOURCES=src/logger.c src/parser.c src/hostname.c tests/parse.c tests/log.c tests/hostname.c tests/test.c
#SOURCES=src/logger.c src/parser.c tests/parse_self.c
OBJECTS=$(SOURCES:.c=.o)
EXE=build/test

all: $(SOURCES) $(EXE)

$(EXE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm $(OBJECTS) $(EXE)
