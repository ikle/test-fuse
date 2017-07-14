TARGETS = cfs

all: $(TARGETS)

clean:
	rm -f *.o $(TARGETS)

PREFIX ?= /usr/local

install: $(TARGETS)
	install -D -d $(DESTDIR)/$(PREFIX)/bin
	install -s -m 0755 $^ $(DESTDIR)/$(PREFIX)/bin

cfs: CFLAGS += `pkg-config fuse --cflags --libs`

test: cfs
	mkdir -p root
	./cfs -o allow_other root
