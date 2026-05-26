#include <stdio.h>
#include <stdlib.h>

#include "tests.h"
#include "../src/alog.h"

static const char *name = "test_lexer";
static int exit_code = 0;

static void date() {
	char *case_name = "date";

	char *log_str = "Tue May 19 13:54:25 2026\n\0";

	lexer_s *lexer_p;
	token_s token;
	lexer_p = lexer(log_str);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_STRING, exit_code, "[%s...%s] Expect token type: TOKEN_STRING.\n", name, case_name);
	exit_code = assert_str(token.lex, "Tue", token.len, exit_code, "[%s...%s] Expect token lexeme: \"Tue\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_SPACE, exit_code, "[%s...%s] Expect token type: TOKEN_SPACE.\n", name, case_name);
	exit_code = assert_str(token.lex, " ", token.len, exit_code, "[%s...%s] Expect token lexeme: \" \".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_STRING, exit_code, "[%s...%s] Expect token type: TOKEN_STRING.\n", name, case_name);
	exit_code = assert_str(token.lex, "May", token.len, exit_code, "[%s...%s] Expect token lexeme: \"May\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_SPACE, exit_code, "[%s...%s] Expect token type: TOKEN_SPACE.\n", name, case_name);
	exit_code = assert_str(token.lex, " ", token.len, exit_code, "[%s...%s] Expect token lexeme: \" \".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_NUMBER, exit_code, "[%s...%s] Expect token type: TOKEN_NUMBER.\n", name, case_name);
	exit_code = assert_str(token.lex, "19", token.len, exit_code, "[%s...%s] Expect token lexeme: \"19\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_SPACE, exit_code, "[%s...%s] Expect token type: TOKEN_SPACE.\n", name, case_name);
	exit_code = assert_str(token.lex, " ", token.len, exit_code, "[%s...%s] Expect token lexeme: \" \".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_NUMBER, exit_code, "[%s...%s] Expect token type: TOKEN_NUMBER.\n", name, case_name);
	exit_code = assert_str(token.lex, "13", token.len, exit_code, "[%s...%s] Expect token lexeme: \"13\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_COLON, exit_code, "[%s...%s] Expect token type: TOKEN_COLON.\n", name, case_name);
	exit_code = assert_str(token.lex, ":", token.len, exit_code, "[%s...%s] Expect token lexeme: \":\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_NUMBER, exit_code, "[%s...%s] Expect token type: TOKEN_NUMBER.\n", name, case_name);
	exit_code = assert_str(token.lex, "54", token.len, exit_code, "[%s...%s] Expect token lexeme: \"54\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_COLON, exit_code, "[%s...%s] Expect token type: TOKEN_COLON.\n", name, case_name);
	exit_code = assert_str(token.lex, ":", token.len, exit_code, "[%s...%s] Expect token lexeme: \":\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_NUMBER, exit_code, "[%s...%s] Expect token type: TOKEN_NUMBER.\n", name, case_name);
	exit_code = assert_str(token.lex, "25", token.len, exit_code, "[%s...%s] Expect token lexeme: \"25\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_SPACE, exit_code, "[%s...%s] Expect token type: TOKEN_SPACE.\n", name, case_name);
	exit_code = assert_str(token.lex, " ", token.len, exit_code, "[%s...%s] Expect token lexeme: \" \".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_NUMBER, exit_code, "[%s...%s] Expect token type: TOKEN_NUMBER.\n", name, case_name);
	exit_code = assert_str(token.lex, "2026", token.len, exit_code, "[%s...%s] Expect token lexeme: \"2026\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_NEW_LINE, exit_code, "[%s...%s] Expect token type: TOKEN_NEW_LINE.\n", name, case_name);
	exit_code = assert_str(token.lex, "\n", token.len, exit_code, "[%s...%s] Expect token lexeme: \"\\n\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_EOF, exit_code, "[%s...%s] Expect token type: TOKEN_EOF.\n", name, case_name);

	free(lexer_p);
}

static void elapsed_time() {
	char *case_name = "elapsed_time";

	char *log_str = "Elapsed Time (s): 1273\n\0";

	lexer_s *lexer_p;
	token_s token;
	lexer_p = lexer(log_str);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_STRING, exit_code, "[%s...%s] Expect token type: TOKEN_STRING.\n", name, case_name);
	exit_code = assert_str(token.lex, "Elapsed", token.len, exit_code, "[%s...%s] Expect token lexeme: \"Elapsed\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_SPACE, exit_code, "[%s...%s] Expect token type: TOKEN_SPACE.\n", name, case_name);
	exit_code = assert_str(token.lex, " ", token.len, exit_code, "[%s...%s] Expect token lexeme: \" \".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_STRING, exit_code, "[%s...%s] Expect token type: TOKEN_STRING.\n", name, case_name);
	exit_code = assert_str(token.lex, "Time", token.len, exit_code, "[%s...%s] Expect token lexeme: \"Time\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_SPACE, exit_code, "[%s...%s] Expect token type: TOKEN_SPACE.\n", name, case_name);
	exit_code = assert_str(token.lex, " ", token.len, exit_code, "[%s...%s] Expect token lexeme: \" \".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_STRING, exit_code, "[%s...%s] Expect token type: TOKEN_STRING.\n", name, case_name);
	exit_code = assert_str(token.lex, "(s)", token.len, exit_code, "[%s...%s] Expect token lexeme: \"(s)\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_COLON, exit_code, "[%s...%s] Expect token type: TOKEN_COLON.\n", name, case_name);
	exit_code = assert_str(token.lex, ":", token.len, exit_code, "[%s...%s] Expect token lexeme: \":\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_SPACE, exit_code, "[%s...%s] Expect token type: TOKEN_SPACE.\n", name, case_name);
	exit_code = assert_str(token.lex, " ", token.len, exit_code, "[%s...%s] Expect token lexeme: \" \".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_NUMBER, exit_code, "[%s...%s] Expect token type: TOKEN_NUMBER.\n", name, case_name);
	exit_code = assert_str(token.lex, "1273", token.len, exit_code, "[%s...%s] Expect token lexeme: \"1273\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_NEW_LINE, exit_code, "[%s...%s] Expect token type: TOKEN_NEW_LINE.\n", name, case_name);
	exit_code = assert_str(token.lex, "\n", token.len, exit_code, "[%s...%s] Expect token lexeme: \"\\n\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_EOF, exit_code, "[%s...%s] Expect token type: TOKEN_EOF.\n", name, case_name);

	free(lexer_p);
}

static void log_msg() {
	char *case_name = "log_msg";

	char *log_str = "Log message 1.\n\0";

	lexer_s *lexer_p;
	token_s token;
	lexer_p = lexer(log_str);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_STRING, exit_code, "[%s...%s] Expect token type: TOKEN_STRING.\n", name, case_name);
	exit_code = assert_str(token.lex, "Log", token.len, exit_code, "[%s...%s] Expect token lexeme: \"Log\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_SPACE, exit_code, "[%s...%s] Expect token type: TOKEN_SPACE.\n", name, case_name);
	exit_code = assert_str(token.lex, " ", token.len, exit_code, "[%s...%s] Expect token lexeme: \" \".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_STRING, exit_code, "[%s...%s] Expect token type: TOKEN_STRING.\n", name, case_name);
	exit_code = assert_str(token.lex, "message", token.len, exit_code, "[%s...%s] Expect token lexeme: \"message\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_SPACE, exit_code, "[%s...%s] Expect token type: TOKEN_SPACE.\n", name, case_name);
	exit_code = assert_str(token.lex, " ", token.len, exit_code, "[%s...%s] Expect token lexeme: \" \".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_NUMBER, exit_code, "[%s...%s] Expect token type: TOKEN_NUMBER.\n", name, case_name);
	exit_code = assert_str(token.lex, "1", token.len, exit_code, "[%s...%s] Expect token lexeme: \"1\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_STRING, exit_code, "[%s...%s] Expect token type: TOKEN_STRING.\n", name, case_name);
	exit_code = assert_str(token.lex, ".", token.len, exit_code, "[%s...%s] Expect token lexeme: \".\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_NEW_LINE, exit_code, "[%s...%s] Expect token type: TOKEN_NEW_LINE.\n", name, case_name);
	exit_code = assert_str(token.lex, "\n", token.len, exit_code, "[%s...%s] Expect token lexeme: \"\\n\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_EOF, exit_code, "[%s...%s] Expect token type: TOKEN_EOF.\n", name, case_name);

	free(lexer_p);
}

static void divider() {
	char *case_name = "divider";

	char *log_str = "==========\n\0";

	lexer_s *lexer_p;
	token_s token;
	lexer_p = lexer(log_str);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_DIVIDER, exit_code, "[%s...%s] Expect token type: TOKEN_DIVIDER.\n", name, case_name);
	exit_code = assert_str(token.lex, "==========", token.len, exit_code, "[%s...%s] Expect token lexeme: \"==========\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_NEW_LINE, exit_code, "[%s...%s] Expect token type: TOKEN_NEW_LINE.\n", name, case_name);
	exit_code = assert_str(token.lex, "\n", token.len, exit_code, "[%s...%s] Expect token lexeme: \"\\n\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_EOF, exit_code, "[%s...%s] Expect token type: TOKEN_EOF.\n", name, case_name);

	free(lexer_p);
}

static void divider_string_1() {
	char *case_name = "divider_string_1";

	char *log_str = "=========\n\0";

	lexer_s *lexer_p;
	token_s token;
	lexer_p = lexer(log_str);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_STRING, exit_code, "[%s...%s] Expect token type: TOKEN_STRING.\n", name, case_name);
	exit_code = assert_str(token.lex, "=========", token.len, exit_code, "[%s...%s] Expect token lexeme: \"=========\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_NEW_LINE, exit_code, "[%s...%s] Expect token type: TOKEN_NEW_LINE.\n", name, case_name);
	exit_code = assert_str(token.lex, "\n", token.len, exit_code, "[%s...%s] Expect token lexeme: \"\\n\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_EOF, exit_code, "[%s...%s] Expect token type: TOKEN_EOF.\n", name, case_name);

	free(lexer_p);
}

static void divider_string_2() {
	char *case_name = "divider_string_2";

	char *log_str = "===========\n\0";

	lexer_s *lexer_p;
	token_s token;
	lexer_p = lexer(log_str);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_DIVIDER, exit_code, "[%s...%s] Expect token type: TOKEN_DIVIDER.\n", name, case_name);
	exit_code = assert_str(token.lex, "==========", token.len, exit_code, "[%s...%s] Expect token lexeme: \"==========\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_STRING, exit_code, "[%s...%s] Expect token type: TOKEN_STRING.\n", name, case_name);
	exit_code = assert_str(token.lex, "=", token.len, exit_code, "[%s...%s] Expect token lexeme: \"=\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_NEW_LINE, exit_code, "[%s...%s] Expect token type: TOKEN_NEW_LINE.\n", name, case_name);
	exit_code = assert_str(token.lex, "\n", token.len, exit_code, "[%s...%s] Expect token lexeme: \"\\n\".\n", name, case_name);

	token = get_next_token(lexer_p);
	exit_code = assert_int(token.type, TOKEN_EOF, exit_code, "[%s...%s] Expect token type: TOKEN_EOF.\n", name, case_name);

	free(lexer_p);
}

int test_lexer() {
	date();
	elapsed_time();
	log_msg();
	divider();
	divider_string_1();
	divider_string_2();

	if (exit_code == 0) {
		fprintf(stdout_test_fp, "[%s] PASSED\n", name);
	} else {
		fprintf(stderr_test_fp, "[%s] FAILED\n", name);
	}

	return exit_code;
}
