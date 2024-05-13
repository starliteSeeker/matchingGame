GCC = gcc -std=c99 -Wall
LDFLAGS = -lncurses
EXE = cardMatching.out

SRCS = main.c graphics.c
OBJS = $(SRCS:%.c=%.o)

.c.o: 
	$(GCC) -c $*.c 

all: $(OBJS)
	$(GCC) $(OBJS) -o $(EXE) $(LDFLAGS)

clean:
	rm -f $(EXE) *.o
