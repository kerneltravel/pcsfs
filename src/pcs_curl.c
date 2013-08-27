#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pcs.h"

size_t pcs_write_callback(void *contents, size_t size, size_t nmemb,
			  void *userp)
{
	size_t realsize = size * nmemb;
	struct pcs_curl_buf *buf = (struct pcs_curl_buf *)userp;
	buf->buf = realloc(buf->buf, buf->size + realsize + 1);
	if (buf->buf == NULL) {
		perror("not enough memory (realloc returned NULL)\n");
		return 0;
	}
	memcpy(buf->buf + buf->size, contents, realsize);
	buf->size += realsize;
	buf->buf[buf->size] = 0;
	return realsize;
}
