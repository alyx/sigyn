CC=clang
CFLAGS=-c -Wall -Werror -Iinclude -std=c99 -ggdb
LDFLAGS=
#SOURCES=src/parser.c tests/parse.c tests/log.c tests/test.c
SOURCES=src/atheme_string.c tests/string.c
#SOURCES=tests/log.c
OBJECTS=$(SOURCES:.c=.o)
EXE=build/string

all: $(SOURCES) $(EXE)

$(EXE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm $(OBJECTS) $(EXE)
