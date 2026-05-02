#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "alog.h"

int main(int argc, char *argv[]) {
	int exit_code = 0;

	char *home_path = getenv("HOME");
	char alog_path[PATH_MAX];
	strcpy(alog_path, home_path);
	strcat(alog_path, "/.alog");

	start_activity("project-1", alog_path);

	exit(exit_code);
}
