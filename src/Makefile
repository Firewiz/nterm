SRCS=evloop.c nterm.c gui.c term.o
OBJS=$(subst .c,.o,$(SRCS))
PROG=nterm

CFLAGS=-g
LDFLAGS=
LIBS=-lX11

$(PROG):$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROG) $(LDFLAGS) $(LIBS)
