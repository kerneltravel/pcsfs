#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <json.h>

#include "pcs.h"

static int parse_json_result(const char* result, struct pcs_quota_t *quota);



struct pcs_quota_t* pcs_get_quota(void)
{
	char *url;
	CURL *curl;
	CURLcode res;
	struct pcs_curl_buf buf;

	buf.size = 0;
	buf.buf = malloc(1);
	url = malloc(URL_MAXLEN);
	
	curl = curl_easy_init();
	if(curl){
		int ret;
		int retry_time = 0;
		struct pcs_quota_t *quota;
		
		quota = calloc(1, sizeof(struct pcs_quota_t));
REQUEST:
		snprintf(url, URL_MAXLEN, "%s?method=info&access_token=%s", 
				PCS_GET_QUOTA, conf.access_token);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, pcs_write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buf);
		res = curl_easy_perform(curl);
		if(res != CURLE_OK){
			fprintf(stderr, "get quota error: %s\n", 
					curl_easy_strerror(res));
		}
		printf("%s\n", buf.buf);
		ret = parse_json_result(buf.buf, quota);
		free(buf.buf);
		if(ret)
		{
			retry_time ++;
			if(retry_time > MAX_RETRY_TIMES || ret != 2){
				free(quota);
				free(url);
				curl_easy_cleanup(curl);
				return NULL;
			}else{
				pcs_refresh_token();
				buf.size = 0;
				buf.buf = malloc(1);
				goto REQUEST;
			}
		}
		curl_easy_cleanup(curl);
		free(url);
		return quota;
	}else{
		free(url);
		perror("curl init error!\n");
		return NULL;
	}
}


static int parse_json_result(const char* result, struct pcs_quota_t *quota)
{
	struct json_object *result_obj;
	int errcode;

	result_obj = json_tokener_parse(result);
	
	if (!result_obj){
		perror("parse json object error!\n");
		return 1;
	}

	errcode = get_json_int(result_obj, ERROR_CODE_KEY);
	if(errcode == ERROR_INVALID_ACCESS_TOKEN){
		json_object_put(result_obj);
		return 2;
	}
	
	quota->total = get_json_size_t(result_obj, QUOTA_QUOTA_KEY);
	quota->used = get_json_size_t(result_obj, QUOTA_USED_KEY);
	json_object_put(result_obj);
	return 0;

}
