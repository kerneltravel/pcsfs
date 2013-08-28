#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef LOGFILE
#define LOGFILE "/tmp/pcsfs.log"
#endif

void debugf(const char *s, ...)
{
	/* OK, I'm going to nail this damn bug for once and all... */
	/* Time to size the string by vfprint'ing it to /dev/null... */
	FILE *errfile;
	va_list ap;

	errfile = fopen(LOGFILE, "a");
	va_start(ap, s);
	vfprintf(errfile, s, ap);
	fclose(errfile);
	va_end(ap);
}
