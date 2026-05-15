#ifndef ALOG_H
#define ALOG_H

int start_activity(char *actv_name, char *alog_path);
int stop_activity(char *actv_name, char *alog_path, char *log_msg);

int create_alog_dir(char *alog_path);

#endif
