#ifndef TESTS_DEF_H
#define TESTS_DEF_H

int test_start_activity();
int test_stop_activity();
int test_create_alog_dir();

/* STDOUT and STDERR use by the test program to
   write out test results.  */
extern FILE *stdout_test_fp;
extern FILE *stderr_test_fp;

int reset_file(int fd);
int read_err(char *buf, size_t len);
int read_out(char *buf, size_t len);
int create_alog_test_dir();
int remove_alog_test_dir();

#endif
