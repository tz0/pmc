FLAGS = -O0 -g

SRCS = $(wildcard *.c)

PROGS = $(patsubst %.c,%,$(SRCS))
ALL_PROGS = $(PROGS)

all: $(PROGS) 

dump: $(DUMPS)

%: %.c $(HEADS)
	gcc $(FLAGS) -o $@ $< 

clean:
	rm -f $(ALL_PROGS)

