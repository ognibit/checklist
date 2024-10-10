# make APPNAME=myapp

# default configuration
APPNAME ?= checklist

CC=gcc
CFLAGS=-Wall -Wextra -pedantic -ggdb -fsanitize=undefined -DAPPNAME=$(APPNAME)
LDFLAGS=-I./src/
LDTESTFLAGS=$(LDFLAGS)

SRCDIR=src
TESTDIR=tests
OBJDIR=build
BIN=build/$(APPNAME).elf

TESTSRC=$(wildcard $(TESTDIR)/test_*.c)
TESTELF=${TESTSRC:.c=.elf}
SRCS=$(wildcard $(SRCDIR)/*.c)
OBJS=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))
HEADERS=$(wildcard $(SRCDIR)/*.h)

all: $(OBJDIR) $(BIN)

release: CFLAGS=-Wall -O2 -DNDEBUG
release: clean
release: $(BIN)

$(BIN): $(OBJS) main.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

tests: $(TESTELF) 
	#@run-parts --regex '.*\.elf' $(TESTDIR)/
	echo "NOT IMPLEMENTED"

$(TESTDIR)/%.elf: $(TESTDIR)/%.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDTESTFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ -c $< $(LDFLAGS)

clean:
	rm -f ./$(OBJDIR)/*.o ./$(OBJDIR)/*.elf ./$(BIN)

$(OBJDIR):
	mkdir -p $@

