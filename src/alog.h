#ifndef ALOG_H
#define ALOG_H

int start_activity(char *actv_name, char *alog_path);
int stop_activity(char *actv_name, char *alog_path, char *log_msg);
int list_activities(char *alog_path, int types);
int get_activity_info(char *actv_name, char *alog_path, int types, int options);

int create_alog_dir(char *alog_path);

/* Activity types
   T_REC: Recorded activities
   T_ACT: Active activities  */
#define T_REC 01
#define T_ACT 02

/* Output options
   O_SLNT: Silent log messages  */
#define O_SLNT 01

typedef enum {
	TOKEN_STRING,
	TOKEN_SPACE,
	TOKEN_NEW_LINE,
	TOKEN_COLON,
	TOKEN_NUMBER,
	TOKEN_DIVIDER,
	TOKEN_EOF
} token_type;

typedef struct {
	char *strt;
	char *curr;
} lexer_s;

typedef struct {
	token_type type;
	char *lex;
	size_t len;
} token_s;

lexer_s *lexer(char *log_str);
token_s get_next_token(lexer_s *lexer);

#endif
