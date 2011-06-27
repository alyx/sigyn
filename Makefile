CC=clang
CFLAGS=-c -Wall -Werror -Iinclude -std=c99 -ggdb
LDFLAGS=
SOURCES=src/parser.c tests/parse.c tests/log.c tests/test.c
OBJECTS=$(SOURCES:.c=.o)
EXE=build/test

all: $(SOURCES) $(EXE)

$(EXE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm $(OBJECTS) $(EXE)
