#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/mman.h>

#include "tests.h"

int main(int argc, char *argv[]) {
	int exit_code = 0;

	/* Replace main program's STDERR with a memory file.
	   This new memory file is use by the test program to
	   read error messages written by the main program.  */
	close(2);
	memfd_create("test_stderr_file", 0);

	exit_code = test_start_activity();

	exit(exit_code);
}
