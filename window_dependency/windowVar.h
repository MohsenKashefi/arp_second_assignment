#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include "../include/const.h"

#include <ncurses.h>
#include <stdbool.h>
#include <sys/ipc.h>

pid_t processId_of_watchdog;
pid_t process_id;
char *name_of_process;
struct timeval prev_t;
char name_of_log_file[256] = WATCHDOG_FILE_NAME;


int number_of_process;

struct stat buffer_of_stat;

int pipe_for_server_world[2];
int pipe_for_world_server[2];
int pipe_for_server_drone_screen[2];


struct Drone
{
    int x;
    int y;
    char symbol;
    short color_pair;
};

struct Screen
{
    int height;
    int width;
};

struct Obstacle
{
    int x;
    int y;
    char symbol;
};
struct Target
{
    int x;
    int y;
    char symbol;
    bool is_active;
    bool is_visible;
};

struct World
{
    struct Drone drone;
    struct Obstacle obstacle[20];
    struct Screen screen;
    struct Target target[9];
};