#ifndef _PCS_H
#define _PCS_H

#include <stdlib.h>
#include <json.h>
#include <sys/stat.h>
#include <fuse.h>

#define URL_MAXLEN 1024
#define PATH_MAXLEN 1024
#define MAX_RETRY_TIMES 3

#define ERROR_INVALID_ACCESS_TOKEN 110

#define PCS_CLIENT_ID "GLyCkBENOkwPeVxqBC1rkLQ4"
#define PCS_CLIENT_SECRET "G4B3gNPBAyELN0K8qkTYWKfEktv9S6Gr"
#define PCS_REFRESH_TOKEN "https://openapi.baidu.com/oauth/2.0/token"
#define PCS_GET_QUOTA "https://pcs.baidu.com/rest/2.0/pcs/quota"
#define PCS_FILE_OP "https://pcs.baidu.com/rest/2.0/pcs/file"

#define PCS_FILE_OP_MKDIR "mkdir"
#define PCS_FILE_OP_STAT "meta"
#define PCS_FILE_OP_LIST "list"
#define PCS_FILE_OP_MOVE "move"

#define ERROR_CODE_KEY "error_code"
#define ACCESS_TOKEN_KEY "access_token"
#define REFRESH_TOKEN_KEY "refresh_token"
#define QUOTA_QUOTA_KEY "quota"
#define QUOTA_USED_KEY "used"
#define META_PATH_KEY "path"
#define META_CTIME_KEY "ctime"
#define META_MTIME_KEY "mtime"
#define META_SIZE_KEY "size"
#define META_ISDIR_KEY "isdir"
#define META_HASSUBDIR_KEY "ifhassubdir"

#define PCS_PATH_PREFIX "/apps/fuse_pcs"

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

struct pcs_stat_t
{
	char path[PATH_MAXLEN];
	unsigned int ctime;
	unsigned int mtime;
	size_t size;
	char isdir;
	char hassubdir;
};


extern struct pcs_t conf;

size_t pcs_write_callback(void *contents, size_t size, size_t nmemb, void * userp);

char* get_json_str(struct json_object *jobj, const char *key);
size_t get_json_size_t(struct json_object *jobj, const char *key);
int get_json_int(struct json_object *jobj, const char *key);
unsigned int get_json_uint(struct json_object *jobj, const char *key);

int pcs_refresh_token(void);
struct pcs_quota_t* pcs_get_quota(void);
int pcs_mkdir(const char *path);
int pcs_stat(const char *path, struct pcs_stat_t *st);
int pcs_lsdir(const char *path, struct pcs_stat_t **st, size_t* nmemb);
int pcs_mv(const char *from, const char *to);

int pcsfs_getattr(const char *path, struct stat *stbuf);
int pcsfs_opendir(const char *path, struct fuse_file_info *fi);
int pcsfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, 
		off_t offset, struct fuse_file_info *fi);
int pcsfs_mkdir(const char *path, mode_t mode);
#endif
