
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
// Initialize position and velocity
double x, y;
double vx = 0, vy = 0;

// Initialize forces
double fx = 0, fy = 0;

pid_t processId_of_watchdog;
pid_t process_id;
char *name_of_process;
struct timeval prev_t;
char name_of_log_file[256] = WATCHDOG_FILE_NAME;
char command;
int number_of_process;

struct stat buffer_of_stat;


int pipe_for_drone_input[2];
int pipe_for_server_drone[2];
int pipe_for_drone_server[2];
int pipe_for_drone_server_target[2];
int pipe_for_drone_server_screen[2];

float M, K;



