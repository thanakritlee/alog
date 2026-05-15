#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include "buffer.h"

/* Stop the active activity and record the activity log.

   The process consist of the following steps:
   1. Check if tmp activity file exist.
   2. Open and read tmp activity file.
   3. Calculate activity elapsed time.
   4. Retrieve activity log message (either through command-line argument or editor).
   5. Open (and create, if it doesn't exist) the activity log file.
   6. Append log to activity file.
   7. Output success message.
   8. Unlink tmp activity file.  */
int stop_activity(char *actv_name_arg, char *alog_path_arg, char *log_msg_arg) {
	char *actv_name = (char*)malloc(sizeof(char) * (strlen(actv_name_arg) + 1));
	strcpy(actv_name, actv_name_arg);
	actv_name[strlen(actv_name_arg)] = '\0';

	/* Check if activity is inactive.  */
	char tmp_path[PATH_MAX];
	strcpy(tmp_path, alog_path_arg);
	struct stat stat_buf;
	strcat(tmp_path, "/.tmp/");
	strcat(tmp_path, actv_name);
	if (stat(tmp_path, &stat_buf) == -1) {
		/* The tmp activity file doesn't exist
		   i.e. activity is inactive.  */
		fprintf(stderr, "[ERROR] The activity: \"%s\" is inactive.\n", actv_name);
		free(actv_name);
		return -1;
	}

	/* Read activity start time to calculate elapsed time.  */
	int tmp_fd = open(tmp_path, O_RDONLY);
	if (tmp_fd == -1) {
		fprintf(stderr, "[ERROR] Failed to open tmp file: %s\n", strerror(errno));
		free(actv_name);
		return -1;
	}
	char start_time_buf[20];
	ssize_t time_char_len = read(tmp_fd, start_time_buf, 19);
	if (time_char_len == -1) {
		fprintf(stderr, "[ERROR] Failed to read tmp file: %s\n", strerror(errno));
		free(actv_name);
		close(tmp_fd);
		return -1;
	}
	close(tmp_fd);
	start_time_buf[time_char_len] = '\0';
	time_t start_time = atoi(start_time_buf);
	char *start_time_str = ctime(&start_time);

	/* Write activity start time and elapsed time to a buffer.  */
	time_t end_time = time(NULL);
	double elapsed_time = difftime(end_time, start_time);
	char elapsed_time_str[21];
	sprintf(elapsed_time_str, "%0.lf\n", elapsed_time);
	Buffer *log_buf = init_buffer();
	write_to_buffer(log_buf, start_time_str, strlen(start_time_str));
	write_to_buffer(log_buf, "Elapsed Time (s): ", 18);
	write_to_buffer(log_buf, elapsed_time_str, strlen(elapsed_time_str));

	/* If log message hasn't been provided as a command-line argument,
	   then open a text editor on file .alog/.log_edit_msg and let user
	   write their log message in there.

	   Once written to file, read the log message from the file.  */
	char *log_msg = NULL;
	if (log_msg_arg != NULL) {
		log_msg = (char*)malloc(sizeof(char) * (strlen(log_msg_arg) + 1));
		strcpy(log_msg, log_msg_arg);
		log_msg[strlen(log_msg_arg)] = '\0';
	}
	if (log_msg == NULL) {
		/* Log message not provided as command-line argument.
		   Let user input log message via text editor.  */

		char log_edit_msg_path[PATH_MAX];
		strcpy(log_edit_msg_path, alog_path_arg);
		strcat(log_edit_msg_path, "/.log_edit_msg");

		/* Truncate file before start editing.  */
		if (truncate(log_edit_msg_path, 0) == -1) {
			fprintf(stderr, "[ERROR] Failed to truncate log edit message file: %s\n", strerror(errno));
			return -1;
		}

		/* Create child process to edit the log message
		   file with nano.  */
		char edit_cmd[PATH_MAX + 5];
		sprintf(edit_cmd, "nano %s", log_edit_msg_path);
		system(edit_cmd);

		int log_edit_msg_fd = open(log_edit_msg_path, O_RDONLY);
		if (log_edit_msg_fd == -1) {
			fprintf(stderr, "[ERROR] Failed to open log edit message file: %s\n", strerror(errno));
			return -1;
		}
		/* Offset at end of file indicate the file size (not including \0).  */
		off_t f_size = lseek(log_edit_msg_fd, 0, SEEK_END);
		/* Plus 1 for the \0 byte.  */
		log_msg = (char*)malloc(sizeof(char) * (f_size + 1));
		lseek(log_edit_msg_fd, 0, SEEK_SET);
		if (read(log_edit_msg_fd, log_msg, f_size) == -1) {
			fprintf(stderr, "[ERROR] Failed to read from log edit message file: %s\n", strerror(errno));
			close(log_edit_msg_fd);
			return -1;
		}
		log_msg[f_size] = '\0';
		close(log_edit_msg_fd);
	}

	/* Add a single trailing newline at the end of the log message.  */
	char *log_msg_str = (char*)malloc(sizeof(char) * (strlen(log_msg) + 2));
	strcpy(log_msg_str, log_msg);
	/* last_nl is a pointer to the last occurence of \n in the log
	   message, if it exist.  */
	char *last_nl = strrchr(log_msg_str, '\n');
	if (last_nl == NULL) {
		/* No trailing newline provided in log message.
		   Add in a trailing newline.  */
		log_msg_str[strlen(log_msg)] = '\n';
		log_msg_str[strlen(log_msg) + 1] = '\0';
	} else {
		/* Trailing newline found in log message.
		   Replace all trailing newlines with a single newline.  */
		while (*last_nl == '\n') {
			/* Decrement the pointer until a character that's
			   not a newline is found.  */
			last_nl--;
		}
		/* Replace the char byte after the first trailing newline
		   char with the terminating null byte, thereby cutting
		   off the rest of the trailing newlines in the log
		   message.  */
		last_nl = last_nl + 2;
		*last_nl = '\0';
	}
	write_to_buffer(log_buf, log_msg_str, strlen(log_msg_str));
	write_to_buffer(log_buf, "==========\n", 11);

	/* Append log to activity log file.
	   Create the file, if it doesn't already exist.  */
	char log_path[PATH_MAX];
	strcpy(log_path, alog_path_arg);
	strcat(log_path, "/");
	strcat(log_path, actv_name);
	int log_fd = open(log_path, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
	if (log_fd == -1) {
		fprintf(stderr, "[ERROR] Failed to open log file: %s\n", strerror(errno));
		free(actv_name);
		return -1;
	}
	write(log_fd, log_buf->mem, log_buf->length);
	close(log_fd);
	destroy_buffer(log_buf);

	fprintf(stdout, "Activity recorded.\n");

	free(log_msg);
	free(log_msg_str);
	unlink(tmp_path);
	free(actv_name);
	return 0;
}
