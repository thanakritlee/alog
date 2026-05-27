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

#include "alog.h"
#include "buffer.h"

static buf_s *buf_p;
static lexer_s *lexer_p;
static token_s token;

static bool silent;

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
	double result = (elapsed_time / 60) / 60;
	double hours;
	result = modf(result, &hours);

	double minutes;
	result = result * 60;
	result = modf(result, &minutes);

	double seconds;
	seconds = result * 60;

	/* Allocate enought space for the following
	   elapsed time format:
	   "xxxx hours xx minutes xx seconds\0"  */
	char elapsed_time_formatted_str[33];
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

	if (types & T_REC) {
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

			lexer_p = lexer(log_str);
			token = get_next_token(lexer_p);
			while (token.type != TOKEN_EOF) {
				log_rule();
			}

			free(lexer_p);
		}
	}

	if (types & T_ACT) {
		write_to_buffer(buf_p, "[ACTIVE]\n", 9);
	}

	if (buf_p->length > 0) {
		write_to_buffer(buf_p, "\0", 1);
		fprintf(stdout, "%s", buf_p->mem);
	}

	destroy_buffer(buf_p);

	return 0;
}
