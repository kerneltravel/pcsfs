#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <json.h>

#include "pcs.h"

static int parse_json_result(const char *result);

int pcs_refresh_token(void)
{
	char url[URL_MAXLEN];
	CURL *curl;
	CURLcode res;
	struct pcs_curl_buf buf;

	buf.size = 0;
	buf.buf = malloc(1);
	snprintf(url, URL_MAXLEN,
		 "%s?grant_type=refresh_token&refresh_token=%s&client_id=%s&client_secret=%s",
		 PCS_REFRESH_TOKEN, conf.refresh_token, PCS_CLIENT_ID,
		 PCS_CLIENT_SECRET);
	curl = curl_easy_init();
	if (curl) {
		int ret;
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
				 pcs_write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buf);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			debugf("refresh token error: %s\n",
				curl_easy_strerror(res));
		}
		curl_easy_cleanup(curl);
		debugf("%s\n", buf.buf);
		ret = parse_json_result(buf.buf);
		free(buf.buf);
		return ret;
	} else {
		perror("curl init error!\n");
		return 1;
	}
}

static int parse_json_result(const char *result)
{
	struct json_object *result_obj;

	result_obj = json_tokener_parse(result);

	if (!result_obj) {
		perror("parse json object error!\n");
		return 1;
	}

	free((void *)conf.access_token);
	conf.access_token = get_json_str(result_obj, ACCESS_TOKEN_KEY);
	free((void *)conf.refresh_token);
	conf.refresh_token = get_json_str(result_obj, REFRESH_TOKEN_KEY);

	json_object_put(result_obj);
	return 0;

}
