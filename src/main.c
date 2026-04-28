#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#include "buffer.h"

#define DEFAULT_ACTIVITY "log"

char *homePath;
char tmpFilePathName[PATH_MAX];
time_t startTime;
char *actName;

/**
 * Get the activity name from command-line argument
 * , if given.
 * If not given, then use the default activity name.
 */
static char *get_activity_name(int argc, char *argv[]) {
    /* Default activity name.  */
    char *actName = DEFAULT_ACTIVITY;
    if (argc > 1) {
        /* If an argument is given, the first argument
           is the activity name.  */
        actName = argv[1];
    }
    return actName;
}

/**
 * Create $HOME/.alog and $HOME/.alog/.tmp directories
 * if it doesn't already exist.
 */
static void create_alog_dir() {
    struct stat *statBuf;

    /* Make a copy of the home path string.  */
    char dirPath[PATH_MAX];
    strcpy(dirPath, homePath);

    /* Attempt to create $HOME/.alog directory.  */
    strcat(dirPath, "/.alog");
    if (stat(dirPath, statBuf) < 0 && errno == ENOENT) {
        mkdir(dirPath, 0700);
    }

    /* Attempt to create $HOME/.alog/.tmp directory.  */
    strcat(dirPath, "/.tmp");
    if (stat(dirPath, statBuf) < 0 && errno == ENOENT) {
        mkdir(dirPath, 0700);
    }
}

/**
 * Create a tempory file use for storing "in-progress" activity log
 * information.
 * The purpose of the file is to be use by the program after the
 * process was terminated without saving the activity log.
 */
static int create_tmp_file() {
    strcpy(tmpFilePathName, homePath);

    strcat(tmpFilePathName, "/.alog/.tmp/tmp.XXXXXX");
    int tmpFileFD = mkstemp(tmpFilePathName);
    if (tmpFileFD < 0) {
        fprintf(stderr, "[ERROR] %s", strerror(errno));
        exit(errno);
    }
    return tmpFileFD;
}

static void log_activity_start() {
    /* Start timer.  */
    startTime = time(NULL);

    int tmpFileFD = create_tmp_file();
    Buffer *tmpBuf = init_buffer();
    write_to_buffer(tmpBuf, actName, strlen(actName));
    write_to_buffer(tmpBuf, "\n", 1);

    /* 10 bytes of data because time_t is a long int data type
       which means it's stored as a 32 bits data.
       2**32 = 4294967296, 10 digits (positions), therefore 10 bytes.  */
    char startTimeEpocStr[10];
    sprintf(startTimeEpocStr, "%ld", startTime);
    write_to_buffer(tmpBuf, startTimeEpocStr, 10);
    write(tmpFileFD, tmpBuf->mem, tmpBuf->length);
    close(tmpFileFD);
    destroy_buffer(tmpBuf);
}

static void main_loop() {
    fprintf(stdout, "Timer started.\n");
    char key;

    while (1) {
        do {
            fprintf(stdout, "Press [SPACEBAR] to pause the timer...\n");
            fscanf(stdin, "%s", &key);
        } while (key != 'p');

        fprintf(stdout, "PAUSED\n");
        fprintf(stdout, "Stop and log activity? (y/n):    ");
        fscanf(stdin, "%s", &key);
        if (key == 'y') {
            break;
        }
        fprintf(stdout, "UNPAUSED\n");
        key = '\0';
    }
}

static void delete_tmp_file() {
    unlink(tmpFilePathName);
}

static void log_activity_end() {
    char logFilePath[PATH_MAX];
	strcpy(logFilePath, homePath);
	strcat(logFilePath, "/.alog/");
	strcat(logFilePath, actName);

    int logFD = open(logFilePath, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);

    char *startTimeStr1 = ctime(&startTime);
    write(logFD, startTimeStr1, strlen(startTimeStr1));
    time_t endTime = time(NULL);
    double diff = difftime(endTime, startTime);
    char diffTimeStr[50];
    sprintf(diffTimeStr, "Elapsed Time (s): %.0lf\n", diff);
    write(logFD, diffTimeStr, strlen(diffTimeStr));

    char cmdBuf[PATH_MAX + 9];
    sprintf(cmdBuf, "nano +-1 %s", logFilePath);
	system(cmdBuf);

	write(logFD, "==========\n", 11);
	close(logFD);

    delete_tmp_file();
}

int main(int argc, char *argv[]) {
    actName = get_activity_name(argc, argv);
    homePath = getenv("HOME");

    create_alog_dir();

    log_activity_start();

    main_loop();

    log_activity_end();

    exit(0);
}
