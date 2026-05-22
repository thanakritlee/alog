#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "alog.h"

int list_activities(char *alog_path_arg, int types) {

	if (types & T_REC) {
		/* List recorded activities.  */
		fprintf(stdout, "[RECORDED]\n");

		struct dirent **dir_ents;
		int n;
		/* Scan .alog directory for files, sort them in alphabetical
		   order, then print them to STDOUT.
		   Exclude directories: . .. .tmp .artefacts  */
		n = scandir(alog_path_arg, &dir_ents, NULL, alphasort);
		if (n == -1) {
			fprintf(stderr, "[ERROR] Failed to scan directory \"%s\": %s\n", alog_path_arg, strerror(errno));
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
	}
	if (types & T_ACT) {
		/* List active activities.  */
		fprintf(stdout, "[ACTIVE]\n");
	}

	return 0;
}
