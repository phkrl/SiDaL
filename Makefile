include config.mk

SRC = svc.c utils.c
OBJ = ${SRC:.c=.o}

all: options svc

options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

svc:
	${CC} -o $@ ${OBJ}

clean:
	rm -f svc svc-$(VERSION).tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p svc-${VERSION}
	@cp -R LICENSE Makefile README.md config.mk \
		util.h ${SRC} svc-${VERSION}
	@tar -cf svc-${VERSION}.tar svc-${VERSION}
	@gzip svc-${VERSION}.tar
	@rm -rf svc-${VERSION}

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f svc $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/svc

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/svc

.PHONY: all options clean dist install uninstall
