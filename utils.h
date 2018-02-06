#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <error.h>
#include <stdarg.h>
#include <unistd.h>

void die(const char *errstr, ...);
char *smprintf(char *fmt, ...);
void spawn(char *const []);
