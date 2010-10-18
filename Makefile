MYCFLAGS= $(shell pkg-config --cflags cairo libpng) -fPIC -Wall -g -Wpointer-arith -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -fno-strict-aliasing
LDLIBS	= $(shell pkg-config --libs cairo libpng) -g -fPIC

PROGRAM = chartesque
OBJECTS = strlcpy.o dataplot.o axis.o
DEMO    = chartesque.o

all: $(PROGRAM)

%.o:%.c
	$(CC) $(CFLAGS) $(MYCFLAGS) -c $^

libchartesque.so: $(OBJECTS)
	gcc -shared $(OBJECTS) $(LDLIBS) -o chartesque.so

chartesque: $(DEMO) $(OBJECTS)

clean:
	rm -f $(PROGRAM) $(OBJECTS)
