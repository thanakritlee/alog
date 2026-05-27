#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "tests.h"
#include "../src/alog.h"

static const char *name = "test_activity_info";
static int exit_code = 0;

/* GIVEN there are no recorded logs of activitiy: project-1,
   WHEN getting recorded activity info of project-1,
   THEN an empty recorded log info list should be written to STDOUT,
     AND should exit with code of 0.  */
static void get_recorded_info_no_recorded() {
	char *case_name = "get_recorded_info_no_recorded";

	create_alog_test_dir();

	int inter_exit_code = get_activity_info("project-1", "/tmp/.alog", T_REC, 0);

	/* Assert that an empty recorded activity info list is written
	   to STDOUT.  */
	char *expt_out_msg = "[RECORDED]\n";
	char *actu_out_msg = (char*)malloc(sizeof(char) * strlen(expt_out_msg));
	read_out(actu_out_msg, strlen(expt_out_msg));
	exit_code = assert_str(actu_out_msg, expt_out_msg, strlen(expt_out_msg),
							exit_code, "[%s...%s] Expect the following info to be written to STDOUT: \n%s",
							name, case_name, expt_out_msg);
	free(actu_out_msg);

	/* Assert that get_activity_info return a code of 0.  */
	exit_code = assert_int(inter_exit_code, 0, exit_code,
							"[%s...%s] Expect get_activity_info to return 0.\n",
							name, case_name);

	remove_alog_test_dir();
}

/* GIVEN there is no active activity: project-1,
   WHEN getting active activity info of project-1,
   THEN an empty active info section should be written to STDOUT,
     AND should exit with code of 0.  */
static void get_active_info_no_active() {
	char *case_name = "get_active_info_no_active";

	create_alog_test_dir();

	int inter_exit_code = get_activity_info("project-1", "/tmp/.alog", T_ACT, 0);

	/* Assert that an empty active info section is written
	   to STDOUT.  */
	char *expt_out_msg = "[ACTIVE]\n";
	char *actu_out_msg = (char*)malloc(sizeof(char) * strlen(expt_out_msg));
	read_out(actu_out_msg, strlen(expt_out_msg));
	exit_code = assert_str(actu_out_msg, expt_out_msg, strlen(expt_out_msg),
							exit_code, "[%s...%s] Expect the following info to be written to STDOUT: \n%s",
							name, case_name, expt_out_msg);
	free(actu_out_msg);

	/* Assert that get_activity_info return a code of 0.  */
	exit_code = assert_int(inter_exit_code, 0, exit_code,
							"[%s...%s] Expect get_activity_info to return 0.\n",
							name, case_name);

	remove_alog_test_dir();
}

/* GIVEN there are no recorded logs of activity: project-1,
     AND there is no active activity: project-1,
   WHEN getting recorded and active activity info of project-1,
   THEN an empty recorded log info list should be written to STDOUT,
     AND an empty active info section should be written to STDOUT,
     AND should exit with code of 0.  */
static void get_recorded_active_info_no_recorded_active() {
	char *case_name = "get_recorded_active_info_no_recorded_active";

	create_alog_test_dir();

	int inter_exit_code = get_activity_info("project-1", "/tmp/.alog", T_REC | T_ACT, 0);

	/* Assert that an empty recorded activity info list and an
	   empty active info section is written to STDOUT.  */
	char *expt_out_msg =
	"[RECORDED]\n"
	"[ACTIVE]\n";
	char *actu_out_msg = (char*)malloc(sizeof(char) * strlen(expt_out_msg));
	read_out(actu_out_msg, strlen(expt_out_msg));
	exit_code = assert_str(actu_out_msg, expt_out_msg, strlen(expt_out_msg),
							exit_code, "[%s...%s] Expect the following info to be written to STDOUT: \n%s",
							name, case_name, expt_out_msg);
	free(actu_out_msg);

	/* Assert that get_activity_info return a code of 0.  */
	exit_code = assert_int(inter_exit_code, 0, exit_code,
							"[%s...%s] Expect get_activity_info to return 0.\n",
							name, case_name);

	remove_alog_test_dir();
}

/*  GIVEN there are recorded logs of activity: project-1,
    WHEN getting recorded activity info of project-1,
    THEN a recorded log info list of project-1 should be written to STDOUT,
      AND should exit with code of 0.  */
static void get_recorded_info_has_recorded() {
	char *case_name = "get_recorded_info_has_recorded";

	create_alog_test_dir();
	/* Create activity log files for testing.

	   Create 2 log files: project-1 and project-2.
	   This test case should only retrieve project-1 info.  */
	/* Create and write to project-1 log file.  */
	int actv_log_fd = open("/tmp/.alog/project-1", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
	char *proj_1_log_str =
		"Sun May 24 12:00:00 2026\n"
		"Elapsed Time (s): 60\n"	/* 1 minute.  */
		"Log message 1\n"
		"\n"
		"Log message 2\n"
		"==========\n"
		"Mon May 25 13:45:00 2026\n"
		"Elapsed Time (s): 5400\n"	/* 1 hour 30 minutes.  */
		"Log message 1\n"
		"Log message 2\n"
		"==========\n";
	write(actv_log_fd, proj_1_log_str, strlen(proj_1_log_str));
	close(actv_log_fd);
	/* Create and write to project-2 log file.  */
	actv_log_fd = open("/tmp/.alog/project-2", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
	char *proj_2_log_str =
		"Fri May 1 10:00:00 2026\n"
		"Elapsed Time (s): 300\n"	/* 5 minute.  */
		"Log message X\n"
		"\n"
		"Log message Y\n"
		"==========\n"
		"Tue May 5 09:10:00 2026\n"
		"Elapsed Time (s): 8110\n"	/* 2 hour 15 minutes 10 seconds  */
		"Log message X\n"
		"Log message Y\n"
		"==========\n";
	write(actv_log_fd, proj_2_log_str, strlen(proj_2_log_str));
	close(actv_log_fd);

	int inter_exit_code = get_activity_info("project-1", "/tmp/.alog", T_REC, 0);

	/* Assert that recorded activity info of project-1 is written
	   to STDOUT.  */
	char *expt_out_msg =
		"[RECORDED]\n"
		"Sun May 24 12:00:00 2026\n"
		"Elapsed Time: 0 hours 1 minutes 0 seconds\n"
		"\n"
		"Log message 1\n"
		"\n"
		"Log message 2\n"
		"\n"
		"==========\n"
		"Mon May 25 13:45:00 2026\n"
		"Elapsed Time: 1 hours 30 minutes 0 seconds\n"
		"\n"
		"Log message 1\n"
		"Log message 2\n"
		"\n"
		"==========\n";
	char *actu_out_msg = (char*)malloc(sizeof(char) * strlen(expt_out_msg));
	read_out(actu_out_msg, strlen(expt_out_msg));
	exit_code = assert_str(actu_out_msg, expt_out_msg, strlen(expt_out_msg),
							exit_code, "[%s...%s] Expect the following info to be written to STDOUT: \n%s",
							name, case_name, expt_out_msg);
	free(actu_out_msg);

	/* Assert that get_activity_info return a code of 0.  */
	exit_code = assert_int(inter_exit_code, 0, exit_code,
							"[%s...%s] Expect get_activity_info to return 0.\n",
							name, case_name);

	unlink("/tmp/.alog/project-2");
	unlink("/tmp/.alog/project-1");
	remove_alog_test_dir();
}

/* GIVEN there are recorded logs of activity: project-1,
   WHEN getting recorded activity info of project-1,
     AND the silent option is specified,
   THEN a recorded log info list of project-1 should be written to STDOUT,
     AND the log messages in the recorded info list should not be included,
     AND should exit with code of 0.  */
static void get_recorded_info_has_recorded_silent() {
	char *case_name = "get_recorded_info_has_recorded_silent";

	create_alog_test_dir();
	/* Create activity log files for testing.

	   Create 2 log files: project-1 and project-2.
	   This test case should only retrieve project-1 info.  */
	/* Create and write to project-1 log file.  */
	int actv_log_fd = open("/tmp/.alog/project-1", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
	char *proj_1_log_str =
		"Sun May 24 12:00:00 2026\n"
		"Elapsed Time (s): 60\n"	/* 1 minute.  */
		"Log message 1\n"
		"\n"
		"Log message 2\n"
		"==========\n"
		"Mon May 25 13:45:00 2026\n"
		"Elapsed Time (s): 5400\n"	/* 1 hour 30 minutes.  */
		"Log message 1\n"
		"Log message 2\n"
		"==========\n";
	write(actv_log_fd, proj_1_log_str, strlen(proj_1_log_str));
	close(actv_log_fd);
	/* Create and write to project-2 log file.  */
	actv_log_fd = open("/tmp/.alog/project-2", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
	char *proj_2_log_str =
		"Fri May 1 10:00:00 2026\n"
		"Elapsed Time (s): 300\n"	/* 5 minute.  */
		"Log message X\n"
		"\n"
		"Log message Y\n"
		"==========\n"
		"Tue May 5 09:10:00 2026\n"
		"Elapsed Time (s): 8110\n"	/* 2 hour 15 minutes 10 seconds  */
		"Log message X\n"
		"Log message Y\n"
		"==========\n";
	write(actv_log_fd, proj_2_log_str, strlen(proj_2_log_str));
	close(actv_log_fd);

	int inter_exit_code = get_activity_info("project-1", "/tmp/.alog", T_REC, O_SLNT);

	/* Assert that recorded activity info of project-1 is written
	   to STDOUT and that the log messages aren't included.  */
	char *expt_out_msg =
		"[RECORDED]\n"
		"Sun May 24 12:00:00 2026\n"
		"Elapsed Time: 0 hours 1 minutes 0 seconds\n"
		"==========\n"
		"Mon May 25 13:45:00 2026\n"
		"Elapsed Time: 1 hours 30 minutes 0 seconds\n"
		"==========\n";
	char *actu_out_msg = (char*)malloc(sizeof(char) * strlen(expt_out_msg));
	read_out(actu_out_msg, strlen(expt_out_msg));
	exit_code = assert_str(actu_out_msg, expt_out_msg, strlen(expt_out_msg),
							exit_code, "[%s...%s] Expect the following info to be written to STDOUT: \n%s",
							name, case_name, expt_out_msg);
	free(actu_out_msg);

	/* Assert that get_activity_info return a code of 0.  */
	exit_code = assert_int(inter_exit_code, 0, exit_code,
							"[%s...%s] Expect get_activity_info to return 0.\n",
							name, case_name);

	unlink("/tmp/.alog/project-2");
	unlink("/tmp/.alog/project-1");
	remove_alog_test_dir();
}

/* GIVEN there is an active activity: project-1,
   WHEN getting active activity info of project-1,
   THEN the active info of project-1 should be written to STDOUT,
     AND should exit with code of 0.  */
static void get_active_info_has_active() {}

/* GIVEN there is an active activity: project-1,
   WHEN getting active activity info of project-1,
     AND the silent option is specified,
   THEN the active info of project-1 should be written to STDOUT,
     AND the silent option should not have any effect on the active info output,
     AND should exit with code of 0.  */
static void get_active_info_has_active_silent() {}

/* GIVEN there are recorded logs of activity: project-1,
     AND there is no active activity: project-1,
   WHEN getting recorded and active activity info of project-1,
   THEN a recorded log info list of project-1 should be written to STDOUT,
     AND an empty active info section should be written to STDOUT,
     AND should exit with code of 0.  */
static void get_recorded_active_info_has_recorded() {}

/* GIVEN there are no recorded logs of activity: project-1,
     AND there is an active activity: project-1,
   WHEN getting recorded and active activity info of project-1,
   THEN an empty recorded log info list should be written to STDOUT,
     AND the active info of project-1 should be written to STDOUT,
     AND should exit with code of 0.  */
static void get_recorded_active_info_has_active() {}

/* GIVEN there are recorded logs of activity: project-1
     AND there is an active activity: project-1,
   WHEN getting recorded and active activity info of project-1,
   THEN a recorded log info list of project-1 should be written to STDOUT,
     AND the active info of project-1 should be written to STDOUT,
     AND should exit with code of 0.  */
static void get_recorded_active_info_has_recorded_active() {}

/* GIVEN there are recorded logs of activity: project-1
     AND there is an active activity: project-1,
   WHEN getting recorded and active activity info of project-1,
     AND the silent option is specified,
   THEN a recorded log info list of project-1 should be written to STDOUT,
     AND the log messages in the recorded info list should not be included,
     AND the active info of project-1 should be written to STDOUT,
     AND the silent option should not have any effect on the active info output,
     AND should exit with code of 0.  */
static void get_recorded_active_info_has_recorded_active_silent() {}

/* GIVEN there are recorded logs of activity: project-1,
     AND there is an active activity: project-1,
   WHEN getting activity info of project-1 without specifying any type option
   (effectively equivalent to getting recorded and active activity info),
   THEN a recorded log info list of project-1 should be written to STDOUT,
     AND the active info of project-1 should be written to STDOUT,
     AND should exit with code of 0.  */
static void get_default_info_has_recorded_active() {}

/* GIVEN there are recorded logs of activity: project-1,
     AND there is an active activity: project-1,
   WHEN getting activity info of project-1 without specifying any type option
   (effectively equivalent to getting recorded and active activity info),
     AND the silent option is specified,
   THEN a recorded log info list of project-1 should be written to STDOUT,
     AND the log messages in the recorded info list should not be included,
     AND the active info of project-1 should be written to STDOUT,
     AND the silent option should not have any effect on the active info output,
     AND should exit with code of 0.  */
static void get_default_info_has_recorded_active_silent() {}

int test_activity_info() {
	get_recorded_info_no_recorded();
	get_active_info_no_active();
	get_recorded_active_info_no_recorded_active();
	get_recorded_info_has_recorded();
	get_recorded_info_has_recorded_silent();
	get_active_info_has_active();
	get_active_info_has_active_silent();
	get_recorded_active_info_has_recorded();
	get_recorded_active_info_has_active();
	get_recorded_active_info_has_recorded_active();
	get_recorded_active_info_has_recorded_active_silent();
	get_default_info_has_recorded_active();
	get_default_info_has_recorded_active_silent();

	if (exit_code == 0) {
		fprintf(stdout_test_fp, "[%s] PASSED\n", name);
	} else {
		fprintf(stderr_test_fp, "[%s] FAILED\n", name);
	}

	return exit_code;
}
