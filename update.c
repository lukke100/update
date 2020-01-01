#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

bool timegt(struct timespec left, struct timespec right)
{
	if (left.tv_sec != right.tv_sec)
		return left.tv_sec > right.tv_sec;
	else
		return left.tv_nsec > right.tv_nsec;
}

int main(int argc, char *argv[])
{
	struct stat stbuf;
	struct timespec pmtime, tmtime;
	char *progname;
	int option;
	bool pfound, terror, tfound;

	progname = basename(argv[0]);
	pfound = terror = tfound = false;

	while (option = getopt(argc, argv, ":p:t:"), option != -1) {
		switch (option) {
		case 'p':
			if (stat(optarg, &stbuf)) {
				fprintf(stderr, "%s: %s: %s\n", progname,
				        optarg, strerror(errno));
				return EXIT_FAILURE;
			}
			if (!pfound || timegt(stbuf.st_mtim, pmtime)) {
				pmtime = stbuf.st_mtim;
				pfound = true;
			}
			break;
		case 't':
			if (stat(optarg, &stbuf)) {
				terror = true;
			} else if (!tfound || timegt(tmtime, stbuf.st_mtim)) {
				tmtime = stbuf.st_mtim;
				tfound = true;
			}
			break;
		case ':':
			fprintf(stderr,
			        "%s: option requires an argument -- %c\n",
			        progname, optopt);
			return EXIT_FAILURE;
		default:
			fprintf(stderr, "%s: illegal option -- %c\n",
			        progname, optopt);
			return EXIT_FAILURE;
		}
	}

	if (!terror && !(pfound && tfound && !timegt(tmtime, pmtime)))
		return EXIT_SUCCESS;

	argc -= optind;
	argv += optind;

	if (argv[0] == NULL)
		return EXIT_SUCCESS;

	execvp(argv[0], argv);

	fprintf(stderr, "%s: %s: %s\n", progname, argv[0], strerror(errno));

	return EXIT_FAILURE;
}
