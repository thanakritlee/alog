#include <stdio.h>

#include "alog.h"

int get_activity_info(char *actv_name_arg, char *alog_path_arg, int types, int options) {
	if (types & T_REC) {
		fprintf(stdout, "[RECORDED]\n");
	}

	if (types & T_ACT) {
		fprintf(stdout, "[ACTIVE]\n");
	}

	return 0;
}
