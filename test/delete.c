#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "tests.h"
#include "../src/alog.h"

static char *name = "test_delete_activity";
static int exit_code = 0;

/* GIVEN that the provided activity isn't a recorded activity,
   WHEN deleting the activity,
   THEN should write an error message to STDERR,
     AND should exit with code of -1.  */
static void error_no_activity() {
	char *case_name = "error_no_activity";

	create_alog_test_dir();

	int inter_exit_code = delete_activity("project-1", "/tmp/.alog", 0);

	/* Assert error message is written to STDERR.  */
	char *expt_err_msg = "[ERROR] The activity: \"project-1\" isn't recorded.\n";
	char *actu_err_msg = (char *)malloc(sizeof(char) * strlen(expt_err_msg));
	read_err(actu_err_msg, strlen(expt_err_msg));
	exit_code = assert_str(actu_err_msg, expt_err_msg, strlen(expt_err_msg), exit_code,
					"[%s...%s] Expect error message: \"%s\"", name, case_name, expt_err_msg);

	/* Assert delete_activity return code of -1.  */
	exit_code = assert_int(inter_exit_code, -1, exit_code,
					"[%s...%s] Expect delete_activity to return -1.\n", name, case_name);

	remove_alog_test_dir();
}

/* GIVEN that the provided activity is a recorded activity,
   WHEN delete the activity,
   THEN the recorded activity log file should be removed,
     AND the other recorded activity log files should not be removed,
     AND should write a success message to STDOUT,
     AND should exit with code of 0.  */
static void normal_delete_activity() {
	char *case_name = "normal_delete_activity";

	create_alog_test_dir();

	/* Create 2 recorded activity files.
	   Only project-1 should be removed, while project-2
	   stays unaffected.  */
	int fd;
	fd = open("/tmp/.alog/project-1", O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR);
	fd = open("/tmp/.alog/project-2", O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR);

	int inter_exit_code = delete_activity("project-1", "/tmp/.alog", 0);

	struct stat stat_buf;
	int stat_result;
	/* Assert recorded activity project-1 has been removed.  */
	stat_result = stat("/tmp/.alog/project-1", &stat_buf);
	exit_code = assert_int(stat_result, -1, exit_code,
				"[%s...%s] Expect project-1 recorded activity file to be removed.\n",
				name, case_name);
	/* Assert recorded activity project-2 is unaffected.  */
	stat_result = stat("/tmp/.alog/project-2", &stat_buf);
	exit_code = assert_int(stat_result, 0, exit_code,
				"[%s...%s] Expect project-2 recorded activity file to be unaffected\n",
				name, case_name);

	/* Assert success message is written to STDOUT.  */
	char *expt_out_msg = "Deleted activity: \"project-1\".\n";
	char *actu_out_msg = (char *)malloc(sizeof(char) * strlen(expt_out_msg));
	read_out(actu_out_msg, strlen(expt_out_msg));
	exit_code = assert_str(actu_out_msg, expt_out_msg, strlen(expt_out_msg), exit_code,
					"[%s...%s] Expect success message: \"%s\"", name, case_name, expt_out_msg);

	/* Assert delete_activity return code of 0.  */
	exit_code = assert_int(inter_exit_code, 0, exit_code,
					"[%s...%s] Expect delete_activity to return 0.\n", name, case_name);

	unlink("/tmp/.alog/project-2");
	remove_alog_test_dir();
}

int test_delete_activity() {
	error_no_activity();
	normal_delete_activity();

	if (exit_code == 0) {
		fprintf(stdout_test_fp, "[%s] PASSED\n", name);
	} else {
		fprintf(stderr_test_fp, "[%s] FAILED\n", name);
	}

	return exit_code;
}
