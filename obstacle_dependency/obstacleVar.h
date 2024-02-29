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

int pipe_for_server_obstacle[2]; // Array to hold file descriptors for communication from server to obstacle process.
int pipe_for_obstacle_server[2]; // Array to hold file descriptors for communication from obstacle to server process.

pid_t processId_of_watchdog;                     // Process ID of the watchdog process.
pid_t process_id;                                // Process ID of the current process.
char *name_of_process;                           // Name of the current process.
struct timeval prev_t;                           // Timestamp used for time calculations.
char name_of_log_file[256] = WATCHDOG_FILE_NAME; // File name for the log file, initialized to WATCHDOG_FILE_NAME.

int number_of_process; // Total number of processes.

struct stat buffer_of_stat; // Structure to hold file status information.

char line[256];                 // Array to hold input lines.
int NUMBER_OF_OBSTACLES = 0;          // Number of obstacles.
int time_for_refreshing_obstacles = 0; // Refresh time for obstacles.

struct World world; // Structure representing the world.
