#include <stdio.h>

#include "alog.h"

int list_activities(char *alog_path_arg, int types) {

	if (types & T_REC) {
		/* List recorded activities.  */
		fprintf(stdout, "[RECORDED]\n");
	}

	return 0;
}
