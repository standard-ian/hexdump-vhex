#ian leuty CS205 lab3 ileuty@pdx.edu 10/24/2024

CC = gcc
DEFINES =
DEBUG = -g3 -O0
WERROR = -Werror
CFLAGS = -Wall -Wshadow -Wunreachable-code -Wredundant-decls \
	-Wmissing-declarations -Wold-style-definition -Wmissing-prototypes \
	-Wdeclaration-after-statement -Wextra -Werror -Wpedantic \
	-Wno-return-local-addr -Wunsafe-loop-optimizations -Wuninitialized\
	$(WERROR) $(DEBUG) $(DEFINES)

LDFLAGS = 

PROG1 = vhex

PROGS = $(PROG1) $(PROG2) $(PROG3)

all: $(PROGS)

$(PROG1): $(PROG1).o 
	$(CC) $(LDFLAGS) $(PROG1).o -o $(PROG1)
	chmod og+x $(PROG1)

$(PROG1).o: $(PROG1).c #$(PROG1).h
	$(CC) $(CFLAGS) -c $(PROG1).c -o $(PROG1).o

clean cls:
	rm -f $(PROGS) *.o *~ \#*

TAR_FILE = ${LOGNAME}_$(PROG1).tar.gz

tar:
	rm -f $(TAR_FILE)
	tar cvaf $(TAR_FILE) *.[ch] [Mm]akefile
	tar tvaf $(TAR_FILE)

git:
	if [ ! -d .git ] ; then git init; fi
	git add *.[ch] ?akefile
	git commit
	git push
