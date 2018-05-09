#include <error.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void die(const char *errstr, ...);
char *smprintf(char *fmt, ...);
pid_t spawn(char *const []);
