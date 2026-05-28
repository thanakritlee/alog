#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "tests.h"
#include "../src/alog.h"

static char *name = "test_cancel_activity";
static int exit_code = 0;

/* GIVEN that the provided activity isn't an active activity,
   WHEN cancelling the activity,
   THEN should write an error message to STDERR,
     AND should exit with code of -1.  */
static void error_no_activity() {
	char *case_name = "error_no_activity";

	create_alog_test_dir();

	int inter_exit_code = cancel_activity("project-1", "/tmp/.alog", 0);

	/* Assert error message is written to STDERR.  */
	char *expt_err_msg = "[ERROR] The activity: \"project-1\" is inactive.\n";
	char *actu_err_msg = (char*)malloc(sizeof(char) * strlen(expt_err_msg));
	read_err(actu_err_msg, strlen(expt_err_msg));
	exit_code = assert_str(actu_err_msg, expt_err_msg, strlen(expt_err_msg), exit_code,
							"[%s...%s] Expect error message: %s", name, case_name, expt_err_msg);
	free(actu_err_msg);

	/* Assert cancel_activity return a code of -1.  */
	assert_int(inter_exit_code, -1, exit_code,
				"[%s...%s] Expect cancel_activity to return -1.\n", name, case_name);

	remove_alog_test_dir();
}

/* GIVEN that the provided activity is an active activity,
   WHEN cancelling the activity,
   THEN the tmp activity file should be removed,
   	 AND other tmp activity files should not be removed,
     AND should write a success message to STDOUT,
     AND should exit with code of 0.  */
static void normal_cancel_activity() {
	char *case_name = "normal_cancel_activity";

	create_alog_test_dir();
	/* Create 2 tmp active activity files for testing.  */
	open("/tmp/.alog/.tmp/project-1", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
	open("/tmp/.alog/.tmp/project-2", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);

	int inter_exit_code = cancel_activity("project-1", "/tmp/.alog", 0);

	struct stat stat_buf;
	int stat_result;
	/* Assert that project-1 tmp file was removed.  */
	stat_result = stat("/tmp/.alog/.tmp/project-1", &stat_buf);
	exit_code = assert_int(stat_result, -1, exit_code,
							"[%s...%s] Expect project-1 tmp file to be removed.\n",
							name, case_name);
	/* Assert that project-2 tmp file is unaffected.  */
	stat_result = stat("/tmp/.alog/.tmp/project-2", &stat_buf);
	exit_code = assert_int(stat_result, 0, exit_code,
							"[%s...%s] Expect project-2 tmp file to exist.\n",
							name, case_name);

	/* Assert success message is written to STDOUT.  */
	char *expt_out_msg = "Cancelled activity: \"project-1\".\n";
	char *actu_out_msg = (char*)malloc(sizeof(char) * strlen(expt_out_msg));
	read_out(actu_out_msg, strlen(expt_out_msg));
	exit_code = assert_str(actu_out_msg, expt_out_msg, strlen(expt_out_msg), exit_code,
							"[%s...%s] Expect success message: %s", name, case_name, expt_out_msg);
	free(actu_out_msg);

	/* Assert cancel_activity return a code of 0.  */
	assert_int(inter_exit_code, 0, exit_code,
				"[%s...%s] Expect cancel_activity to return 0.\n", name, case_name);

	unlink("/tmp/.alog/.tmp/project-2");
	remove_alog_test_dir();
}


int test_cancel_activity() {
	error_no_activity();
	normal_cancel_activity();

	if (exit_code == 0) {
		fprintf(stdout_test_fp, "[%s] PASSED\n", name);
	} else {
		fprintf(stderr_test_fp, "[%s] FAILED\n", name);
	}

	return exit_code;
}
