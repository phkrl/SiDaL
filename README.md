# svc
svc from git.r-36.net/svc rewritten in C
To be extended

Usage:
svc [-adklrsh] [service]
-a - list all links in /etc/svc/run (all activated services)
-a [ser] - activate service »ser« to be run on startup (create link in /etc/svc/run)
-c - prepare svc directories (/etc/svc/avail, /etc/svc/run, /run/svc)
-d [ser] - deactivate service »ser«
-k - kill all services
-k [ser] - kill the service »ser«
-l - list all services in /etc/svc/avail
-r [ser] - restart service »ser«
-s - run all services
-s [ser] - run service »ser«
