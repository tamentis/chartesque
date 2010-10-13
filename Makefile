MYCFLAGS= $(shell pkg-config --cflags cairo libpng) -Wall -g -Wpointer-arith -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -fno-strict-aliasing
LDLIBS	= $(shell pkg-config --libs cairo libpng) -g

all: chartesque

%.o:%.c
	$(CC) $(CFLAGS) $(MYCFLAGS) -c $^

chartesque : chartesque.o strlcpy.o

clean :
	rm chartesque.o chartesque
