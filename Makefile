MYCFLAGS= $(shell pkg-config --cflags cairo libpng) -Wall -g -Wpointer-arith -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -fno-strict-aliasing
LDLIBS	= $(shell pkg-config --libs cairo libpng) -g

PROGRAM = chartesque
OBJECTS = chartesque.o strlcpy.o dataplot.o axis.o

all: $(PROGRAM)

%.o:%.c
	$(CC) $(CFLAGS) $(MYCFLAGS) -c $^

chartesque: $(OBJECTS)

clean:
	rm -f $(PROGRAM) $(OBJECTS)
