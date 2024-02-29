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
#include "../master_dependency/masterConst.h"
#include "../master_dependency/masterVar.h"

int res;
int num_children = 0;

int log_info_file();
void open_file();
int make_watchdog();

int *pipes[] = {
    pipe_for_server_world,
    pipe_for_world_server,
    pipe_for_server_drone,
    pipe_for_drone_server,
    pipe_for_server_obstacle,
    pipe_for_obstacle_server,
    pipe_for_server_target,
    pipe_for_target_server,
    pipe_for_server_drone_target,
    pipe_for_drone_input,
    pipe_for_server_drone_s,
    pipe_for_input_server,
};

const char *name_of_pipes[] = {
    "pipe_for_server_world",
    "pipe_for_world_server",
    "pipe_for_server_drone",
    "pipe_for_drone_server",
    "pipe_for_server_obstacle",
    "pipe_for_obstacle_server",
    "pipe_for_server_target",
    "pipe_for_target_server",
    "pipe_for_server_drone_target",
    "pipe_for_drone_input",
    "pipe_for_server_drone_s",
    "pipe_for_input_server"};


//----------------------------------------------------------------------
int main(int argc, char *argv[])
{

    log_info_file();

    //server_World

    if (pipe(pipe_for_server_world) == -1)
    {
        perror("there is an error in  pipe_for_server_world");
        return -1;
    }
    printf("pipe_for_server_world created successfully\n");

    sprintf(read_for_server_world, "%d", pipe_for_server_world[PIPE_READ]);
    sprintf(write_for_server_world, "%d", pipe_for_server_world[PIPE_WRITE]);

    // world_server

    if (pipe(pipe_for_world_server) == -1)
    {
        perror("there is an error in  pipe_for_world_server");
        return -1;
    }
    printf("pipe_for_world_server created successfully\n");

    sprintf(read_pipe_for_windows_server, "%d", pipe_for_world_server[PIPE_READ]);
    sprintf(write_pipe_for_world_server, "%d", pipe_for_world_server[PIPE_WRITE]);

    // server_drone

    if (pipe(pipe_for_server_drone) == -1)
    {
        perror("there is an error in  pipe_for_server_drone");
        return -1;
    }
    printf("pipe_for_server_drone created successfully\n");

    sprintf(read_pipe_for_server_drone, "%d", pipe_for_server_drone[PIPE_READ]);
    sprintf(write_pipe_for_server_drone, "%d", pipe_for_server_drone[PIPE_WRITE]);

    // drone_server

    if (pipe(pipe_for_drone_server) == -1)
    {
        perror("there is an error in  pipe_for_drone_server");
        return -1;
    }
    printf("pipe_for_drone_server created successfully\n");

    sprintf(read_pipe_for_drone_server, "%d", pipe_for_drone_server[PIPE_READ]);
    sprintf(write_pipe_for_drone_server, "%d", pipe_for_drone_server[PIPE_WRITE]);

    // server_obstacle

    if (pipe(pipe_for_server_obstacle) == -1)
    {
        perror("there is an error in  pipe_for_server_obstacle");
        return -1;
    }
    printf("pipe_for_server_obstacle created successfully\n");

    sprintf(read_pipe_for_server_obstacle, "%d", pipe_for_server_obstacle[PIPE_READ]);
    sprintf(write_pipe_for_server_obstacle, "%d", pipe_for_server_obstacle[PIPE_WRITE]);

    // obstacle_server

    if (pipe(pipe_for_obstacle_server) == -1)
    {
        perror("there is an error in  pipe_for_obstacle_server");
        return -1;
    }
    printf("pipe_for_obstacle_server created successfully\n");

    sprintf(read_pipe_for_obstacle_server, "%d", pipe_for_obstacle_server[PIPE_READ]);
    sprintf(write_pipe_for_obstacle_server, "%d", pipe_for_obstacle_server[PIPE_WRITE]);

    //server_target

    if (pipe(pipe_for_server_target) == -1)
    {
        perror("there is an error in  pipe_for_server_target");
        return -1;
    }
    printf("pipe_for_server_target created successfully\n");

    sprintf(read_pipe_for_server_target, "%d", pipe_for_server_target[PIPE_READ]);
    sprintf(write_pipe_for_server_target, "%d", pipe_for_server_target[PIPE_WRITE]);

    // target_server

    if (pipe(pipe_for_target_server) == -1)
    {
        perror("there is an error in  pipe_for_target_server");
        return -1;
    }
    printf("pipe_for_target_server created successfully\n");

    sprintf(read_pipe_for_target_server, "%d", pipe_for_target_server[PIPE_READ]);
    sprintf(write_pipe_for_target_server, "%d", pipe_for_target_server[PIPE_WRITE]);

    // drone_input

    if (pipe(pipe_for_drone_input) == -1)
    {
        perror("there is an error in  pipe_for_drone_input");
        return -1;
    }
    printf("pipe_for_drone_input created successfully\n");

    sprintf(read_pipe_for_drone_input, "%d", pipe_for_drone_input[PIPE_READ]);
    sprintf(write_pipe_for_drone_input, "%d", pipe_for_drone_input[PIPE_WRITE]);

    // server_drone_trarget

    if (pipe(pipe_for_server_drone_target) == -1)
    {
        perror("there is an error in  pipe_for_server_drone_target");
        return -1;
    }
    printf("pipe_for_server_drone_target created successfully\n");

    sprintf(read_pipe_for_server_drone_target, "%d", pipe_for_server_drone_target[PIPE_READ]);
    sprintf(write_pipe_for_server_drone_target, "%d", pipe_for_server_drone_target[PIPE_WRITE]);

    //  server_drone_screen

    if (pipe(pipe_for_server_drone_s) == -1)
    {
        perror("there is an error in  pipe_for_server_drone_s");
        return -1;
    }
    printf("pipe_for_server_drone_s created successfully\n");

    sprintf(read_pipe_for_server_drone_screen, "%d", pipe_for_server_drone_s[PIPE_READ]);
    sprintf(write_pipe_for_server_drone_screen, "%d", pipe_for_server_drone_s[PIPE_WRITE]);

    // input_server

    if (pipe(pipe_for_input_server) == -1)
    {
        perror("there is an error in  pipe_for_input_server");
        return -1;
    }
    printf("pipe_for_input_server created successfully\n");

    sprintf(read_pipe_for_input_server, "%d", pipe_for_input_server[PIPE_READ]);
    sprintf(write_pipe_for_input_server, "%d", pipe_for_input_server[PIPE_WRITE]);
    open_file();

    make_watchdog();
    // Make child processes
    process_zero = fork();
    if (process_zero < 0)
    {
        perror("there is an error in fork");
        return -1;
    }
    printf("Server process created successfully with PID %d\n", process_zero);

    if (process_zero == 0)
    {
        char *arg_list[] = {"./server", "0", read_pipe_for_server_drone, write_pipe_for_server_drone, read_pipe_for_drone_server, write_pipe_for_drone_server, read_pipe_for_server_obstacle, write_pipe_for_server_obstacle, read_pipe_for_obstacle_server, write_pipe_for_obstacle_server, read_pipe_for_server_target, write_pipe_for_server_target, read_pipe_for_target_server, write_pipe_for_target_server, read_for_server_world, write_for_server_world, read_pipe_for_windows_server, write_pipe_for_world_server, read_pipe_for_server_drone_target, write_pipe_for_server_drone_target, read_pipe_for_input_server, write_pipe_for_drone_input, NULL};
        execvp(arg_list[0], arg_list);
        perror("there is an error in  execvp server");
        return 0;
    }
    num_children += 1;

    process_one = fork();
    if (process_one < 0)
    {
        perror("Fork");
        return -1;
    }
    printf("Drone process created successfully with PID %d\n", process_one);

    if (process_one == 0)
    {
        char *arg_list[] = {"./drone", "1", read_pipe_for_drone_input, write_pipe_for_drone_input, read_pipe_for_server_drone, write_pipe_for_server_drone, read_pipe_for_drone_server, write_pipe_for_drone_server, read_pipe_for_server_drone_target, write_pipe_for_server_drone_target, read_pipe_for_server_drone_screen, write_pipe_for_server_drone_screen, NULL};
        execvp(arg_list[0], arg_list);
        perror("there is an error in  execvp drone");
        return 0;
    }
    num_children += 1;

    process_two = fork();
    if (process_two < 0)
    {
        perror("Fork");
        return -1;
    }
    printf("Input process created successfully with PID %d\n", process_two);

    if (process_two == 0)
    {
        char *arg_list[] = {"konsole", "-e", "./input", "2", read_pipe_for_input_server, write_pipe_for_input_server, NULL};
        execvp("konsole", arg_list);
        perror("there is an error in  execvp input");
        return 0;
    }
    num_children += 1;

    process_three = fork();
    if (process_three < 0)
    {
        perror("Fork");
        return -1;
    }
    printf("World process created successfully with PID %d\n", process_three);

    if (process_three == 0)
    {
        char *arg_list[] = {"konsole", "-e", "./window", "3", read_for_server_world, write_for_server_world, read_pipe_for_windows_server, write_pipe_for_world_server, read_pipe_for_server_drone_screen, write_pipe_for_server_drone_screen, NULL};
        execvp("konsole", arg_list);
        perror("there is an error in  execvp world");
        return 0;
    }
    num_children += 1;

    process_four = fork();
    if (process_four < 0)
    {
        perror("Fork");
        return -1;
    }
    printf("Obstacles process created successfully with PID %d\n", process_four);

    if (process_four == 0)
    {
        char *arg_list[] = {"./obstacles", "4", read_pipe_for_server_obstacle, write_pipe_for_server_obstacle, read_pipe_for_obstacle_server, write_pipe_for_obstacle_server, NULL};
        execvp(arg_list[0], arg_list);
        perror("there is an error in  execvp obstacles");
        return 0;
    }
    num_children += 1;

    process_five = fork();
    if (process_five < 0)
    {
        perror("Fork");
        return -1;
    }
    printf("Targets process created successfully with PID %d\n", process_five);

    if (process_five == 0)
    {
        char *arg_list[] = {"./targets", "5", read_pipe_for_server_target, write_pipe_for_server_target, read_pipe_for_target_server, write_pipe_for_target_server, NULL};
        execvp(arg_list[0], arg_list);
        perror("there is an error in  execvp targets");
        return 0;
    }
    num_children += 1;

    // Wait for all child processes to terminate
    int status;
    int latest_status = 0;
    for (int i = 0; i < num_children; i++)
    {
        pid_t pid = wait(&status); // Wait for a child process to terminate
        if (WIFEXITED(status))     // If the child process terminated normally
        {
            printf("Child process %d exited with status %d\n", pid, WEXITSTATUS(status)); // Print the PID and exit status of the terminated child process
            if (WEXITSTATUS(status) != 0)                                                 // If the exit status is not 0 (indicating an error)
            {
                latest_status = WEXITSTATUS(status); // Store the non-zero exit status
            }
        }
        else // If the child process did not terminate normally
        {
            printf("Child process %d did not exit successfully\n", pid); // Print an error message
        }
    }
    printf("childs process are terminated d\n"); // Print a message indicating that all child processes have terminated

   // size_t size = sizeof(pipes);
   // printf("SIZE OF PIPES %d =======>", size);
    for (int i = 0; i < 12; ++i)

    {

        close(pipes[i][PIPE_READ]);
        close(pipes[i][PIPE_WRITE]);
    }


    for (int i = 0; i < 12; ++i)

    {

        printf("the (%s) ====> %d\n", name_of_pipes[i], pipes[i][PIPE_READ]);
        printf("the (%s) ====>%d\n", name_of_pipes[i], pipes[i][PIPE_WRITE]);
    }


    printf("all of the pipes are closed\n"); // Print a message indicating that the pipe has been closed successfully

    return latest_status;
}

int log_info_file()
{
    // Create the log file name using sprintf
    sprintf(name_of_log_file, WATCHDOG_FILE_NAME);

    // Remove any existing log file with the same name
    remove(name_of_log_file);

    // Open the log file for writing
    FILE *file_for_log = fopen(name_of_log_file, "w");
    if (file_for_log == NULL)
    {
        perror("there is an error in opening file"); // Print error message if file opening fails
        return -1;                                   // Return -1 to indicate failure
    }
    // Note: The function should return an integer, but there's no return statement for success.
    // You might want to add a return statement here for the success case.
}

void open_file()
{
    fopen(NAME_FOR_WATCHDOG_PEOCESS_ID_FILE, "w"); // Open a file with a specific name for writing (mode "w")

    char *fnames[NUMBER_OF_PROCESS] = NAME_FOR_PROCESS_ID_FILE; // Define an array of file names

    // Iterate over the file names array
    for (int i = 0; i < NUMBER_OF_PROCESS; i++)
    {
        fopen(fnames[i], "w"); // Open each file in the array for writing (mode "w")
    }
}

int make_watchdog()
{
    // Create watchdog
    watchdog_process_master = fork();
    if (watchdog_process_master < 0)
    {
        perror("Fork");
        return -1;
    }
    printf("Watchdog process created successfully with PID %d\n", watchdog_process_master);

    if (watchdog_process_master == 0)
    {
        char *arg_list[] = {"./watchdog", NULL};
        // spawn(arg_list);
        execvp(arg_list[0], arg_list);
        perror("there is an error in  execvp watchdog");
        return 0;
    }
    num_children += 1;
}