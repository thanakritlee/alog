#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>

#include "tests.h"

FILE *stdout_test_fp;
FILE *stderr_test_fp;
int main_stdout_fd;
int main_stderr_fd;

int main(int argc, char *argv[]) {
	int exit_code = 0;

	/* Replace main program's STDOUT and STDERR with temporary files.
	   These new temporary files are used by the test program to
	   read result messages and error messages written by the main program.  */
	char test_stdout_tmp_filename[PATH_MAX] = "test_stdout_XXXXXX";
	char test_stderr_tmp_filename[PATH_MAX] = "test_stderr_XXXXXX";
	int tmp_out_fd = mkstemp(test_stdout_tmp_filename); /* FD: 3.  */
	int tmp_err_fd = mkstemp(test_stderr_tmp_filename); /* FD: 4.  */
	close(tmp_out_fd); /* FD 3 removed.  */
	close(tmp_err_fd); /* FD 4 removed.  */

	/* Save Standard I/O: STDOUT/STDERR file descriptors
	   and opened file handles.
	   This is the STDOUT/STDERR I/O created by the parent
	   process (e.g. shell).  */
	int saved_stdout_fd = dup(STDOUT_FILENO); /* FD: 3.  */
	int saved_stderr_fd = dup(STDERR_FILENO); /* FD: 4.  */

	/* Re-open the stdout/stderr FILE streams to point to
	   the temporary files.

	   FD 1/2 are closed and re-opened to point to the
	   temporary files.  */
	freopen(test_stdout_tmp_filename, "r+", stdout); /* FD: 1.  */
	freopen(test_stderr_tmp_filename, "r+", stderr); /* FD: 2.  */
	main_stdout_fd = fileno(stdout); /* FD: 1.  */
	main_stderr_fd = fileno(stderr); /* FD: 2.  */

	/* Set buffering mode of STDOUT and STDERR FILE streams use by
	   the main program.

	   Set the FILE streams to not buffer any data in the stdio
	   buffer, and instead send the data directly to the kernel
	   buffer. This allows the test cases to read the data from
	   the kernel buffer without having to manually flush the
	   stdio buffer each time before reading.  */
	if (setvbuf(stdout, NULL, _IONBF, 0) != 0) {
		exit(1);
	}
	if (setvbuf(stderr, NULL, _IONBF, 0) != 0) {
		exit(1);
	}

	/* Open the saved Standard I/O FD: STDOUT and STDERR as FILE pointers.
	   This enables the test program to write the test results to
	   the terminal.

	   It also enables I/O redirections from the shell.  */
	stdout_test_fp = fdopen(saved_stdout_fd, "w");
	stderr_test_fp = fdopen(saved_stderr_fd, "w");

	/* Run test cases.  */
	exit_code = test_start_activity();
	exit_code = test_stop_activity() != 0 ? -1 : exit_code;
	exit_code = test_create_alog_dir() != 0 ? -1 : exit_code;
	exit_code = test_list_activities() != 0 ? -1 : exit_code;
	exit_code = test_activity_info() != 0 ? -1 : exit_code;
	exit_code = test_lexer() != 0 ? -1 : exit_code;
	exit_code = test_cancel_activity() != 0 ? -1 : exit_code;
	exit_code = test_delete_activity() != 0 ? -1 : exit_code;

	/* Close main/test program's STDOUT and STDERR.  */
	close(main_stdout_fd);
	close(main_stderr_fd);
	close(saved_stdout_fd);
	close(saved_stderr_fd);

	/* Remove temporary files.  */
	unlink(test_stdout_tmp_filename);
	unlink(test_stderr_tmp_filename);

	exit(exit_code);
}
