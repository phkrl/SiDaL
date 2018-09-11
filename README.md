# sidal
sidal is service launcher based on svc from git.r-36.net/svc rewritten in C

Usage:

sidal [acefklmprsuw] [cmd] services

the first argument is command string. it consists of following commands:

a - link all given services to /etc/sidal/run (or list /etc/sidal/avail if no services given). Services are searched in /etc/sidal/avail only.
c - prepare directories (/etc/sidal/avail, /etc/sidal/run, /etc/sidal/default, /run/sidal)
e - set custom arguments to all services in the list. The new command line should be second argument then
f - do not start services if there are links in /run/sidal; do not kill services if there are no links in /run/sidal
k - kill services. Services are searched in /run/sidal and /etc/sidal/run only
l - list /etc/sidal/run
m - link started services to /run/sidal or remove links of killed services
p - run services in parallel
r - restart services. Services are searched in /run/sidal and /etc/sidal/run only.
s - start services. If no services given, list /run/sidal. Services are searched in /etc/sidal/run, /etc/sidal/avail all *bin directories, as absolute path and in .
u - remove links from /etc/sidal/run
w - wait until job finishes before processing next service

You may use relative paths as services
