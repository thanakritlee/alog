#ifndef TESTS_DEF_H
#define TESTS_DEF_H

int test_start_activity();
int test_stop_activity();
int test_create_alog_dir();
int test_list_activities();
int test_activity_info();
int test_lexer();
int test_cancel_activity();
int test_delete_activity();

/* STDOUT and STDERR use by the test program to
   write out test results.  */
extern FILE *stdout_test_fp;
extern FILE *stderr_test_fp;

/* STDOUT and STDERR use by the main program.  */
extern int main_stdout_fd;
extern int main_stderr_fd;

int read_err(char *buf, size_t len);
int read_out(char *buf, size_t len);
int create_alog_test_dir();
int remove_alog_test_dir();
int assert_int(int actu, int expt, int exit_code, char *err_msg_format, ...);
int assert_str(char *actu, char *expt, size_t len, int exit_code, char *err_msg_format, ...);

#endif
