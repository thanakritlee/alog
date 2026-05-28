#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>

#include "alog.h"

/* Cancel an active activity by unlinking the tmp file in .alog/.tmp/  */
int cancel_activity(char *actv_name_arg, char *alog_path_arg, int options) {
	char tmp_path[PATH_MAX];
	strcpy(tmp_path, alog_path_arg);
	strcat(tmp_path, "/.tmp/");
	strcat(tmp_path, actv_name_arg);

	struct stat stat_buf;
	if (stat(tmp_path, &stat_buf) == -1) {
		/* Activity tmp file doesn't exist.
		   Activity isn't active.  */

		fprintf(stderr, "[ERROR] The activity: \"%s\" is inactive.\n", actv_name_arg);
		return -1;
	}

	if (options & O_PRMT) {
		/* Prompt user for confirmation.  */
		char in;
		while (1) {
			fprintf(stdout, "Are you sure you want to cancel: \"%s\"? (y|n):    ", actv_name_arg);
			fscanf(stdin, "%s", &in);
			if (in == 'y') {
				break;
			} else if (in == 'n') {
				return 0;
			}
		}
	}

	unlink(tmp_path);
	fprintf(stdout, "Cancelled activity: \"%s\".\n", actv_name_arg);
	return 0;
}
