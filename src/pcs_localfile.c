#include <openssl/md5.h>
#include <string.h>

#include "pcs.h"

#define LOCAL_TMP_FILE_PREFIX "/tmp/"
#define LOCAL_TMP_FILE_POSTFIX ".pcstmp"


int pcs_localpath(const char* path, char *localpath, size_t len)
{
	unsigned char digest[MD5_DIGEST_LENGTH];
	int i;
	int l;
	if(len < strlen(LOCAL_TMP_FILE_PREFIX) + strlen(LOCAL_TMP_FILE_POSTFIX) + MD5_DIGEST_LENGTH * 2){
		return 1;
	}
	snprintf(localpath, len, "%s", LOCAL_TMP_FILE_PREFIX);
	MD5(path, strlen(path), digest);
	l = strlen(localpath);
	for(i = 0; i < MD5_DIGEST_LENGTH; i ++){
		snprintf(localpath + l, len - l, "%s%02x", localpath, digest[i]);
		l+=2;
	}
	snprintf(localpath + l, len -l , "%s%s", localpath, LOCAL_TMP_FILE_POSTFIX);
	return 0;
}
