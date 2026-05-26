#include <stdlib.h>
#include <stdbool.h>

#include "alog.h"

/* Construct and initialise a lexer.

   The struct pointer memory is allocated using malloc.
   Caller will need to free the allocated memory once done.  */
lexer_s *lexer(char *log_str) {
	lexer_s *lexer_p = (lexer_s *)malloc(sizeof(lexer_s));
	lexer_p->strt = log_str;
	lexer_p->curr = log_str;
	return lexer_p;
}

static token_s token(lexer_s *lexer, token_type type) {
	token_s tokn;
	tokn.type = type;
	tokn.lex = lexer->strt;
	tokn.len = (size_t)(lexer->curr - lexer->strt);
	return tokn;
}

/* Advance the lexer 1 position forward and return
   the consumed character.  */
static char advance(lexer_s *lexer) {
	lexer->curr++;
	return lexer->curr[-1];
}

/* Return the next character to consume without
   consuming it yet.  */
static char peek(lexer_s *lexer) {
	return *lexer->curr;
}

static bool is_digit(char c) {
	if (c == '0' ||
		c == '1' ||
	    c == '2' ||
	    c == '3' ||
	    c == '4' ||
	    c == '5' ||
	    c == '6' ||
	    c == '7' ||
	    c == '8' ||
	    c == '9') {
	    return true;
	}
	return false;
}

/* Note that a string can contain the "=" character.
   This is because DIVIDER tokens are expected to not
   be concatenated with other strings.*/
static bool is_string(char c) {
	if (!is_digit(c) &&
		c != ' ' &&
		c != '\n' &&
		c != ':') {
		return true;
		}
	return false;
}

static token_s token_string(lexer_s *lexer) {
	while(is_string(peek(lexer))) {
		advance(lexer);
	}
	return token(lexer, TOKEN_STRING);
}

static token_s token_number_or_string(lexer_s *lexer, char curr_char) {
	if (is_digit(curr_char)) {
		/* A NUMBER token.  */
		while (is_digit(peek(lexer))) {
			advance(lexer);
		}
		return token(lexer, TOKEN_NUMBER);
	} else {
		/* A STRING token.  */
		return token_string(lexer);
	}
}

/* A DIVIDER token is 10 "=" characters. If there's more
   or less than 10, then it's a STRING token.*/
static token_s token_divider_or_string(lexer_s *lexer) {
	int count = 1;

	while (peek(lexer) == '=' && count < 10) {
		count++;
		advance(lexer);
	}

	if (count == 10) {
		/* A DIVIDER token.  */
		return token(lexer, TOKEN_DIVIDER);
	} else {
		/* A STRING token.  */
		return token_string(lexer);
	}
}

/* Advance through the log string, construct a token
   and return it.  */
token_s get_next_token(lexer_s *lexer) {
	if (*lexer->curr == '\0') {
		return token(lexer, TOKEN_EOF);
	}

	/* Set the starting position of the
	   current token lexeme.  */
	lexer->strt = lexer->curr;

	char curr_char = advance(lexer);

	switch (curr_char) {
		case ' ':
			return token(lexer, TOKEN_SPACE);
		case '\n':
			return token(lexer, TOKEN_NEW_LINE);
		case ':':
			return token(lexer, TOKEN_COLON);
		case '=':
			return token_divider_or_string(lexer);
	}

	/* If the current character isn't part of a lexeme
	   for any of the token type above, then it's either
	   a NUMBER or STRING.  */
	return token_number_or_string(lexer, curr_char);
}
