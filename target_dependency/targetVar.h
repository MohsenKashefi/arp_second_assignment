#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <errno.h>
#include "../include/const.h"

int pipr_for_server_target[2];
int pipe_for_target_server[2];

pid_t processId_of_watchdog;
pid_t process_id;
char *name_of_process;
struct timeval prev_t;
char name_of_log_file[256] = WATCHDOG_FILE_NAME;

int number_of_process;
struct stat buffer_of_stat;

char line[256];
int NUM_TARGETS = 0;
int refresh_time_targets = 0;