#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#include "tests.h"
#include "../src/alog.h"

static const char *name = "test_stop_activity";
static int exit_code = 0;

/* GIVEN activity is currently not active,
   WHEN stopping an activity,
   THEN should write error message to STDERR,
     AND should exit with code of -1.  */
static void error_on_inactive_activity() {
	char *case_name = "error_on_inactive_activity";

	create_alog_test_dir();

	int inter_exit_code = stop_activity("project-1", "/tmp/.alog", NULL);

	/* Assert main program return exit code of -1.  */
	if (inter_exit_code != -1) {
		exit_code = -1;
		fprintf(stderr_test_fp, "[%s...%s] Expect stop_activity to return -1.\n", name, case_name);
	}

	/* Assert main program write error message to STDERR.  */
	char *expt_err_msg = "[ERROR] The activity: \"project-1\" is inactive.\n";
	char *actu_err_msg = (char*)malloc(sizeof(char) * strlen(expt_err_msg));
	read_err(actu_err_msg, strlen(expt_err_msg));
	if (strncmp(expt_err_msg, actu_err_msg, strlen(expt_err_msg)) != 0) {
		exit_code = -1;
		fprintf(stderr_test_fp, "[%s...%s] Expect error message: %s", name, case_name, expt_err_msg);
	}
	free(actu_err_msg);

	remove_alog_test_dir();
}

/* GIVEN activity is active,
     AND activity has been logged before (activity file exist),
   WHEN stopping an activity with a provided log message via command-line argument,
   THEN activity should become inactive (tmp file removed),
     AND activity file should be appended with log,
     AND a success message should be written to STDOUT.
     AND should exit with code of 0;
*/
void normal_file_append_provided_log_message_stop_activity() {
	char *case_name = "normal_file_append_provided_log_message_stop_activity";

	create_alog_test_dir();
	/* Create tmp file to simulate active activity.  */
	int tmp_actv_fd = open("/tmp/.alog/.tmp/project-1", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	/* Write Epoch timestamp to tmp file.
	   This timestamp is read (and use) by the stop_activity program
	   to calculate the activity elapsed time.

	   Simulating timestamp of 10 seconds after Epoch time 1970 Jan 01.  */
	write(tmp_actv_fd, "10", 2);
	close(tmp_actv_fd);

	/* Create activity log file to simulate that the activity
	   has been logged before.  */
	int actv_log_fd = open("/tmp/.alog/project-1", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	char *past_logged_info = "Wed May 13 10:57:00 2026\nElapsed Time (s): 60\nSome note about the activity.\n==========\n";
	/* Is 87 bytes.  */
	size_t past_logged_info_len = strlen(past_logged_info);
	write(actv_log_fd, past_logged_info, past_logged_info_len);

	int inter_exit_code = stop_activity("project-1", "/tmp/.alog", "Some log message.\nSomething else.\n\n");

	/* Assert that tmp file has been removed.  */
	struct stat stat_buf;
	if (stat("/tmp/.alog/.tmp/project-1", &stat_buf) != -1) {
		exit_code = -1;
		fprintf(stderr_test_fp, "[%s...%s] Expect tmp file: \"/tmp/.alog/.tmp/project-1\" to have been removed.\n", name, case_name);
	}

	/* Assert that previously logged information hasn't been overwritten.  */
	char log_info[101];
	/* Reset activity FD offset back to the start of the file, so that
	   previously logged info can be asserted.  */
	lseek(actv_log_fd, 0, SEEK_SET);
	int log_info_len = read(actv_log_fd, log_info, past_logged_info_len);
	if (strncmp(past_logged_info, log_info, past_logged_info_len) != 0) {
		exit_code = -1;
		fprintf(stderr_test_fp, "[%s...%s] Expect previously logged information to still be in the activity file.\n", name, case_name);
	}

	/* Assert that activity log file has been appended with new log information.  */
	log_info_len = read(actv_log_fd, log_info, 100);
	close(actv_log_fd);
	log_info[log_info_len] = '\0';
	/* Assert correct new log start date.  */
	char *log_date = strtok(log_info, "\n");
	if (log_date == NULL) {
		exit_code = -1;
		fprintf(stderr_test_fp, "[%s...%s] Expect start date to be logged.\n", name, case_name);
	} else {
		struct tm log_date_tm;
		/* Initialise all fields within struct tm to be 0 before using it.

		   strptime() converts date/time string to struct tm. Any fields
		   unspecified in the string won't be set in struct tm.

		   Zero-ing out the fields is done to ensure that when tm
		   is converted to time_t no unset fields interfere with the
		   result.  */
		memset(&log_date_tm, 0, sizeof(struct tm));
		if (strptime(log_date, "%a %b %d %T %Y", &log_date_tm) == NULL) {
			exit_code = -1;
			fprintf(stderr_test_fp, "[%s...%s] Expect start date to have the format: \"Thu Jan 01 09:00:00 2026\".\n", name, case_name);
		} else if (mktime(&log_date_tm) != 10) {
			exit_code = -1;
			fprintf(stderr_test_fp, "[%s...%s] Expect start date to be 10 seconds after Epoch.\n", name, case_name);
		}
	}
	/* Assert new elapsed time header has been logged.  */
	char *log_elapsed_time = strtok(NULL, "\n");
	if (log_elapsed_time == NULL || strncmp("Elapsed Time (s): ", log_elapsed_time, 18) != 0) {
		exit_code = -1;
		fprintf(stderr_test_fp, "[%s...%s] Expect elapsed time to be logged.\n", name, case_name);
	}
	/* Assert new log message has been logged.

	   Note that the main program should also add a trailing newline (\n) if one isn't
	   already provided in the log message.
	   If multiple trailing newlines are provided in the log message, then they're expected
	   to be replaced with a single newline.  */
	char *log_msg = strtok(NULL, "\0");
	if (log_msg == NULL || strncmp("Some log message.\nSomething else.\n==========\n", log_msg, 45) != 0) {
		exit_code = -1;
		fprintf(stderr_test_fp, "[%s...%s] Expect log message to be logged.\n", name, case_name);
	}

	/* Assert that success message has been written to STDOUT.  */
	char *expt_out_msg = "Activity recorded.\n";
	char *actu_out_msg = (char*)malloc(sizeof(char) * strlen(expt_out_msg));
	read_out(actu_out_msg, strlen(expt_out_msg));
	if (strncmp(expt_out_msg, actu_out_msg, strlen(expt_out_msg)) != 0) {
		exit_code = -1;
		fprintf(stderr_test_fp, "[%s...%s] Expect message: %s", name, case_name, expt_out_msg);
	}
	free(actu_out_msg);

	/* Assert that main program return exit code of 0.  */
	if (inter_exit_code != 0) {
		exit_code = -1;
		fprintf(stderr_test_fp, "[%s...%s] Expect stop_activity to return 0.\n", name, case_name);
	}

	/* Remove activity log file and directories used for testing.  */
	unlink("/tmp/.alog/project-1");
	remove_alog_test_dir();
}

/* GIVEN activity is active,
     AND activity has not been logged before (activity file doesn't exist),
   WHEN stopping an activity with a provided log message via command-line argument,
   THEN activity should become inactive (tmp file removed),
     AND activity file should be created,
     AND log should be written to activity file,
     AND a success message should be written to STDOUT.
     AND should exit with code of 0;
*/
void normal_file_create_provided_log_message_stop_activity() {
	char *case_name = "normal_file_create_provided_log_message_stop_activity";

	create_alog_test_dir();
	/* Create tmp file to simulate active activity.  */
	int tmp_actv_fd = open("/tmp/.alog/.tmp/project-1", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	/* Write Epoch timestamp to tmp file.
	   This timestamp is read (and use) by the stop_activity program
	   to calculate the activity elapsed time.

	   Simulating timestamp of 10 seconds after Epoch time 1970 Jan 01.  */
	write(tmp_actv_fd, "10", 2);
	close(tmp_actv_fd);

	int inter_exit_code = stop_activity("project-1", "/tmp/.alog", "Some log message.");

	/* Assert that tmp file has been removed.  */
	struct stat stat_buf;
	if (stat("/tmp/.alog/.tmp/project-1", &stat_buf) != -1) {
		exit_code = -1;
		fprintf(stderr_test_fp, "[%s...%s] Expect tmp file: \"/tmp/.alog/.tmp/project-1\" to have been removed.\n", name, case_name);
	}

	/* Assert that activity file has been created.  */
	if (stat("/tmp/.alog/project-1", &stat_buf) == -1) {
		exit_code = -1;
		fprintf(stderr_test_fp, "[%s...%s] Expect activity file: \"/tmp/.alog/project-1\" to have been created.\n", name, case_name);
	}
	/* Assert that created activity file has the correct permissions.  */
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
		fprintf(stderr_test_fp, "[%s...%s] Expect \"/tmp/.alog/project-1\" to have permission 0600 but got 0%o\n", name, case_name, stat_buf.st_mode & 0777);
	}

	/* Assert that activity log file contain the new log information.  */
	char log_info[101];
	int log_info_len;
	int actv_log_fd = open("/tmp/.alog/project-1", O_RDONLY);
	log_info_len = read(actv_log_fd, log_info, 100);
	close(actv_log_fd);
	log_info[log_info_len] = '\0';
	/* Assert correct log start date.  */
	char *log_date = strtok(log_info, "\n");
	if (log_date == NULL) {
		exit_code = -1;
		fprintf(stderr_test_fp, "[%s...%s] Expect start date to be logged.\n", name, case_name);
	} else {
		struct tm log_date_tm;
		/* Initialise all fields within struct tm to be 0 before using it.

		   strptime() converts date/time string to struct tm. Any fields
		   unspecified in the string won't be set in struct tm.

		   Zero-ing out the fields is done to ensure that when tm
		   is converted to time_t no unset fields interfere with the
		   result.  */
		memset(&log_date_tm, 0, sizeof(struct tm));
		if (strptime(log_date, "%a %b %d %T %Y", &log_date_tm) == NULL) {
			exit_code = -1;
			fprintf(stderr_test_fp, "[%s...%s] Expect start date to have the format: \"Thu Jan 01 09:00:00 2026\".\n", name, case_name);
		} else if (mktime(&log_date_tm) != 10) {
			exit_code = -1;
			fprintf(stderr_test_fp, "[%s...%s] Expect start date to be 10 seconds after Epoch.\n", name, case_name);
		}
	}
	/* Assert elapsed time header has been logged.  */
	char *log_elapsed_time = strtok(NULL, "\n");
	if (log_elapsed_time == NULL || strncmp("Elapsed Time (s): ", log_elapsed_time, 18) != 0) {
		exit_code = -1;
		fprintf(stderr_test_fp, "[%s...%s] Expect elapsed time to be logged.\n", name, case_name);
	}
	/* Assert log message has been logged.

	   Note that the main program should also add a trailing newline (\n) if one isn't
	   already provided in the log message.  */
	char *log_msg = strtok(NULL, "\0");
	if (log_msg == NULL || strncmp("Some log message.\n==========\n", log_msg, 29) != 0) {
		exit_code = -1;
		fprintf(stderr_test_fp, "[%s...%s] Expect log message to be logged.\n", name, case_name);
	}

	/* Assert that success message has been written to STDOUT.  */
	char *expt_out_msg = "Activity recorded.\n";
	char *actu_out_msg = (char*)malloc(sizeof(char) * strlen(expt_out_msg));
	read_out(actu_out_msg, strlen(expt_out_msg));
	if (strncmp(expt_out_msg, actu_out_msg, strlen(expt_out_msg)) != 0) {
		exit_code = -1;
		fprintf(stderr_test_fp, "[%s...%s] Expect message: %s", name, case_name, expt_out_msg);
	}
	free(actu_out_msg);

	/* Assert that main program return exit code of 0.  */
	if (inter_exit_code != 0) {
		exit_code = -1;
		fprintf(stderr_test_fp, "[%s...%s] Expect stop_activity to return 0.\n", name, case_name);
	}

	/* Remove activity log file and directories used for testing.  */
	unlink("/tmp/.alog/project-1");
	remove_alog_test_dir();
}

int test_stop_activity() {
	error_on_inactive_activity();
	normal_file_append_provided_log_message_stop_activity();
	normal_file_create_provided_log_message_stop_activity();

	if (exit_code == 0) {
		fprintf(stdout_test_fp, "[%s] PASSED\n", name);
	} else {
		fprintf(stderr_test_fp, "[%s] FAILED\n", name);
	}

	return exit_code;
}

