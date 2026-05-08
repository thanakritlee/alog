#ifndef TESTS_DEF_H
#define TESTS_DEF_H

int test_start_activity();

/* STDOUT and STDERR use by the test program to
   write out test results.  */
extern FILE *stdout_test_fp;
extern FILE *stderr_test_fp;

#endif
