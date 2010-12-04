MYCFLAGS= $(shell pkg-config --cflags cairo libpng) -fPIC -Wall -g -Wpointer-arith -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -fno-strict-aliasing
LDLIBS	= $(shell pkg-config --libs cairo libpng) -g -fPIC

NAME    = chartesque
PREFIX ?= /usr/local
MAJOR   = 0
VERSION = $(MAJOR).1.0
HEADER  = $(NAME).h
LIBRARY = lib$(NAME).so
OBJECTS = strlcpy.o dataplot.o axis.o
DEMOBJS = chartesque.o
PKGCONF = $(NAME).pc

all: $(PROGRAM)

%.o:%.c
	$(CC) $(CFLAGS) $(MYCFLAGS) -c $^

$(PKGCONF):
	echo "Name: chartesque" > $(PKGCONF)
	echo "Description: chart library based on Cairo" >> $(PKGCONF)
	echo "Requires.private: cairo" >> $(PKGCONF)
	echo "Version: $(VERSION)" >> $(PKGCONF)
	echo "Libs: -L$(PREFIX)/lib" >> $(PKGCONF)
	echo "Libs.private: -lm" >> $(PKGCONF)
	echo "Cflags: -I$(PREFIX)/includes" >> $(PKGCONF)

$(LIBRARY): $(OBJECTS)
	gcc -shared $(OBJECTS) $(LDLIBS) -o libchartesque.so

demo: $(DEMO) $(OBJECTS)

clean:
	rm -f $(PROGRAM) $(OBJECTS)

install: $(LIBRARY) $(PKGCONF)
	install -d $(PREFIX)/lib
	install -d $(PREFIX)/includes
	install -d $(PREFIX)/pkgconfig
	install $(LIBRARY) $(PREFIX)/lib/$(LIBRARY).$(VERSION)
	install $(HEADER) $(PREFIX)/includes/$(HEADER)
	install $(PKGCONF) $(PREFIX)/pkgconfig/$(PKGCONF)
	ln -sf $(LIBRARY).$(VERSION) $(PREFIX)/lib/$(LIBRARY).$(MAJOR)
	ln -sf $(LIBRARY).$(VERSION) $(PREFIX)/lib/$(LIBRARY)
	@echo
	@echo "    Don't forget to run ldconfig ;)"
	@echo
