#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fuse.h>

#include "pcs.h"

#define ARG_FORMAT "a:r:h"

struct pcs_t conf;

static struct fuse_operations pcs_handler = {
	.getattr	= pcs_getattr,
	.opendir	= pcs_opendir,
	.readdir	= pcs_readdir,
	.mkdir		= NULL,
	.unlink		= NULL,
	.rmdir		= NULL,
	.rename		= NULL,
	.link		= NULL,
	.truncate	= NULL,
	.create		= NULL,
	.open		= NULL,
	.read		= NULL,
	.write		= NULL,
	.flush		= NULL,
	.release	= NULL,
};

static void usage(char *argv0, int status)
{
	fprintf(stderr,
		"usage: %s -a access_token -r refresh_token mountpoint\n",
		argv0);
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
			conf.access_token = strdup(optarg);
			break;
		case 'r':
			conf.refresh_token = strdup(optarg);
			break;
		case 'h':
			usage(argv[0], EXIT_SUCCESS);
			break;
		default:
			usage(argv[0], EXIT_FAILURE);
		}
	}
	if (!conf.access_token || !conf.refresh_token) {
		usage(argv[0], EXIT_FAILURE);
	}
	if (argc - optind != 1) {
		usage(argv[0], EXIT_FAILURE);
	}
	conf.mount_point = argv[optind];
}

int main(int argc, char **argv)
{
	int ret;
	struct fuse_args fargs = FUSE_ARGS_INIT(0, NULL);
	memset(&conf, 0, sizeof(struct pcs_t));
	parse_arg(argc, argv);
	fuse_opt_add_arg(&fargs, argv[0]);
	fuse_opt_add_arg(&fargs, conf.mount_point);
	return fuse_main(fargs.argc, fargs.argv, &pcs_handler, NULL);
}
