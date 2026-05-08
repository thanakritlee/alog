#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "alog.h"

int main(int argc, char *argv[]) {
	int exit_code = 0;
	char *home_path = getenv("HOME");
	char alog_path[PATH_MAX];
	strcpy(alog_path, home_path);
	strcat(alog_path, "/.alog");

	if (argc < 2) {
		/* Expect at least 1 command-line argments i.e. alog command.
		   Asserting for at least 2 arguments, because the first
		   argument is always the name of the program.  */
		/* TODO: Output help text.  */
		exit(-1);
	}

	if (strcmp(argv[1], "start") == 0) {
		if (argc < 3) {
			/* Expect at least 3 command  */
			/* TODO: Output help text.  */
			exit(-1);
		}
		exit_code = start_activity(argv[2], alog_path);
	}

	exit(exit_code);
}
