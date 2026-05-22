#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "alog.h"

static void print_help(char *progname) {
	fprintf(stderr, "Usage: %s command\n", progname);
	fprintf(stderr, " %s start activity_name\n", progname);
	fprintf(stderr, " %s stop activity_name [-m log_message]\n", progname);
	fprintf(stderr, " %s list [-a] [-r]\n", progname);
}

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
		print_help(argv[0]);
		exit(-1);
	}

	if (strcmp(argv[1], "start") == 0) {
		if (argc < 3) {
			/* Expect at least 3 command-line arguments: alog start activity  */
			fprintf(stderr, "Usage: %s start activity_name\n", argv[0]);
			exit(-1);
		}
		exit_code = start_activity(argv[2], alog_path);

	} else if (strcmp(argv[1], "stop") == 0) {
		char *log_msg = NULL;
		if (argc < 3) {
			/* Expect at least 3 command-line argument: alog stop activity  */
			fprintf(stderr, "Usage: %s stop activity_name [-m log_message]\n", argv[0]);
			exit(-1);
		}
		/* Store activity name command-line argument before getopt permutes it.  */
		char *activity_name = argv[2];

		int opt;
		while ((opt = getopt(argc, argv, ":m:")) != -1) {
			switch (opt) {
				case 'm': {
					/* Log message provided via command-line argument.
					   e.g. alog stop activity -m "log message"  */
					log_msg = optarg;
					break;
				}
				default: {
					fprintf(stderr, "Usage: %s stop activity_name [-m log_message]\n", argv[0]);
					exit(-1);
				}
			}
		}

		exit_code = stop_activity(activity_name, alog_path, log_msg);

	} else if (strcmp(argv[1], "list") == 0) {
		int activity_types = 0;

		int opt;
		while ((opt = getopt(argc, argv, ":ar")) != -1) {
			switch (opt) {
				case 'a': {
					activity_types |= T_ACT;
					break;
				}
				case 'r': {
					activity_types |= T_REC;
					break;
				}
				default: {
					fprintf(stderr, "Usage: %s list [-a] [-r]\n", argv[0]);
					exit(-1);
				}
			}
		}

		if (activity_types == 0) {
			/* Activity type to list hasn't been provided.
			   Default to listing only recorded activities.  */
			activity_types |= T_REC;
		}
		exit_code = list_activities(alog_path, activity_types);

	} else {
		/* Command invalid.  */
		print_help(argv[0]);
		exit(-1);
	}

	exit(exit_code);
}
