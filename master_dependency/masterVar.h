
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/wait.h>
#include "../include/const.h"
// pids for all children
pid_t watchdog_process_master;
pid_t process_zero;
pid_t process_one;
pid_t process_two;
pid_t process_three;
pid_t process_four;
pid_t process_five;



char name_of_log_file[256];

int pipe_for_server_world[2];
int pipe_for_world_server[2];
int pipe_for_server_drone[2];
int pipe_for_drone_server[2];
int pipe_for_server_obstacle[2];
int pipe_for_obstacle_server[2];
int pipe_for_server_target[2];
int pipe_for_target_server[2];
int pipe_for_server_drone_target[2];
int pipe_for_drone_input[2];
int pipe_for_server_drone_s[2];
int pipe_for_input_server[2];

char read_for_server_world[10];
char write_for_server_world[10];

char read_pipe_for_windows_server[10];
char write_pipe_for_world_server[10];

char read_pipe_for_server_drone[10];
char write_pipe_for_server_drone[10];

char read_pipe_for_drone_server[10];
char write_pipe_for_drone_server[10];

char read_pipe_for_server_obstacle[10];
char write_pipe_for_server_obstacle[10];

char read_pipe_for_obstacle_server[10];
char write_pipe_for_obstacle_server[10];

char read_pipe_for_server_target[10];
char write_pipe_for_server_target[10];

char read_pipe_for_target_server[10];
char write_pipe_for_target_server[10];

char read_pipe_for_drone_input[10];
char write_pipe_for_drone_input[10];

char read_pipe_for_server_drone_target[10];
char write_pipe_for_server_drone_target[10];

char read_pipe_for_server_drone_screen[10];
char write_pipe_for_server_drone_screen[10];

char read_pipe_for_input_server[10];
char write_pipe_for_input_server[10];