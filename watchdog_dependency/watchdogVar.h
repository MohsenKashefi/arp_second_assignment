#include <ncurses.h>
#include <string.h>
#include <time.h>
#include "../include/const.h"
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>


pid_t sp_pids[NUMBER_OF_PROCESS];
struct timeval prev_ts[NUMBER_OF_PROCESS];
int process_data_recieved[NUMBER_OF_PROCESS] = {0, 0, 0, 0, 0, 0};
char name_of_log_file[256] = WATCHDOG_FILE_NAME;
int logfile_line = 0;                               // line to read from in the log file
char *name_of_processes[NUMBER_OF_PROCESS] = NAME_OF_PROCESS_LIST;