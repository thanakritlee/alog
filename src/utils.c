#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

#include "alog.h"

/* Create .alog and .alog/.tmp directories if
   they don't already exist.  */
int create_alog_dir(char *alog_path) {
	char alog_tmp_path[PATH_MAX];
	strcpy(alog_tmp_path, alog_path);
	strcat(alog_tmp_path, "/.tmp");
	struct stat stat_buf;

	if (stat(alog_path, &stat_buf) == -1) {
		/* .alog directory doesn't exist.
		   Create the directory.  */
		if (mkdir(alog_path, 0700) == -1) {
			fprintf(stderr, "[ERROR] Failed to create .alog directory: %s\n", strerror(errno));
			return -1;
		}
	}

	if (stat(alog_tmp_path, &stat_buf) == -1) {
		/* .alog/.tmp directory doesn't exist.
		   Create the directory.  */
		if (mkdir(alog_tmp_path, 0700) == -1) {
			fprintf(stderr, "[ERROR] Failed to create .alog/.tmp directory: %s\n", strerror(errno));
			return -1;
		}
	}

	return 0;
}
