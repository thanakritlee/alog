#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>

#include "tests.h"

/* Truncate file and set the file descriptor offset to
   the beginning of the file.

   Use by each test case to clean the files slate after
   reading from it.  */
static int reset_file(int fd) {
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
	if (lseek(main_stderr_fd, 0, SEEK_SET)) {
		fprintf(stderr_test_fp, "[ERROR] Failed to lseek on STDERR: %s\n", strerror(errno));
		return -1;
	}
	if (read(main_stderr_fd, buf, len) == -1) {
		fprintf(stderr_test_fp, "[ERROR] Failed to read from STDERR: %s\n", strerror(errno));
		return -1;
	}
	if (reset_file(main_stderr_fd) == -1) {
		fprintf(stderr_test_fp, "[ERROR] Failed to reset STDERR file\n");
		return -1;
	}
	return 0;
}

/* Read data from STDOUT into buffer, then reset the
   STDOUT file.  */
int read_out(char *buf, size_t len) {
	if (lseek(main_stdout_fd, 0, SEEK_SET)) {
		fprintf(stderr_test_fp, "[ERROR] Failed to lseek on STDOUT: %s\n", strerror(errno));
		return -1;
	}
	if (read(main_stdout_fd, buf, len) == -1) {
		fprintf(stderr_test_fp, "[ERROR] Failed to read from STDOUT: %s\n", strerror(errno));
		return -1;
	}
	if (reset_file(main_stdout_fd) == -1) {
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
	if (mkdir("/tmp/.alog/.artefacts", 0700) == -1) {
		fprintf(stderr_test_fp, "[ERROR] Failed to create directory \"/tmp/.alog/.artefacts\": %s\n", strerror(errno));
		return -1;
	}
	int fd = open("/tmp/.alog/.artefacts/.log_edit_msg", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		fprintf(stderr_test_fp, "[ERROR] Failed to open/create file \"/tmp/.alog/.artefacts/.log_edit_msg\": %s\n", strerror(errno));
		return -1;
	}
	close(fd);
	return 0;
}

/* Remove .alog directory.  */
int remove_alog_test_dir() {
	int exit_code = 0;
	if (unlink("/tmp/.alog/.artefacts/.log_edit_msg") == -1) {
		fprintf(stderr_test_fp, "[ERROR] Failed to unlink file \"/tmp/.alog/.artefacts/.log_edit_msg\": %s\n", strerror(errno));
		exit_code = -1;
	}
	if (rmdir("/tmp/.alog/.artefacts") == -1) {
		fprintf(stderr_test_fp, "[ERROR] Failed to remove directoy \"/tmp/.alog/.artefacts\": %s\n", strerror(errno));
		exit_code = -1;
	}
	if (rmdir("/tmp/.alog/.tmp") == -1) {
		fprintf(stderr_test_fp, "[ERROR] Failed to remove directory \"/tmp/.alog\": %s\n", strerror(errno));
		exit_code = -1;
	}
	if (rmdir("/tmp/.alog") == -1) {
		fprintf(stderr_test_fp, "[ERROR] Failed to remove directory \"/tmp/.alog/.tmp\": %s\n", strerror(errno));
		exit_code = -1;
	}
	return exit_code;
}

/* Assert that an int value produced by the program is equals to the
   expected int value defined in the test case.

   If actual value doesn't equal to the expected value, then print
   an error message to STDERR and return -1.

   Otherwise, if actual value equal to the expected value, then
   return the existing exit_code. This is done so that existing
   exit_code that is potentially a -1 doesn't get overwritten with
   a good 0.  */
int assert_int(int actu, int expt, int exit_code, char *err_msg_format, ...) {
	if (actu != expt) {
		/* Assertion failed.
		   Print error message to STDERR.  */
		va_list  argList;
		va_start(argList, err_msg_format);
		vfprintf(stderr_test_fp, err_msg_format, argList);
		va_end(argList);

		return -1;
	} else {
		/* Assertion passed.
		   Return existing exit_code.  */
		return exit_code;
	}
}

/* Assert that a string value produced by the program is equal to the
   expected string value defined in the test cases.

   Uses `strncmp(str1, str2, len)` for string comparision.  */
int assert_str(char *actu, char *expt, size_t len, int exit_code, char *err_msg_format, ...) {
	if (strncmp(actu, expt, len) != 0) {
		/* Assertion failed.
		   Print error message to STDERR.  */
		va_list  argList;
		va_start(argList, err_msg_format);
		vfprintf(stderr_test_fp, err_msg_format, argList);
		va_end(argList);

		return -1;
	} else {
		/* Assertion passed.
		   Return existing exit_code.  */
		return exit_code;
	}
}
