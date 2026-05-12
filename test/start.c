#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#include "tests.h"
#include "../src/alog.h"

static const char *name = "test_start_activity";
static int exit_code = 0;

/* GIVEN that the activity is an active activity (has tmp file),
   WHEN starting an activity log,
   THEN should write error message to STDERR
     AND should exit with code of -1.  */
static void error_on_active_activity() {
	const char *case_name = "error_on_active_activity";

	/* Create temporary directories and file for testing.

	   This is use to indicate that the activity is currently
	   active.  */
	create_alog_test_dir();
	open("/tmp/.alog/.tmp/project-1", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

	int inter_exit_code = start_activity("project-1", "/tmp/.alog");

	/* Assert main program return exit code of -1.  */
	if (inter_exit_code != -1) {
		exit_code = -1;
		fprintf(stderr_test_fp, "[%s...%s] Expect start_activity to return -1.\n", name, case_name);
	}

	/* Assert main program write error message to STDERR.  */
	char *expt_err_msg = "[ERROR] The activity: \"project-1\" is already active.\n";
	char *actu_err_msg = (char*)malloc(sizeof(char) * strlen(expt_err_msg));
	read_err(actu_err_msg, strlen(expt_err_msg));
	if (strncmp(expt_err_msg, actu_err_msg, strlen(expt_err_msg)) != 0) {
		exit_code = -1;
		fprintf(stderr_test_fp, "[%s...%s] Expect error message: %s", name, case_name, expt_err_msg);
	}
	free(actu_err_msg);

	/* Remove temporary directories and file.  */
	unlink("/tmp/.alog/.tmp/project-1");
	remove_alog_test_dir();
}

/* GIVEN activity is not an active activity,
   WHEN starting an activity,
   THEN activity should become active (tmp file created),
   	 AND activity start time should have been recorded,
     AND a message should be written to STDOUT,
     AND should exit with code of 0.  */
static void normal_start_activity() {
	const char *case_name = "normal_start_activity";

	/* Create temporary directories and file for testing.  */
	create_alog_test_dir();

	int inter_exit_code = start_activity("project-1", "/tmp/.alog");

	/* Assert that the active activity file:
	   "/tmp/.alog/.tmp/project-1" has been created.  */
	struct stat stat_buf;
	if (stat("/tmp/.alog/.tmp/project-1", &stat_buf) == -1) {
		exit_code = -1;
		fprintf(stderr_test_fp, "[%s...%s] Expect file /tmp/.alog/.tmp/project-1 to have been created.\n", name, case_name);
	}
	/* Assert that the active activity file has
	   the following permissions:
	   - User: RW
	   - Group: -
	   - Other: -  */
	else if (!(stat_buf.st_mode & S_IRUSR &&
			stat_buf.st_mode & S_IWUSR &&
			!(stat_buf.st_mode & S_IXUSR) &&
			!(stat_buf.st_mode & S_IRGRP) &&
			!(stat_buf.st_mode & S_IWGRP) &&
			!(stat_buf.st_mode & S_IXGRP) &&
			!(stat_buf.st_mode & S_IROTH) &&
			!(stat_buf.st_mode & S_IWOTH) &&
			!(stat_buf.st_mode & S_IXOTH))) {
				exit_code = -1;
				fprintf(stderr_test_fp, "[%s...%s] Expect file /tmp/.alog/.tmp/project-1 to have permission 0600, but got 0%o.\n", name, case_name, stat_buf.st_mode & 0777);
			}

	/* Assert that the active activity file contains the
	   activity start time. Activity start time is a number
	   representing the Epoch time.  */
	int actv_tmp_fd = open("/tmp/.alog/.tmp/project-1", O_RDONLY, 0);
	/* On 32-bit Linux system time_t is a 32 bit signed integer
	   , which means the maximum value it can be is
	   (2^31) -1 = 2147483647. A 10 digit number.

	   On 64-bit Linux system time_t is a 64 bit signed integer.
	   The maximum value of time_t is (2^63) - 1 = 9.223372*(10^18).
	   A 19 digit number.

	   In this test, we'll make the assertion compatible with both
	   systems by reading in the epoch time value with maximum digit
	   of 19.  */
	char time_buf[20];
	ssize_t time_char_len;
	time_t epoch_time;
	time_char_len = read(actv_tmp_fd, time_buf, 19);
	/* Set a string null terminator at the end of the number.  */
	time_buf[time_char_len] = '\0';
	epoch_time = atoi(time_buf);
	if (epoch_time <= 0) {
		exit_code = -1;
		fprintf(stderr_test_fp, "[%s...%s] Expect Epoch start time to have been recorded in the tmp activity file.\n", name, case_name);
	}
	close(actv_tmp_fd);

	/* Assert main program return exit code of 0.  */
	if (inter_exit_code != 0) {
		exit_code = -1;
		fprintf(stderr_test_fp, "[%s...%s] Expect start_activity to return 0.\n", name, case_name);
	}

	/* Assert main program write message to STDOUT.  */
	char *expt_stdout_msg = "Recording activity: \"project-1\".\n";
	char *actu_stdout_msg = (char*)malloc(sizeof(char) * strlen(expt_stdout_msg));
	read_out(actu_stdout_msg, strlen(expt_stdout_msg));
	if (strncmp(expt_stdout_msg, actu_stdout_msg, strlen(expt_stdout_msg)) != 0) {
		exit_code = 1;
		fprintf(stderr_test_fp, "[%s...%s] Expect message: %s", name, case_name, expt_stdout_msg);
	}
	free(actu_stdout_msg);

	/* Remove created active activity file.  */
	unlink("/tmp/.alog/.tmp/project-1");
	/* Remove temporary directories.  */
	remove_alog_test_dir();
}

int test_start_activity() {
	error_on_active_activity();
	normal_start_activity();

	if (exit_code == 0) {
		fprintf(stdout_test_fp, "[%s] PASSED\n", name);
	} else {
		fprintf(stderr_test_fp, "[%s] FAILED\n", name);
	}

	return exit_code;
}
