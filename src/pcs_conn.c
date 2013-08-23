#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <json.h>

#include "pcs.h"
#include "pcs_conn.h"

#define URL_MAXLEN 1024
#define PCS_CLIENT_ID "GLyCkBENOkwPeVxqBC1rkLQ4"
#define PCS_CLIENT_SECRET "G4B3gNPBAyELN0K8qkTYWKfEktv9S6Gr"
#define PCS_REFRESH_TOKEN "https://openapi.baidu.com/oauth/2.0/token"
#define PCS_GET_FILE_INFO "https://pcs.baidu.com/rest/2.0/pcs/file"
#define ACCESS_TOKEN_KEY "access_token"
#define REFRESH_TOKEN_KEY "refresh_token"

extern struct pcs_t *conf;

static size_t pcs_write_callback(void *contents, size_t size, size_t nmemb, void * userp)
{
	size_t realsize = size * nmemb;
	struct pcs_curl_buf *buf = (struct pcs_curl_buf *)userp;
	buf->buf = realloc(buf->buf, buf->size + realsize + 1);
	if(buf->buf == NULL){
		perror("not enough memory (realloc returned NULL)\n");
		return 0;
	}
	memcpy(buf->buf + buf->size, contents, realsize);
	buf->size += realsize;
	buf->buf[buf->size] = 0;
	return realsize;
}

int refresh_token()
{
	char *url;
	CURL *curl;
	CURLcode res;
	struct pcs_curl_buf buf;

	buf.size = 0;
	buf.buf = malloc(1);
	url = malloc(URL_MAXLEN);
	snprintf(url, URL_MAXLEN,
		 "%s?grant_type=refresh_token&refresh_token=%s&client_id=%s&client_secret=%s",
		 PCS_REFRESH_TOKEN, conf->refresh_token, PCS_CLIENT_ID,
		 PCS_CLIENT_SECRET);
	curl = curl_easy_init();
	if(curl){
		struct json_object *result_obj;
		struct json_object *access_token_obj, *refresh_token_obj;
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, pcs_write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buf);
		res = curl_easy_perform(curl);
		if(res != CURLE_OK){
			fprintf(stderr, "refresh token error: %s\n", 
					curl_easy_strerror(res));
		}
		curl_easy_cleanup(curl);
#if 1
		{
		FILE *fp;
		fp=fopen("/tmp/aaaa", "w");
		fprintf(fp, "%s\n", buf.buf);
		fclose(fp);
		}
#endif
		result_obj = json_tokener_parse(buf.buf);
		if (!result_obj){
			perror("parse json object error!\n");
			return 0;
		}
		access_token_obj = json_object_object_get(result_obj, ACCESS_TOKEN_KEY);
		if(access_token_obj){
			free((void *)conf->access_token);
			conf->access_token = strdup(json_object_get_string(access_token_obj));
			json_object_put(access_token_obj);
		}
		refresh_token_obj = json_object_object_get(result_obj, REFRESH_TOKEN_KEY);
		if(refresh_token_obj){
			free((void *)conf->refresh_token);
			conf->refresh_token = strdup(json_object_get_string(refresh_token_obj));
			json_object_put(refresh_token_obj);
		}
		json_object_put(result_obj);
		return 0;
	}else{
		perror("curl init error!\n");
		return 1;
	}
}
