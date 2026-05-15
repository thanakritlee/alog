#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "alog.h"

/* Create .alog, .alog/.tmp directories, and .alog/.log_edit_msg
   file if they don't already exist.  */
int create_alog_dir(char *alog_path) {
	char alog_tmp_path[PATH_MAX];
	char alog_edit_msg_path[PATH_MAX];
	strcpy(alog_tmp_path, alog_path);
	strcat(alog_tmp_path, "/.tmp");
	strcpy(alog_edit_msg_path, alog_path);
	strcat(alog_edit_msg_path, "/.log_edit_msg");
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

	if (stat(alog_edit_msg_path, &stat_buf) == -1) {
		/* .alog/.log_edit_msg doesn't exist.
		   Create the file.  */
		int fd = open(alog_edit_msg_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
		if (fd == -1) {
			fprintf(stderr, "[ERROR] Failed to create .alog/.log_edit_msg file: %s\n", strerror(errno));
			return -1;
		}
		close(fd);
	}

	return 0;
}
