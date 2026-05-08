#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "tests.h"

FILE *stdout_test_fp;
FILE *stderr_test_fp;

int main(int argc, char *argv[]) {
	int exit_code = 0;

	/* Replace main program's STDOUT and STDERR with a memory file.
	   This new memory file is use by the test program to
	   read stdout and error messages written by the main program.  */
	close(1); /* STDOUT.  */
	close(2); /* STDERR.  */
	memfd_create("test_stdout_file", 0); /* New STDOUT with FD: 1.  */
	memfd_create("test_stderr_file", 0); /* New STDERR with FD: 2.  */
	/* Set buffering mode of STDOUT and STDERR file streams use by
	   the main program.

	   Set the file streams to not buffer any data in the stdio
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

	/* Open terminal device file for test runner to use as STDOUT
	   and STDERR for writing test results.  */
	int stdout_test_fd;
	int stderr_test_fd;
	/* New STDOUT for test program. FD: 3.  */
	if (stdout_test_fd = open("/dev/tty", O_WRONLY) == -1) {
		exit(1);
	}
	/* New STDERR for test program. FD: 4.  */
	if (stderr_test_fd = open("/dev/tty", O_WRONLY) == -1) {
		exit(1);
	}
	/* Open the file descriptors as FILE pointers.*/
	stdout_test_fp = fdopen(stdout_test_fd, "w");
	stderr_test_fp = fdopen(stderr_test_fd, "w");

	exit_code = test_start_activity();

	/* Close main program's and test program's STDOUT and STDERR.  */
	close(1);
	close(2);
	close(3);
	close(4);

	exit(exit_code);
}
