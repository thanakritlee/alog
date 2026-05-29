#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#include "alog.h"

int delete_activity(char *actv_name_arg, char *alog_path_arg, int options) {
	char actv_path[PATH_MAX];
	strcpy(actv_path, alog_path_arg);
	strcat(actv_path, "/");
	strcat(actv_path, actv_name_arg);

	struct stat stat_buf;
	if (stat(actv_path, &stat_buf) == -1) {
		/* Recorded activity file doesn't exist.  */
		fprintf(stderr, "[ERROR] The activity: \"%s\" isn't recorded.\n", actv_name_arg);
		return -1;
	}

	if (options & O_PRMT) {
		/* Prompt user for confirmation.  */
		char in;
		while (1) {
			fprintf(stdout, "Are you sure you want to delete: \"%s\"? (y|n):    ", actv_name_arg);
			fscanf(stdin, "%s", &in);
			if (in == 'y') {
				break;
			} else if (in == 'n') {
				return 0;
			}
		}
	}

	if (unlink(actv_path) == -1) {
		fprintf(stderr, "[ERROR] Failed to unlink \"%s\": %s\n", actv_path, strerror(errno));
		return -1;
	}

	fprintf(stdout, "Deleted activity: \"%s\".\n", actv_name_arg);
	return 0;
}
