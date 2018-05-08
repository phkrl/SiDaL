include config.mk

SRC = sidal.c service.c utils.c
OBJ = ${SRC:.c=.o}

all: options sidal service

options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.mk

sidal: ${OBJ}
	${CC} -o $@ sidal.o utils.o

service: ${OBJ}
	${CC} -o $@ service.o utils.o

clean:
	rm -f sidal

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f sidal $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/svc

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/svc

.PHONY: all options clean install uninstall
