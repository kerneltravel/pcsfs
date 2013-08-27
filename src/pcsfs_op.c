#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "pcs.h"

int pcsfs_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;
	
	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else{
		char pcs_path[URL_MAXLEN];
		struct pcs_stat_t st;
		int ret;
		snprintf(pcs_path, URL_MAXLEN, "%s%s", PCS_PATH_PREFIX, path);
		memset(&st, 0, sizeof(struct pcs_stat_t));
		ret = pcs_stat(pcs_path, &st);
		if(ret){
			res = -ENOENT;
		}else{
			if(st.isdir){
				stbuf->st_mode = S_IFDIR | 0755;
			}else{
				stbuf->st_mode = S_IFREG | 0644;
			}
			stbuf->st_size = st.size;
			stbuf->st_ctime = st.ctime;
			stbuf->st_mtime = st.mtime;
		}
	}
	return res;
}

int pcsfs_opendir(const char *path, struct fuse_file_info *fi)
{
	char pcs_path[URL_MAXLEN];
	struct pcs_stat_t st;
	int ret;
	
	snprintf(pcs_path, URL_MAXLEN, "%s%s", PCS_PATH_PREFIX, path);
	memset(&st, 0, sizeof(struct pcs_stat_t));
	ret = pcs_stat(pcs_path, &st);
	if(ret){
		return -ENOENT;
	}else{	
		return 0;
	}
}

int pcsfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info *fi)
{
	char pcs_path[URL_MAXLEN];
	struct pcs_stat_t *pcs_st;
	size_t nmemb;
	int ret;
	int i;
	
	snprintf(pcs_path, URL_MAXLEN, "%s%s", PCS_PATH_PREFIX, path);
	ret = pcs_lsdir(pcs_path, &pcs_st, &nmemb);
	if(ret){
		return -ENOENT;
	}
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	for(i = 0; i < nmemb; i ++){
		char *filename;
		struct stat st;
		memset(&st, 0, sizeof(st));
		filename = strrchr(pcs_st[i].path, '/') + 1;
		if(pcs_st[i].isdir){
			st.st_mode = S_IFDIR | 0755;
		}else{
			st.st_mode = S_IFREG | 0644;
		}
		st.st_size = pcs_st[i].size;
		st.st_ctime = pcs_st[i].ctime;
		st.st_mtime = pcs_st[i].mtime;
		filler(buf, filename, &st, 0);
	}
	free(pcs_st);
	return 0;
}

int pcsfs_mkdir(const char *path, mode_t mode)
{
	char pcs_path[URL_MAXLEN];
	snprintf(pcs_path, URL_MAXLEN, "%s%s", PCS_PATH_PREFIX, path);
	return pcs_mkdir(pcs_path);
}
