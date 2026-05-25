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

#endif
