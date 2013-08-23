#ifndef _PCS_H
#define _PCS_H

#include <stdlib.h>

struct pcs_t
{
	char *server_url;
	char *access_token;
	char *refresh_token;
	char *mount_point;
};

int refresh_token();

#endif
