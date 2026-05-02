#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>

int start_activity(char *actv_name_arg, char *alog_path_arg) {
	char *actv_name = malloc(sizeof(char) * strlen(actv_name_arg));
	strcpy(actv_name, actv_name_arg);

	char alog_path[PATH_MAX];
	strcpy(alog_path, alog_path_arg);

	/* Check if activity is already active.  */
	struct stat stat_buf;
	strcat(alog_path, "/.tmp/");
	strcat(alog_path, actv_name);
	if (stat(alog_path, &stat_buf) != -1) {
		/* Activity is already active.  */
		fprintf(stderr, "[ERROR] The activity: \"%s\" is already active.\n", actv_name);
		free(actv_name);
		return 1;
	}

	free(actv_name);
	return 0;
}
