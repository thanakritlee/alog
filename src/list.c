#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "alog.h"

static int print_activities(char *dir_path) {
	struct dirent **dir_ents;
	int n;
	/* Scan directory for files, sort them in alphabetical
	   order, then print them to STDOUT.
	   Exclude directories: . .. .tmp .artefacts  */
	n = scandir(dir_path, &dir_ents, NULL, alphasort);
	if (n == -1) {
		fprintf(stderr, "[ERROR] Failed to scan directory \"%s\": %s\n", dir_path, strerror(errno));
		return -1;
	}

	for (int i = 0; i < n; i++) {
		if (strcmp(dir_ents[i]->d_name, ".") == 0 || strcmp(dir_ents[i]->d_name, "..") == 0 ||
			strcmp(dir_ents[i]->d_name, ".tmp") == 0 || strcmp(dir_ents[i]->d_name, ".artefacts") == 0) {
			/* Skip directory links.  */
			continue;
		}
		fprintf(stdout, "%s\n", dir_ents[i]->d_name);
		free(dir_ents[i]);
	}
	free(dir_ents);
	return 0;
}

int list_activities(char *alog_path_arg, int types) {

	if (types & T_REC) {
		/* List recorded activities.  */
		fprintf(stdout, "[RECORDED]\n");

		if (print_activities(alog_path_arg) == -1) {
			return -1;
		}
	}
	if (types & T_ACT) {
		/* List active activities.  */
		fprintf(stdout, "[ACTIVE]\n");

		char tmp_path[PATH_MAX];
		strcpy(tmp_path, alog_path_arg);
		strcat(tmp_path, "/.tmp");
		if (print_activities(tmp_path) == -1) {
			return -1;
		}
	}

	return 0;
}
