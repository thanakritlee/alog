#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

#include "alog.h"

static void print_usage(char *progname) {
	char *usage_str =
		"Usage: %1$s command\n"
		" %1$s start activity_name\n"
		" %1$s stop activity_name [-m log_message]\n"
		" %1$s list [-a] [-r]\n"
		" %1$s info activity_name [-a] [-r] [-s]\n"
		" %1$s cancel activity_name [-f]\n"
		" %1$s delete activity_name [-f]\n"
		" %1$s help\n";
	fprintf(stdout, usage_str, progname);
}

static void print_help() {
	char *help_str =
		"Commands:\n"
		" start activity_name\n"
		"     Start logging an activity. A timer is started when this command\n"
		"     is executed, and it'll be stopped when the \"stop\" command is executed.\n"
		"\n"
		"     An activity that has started, but not stopped is an active activity.\n"
		"\n"
		"     When this command is executed and the provided activity_name is an\n"
		"     active activity, then it'll result in an error.\n"
		"\n"
		" stop activity_name [-m log_message]\n"
		"     Stop logging an active activity. Stops the timer that was started\n"
		"     when the \"start\" command was executed, and calculate the elapsed time\n"
		"     of the activity.\n"
		"\n"
		"     If the \"-m\" option was provided, then log_message is recorded into the\n"
		"     activity log file. Otherwise, the Nano editor is opened to interactively\n"
		"     record the log message.\n"
		"\n"
		"     An activity that has been logged is a recorded activity.\n"
		"\n"
		"     New activity logs are appended to the end of the activity log file.\n"
		"     See the \"info\" command.\n"
		"\n"
		"     When this command is executed and the provided activity_name isn't\n"
		"     an active activity, then it'll result in an error.\n"
		"\n"
		" list [-a] [-r]\n"
		"     List the user's activities.\n"
		"\n"
		"     If option \"-r\" is provided, then recorded activities will be listed.\n"
		"\n"
		"     If option \"-a\" is provided, then active activities will be listed.\n"
		"\n"
		"     The default behaviour, if no options are provided, is to list only\n"
		"     the recorded activities.\n"
		"\n"
		" info activity_name [-a] [-r] [-s]\n"
		"     Display activity info.\n"
		"\n"
		"     If option \"-r\" is provided, then display the recorded activity\n"
		"     log info.\n"
		"\n"
		"     If option \"-a\" is provided, then display the active activity info.\n"
		"\n"
		"     If neither \"-r\" nor \"-a\" is provided, then default behaviour is to\n"
		"     display both recorded and active activity info, which correspond\n"
		"     with the \"-ar\" option.\n"
		"\n"
		"     If option \"-s\" is provided, then don't display the log messages\n"
		"     from the recorded activity logs.\n"
		"\n"
		" cancel activity_name [-f]\n"
		"     Cancel an active activity.\n"
		"\n"
		"     Has no effect on recorded activity of the same name.\n"
		"\n"
		"     If option \"-f\" is provided, then don't prompt for confirmation.\n"
		"     The default behaviour is to always prompt for confirmation.\n"
		"\n"
		" delete activity_name [-f]\n"
		"     Delete a recorded activity.\n"
		"\n"
		"     Has no effect on active activity of the same name.\n"
		"\n"
		"     If option \"-f\" is provided, then don't prompt for confirmation.\n"
		"     The default behaviour is to always prompt for confirmation.\n"
		"\n"
		" help\n"
		"     Display program help text.\n";

	fprintf(stdout, "%s", help_str);
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
		print_usage(argv[0]);
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

	} else if (strcmp(argv[1], "info") == 0) {
		if (argc < 3) {
			/* Expect at least 3 command-line argument: alog info activity  */
			fprintf(stderr, "Usage: %s info activity_name [-a] [-r] [-s]\n", argv[0]);
			exit(-1);
		}
		/* Store activity name command-line argument before getopt permutes it.  */
		char *activity_name = argv[2];

		int activity_types = 0;
		int options = 0;

		int opt;
		while ((opt = getopt(argc, argv, ":ars")) != -1) {
			switch (opt) {
				case 'a': {
					activity_types |= T_ACT;
					break;
				}
				case 'r': {
					activity_types |= T_REC;
					break;
				}
				case 's': {
					options |= O_SLNT;
					break;
				}
				default: {
					fprintf(stderr, "Usage: %s info activity_name [-a] [-r] [-s]\n", argv[0]);
					exit(-1);
				}
			}
		}

		if (activity_types == 0) {
			/* Activity type to get hasn't been provided.
			   Default to getting info of both recorded
			   and active activities.  */
			activity_types = T_REC | T_ACT;
		}
		exit_code = get_activity_info(activity_name, alog_path, activity_types, options);

	} else if (strcmp(argv[1], "cancel") == 0) {
		if (argc < 3) {
			/* Expect at least 3 command-line argument: alog cancel activity  */
			fprintf(stderr, "Usage: %s cancel activity_name [-f]\n", argv[0]);
			exit(-1);
		}
		/* Store activity name command-line argument before getopt permutes it.  */
		char *activity_name = argv[2];

		/* Default option is to prompt user for confirmation.  */
		int options = O_PRMT;

		int opt;
		while ((opt = getopt(argc, argv, ":f")) != -1) {
			switch (opt) {
				case 'f': {
					/* Disable user confirmation prompt.  */
					options = 0;
					break;
				}
				default: {
					fprintf(stderr, "Usage: %s cancel activity_name [-f]\n", argv[0]);
					exit(-1);
				}
			}
		}

		exit_code = cancel_activity(activity_name, alog_path, options);

	} else if (strcmp(argv[1], "help") == 0) {
		print_usage(argv[0]);
		fprintf(stderr, "\n");
		print_help();

	}
	else {
		/* Command invalid.  */
		print_usage(argv[0]);
		exit(-1);

	}

	exit(exit_code);
}
