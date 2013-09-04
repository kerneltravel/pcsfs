#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <json.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "pcs.h"

static int parse_json_result(const char *result);
static int parse_json_stat_result(const char *result, struct pcs_stat_t *st);
static int parse_json_lsdir_result(const char *result, struct pcs_stat_t **st,
				   size_t * nmemb);

int pcs_mkdir(const char *path)
{
	char url[URL_MAXLEN];
	char *escaped_path;
	CURL *curl;
	CURLcode res;
	struct pcs_curl_buf buf;

	buf.size = 0;
	buf.buf = malloc(1);

	curl = curl_easy_init();
	if (curl) {
		int ret;
		int retry_time = 0;

 REQUEST:
		escaped_path = curl_easy_escape(curl, path, strlen(path));
		snprintf(url, URL_MAXLEN,
			 "%s?method=%s&access_token=%s&path=%s", PCS_FILE_OP,
			 PCS_FILE_OP_MKDIR, conf.access_token, escaped_path);
		debugf("mkdir_url:%s\n", url);
		curl_free(escaped_path);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
				 pcs_write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buf);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, NULL);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			debugf("mkdir error: %s\n", curl_easy_strerror(res));
		}
		debugf("%s\n", buf.buf);
		ret = parse_json_result(buf.buf);
		free(buf.buf);
		if (ret) {
			retry_time++;
			if (retry_time > MAX_RETRY_TIMES || ret != 2) {
				curl_easy_cleanup(curl);
				return 1;
			} else {
				pcs_refresh_token();
				buf.size = 0;
				buf.buf = malloc(1);
				goto REQUEST;
			}
		}
		curl_easy_cleanup(curl);
		return 0;
	} else {
		perror("curl init error!\n");
		return 1;
	}
}

int pcs_stat(const char *path, struct pcs_stat_t *st)
{
	char url[URL_MAXLEN];
	char *escaped_path;
	CURL *curl;
	CURLcode res;
	struct pcs_curl_buf buf;

	buf.size = 0;
	buf.buf = malloc(1);

	curl = curl_easy_init();
	if (curl) {
		int ret;
		int retry_time = 0;

 REQUEST:
		escaped_path = curl_easy_escape(curl, path, strlen(path));
		snprintf(url, URL_MAXLEN,
			 "%s?method=%s&access_token=%s&path=%s", PCS_FILE_OP,
			 PCS_FILE_OP_STAT, conf.access_token, escaped_path);
		debugf("stat_url:%s\n", url);
		curl_free(escaped_path);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
				 pcs_write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buf);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			debugf("stat error: %s\n", curl_easy_strerror(res));
		}
		debugf("%s\n", buf.buf);
		ret = parse_json_stat_result(buf.buf, st);
		debugf("return value:%d\n", ret);
		free(buf.buf);
		if (ret) {
			retry_time++;
			if (retry_time > MAX_RETRY_TIMES || ret != 2) {
				curl_easy_cleanup(curl);
				return 1;
			} else {
				pcs_refresh_token();
				buf.size = 0;
				buf.buf = malloc(1);
				goto REQUEST;
			}
		}
		curl_easy_cleanup(curl);
		return 0;
	} else {
		perror("curl init error!\n");
		return 1;
	}
}

int pcs_lsdir(const char *path, struct pcs_stat_t **st, size_t * nmemb)
{
	char url[URL_MAXLEN];
	char *escaped_path;
	CURL *curl;
	CURLcode res;
	struct pcs_curl_buf buf;

	buf.size = 0;
	buf.buf = malloc(1);

	curl = curl_easy_init();
	if (curl) {
		int ret;
		int retry_time = 0;

 REQUEST:
		escaped_path = curl_easy_escape(curl, path, strlen(path));
		snprintf(url, URL_MAXLEN,
			 "%s?method=%s&access_token=%s&path=%s", PCS_FILE_OP,
			 PCS_FILE_OP_LIST, conf.access_token, escaped_path);
		debugf("lsdir_url:%s\n", url);
		curl_free(escaped_path);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
				 pcs_write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buf);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			debugf("stat error: %s\n", curl_easy_strerror(res));
		}
		debugf("%s\n", buf.buf);
		ret = parse_json_lsdir_result(buf.buf, st, nmemb);
		free(buf.buf);
		if (ret) {
			retry_time++;
			if (retry_time > MAX_RETRY_TIMES || ret != 2) {
				curl_easy_cleanup(curl);
				return 1;
			} else {
				pcs_refresh_token();
				buf.size = 0;
				buf.buf = malloc(1);
				goto REQUEST;
			}
		}
		curl_easy_cleanup(curl);
		return 0;
	} else {
		perror("curl init error!\n");
		return 1;
	}
}

int pcs_mv(const char *from, const char *to)
{
	char url[URL_MAXLEN];
	char *escaped_from, *escaped_to;
	CURL *curl;
	CURLcode res;
	struct pcs_curl_buf buf;

	buf.size = 0;
	buf.buf = malloc(1);

	curl = curl_easy_init();
	if (curl) {
		int ret;
		int retry_time = 0;

 REQUEST:
		escaped_from = curl_easy_escape(curl, from, strlen(from));
		escaped_to = curl_easy_escape(curl, to, strlen(to));
		snprintf(url, URL_MAXLEN,
			 "%s?method=%s&from=%s&to=%s&access_token=%s",
			 PCS_FILE_OP, PCS_FILE_OP_MOVE, escaped_from,
			 escaped_to, conf.access_token);
		debugf("mv_url:%s\n", url);
		curl_free(escaped_from);
		curl_free(escaped_to);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
				 pcs_write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buf);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, NULL);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			debugf("mv error: %s\n", curl_easy_strerror(res));
		}
		debugf("%s\n", buf.buf);
		ret = parse_json_result(buf.buf);
		free(buf.buf);
		if (ret) {
			retry_time++;
			if (retry_time > MAX_RETRY_TIMES || ret != 2) {
				curl_easy_cleanup(curl);
				return 1;
			} else {
				pcs_refresh_token();
				buf.size = 0;
				buf.buf = malloc(1);
				goto REQUEST;
			}
		}
		curl_easy_cleanup(curl);
		return 0;
	} else {
		perror("curl init error!\n");
		return 1;
	}
}

int pcs_rm(const char *path)
{
	char url[URL_MAXLEN];
	char *escaped_path;
	CURL *curl;
	CURLcode res;
	struct pcs_curl_buf buf;

	buf.size = 0;
	buf.buf = malloc(1);

	curl = curl_easy_init();
	if (curl) {
		int ret;
		int retry_time = 0;

 REQUEST:
		escaped_path = curl_easy_escape(curl, path, strlen(path));
		snprintf(url, URL_MAXLEN,
			 "%s?method=%s&access_token=%s&path=%s", PCS_FILE_OP,
			 PCS_FILE_OP_DELETE, conf.access_token, escaped_path);
		debugf("rm_url:%s\n", url);
		curl_free(escaped_path);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
				 pcs_write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buf);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, NULL);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			debugf("delete error: %s\n", curl_easy_strerror(res));
		}
		debugf("%s\n", buf.buf);
		ret = parse_json_result(buf.buf);
		free(buf.buf);
		if (ret) {
			retry_time++;
			if (retry_time > MAX_RETRY_TIMES || ret != 2) {
				curl_easy_cleanup(curl);
				return 1;
			} else {
				pcs_refresh_token();
				buf.size = 0;
				buf.buf = malloc(1);
				goto REQUEST;
			}
		}
		curl_easy_cleanup(curl);
		return 0;
	} else {
		perror("curl init error!\n");
		return 1;
	}

}

int pcs_download(const char *path, const char *range, char *outbuf,
		 size_t * size)
{
	char url[URL_MAXLEN];
	char *escaped_path;
	CURL *curl;
	CURLcode res;
	struct pcs_curl_buf buf;
	long http_code;

	debugf("path:%s,range:%s,size:%ld\n", path, range, *size);

	buf.size = 0;
	buf.buf = malloc(1);

	curl = curl_easy_init();
	if (curl) {
		int ret;
		int retry_time = 0;

 REQUEST:
		escaped_path = curl_easy_escape(curl, path, strlen(path));
		snprintf(url, URL_MAXLEN,
			 "%s?method=%s&access_token=%s&path=%s",
			 PCS_FILE_DOWNLOAD, PCS_FILE_OP_DOWNLOAD,
			 conf.access_token, escaped_path);
		debugf("download_url:%s\n", url);
		curl_free(escaped_path);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
				 pcs_write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buf);
		curl_easy_setopt(curl, CURLOPT_RANGE, range);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			debugf("download error: %s\n", curl_easy_strerror(res));
			return 1;
		}
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		if (http_code != 200 && http_code != 206) {
			debugf("%s\n", buf.buf);
			ret = parse_json_result(buf.buf);
			free(buf.buf);
			if (ret) {
				retry_time++;
				if (retry_time > MAX_RETRY_TIMES || ret != 2) {
					curl_easy_cleanup(curl);
					return 1;
				} else {
					pcs_refresh_token();
					buf.size = 0;
					buf.buf = malloc(1);
					goto REQUEST;
				}
			} else {
				curl_easy_cleanup(curl);
				return 1;
			}
		}
		if (buf.size > *size) {
			free(buf.buf);
			return 1;
		}
		memcpy(outbuf, buf.buf, buf.size);
		*size = buf.size;
		free(buf.buf);
		curl_easy_cleanup(curl);
		return 0;
	} else {
		perror("curl init error!\n");
		return 1;
	}

}

int pcs_upload(const char *path, const char *inbuf, size_t size)
{
	char url[URL_MAXLEN];
	char tmpfile[] = "tmp_XXXXXX";
	char *escaped_path;
	CURL *curl;
	CURLcode res;
	struct pcs_curl_buf buf;
	struct curl_httppost *post = NULL;
	struct curl_httppost *last = NULL;
	int tmpfd;
	size_t has_write = 0;
	size_t ret;

	buf.size = 0;
	buf.buf = malloc(1);
	if ((tmpfd = mkstemp(tmpfile)) == -1) {
		free(buf.buf);
		return 1;
	}
	while (has_write < size) {
		ret = write(tmpfd, inbuf + has_write, size - has_write);
		if (ret == -1) {
			free(buf.buf);
			close(tmpfd);
			unlink(tmpfile);
			return;
		}
		has_write += ret;
	}
	close(tmpfd);
	curl = curl_easy_init();
	if (curl) {
		int ret;
		int retry_time = 0;

 REQUEST:
		escaped_path = curl_easy_escape(curl, path, strlen(path));
		snprintf(url, URL_MAXLEN,
			 "%s?method=%s&access_token=%s&path=%s&ondup=overwrite",
			 PCS_FILE_UPLOAD, PCS_FILE_OP_UPLOAD, conf.access_token,
			 escaped_path);
		debugf("upload_url:%s\n", url);
		curl_free(escaped_path);

		curl_formadd(&post, &last,
			     CURLFORM_PTRNAME, "file",
			     CURLFORM_FILE, tmpfile, CURLFORM_END);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
				 pcs_write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buf);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
		res = curl_easy_perform(curl);
		curl_formfree(post);
		if (res != CURLE_OK) {
			debugf("upload error: %s\n", curl_easy_strerror(res));
		}
		debugf("%s\n", buf.buf);
		ret = parse_json_result(buf.buf);
		free(buf.buf);
		if (ret) {
			retry_time++;
			if (retry_time > MAX_RETRY_TIMES || ret != 2) {
				curl_easy_cleanup(curl);
				unlink(tmpfile);
				return 1;
			} else {
				pcs_refresh_token();
				buf.size = 0;
				buf.buf = malloc(1);
				goto REQUEST;
			}
		}
		curl_easy_cleanup(curl);
		unlink(tmpfile);
		return 0;
	} else {
		perror("curl init error!\n");
		unlink(tmpfile);
		return 1;
	}
}

#define handle_error() do{\
	if (!result_obj){\
		debugf("parse json object error!\n");\
		return 1;\
	}\
	errcode = get_json_int(result_obj, ERROR_CODE_KEY);\
	debugf("error_code:%d\n",errcode);\
	if( errcode == ERROR_INVALID_ACCESS_TOKEN){\
		json_object_put(result_obj);\
		return 2;\
	}else if(errcode != 0){\
		json_object_put(result_obj);\
		return 1;\
	}\
} while(0)

static int parse_json_result(const char *result)
{
	struct json_object *result_obj;
	int errcode;

	result_obj = json_tokener_parse(result);
	handle_error();
	json_object_put(result_obj);
	return 0;
}

static int parse_json_stat_result(const char *result, struct pcs_stat_t *st)
{
	struct json_object *result_obj;
	int errcode;
	char *path;
	struct json_object *list;
	struct json_object *first_obj;

	result_obj = json_tokener_parse(result);
	handle_error();
	list = json_object_object_get(result_obj, "list");
	if (list == NULL) {
		json_object_put(result_obj);
		return 1;
	}
	first_obj = json_object_array_get_idx(list, 0);
	if (first_obj == NULL) {
		json_object_put(list);
		json_object_put(result_obj);
		return 1;
	}

	path = get_json_str(first_obj, META_PATH_KEY);
	if (path) {
		snprintf(st->path, sizeof(st->path), "%s", path);
		free(path);
	}
	st->ctime = get_json_uint(first_obj, META_CTIME_KEY);
	st->mtime = get_json_uint(first_obj, META_MTIME_KEY);
	st->size = get_json_size_t(first_obj, META_SIZE_KEY);
	st->isdir = (char)get_json_int(first_obj, META_ISDIR_KEY);
	st->hassubdir = (char)get_json_int(first_obj, META_HASSUBDIR_KEY);

	json_object_put(first_obj);
	json_object_put(list);
	json_object_put(result_obj);

	debugf("file path:%s\n", st->path);

	return 0;
}

static int parse_json_lsdir_result(const char *result, struct pcs_stat_t **st,
				   size_t * nmemb)
{
	struct json_object *result_obj;
	int errcode;
	struct json_object *list;
	int i;

	result_obj = json_tokener_parse(result);
	handle_error();
	list = json_object_object_get(result_obj, "list");
	if (list == NULL) {
		json_object_put(result_obj);
		return 1;
	}
	*nmemb = json_object_array_length(list);
	if (*nmemb == 0) {
		json_object_put(list);
		json_object_put(result_obj);
		return 0;
	}
	*st = (struct pcs_stat_t *)calloc(*nmemb, sizeof(struct pcs_stat_t));
	for (i = 0; i < *nmemb; i++) {
		struct json_object *tmp;
		char *path;

		tmp = json_object_array_get_idx(list, i);
		path = get_json_str(tmp, META_PATH_KEY);
		if (path) {
			snprintf((*st)[i].path, sizeof((*st)[i].path), "%s",
				 path);
			free(path);
		}
		(*st)[i].ctime = get_json_uint(tmp, META_CTIME_KEY);
		(*st)[i].mtime = get_json_uint(tmp, META_MTIME_KEY);
		(*st)[i].size = get_json_size_t(tmp, META_SIZE_KEY);
		(*st)[i].isdir = (char)get_json_int(tmp, META_ISDIR_KEY);
		debugf("file path:%s\n", (*st)[i].path);
		json_object_put(tmp);
	}

	json_object_put(list);
	json_object_put(result_obj);
	return 0;
}
