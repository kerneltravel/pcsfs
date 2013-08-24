#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "pcs.h"

#define ARG_FORMAT "a:r:h"

struct pcs_t *conf = NULL;

static void usage(char *argv0, int status)
{
	fprintf(stderr,
		"usage: %s -a access_token -r refresh_token mountpoint\n",
		argv0);
	free(conf);
	exit(status);
}

static void parse_arg(int argc, char **argv)
{
	extern int optind;
	int ch;
	if (argc < 3) {
		usage(argv[0], EXIT_FAILURE);
	}
	while ((ch = getopt(argc, argv, ARG_FORMAT)) != -1) {
		switch (ch) {
		case 'a':
			conf->access_token = strdup(optarg);
			break;
		case 'r':
			conf->refresh_token = strdup(optarg);
			break;
		case 'h':
			usage(argv[0], EXIT_SUCCESS);
			break;
		default:
			usage(argv[0], EXIT_FAILURE);
		}
	}
	if (!conf->access_token || !conf->refresh_token) {
		usage(argv[0], EXIT_FAILURE);
	}
	if (argc - optind != 1) {
		usage(argv[0], EXIT_FAILURE);
	}
	conf->mount_point = argv[optind];
}

int main(int argc, char **argv)
{
	int ret;
/*	
 	extern int errno;
	if((ret = daemon(1, 1)) == -1){
		perror(strerror(errno));
	}
*/
	conf = calloc(1, sizeof(struct pcs_t));
	if (!conf) {
		perror("no memory!\n");
		exit(EXIT_FAILURE);
	}
	parse_arg(argc, argv);
	pcs_stat("/apps/fuse_pcs/test");
	free(conf);
	return 0;
}
