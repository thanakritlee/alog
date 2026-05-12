#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#include "tests.h"

/* Truncate file and set the file descriptor offset to
   the beginning of the file.

   Use by each test case to clean the files slate after
   reading from it.  */
int reset_file(int fd) {
	if (lseek(fd, 0, SEEK_SET) == -1) {
		fprintf(stderr_test_fp, "[ERROR] Failed to lseek on fd (%d): %s\n", fd, strerror(errno));
		return -1;
	}
	if (ftruncate(fd, 0) == -1) {
		fprintf(stderr_test_fp, "[ERROR] Failed to ftruncate on fd (%d): %s\n", fd, strerror(errno));
		return -1;
	}
	return 0;
}

/* Read data from STDERR into buffer, then reset the
   STDERR file.  */
int read_err(char *buf, size_t len) {
	if (lseek(STDERR_FILENO, 0, SEEK_SET)) {
		fprintf(stderr_test_fp, "[ERROR] Failed to lseek on STDERR: %s\n", strerror(errno));
		return -1;
	}
	if (read(STDERR_FILENO, buf, len) == -1) {
		fprintf(stderr_test_fp, "[ERROR] Failed to read from STDERR: %s\n", strerror(errno));
		return -1;
	}
	if (reset_file(STDERR_FILENO) == -1) {
		fprintf(stderr_test_fp, "[ERROR] Failed to reset STDERR file\n");
		return -1;
	}
	return 0;
}

/* Read data from STDOUT into buffer, then reset the
   STDOUT file.  */
int read_out(char *buf, size_t len) {
	if (lseek(STDOUT_FILENO, 0, SEEK_SET)) {
		fprintf(stderr_test_fp, "[ERROR] Failed to lseek on STDOUT: %s\n", strerror(errno));
		return -1;
	}
	if (read(STDOUT_FILENO, buf, len) == -1) {
		fprintf(stderr_test_fp, "[ERROR] Failed to read from STDOUT: %s\n", strerror(errno));
		return -1;
	}
	if (reset_file(STDOUT_FILENO) == -1) {
		fprintf(stderr_test_fp, "[ERROR] Failed to reset STDOUT file\n");
		return -1;
	}
	return 0;
}

/* Create .alog directory for test case usage.  */
int create_alog_test_dir() {
	if (mkdir("/tmp/.alog", 0700) == -1) {
		fprintf(stderr_test_fp, "[ERROR] Failed to create directory \"/tmp/.alog\": %s\n", strerror(errno));
		return -1;
	}
	if (mkdir("/tmp/.alog/.tmp", 0700) == -1) {
		fprintf(stderr_test_fp, "[ERROR] Failed to create directory \"/tmp/.alog/.tmp\": %s\n", strerror(errno));
		return -1;
	}
}

/* Remove .alog directory.  */
int remove_alog_test_dir() {
	if (rmdir("/tmp/.alog/.tmp") == -1) {
		fprintf(stderr_test_fp, "[ERROR] Failed to remove directory \"/tmp/.alog\": %s\n", strerror(errno));
		return -1;
	}
	if (rmdir("/tmp/.alog") == -1) {
		fprintf(stderr_test_fp, "[ERROR] Failed to remove directory \"/tmp/.alog/.tmp\": %s\n", strerror(errno));
		return -1;
	}
}
