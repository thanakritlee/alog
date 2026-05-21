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

	/* Create the .alog directories and files:
	   - $HOME/.alog/
	   - $HOME/.alog/.tmp/
	   - $HOME/.alog/.artefacts/
	   - $HOME/.alog/.artefacts/.log_edit_msg

	   If it's the first time the program is run,
	   then the .alog directories won't exist.  */
	if (create_alog_dir(alog_path) == -1) {
		exit(-1);
	}

	if (argc < 2) {
		/* Expect at least 1 command-line argments i.e. alog command.
		   Asserting for at least 2 arguments, because the first
		   argument is always the name of the program.  */
		/* TODO: Output help text.  */
		exit(-1);
	}

	if (strcmp(argv[1], "start") == 0) {
		if (argc < 3) {
			/* Expect at least 3 command-line arguments: alog start activity  */
			/* TODO: Output help text.  */
			exit(-1);
		}
		exit_code = start_activity(argv[2], alog_path);
	} else if (strcmp(argv[1], "stop") == 0) {
		char *log_msg = NULL;
		if (argc < 3) {
			/* Expect at least 3 command-line argument: alog stop activity  */
			/* TODO: Output help text.  */
			exit(-1);
		}

		if (argc > 4 && strcmp(argv[3], "-m") == 0) {
			/* Log message provided via command-line argument.
		       e.g. alog stop activity -m "log message"  */
		    log_msg = argv[4];
		}
		exit_code = stop_activity(argv[2], alog_path, log_msg);
	}

	exit(exit_code);
}
