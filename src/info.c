#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#include "alog.h"
#include "buffer.h"

static buf_s *buf_p;
static lexer_s *lexer_p;
static token_s token;

static bool silent;

/* Break elapsed time (in seconds) into hours/minutes/seconds.

   hours = ((elapsed_time / 60) / 60)
   minutes = hours_remainder * 60
   seconds = minutes_remainder * 60
*/
static void break_down_elapsed_time(double elapsed_time, double *hours, double *minutes, double *seconds) {
	double result = (elapsed_time / 60) / 60;
	result = modf(result, hours);
	result = result * 60;
	result = modf(result, minutes);
	*seconds = result * 60;
}

/* Write log start date into buffer as is.  */
static void date_rule() {
	while (token.type != TOKEN_NEW_LINE) {
		write_to_buffer(buf_p, token.lex, token.len);
		token = get_next_token(lexer_p);
	}
	/* Write a newline char to the end of the
	   log start date line.  */
	write_to_buffer(buf_p, token.lex, token.len);

	/* Token for next rule.  */
	token = get_next_token(lexer_p);
}

/* Convert elapsed time in seconds into
   hours/minutes/seconds format.  */
static void elapsed_time_rule() {
	write_to_buffer(buf_p, "Elapsed Time: ", 14);

	/* Skip over heading text until found
	   elapsed time number.  */
	while (token.type != TOKEN_NUMBER) {
		token = get_next_token(lexer_p);
	}

	/* Copy elapsed time in seconds from token string.  */
	char *elapsed_time_s_str = (char *)malloc(sizeof(char) * (token.len + 1));
	elapsed_time_s_str[token.len] = '\0';
	for (int i = 0; i < token.len; i++) {
		elapsed_time_s_str[i] = token.lex[i];
	}

	/* Convert elapsed time format.  */
	double elapsed_time = atof(elapsed_time_s_str);
	double hours;
	double minutes;
	double seconds;
	break_down_elapsed_time(elapsed_time, &hours, &minutes, &seconds);

	/* Allocate enought space for the following
	   elapsed time format:
	   "xxxx hours xx minutes xx seconds\0"  */
	char elapsed_time_formatted_str[33];
	if (hours > 9999) {
		/* Cap activity log hours at 9999 hours.  */
		hours = 9999;
		minutes = 0;
		seconds = 0;
	}
	sprintf(elapsed_time_formatted_str, "%.0f hours %.0f minutes %.0f seconds", hours, minutes, seconds);
	write_to_buffer(buf_p, elapsed_time_formatted_str, strlen(elapsed_time_formatted_str));

	/* Write a newline char to the end of the
	   elapsed time line.  */
	token = get_next_token(lexer_p);
	write_to_buffer(buf_p, token.lex, token.len);

	/* Token for next rule.  */
	token = get_next_token(lexer_p);
}

/* Write log message to buffer. Prefix and postfix
   the log message with newlines for formatting.

   If the O_SLNT (silent) option is provided, then
   don't write the log message to the buffer.  */
static void log_msg_rule() {
	if (silent) {
		/* Skip over log message.  */
		while (token.type != TOKEN_DIVIDER) {
			token = get_next_token(lexer_p);
		}
		return;
	}

	/* Prefix log message with a newline.  */
	write_to_buffer(buf_p, "\n", 1);
	while (token.type != TOKEN_DIVIDER) {
		write_to_buffer(buf_p, token.lex, token.len);
		token = get_next_token(lexer_p);
	}

	/* Postfix log message with a newline.  */
	write_to_buffer(buf_p, "\n", 1);
}

/* Write log divider to buffer as is.  */
static void log_divider_rule() {
	/* DIVIDER token to buffer.   */
	write_to_buffer(buf_p, token.lex, token.len);
	token = get_next_token(lexer_p);
	/* Write new line to buffer.  */
	write_to_buffer(buf_p, token.lex, token.len);

	/* Token for next rule.  */
	token = get_next_token(lexer_p);
}

static void log_rule() {
	date_rule();
	elapsed_time_rule();
	log_msg_rule();
	log_divider_rule();
}

int get_activity_info(char *actv_name_arg, char *alog_path_arg, int types, int options) {
	buf_p = init_buffer();

	silent = options & O_SLNT;

	if (types & T_REC || types == 0) {
		write_to_buffer(buf_p, "[RECORDED]\n", 11);

		char log_file_path[PATH_MAX];
		strcpy(log_file_path, alog_path_arg);
		strcat(log_file_path, "/");
		strcat(log_file_path, actv_name_arg);

		struct stat stat_buf;
		if (stat(log_file_path, &stat_buf) != -1) {
			/* Log file exist.  */
			int log_fd = open(log_file_path, O_RDONLY);
			if (log_fd == -1) {
				fprintf(stderr, "[ERROR] Failed to open \"%s\": %s\n", log_file_path, strerror(errno));
				destroy_buffer(buf_p);
				return -1;
			}

			off_t f_size = lseek(log_fd, 0, SEEK_END);
			char *log_str = (char *)malloc(sizeof(char) * (f_size + 1));
			/* Add a null terminator at the end of the log string
			   so that the lexer can pick it up as an EOF token.  */
			log_str[f_size] = '\0';
			lseek(log_fd, 0, SEEK_SET);
			if (read(log_fd, log_str, f_size) == -1) {
				fprintf(stderr, "[ERROR] Failed to read \"%s\": %s\n", log_file_path, strerror(errno));
				close(log_fd);
				destroy_buffer(buf_p);
				return -1;
			}
			close(log_fd);

			/* Parse recorded activity logs, format it, and write it
			   to a buffer. The buffer will be written to STDOUT.  */
			lexer_p = lexer(log_str);
			token = get_next_token(lexer_p);
			while (token.type != TOKEN_EOF) {
				log_rule();
			}

			free(lexer_p);
		}
	}

	if (types & T_ACT || types == 0) {
		write_to_buffer(buf_p, "[ACTIVE]\n", 9);

		char tmp_file_path[PATH_MAX];
		strcpy(tmp_file_path, alog_path_arg);
		strcat(tmp_file_path, "/.tmp/");
		strcat(tmp_file_path, actv_name_arg);

		struct stat stat_buf;
		if (stat(tmp_file_path, &stat_buf) != -1) {
			/* Tmp file exist i.e. activity is active.  */
			int tmp_fd = open(tmp_file_path, O_RDONLY);
			if (tmp_fd == -1) {
				fprintf(stderr, "[ERROR] Failed to open \"%s\": %s\n", tmp_file_path, strerror(errno));
				destroy_buffer(buf_p);
				return -1;
			}

			char start_time_epoch_str[20];
			ssize_t len = read(tmp_fd, start_time_epoch_str, 19);
			close(tmp_fd);
			if (len == -1) {
				fprintf(stderr, "[ERROR] Failed to read from file \"%s\": %s\n", tmp_file_path, strerror(errno));
				destroy_buffer(buf_p);
				return -1;
			}
			start_time_epoch_str[len] = '\0';
			time_t start_time = atoi(start_time_epoch_str);

			/* Write start time formatted string to buffer.  */
			char *start_time_str = ctime(&start_time);
			write_to_buffer(buf_p, start_time_str, strlen(start_time_str));

			/* Write formatted elapsed time to buffer.  */
			double elapsed_time = time(NULL) - start_time;
			double hours;
			double minutes;
			double seconds;
			break_down_elapsed_time(elapsed_time, &hours, &minutes, &seconds);
			/* Allocate enought space for the following
			   elapsed time format:
			   "xxxx hours xx minutes xx seconds\n\0"  */
			char elapsed_time_formatted_str[34];
			if (hours > 9999) {
				/* Cap activity log hours at 9999 hours.  */
				hours = 9999;
				minutes = 0;
				seconds = 0;
			}
			sprintf(elapsed_time_formatted_str, "%.0f hours %.0f minutes %.0f seconds\n", hours, minutes, seconds);
			write_to_buffer(buf_p, "Elapsed Time: ", 14);
			write_to_buffer(buf_p, elapsed_time_formatted_str, strlen(elapsed_time_formatted_str));
		}
	}

	if (buf_p->length > 0) {
		write_to_buffer(buf_p, "\0", 1);
		fprintf(stdout, "%s", buf_p->mem);
	}

	destroy_buffer(buf_p);

	return 0;
}
