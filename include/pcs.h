#ifndef _PCS_H
#define _PCS_H

#include <stdlib.h>
#include <json.h>

#define URL_MAXLEN 1024
#define MAX_RETRY_TIMES 3

#define ERROR_INVALID_ACCESS_TOKEN 110

#define PCS_CLIENT_ID "GLyCkBENOkwPeVxqBC1rkLQ4"
#define PCS_CLIENT_SECRET "G4B3gNPBAyELN0K8qkTYWKfEktv9S6Gr"
#define PCS_REFRESH_TOKEN "https://openapi.baidu.com/oauth/2.0/token"
#define PCS_GET_QUOTA "https://pcs.baidu.com/rest/2.0/pcs/quota"
#define PCS_GET_FILE_INFO "https://pcs.baidu.com/rest/2.0/pcs/file"


#define ERROR_CODE_KEY "error_code"
#define ACCESS_TOKEN_KEY "access_token"
#define REFRESH_TOKEN_KEY "refresh_token"
#define QUOTA_QUOTA_KEY "quota"
#define QUOTA_USED_KEY "used"

struct pcs_t
{
	const char *access_token;
	const char *refresh_token;
	const char *mount_point;
};

struct pcs_curl_buf
{
	size_t size;
	char *buf;
};

struct pcs_quota_t
{
	size_t used;
	size_t total;
};


extern struct pcs_t *conf;

int pcs_refresh_token(void);
size_t pcs_write_callback(void *contents, size_t size, size_t nmemb, void * userp);
char* get_json_str(struct json_object *jobj, const char *key);
struct pcs_quota_t* pcs_get_quota(void);
size_t get_json_size_t(struct json_object *jobj, const char *key);
size_t get_json_int(struct json_object *jobj, const char *key);
#endif
