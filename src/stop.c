#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>

/*
TODO: Update descriptions.
1. Check if tmp activity file exist.
2. Open, read, close, and unlink tmp activity file.
3. Calculate activity elapsed time.
4. Retrieve activity log message (either through command-line argument or editor).
5. Open (and create, if it doesn't exist) the activity log file.
6. Append log to activity file.
7. Output success message.
*/
int stop_activity(char *actv_name_arg, char *alog_path_arg) {
	char *actv_name = (char *)malloc(sizeof(char) * strlen(actv_name_arg));
	strcpy(actv_name, actv_name_arg);

	char alog_path[PATH_MAX];
	strcpy(alog_path, alog_path_arg);

	/* Check if activity is inactive.  */
	struct stat stat_buf;
	strcat(alog_path, "/.tmp/");
	strcat(alog_path, actv_name);
	if (stat(alog_path, &stat_buf) == -1) {
		/* The tmp activity file doesn't exist
		   i.e. activity is inactive.  */
		fprintf(stderr, "[ERROR] The activity: \"%s\" is inactive.\n", actv_name);
		free(actv_name);
		return -1;
	}

	return 0;
}
