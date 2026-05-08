#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

/* Create a tmp activity file in alog_path_arg/.tmp/ and record
   the activity start time. The tmp file will be read when
   stopping the activity to calculate the activity elapsed time.  */
int start_activity(char *actv_name_arg, char *alog_path_arg) {
	char *actv_name = malloc(sizeof(char) * strlen(actv_name_arg));
	strcpy(actv_name, actv_name_arg);

	char alog_path[PATH_MAX];
	strcpy(alog_path, alog_path_arg);

	/* Check if activity is already active.  */
	struct stat stat_buf;
	strcat(alog_path, "/.tmp/");
	strcat(alog_path, actv_name);
	if (stat(alog_path, &stat_buf) != -1) {
		/* Activity is already active.  */
		fprintf(stderr, "[ERROR] The activity: \"%s\" is already active.\n", actv_name);
		free(actv_name);
		return -1;
	}

	int actv_tmp_fd;
	actv_tmp_fd = open(alog_path, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if (actv_tmp_fd == -1) {
		fprintf(stderr, "[ERROR] Failed to create tmp activity file: %s\n", strerror(errno));
		free(actv_name);
		return -1;
	}

	time_t start_time = time(NULL);
	/* Expect maximum 19 digits for 64-bit system, but allocating an
	   extra byte for sprintf to add a '\0' string terminator at the end
	   of the string.  */
	char time_buf[20];
	sprintf(time_buf, "%ld", start_time);
	if (write(actv_tmp_fd, time_buf, strlen(time_buf)) == -1) {
		fprintf(stderr, "[ERROR] Failed to write start time to tmp activity file: %s\n", strerror(errno));
		free(actv_name);
		close(actv_tmp_fd);
		return -1;
	}
	close(actv_tmp_fd);

	fprintf(stdout, "Recording activity: \"%s\".\n", actv_name);

	free(actv_name);
	return 0;
}
