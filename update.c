#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	struct stat stbuf;
	char *progname;
	time_t pmtime, tmtime;
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
			if (!pfound || stbuf.st_mtime > pmtime) {
				pmtime = stbuf.st_mtime;
				pfound = true;
			}
			break;
		case 't':
			if (stat(optarg, &stbuf)) {
				terror = true;
			} else if (!tfound || stbuf.st_mtime < tmtime) {
				tmtime = stbuf.st_mtime;
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

	if (!terror && !(pfound && tfound && pmtime >= tmtime))
		return EXIT_SUCCESS;

	argc -= optind;
	argv += optind;

	if (argv[0] == NULL)
		return EXIT_SUCCESS;

	execvp(argv[0], argv);

	fprintf(stderr, "%s: %s: %s\n", progname, argv[0], strerror(errno));

	return EXIT_FAILURE;
}
