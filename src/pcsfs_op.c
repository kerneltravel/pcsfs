#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "pcs.h"

int pcsfs_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else {
		char pcs_path[URL_MAXLEN];
		struct pcs_stat_t st;
		int ret;
		snprintf(pcs_path, URL_MAXLEN, "%s%s", PCS_PATH_PREFIX, path);
		memset(&st, 0, sizeof(struct pcs_stat_t));
		ret = pcs_stat(pcs_path, &st);
		if (ret) {
			res = -ENOENT;
		} else {
			if (st.isdir) {
				stbuf->st_mode = S_IFDIR | 0755;
			} else {
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
	if (ret || !st.isdir) {
		return -ENOENT;
	} else {
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
	if (ret) {
		return -ENOENT;
	}
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	for (i = 0; i < nmemb; i++) {
		char *filename;
		struct stat st;
		memset(&st, 0, sizeof(st));
		filename = strrchr(pcs_st[i].path, '/') + 1;
		if (pcs_st[i].isdir) {
			st.st_mode = S_IFDIR | 0755;
		} else {
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

int pcsfs_unlink(const char *path)
{
	char pcs_path[URL_MAXLEN];
	snprintf(pcs_path, URL_MAXLEN, "%s%s", PCS_PATH_PREFIX, path);
	return pcs_rm(pcs_path);
}

int pcsfs_rmdir(const char *path)
{
	char pcs_path[URL_MAXLEN];
	snprintf(pcs_path, URL_MAXLEN, "%s%s", PCS_PATH_PREFIX, path);
	return pcs_rm(pcs_path);
}

int pcsfs_rename(const char *from, const char *to)
{
	char pcs_from[URL_MAXLEN], pcs_to[URL_MAXLEN];
	snprintf(pcs_from, URL_MAXLEN, "%s%s", PCS_PATH_PREFIX, from);
	snprintf(pcs_to, URL_MAXLEN, "%s%s", PCS_PATH_PREFIX, to);
	return pcs_mv(pcs_from, pcs_to);
}

int pcsfs_statfs(const char *path, struct statvfs *stvfs)
{
	int ret;
	struct pcs_quota_t quota;

	ret = pcs_get_quota(&quota);
	if (ret) {
		return 1;
	}

	stvfs->f_bsize = 1;
	stvfs->f_frsize = 1;
	stvfs->f_blocks = quota.total;
	stvfs->f_bfree = quota.total - quota.used;
	stvfs->f_bavail = quota.total - quota.used;
	return 0;
}

int pcsfs_open(const char *path, struct fuse_file_info *fi)
{
	char pcs_path[URL_MAXLEN];
	struct pcs_stat_t st;
	int ret;

	snprintf(pcs_path, URL_MAXLEN, "%s%s", PCS_PATH_PREFIX, path);
	memset(&st, 0, sizeof(struct pcs_stat_t));
	ret = pcs_stat(pcs_path, &st);
	if (ret || st.isdir) {
		return -ENOENT;
	} else {
		return 0;
	}
	return 0;
}

int pcsfs_read(const char *path, char *buf, size_t size, off_t offset,
	       struct fuse_file_info *fi)
{
	char pcs_path[URL_MAXLEN];
	char pcs_range[URL_MAXLEN];
	int ret;
	size_t read_size = size;

	snprintf(pcs_path, URL_MAXLEN, "%s%s", PCS_PATH_PREFIX, path);
	snprintf(pcs_range, URL_MAXLEN, "%ld-%ld", offset, offset + size - 1);
	ret = pcs_download(pcs_path, pcs_range, buf, &read_size);
	if (ret) {
		return -1;
	} else {
		return read_size;
	}
}

int pcsfs_write(const char *path, const char *buf, size_t size,
		off_t offset, struct fuse_file_info *fi)
{
	char pcs_path[URL_MAXLEN];
	char localpath[URL_MAXLEN];
	struct stat st;
	int ret;
	snprintf(pcs_path, URL_MAXLEN, "%s%s", PCS_PATH_PREFIX, path);
	if(pcs_localpath(pcs_path, localpath, URL_MAXLEN)){
		return -1;
	}
	ret = lstat(localpath, &st);
	if(!ret && S_IFREG(st)){
		int fd;
		size_t has_write = 0;
		if((fd = open(localpath, O_WRONLY)) < 0){
			return -1;
		}
		lseek(fd, offset, SEEK_SET);
		while(has_write < size){
			has_write += write(fd, buf + has_write, size - has_write);
		}
		close(fd);
	}else{
		
	}
}

int pcsfs_flush(const char *path, struct fuse_file_info *fi)
{
	char pcs_path[URL_MAXLEN];
	char localpath[URL_MAXLEN];
	int ret;
	snprintf(pcs_path, URL_MAXLEN, "%s%s", PCS_PATH_PREFIX, path);
	if(pcs_localpath(pcs_path, localpath, URL_MAXLEN)){
		return -1;
	}
	ret = pcs_upload(pcs_path, localpath);
	if (ret) {
		return -1;
	} else {
		return 0;
	}
}

int pcsfs_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	char pcs_path[URL_MAXLEN];
	snprintf(pcs_path, URL_MAXLEN, "%s%s", PCS_PATH_PREFIX, path);
	return pcs_upload(pcs_path, NULL, 0);
}
