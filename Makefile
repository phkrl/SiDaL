DESTDIR = /usr
PREFIX = /local
all: svc

svc:
	gcc -o svc svc.c

clean:
	rm -f svc svc-$(VERSION).tar.gz

install: svc
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f svc $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/svc

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/svc

.PHONY: all options clean dist install uninstall
