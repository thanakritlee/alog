#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tests.h"
#include "../src/alog.h"

static const char *name = "test_stop_activity";
static int exit_code = 0;

/* GIVEN activity is currently not active,
   WHEN stoping an activity,
   THEN should write error message to STDERR,
     AND should exit with code of -1.  */
static void error_on_inactive_activity() {
	char *case_name = "error_on_inactive_activity";

	create_alog_test_dir();

	int inter_exit_code = stop_activity("project-1", "/tmp/.alog");

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

int test_stop_activity() {
	error_on_inactive_activity();

	if (exit_code == 0) {
		fprintf(stdout_test_fp, "[%s] PASSED\n", name);
	} else {
		fprintf(stderr_test_fp, "[%s] FAILED\n", name);
	}

	return exit_code;
}
