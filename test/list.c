#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdarg.h>

#include "./tests.h"
#include "../src/alog.h"

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

/* Assert that a string value produced by the program is equals to the
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

static const char *name = "test_list_activities";
static int exit_code = 0;

/* GIVEN there aren't any recorded activities,
   WHEN listing recorded activities,
   THEN an empty recorded activity list should be written to STDOUT,
     AND should exit with code of 0.  */
static void get_recorded_no_recorded_activity() {
	char *case_name = "get_recorded_no_recorded_activity";

	create_alog_test_dir();

	int inter_exit_code = list_activities("/tmp/.alog", T_REC);

	/* Assert that an empty recorded activity list is written
	   to STDOUT.  */
	char *expt_out_msg = "[RECORDED]\n";
	char *actu_out_msg = (char*)malloc(sizeof(char) * strlen(expt_out_msg));
	read_out(actu_out_msg, strlen(expt_out_msg));
	exit_code = assert_str(actu_out_msg, expt_out_msg, strlen(expt_out_msg),
							exit_code, "[%s...%s] Expect the following list to be printed to STDOUT: \n%s",
							name, case_name, expt_out_msg);
	free(actu_out_msg);

	/* Assert that list_activities return exit code of 0.  */
	exit_code = assert_int(inter_exit_code, 0, exit_code, "[%s...%s] Expect list_activities to return 0.\n", name, case_name);

	remove_alog_test_dir();
}

/* GiVEN there aren't any active activities,
   WHEN listing active activities,
   THEN an empty active activity list should be written to STDOUT,
     AND should exit with code of 0.  */
static void get_active_no_active_activity() {
	char *case_name = "get_active_no_active_activity";

	create_alog_test_dir();

	int inter_exit_code = list_activities("/tmp/.alog", T_ACT);

	/* Assert that an empty active activity list is written
	   to STDOUT.  */
	char *expt_out_msg = "[ACTIVE]\n";
	char *actu_out_msg = (char*)malloc(sizeof(char) * strlen(expt_out_msg));
	read_out(actu_out_msg, strlen(expt_out_msg));
	exit_code = assert_str(actu_out_msg, expt_out_msg, strlen(expt_out_msg),
							exit_code, "[%s...%s] Expect the following list to be printed to STDOUT: \n%s",
							name, case_name, expt_out_msg);
	free(actu_out_msg);

	/* Assert that list_activities return exit code of 0.  */
	exit_code = assert_int(inter_exit_code, 0, exit_code, "[%s...%s] Expect list_activities to return 0.\n", name, case_name);

	remove_alog_test_dir();
}

/* GIVEN there are recorded activities,
   WHEN listing recorded activities,
   THEN a list of recorded activities should be written to STDOUT,
     AND the list of recorded actitivities should be in alphabetically sorted order,
     AND should exit with code of 0.  */
static void get_recorded_has_recorded_activity() {
	char *case_name = "get_recorded_has_recorded_activity";

	create_alog_test_dir();

	/* Create recorded activity log files for testing.  */
	int fd;
	fd = open("/tmp/.alog/project-1", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	close(fd);
	fd = open("/tmp/.alog/project-2", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	close(fd);

	int inter_exit_code = list_activities("/tmp/.alog", T_REC);

	/* Assert that the recorded activity list includes project-1 and project-2,
	   and that they are written to STDOUT.
	   */
	char *expt_out_msg = "[RECORDED]\nproject-1\nproject-2\n";
	char *actu_out_msg = (char*)malloc(sizeof(char) * strlen(expt_out_msg));
	read_out(actu_out_msg, strlen(expt_out_msg));
	exit_code = assert_str(actu_out_msg, expt_out_msg, strlen(expt_out_msg),
							exit_code, "[%s...%s] Expect the following list to be printed to STDOUT: \n%s",
							name, case_name, expt_out_msg);
	free(actu_out_msg);

	/* Assert that list_activities return exit code of 0.  */
	exit_code = assert_int(inter_exit_code, 0, exit_code, "[%s...%s] Expect list_activities to return 0.\n", name, case_name);

	unlink("/tmp/.alog/project-2");
	unlink("/tmp/.alog/project-1");

	remove_alog_test_dir();
}

/* GIVEN there are active activities,
   WHEN listing active activities,
   THEN a list of active activities should be written to STDOUT,
     AND the list of active activities should be in alphabetically sorted order,
     AND should exit with code of 0.  */
static void get_active_has_active_activity() {
	char *case_name = "get_active_has_active_activity";

	create_alog_test_dir();

	/* Create active activity tmp files for testing.  */
	int fd;
	fd = open("/tmp/.alog/.tmp/project-2", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	close(fd);
	fd = open("/tmp/.alog/.tmp/project-0", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	close(fd);
	fd = open("/tmp/.alog/.tmp/project-1", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	close(fd);

	int inter_exit_code = list_activities("/tmp/.alog", T_ACT);

	/* Assert that the active activity list includes project-0, project-1, and project-2,
	   and that they are written to STDOUT.
	   */
	char *expt_out_msg = "[ACTIVE]\nproject-0\nproject-1\nproject-2\n";
	char *actu_out_msg = (char*)malloc(sizeof(char) * strlen(expt_out_msg));
	read_out(actu_out_msg, strlen(expt_out_msg));
	exit_code = assert_str(actu_out_msg, expt_out_msg, strlen(expt_out_msg),
							exit_code, "[%s...%s] Expect the following list to be printed to STDOUT: \n%s",
							name, case_name, expt_out_msg);
	free(actu_out_msg);

	/* Assert that list_activities return exit code of 0.  */
	exit_code = assert_int(inter_exit_code, 0, exit_code, "[%s...%s] Expect list_activities to return 0.\n", name, case_name);

	unlink("/tmp/.alog/.tmp/project-2");
	unlink("/tmp/.alog/.tmp/project-1");
	unlink("/tmp/.alog/.tmp/project-0");

	remove_alog_test_dir();
}

static void get_all_no_activity() {}

static void get_all_has_recorded_activity() {}

static void get_all_has_active_activity() {}

static void get_all_has_both_activity() {}

/* GIVEN there are recorded activities,
     AND there are active activities,
   WHEN listing activities without providing a type,
   THEN recorded activity list should be written to STDOUT.  */
static void get_default_has_both_activity() {
	//list_activities("/tmp/.alog", 0);
}

int test_list_activities() {
	get_recorded_no_recorded_activity();
	get_active_no_active_activity();
	get_recorded_has_recorded_activity();
	get_active_has_active_activity();
	get_all_no_activity();
	get_all_has_recorded_activity();
	get_all_has_active_activity();
	get_all_has_both_activity();
	get_default_has_both_activity();

	if (exit_code == 0) {
		fprintf(stdout_test_fp, "[%s] PASSED\n", name);
	} else {
		fprintf(stderr_test_fp, "[%s] FAILED\n", name);
	}

	return exit_code;
}
