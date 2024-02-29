#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include "../include/const.h"
#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>


// global pipe for file discriptor
int pipe_for_file_descriptor[2];

int gridHeight = 3;
int gridWidth = 3;
int squareHeight = 5;
int squareWidth = 10;
int maxRows, maxCols;


pid_t processId_of_watchdog;
pid_t process_id;
char *name_of_process;
struct timeval prev_t;
char name_of_log_file[256] = WATCHDOG_FILE_NAME;
int number_of_process;


struct stat buffer_of_stat;

pid_t processId_of_drone;