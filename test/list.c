#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./tests.h"
#include "../src/alog.h"

static const char *name = "test_list_activities";
static int exit_code = 0;

/* GIVEN there aren't any recorded activities,
   WHEN listing recorded activities,
   THEN an empty recorded activity list should be written to STDOUT,
     AND should exit with code of 0.  */
static void get_recorded_no_recorded_activity() {
	char *case_name = "get_recorded_no_recorded_activity";

	create_alog_test_dir();

	int inter_exit_code = list_activities("/tmp/.alog", 01);

	/* Assert that an empty recorded activity list is written
	   to STDOUT.  */
	char *expt_out_msg = "[RECORDED]\n";
	char *actu_out_msg = (char*)malloc(sizeof(char) * strlen(expt_out_msg));
	read_out(actu_out_msg, strlen(expt_out_msg));
	if (strncmp(expt_out_msg, actu_out_msg, strlen(expt_out_msg)) != 0) {
		exit_code = -1;
		fprintf(stderr_test_fp, "[%s...%s] Expect the following list to be printed to STDOUT: \n%s", name, case_name, expt_out_msg);
	}
	free(actu_out_msg);

	/* Assert that list_activities return exit code of 0.  */
	if (inter_exit_code != 0) {
		exit_code = -1;
		fprintf(stderr_test_fp, "[%s...%s] Expect list_activities to return 0.\n", name, case_name);
	}

	remove_alog_test_dir();
}

static void get_active_no_active_activity() {}

static void get_recorded_has_recorded_activity() {}

static void get_active_has_active_activity() {}

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

	if (exit_code == 0) {
		fprintf(stdout_test_fp, "[%s] PASSED\n", name);
	} else {
		fprintf(stderr_test_fp, "[%s] FAILED\n", name);
	}

	return exit_code;
}
