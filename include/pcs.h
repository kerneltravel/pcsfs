#ifndef _PCS_H
#define _PCS_H

#include <stdlib.h>

struct pcs_t
{
	const char *access_token;
	const char *refresh_token;
	const char *mount_point;
};

int refresh_token();

#endif
