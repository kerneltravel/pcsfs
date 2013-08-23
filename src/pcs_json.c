#include <json.h>
#include <string.h>
#include <stdlib.h>

char* get_json_str(struct json_object *jobj, const char *key)
{
	struct json_object *tmp;
	tmp = json_object_object_get(jobj, key);
	if(tmp){
		char* ret = strdup(json_object_get_string(tmp));
		json_object_put(tmp);
		return ret;
	}else{
		return NULL;
	}
}

size_t get_json_size_t(struct json_object *jobj, const char *key)
{
	struct json_object *tmp;
	tmp = json_object_object_get(jobj, key);
	if(tmp){
		size_t ret = (size_t)json_object_get_int64(tmp);
		json_object_put(tmp);
		return ret;
	}else{
		return 0;
	}
}

int get_json_int(struct json_object *jobj, const char *key)
{
	struct json_object *tmp;
	tmp = json_object_object_get(jobj, key);
	if(tmp){
		size_t ret = (size_t)json_object_get_int(tmp);
		json_object_put(tmp);
		return ret;
	}else{
		return 0;
	}
}
