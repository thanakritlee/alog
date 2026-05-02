#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#include "../src/alog.h"

static const char *name = "test_start_activity";
static int exit_code = 0;

/* GIVEN that the activity is an active activity,
   WHEN starting an activity log,
   THEN should write error message to STDERR
     AND should exit with code of 1.  */
static void error_on_active_activity() {
	const char *case_name = "error_on_active_activity";

	/* Create temporary directories and file for testing.

	   This is use to indicate that the activity is currently
	   active.  */
	mkdir("/tmp/.alog", 0700);
	mkdir("/tmp/.alog/.tmp", 0700);
	open("/tmp/.alog/.tmp/project-1", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

	int inter_exit_code = start_activity("project-1", "/tmp/.alog");

	/* Assert main program return exit code of 1.  */
	if (inter_exit_code != 1) {
		exit_code = 1;
		fprintf(stdout, "[%s...%s]: Expect start_activity to return 1.\n", name, case_name);
	}

	/* Assert main program write error message to STDERR.  */
	char *expt_err_msg = "[ERROR] The activity: \"project-1\" is already active.\n";
	char *actu_err_msg = (char*)malloc(sizeof(char) * strlen(expt_err_msg));
	lseek(STDERR_FILENO, 0, SEEK_SET);
	read(STDERR_FILENO, actu_err_msg, strlen(expt_err_msg));
	lseek(STDERR_FILENO, 0, SEEK_SET);
	if (strcmp(expt_err_msg, actu_err_msg) != 0) {
		exit_code = 1;
		fprintf(stdout, "[%s...%s]: Expect error message: \"%s\"", name, case_name, expt_err_msg);
	}
	free(actu_err_msg);

	/* Remove temporary directories and file.  */
	unlink("/tmp/.alog/.tmp/project-1");
	rmdir("/tmp/.alog/.tmp");
	rmdir("/tmp/.alog");
}

int test_start_activity() {
	error_on_active_activity();

	return exit_code;
}
