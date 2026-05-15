#include <stdio.h>
#include <sys/stat.h>

#include "tests.h"
#include "../src/alog.h"

static const char *name = "test_create_alog_dir";
static int exit_code = 0;

/* GIVEN that the program has never been ran before,
   WHEN the program is run for the first time,
   THEN the .alog directory should be created,
     AND the .alog/.tmp directory should be created
     AND the .alog/.log_edit_msg file should be created.  */
static void normal_create_alog_dir() {
	char *case_name = "create_alog_dir";

	/* Main program create the alog directories.  */
	create_alog_dir("/tmp/.alog");

	/* Assert that the directories has been created.  */
	struct stat stat_buf;
	if (stat("/tmp/.alog", &stat_buf) == -1 || !S_ISDIR(stat_buf.st_mode)) {
		fprintf(stderr_test_fp, "[%s...%s] Expect .alog directory to have been created.\n", name, case_name);
		exit_code = -1;
	}
	/* Assert that the directories has the following permissions:
	   - User:  RWX
	   - Group: -
	   - Other: -

	   Execute permission is required on the directory so that files
	   within the directory could be access (read/write etc.).
	   */
	else if (!(stat_buf.st_mode & S_IRUSR &&
		  stat_buf.st_mode & S_IWUSR &&
		  stat_buf.st_mode & S_IXUSR &&
		  !(stat_buf.st_mode & S_IRGRP) &&
		  !(stat_buf.st_mode & S_IWGRP) &&
		  !(stat_buf.st_mode & S_IXGRP) &&
		  !(stat_buf.st_mode & S_IROTH) &&
		  !(stat_buf.st_mode & S_IWOTH) &&
		  !(stat_buf.st_mode & S_IXOTH))) {
		fprintf(stderr_test_fp, "[%s...%s] Expect .alog directory to have permission 0700, but got 0%o.\n", name, case_name, stat_buf.st_mode & 0777);
		exit_code = -1;
	}

	if (stat("/tmp/.alog/.tmp", &stat_buf) == -1 || !S_ISDIR(stat_buf.st_mode)) {
		fprintf(stderr_test_fp, "[%s...%s] Expect .alog/.tmp directory to have been created.\n", name, case_name);
		exit_code = -1;
	}
	else if (!(stat_buf.st_mode & S_IRUSR &&
		  stat_buf.st_mode & S_IWUSR &&
		  stat_buf.st_mode & S_IXUSR &&
		  !(stat_buf.st_mode & S_IRGRP) &&
		  !(stat_buf.st_mode & S_IWGRP) &&
		  !(stat_buf.st_mode & S_IXGRP) &&
		  !(stat_buf.st_mode & S_IROTH) &&
		  !(stat_buf.st_mode & S_IWOTH) &&
		  !(stat_buf.st_mode & S_IXOTH))) {
		fprintf(stderr_test_fp, "[%s...%s] Expect .alog/.tmp directory to have permission 0700, but got 0%o.\n", name, case_name, stat_buf.st_mode & 0777);
		exit_code = -1;
	}

	/* Assert that the file /.alog/.log_edit_msg has been created.  */
	if (stat("/tmp/.alog/.log_edit_msg", &stat_buf) == -1 || !S_ISREG(stat_buf.st_mode)) {
		fprintf(stderr_test_fp, "[%s...%s] Expect .alog/.log_edit_msg file to have been created.\n", name, case_name);
		exit_code = -1;
	}
	else if (!(stat_buf.st_mode & S_IRUSR &&
		  stat_buf.st_mode & S_IWUSR &&
		  !(stat_buf.st_mode & S_IXUSR) &&
		  !(stat_buf.st_mode & S_IRGRP) &&
		  !(stat_buf.st_mode & S_IWGRP) &&
		  !(stat_buf.st_mode & S_IXGRP) &&
		  !(stat_buf.st_mode & S_IROTH) &&
		  !(stat_buf.st_mode & S_IWOTH) &&
		  !(stat_buf.st_mode & S_IXOTH))) {
		fprintf(stderr_test_fp, "[%s...%s] Expect .alog/.log_edit_msg file to have permission 0600, but got 0%o.\n", name, case_name, stat_buf.st_mode & 0777);
		exit_code = -1;
	}

	remove_alog_test_dir();
}

int test_create_alog_dir() {
	normal_create_alog_dir();

	if (exit_code == 0) {
		fprintf(stdout_test_fp, "[%s] PASSED\n", name);
	} else {
		fprintf(stderr_test_fp, "[%s] FAILED\n", name);
	}

	return exit_code;
}
