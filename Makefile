include config.mk

SRC = sidal.c utils.c
OBJ = ${SRC:.c=.o}

all: options sidal

options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.mk

sidal: ${OBJ}
	${CC} -o $@ sidal.o utils.o

clean:
	rm -f sidal 

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f sidal $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/sidal
	mkdir -p $(DESTDIR)$(MANPREFIX)/man8
	sed "s/VERSION/$(VERSION)/g" < sidal.8 > $(DESTDIR)$(MANPREFIX)/man8/sidal.8
	chmod 644 $(DESTDIR)$(MANPREFIX)/man8/sidal.8
	rm -f $(DESTDIR)$(MANPREFIX)/man1/st.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/sidal
	rm -f $(DESTDIR)$(MANPREFIX)/man8/sidal.8

.PHONY: all options clean install uninstall
