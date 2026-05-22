#include <stdio.h>

#include "alog.h"

int list_activities(char *alog_path_arg, int types) {

	if (types & T_REC) {
		/* List recorded activities.  */
		fprintf(stdout, "[RECORDED]\n");
	}
	if (types & T_ACT) {
		/* List active activities.  */
		fprintf(stdout, "[ACTIVE]\n");
	}

	return 0;
}
