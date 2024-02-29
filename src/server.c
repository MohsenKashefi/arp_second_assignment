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
#include <sys/ipc.h>
#include <stdbool.h>
#include "../server_dependency/serverVar.h"
#include "../server_dependency/serverConst.h"

void log_into_file(struct timeval tv);
void handle_watchdog_process(int sig, siginfo_t *info, void *context);
int set_signals();
void scanf_of_arguments(char *argv[]);
void print_status_of_pipes();
void log_process_into_file();
void log_whatchdog_process_into_file();
int status_of_buffer();

int *pipes[] = {

    pipe_for_server_drone,
    pipe_for_drone_server,
    pipe_for_server_obstacle,
    pipe_for_obstacle_server,
    pipe_for_server_target,
    pipe_for_target_server,
    pipe_for_server_world,
    pipe_for_world_server,
    pipe_for_server_drone_target,
    pipe_for_input_server,
};

const char *name_of_pipes[] = {
    "pipe_for_server_drone",
    "pipe_for_drone_server",
    "pipe_for_server_obstacle",
    "pipe_for_obstacle_server",
    "pipe_for_server_target",
    "pipe_for_target_server",
    "pipe_for_server_world",
    "pipe_for_world_server",
    "pipe_for_server_drone_target",
    "pipe_for_input_server",
};

int main(int argc, char *argv[])
{

    set_signals(); // Set up signal handling for the program

    switch (argc)
    {
    case 22:
        scanf_of_arguments(argv); // If there are 22 arguments, read them
        break;
    default:
        printf("there is an error in argument server\n"); // Print an error message if the argument count is not as expected
        return -1;                                        // Return with an error code
        break;
    }

    print_status_of_pipes(); // Print the status of pipes
    // Publish the process ID into a log file
    log_process_into_file();
    status_of_buffer(); // Check and print the status of the buffer
    // Read the watchdog process ID and log it into a file
    log_whatchdog_process_into_file();

    // Read the sleep time for the process
    int time_of_sleep[NUMBER_OF_PROCESS] = PROCESS_SLEEPS_US;
    int time_of_sleep2 = time_of_sleep[number_of_process];
    // Array of names for each process
    char *name_of_processes[NUMBER_OF_PROCESS] = NAME_OF_PROCESS_LIST;
    // Get the name of the current process for logging
    name_of_process = name_of_processes[number_of_process];
    // The name of the process is selected from the list based on the current process number, and it's assigned for logging purposes

    struct World world; // Declare a variable of type struct World named 'world'.
    char entered_command; // Declare a variable of type char named 'entered_command'.

    // Infinite loop
    while (1)
    {
        // Read the screen information from the pipe for the world server and store it in world.screen.
        read(pipe_for_world_server[PIPE_READ], &world.screen, sizeof(world.screen));
        
        // Read the drone information from the pipe for the drone server and store it in world.drone.
        read(pipe_for_drone_server[PIPE_READ], &world.drone, sizeof(world.drone));
        
        // Read the obstacle information from the pipe for the obstacle server and store it in world.obstacle.
        read(pipe_for_obstacle_server[PIPE_READ], &world.obstacle, sizeof(world.obstacle));
        
        // Read the target information from the pipe for the target server and store it in world.target.
        read(pipe_for_target_server[PIPE_READ], &world.target, sizeof(world.target));
        
        // Read the entered command from the input server pipe and store it in entered_command.
        read(pipe_for_input_server[PIPE_READ], &entered_command, sizeof(entered_command));
        
        // Write the entered command to the input server pipe.
        write(pipe_for_input_server[PIPE_WRITE], &entered_command, sizeof(entered_command));
        
        // Print the entered command.
        printf("entered_command: %c\n", entered_command);
        
        // Reset entered_command to '0'.
        entered_command = '0';
        
        // Print the height and width of the screen.
        printf("height of screen is ======> %d, width of screen is ===> %d\n", world.screen.height, world.screen.width);

        // Iterate through targets and print their information if they are active.
        for (int i = 0; i < 9; i++)
        {
            if (world.target[i].is_active == true)
            {
                printf("target %d x: %d, y: %d, is_active: %d\n", i, world.target[i].x, world.target[i].y, world.target[i].is_active);
            }
        }

        // Write drone information to the world server pipe and flush the buffer.
        write(pipe_for_server_world[PIPE_WRITE], &world.drone, sizeof(world.drone));
        fsync(pipe_for_server_world[PIPE_WRITE]);

        // Write obstacle information to the world server pipe and flush the buffer.
        write(pipe_for_server_world[PIPE_WRITE], &world.obstacle, sizeof(world.obstacle));
        fsync(pipe_for_server_world[PIPE_WRITE]);

        // Write target information to the world server pipe and flush the buffer.
        write(pipe_for_server_world[PIPE_WRITE], &world.target, sizeof(world.target));
        fsync(pipe_for_server_world[PIPE_WRITE]);

        // Write obstacle information to the drone server pipe and flush the buffer.
        write(pipe_for_server_drone[PIPE_WRITE], &world.obstacle, sizeof(world.obstacle));
        fsync(pipe_for_server_drone[PIPE_WRITE]);

        // Write target information to the drone server pipe and flush the buffer.
        write(pipe_for_server_drone_target[PIPE_WRITE], &world.target, sizeof(world.target));
        fsync(pipe_for_server_drone_target[PIPE_WRITE]);

        // Write world information to the obstacle server pipe and flush the buffer.
        write(pipe_for_server_obstacle[PIPE_WRITE], &world, sizeof(world));
        fsync(pipe_for_server_obstacle[PIPE_WRITE]);

        // Write world information to the target server pipe and flush the buffer.
        write(pipe_for_server_target[PIPE_WRITE], &world, sizeof(world));
        fsync(pipe_for_server_target[PIPE_WRITE]);

        // Print the current coordinates of the drone.
        printf("x======> %d, y=====> %d\n", world.drone.x, world.drone.y);
    }

    

    return 0;
}

// Function to append time information into a log file
void log_into_file(struct timeval timeval_instance)
{
    FILE *file_for_log = fopen(name_of_log_file, "a");                                                    // Open the log file in append mode
    fprintf(file_for_log, "%d %ld %ld\n", process_id, timeval_instance.tv_sec, timeval_instance.tv_usec); // Write process ID, seconds, and microseconds to the log file
    fclose(file_for_log);                                                                                 // Close the log file
}

// Signal handler function for handling watchdog signals
void handle_watchdog_process(int sig, siginfo_t *signal_info, void *context)
{
    // Check if the signal is from the watchdog process
    if (signal_info->si_pid == processId_of_watchdog)
    {
        gettimeofday(&prev_t, NULL); // Get the current time
        log_into_file(prev_t);       // Log the current time into the file
    }
}
// Function to set up signal handling
int set_signals()
{
    // Define a signal set
    sigset_t signal_set;

    // Initialize the signal set to empty
    sigemptyset(&signal_set);

    // Add SIGUSR1 to the set
    sigaddset(&signal_set, SIGUSR1);

    // Block SIGUSR1
    if (sigprocmask(SIG_BLOCK, &signal_set, NULL) < 0)
    {
        perror("blocking of the signal failed"); // Print an error message if the blocking of the signal fails
        return -1;                               // Return -1 to indicate failure
    }

    // Set up sigaction to handle signals from the watchdog process
    struct sigaction signal_action;
    signal_action.sa_flags = SA_SIGINFO;                  // Set the sa_flags field to indicate the use of sa_sigaction
    signal_action.sa_sigaction = handle_watchdog_process; // Set the signal handler function
    if (sigaction(SIGUSR1, &signal_action, NULL) < 0)
    {
        perror("there is an error related to signla action"); // Print an error message if setting up signal handling fails
    }

    // Unblock SIGUSR1
    if (sigprocmask(SIG_UNBLOCK, &signal_set, NULL) < 0)
    {
        perror("unblocking of the signal failed"); // Print an error message if the unblocking of the signal fails
        return -1;                                 // Return -1 to indicate failure
    }

    // Return 0 to indicate success
    return 0;
}

void scanf_of_arguments(char *argv[])
{
    sscanf(argv[1], "%d", &number_of_process);
    // size_t size = sizeof(pipes);

    // Loop over argv starting from index 2
    int counter = 2;
    int counter2 = 3;
    for (int i = 0; i < 10; ++i)

    {

        // Read the file descriptors for the current pipe
        printf("counter =========>%d\n", counter);
        printf("counter2 =========>%d\n", counter2);
        sscanf(argv[counter], "%d", &pipes[i][PIPE_READ]);
        sscanf(argv[counter2], "%d", &pipes[i][PIPE_WRITE]);

        counter += 2;
        counter2 += 2;
    }
    // sscanf(argv[1], "%d", &number_of_process);
    // sscanf(argv[2], "%d", &pipe_for_server_drone[PIPE_READ]);
    // sscanf(argv[3], "%d", &pipe_for_server_drone[PIPE_WRITE]);
    // sscanf(argv[4], "%d", &pipe_for_drone_server[PIPE_READ]);
    // sscanf(argv[5], "%d", &pipe_for_drone_server[PIPE_WRITE]);
    // sscanf(argv[6], "%d", &pipe_for_server_obstacle[PIPE_READ]);
    // sscanf(argv[7], "%d", &pipe_for_server_obstacle[PIPE_WRITE]);
    // sscanf(argv[8], "%d", &pipe_for_obstacle_server[PIPE_READ]);
    // sscanf(argv[9], "%d", &pipe_for_obstacle_server[PIPE_WRITE]);
    // sscanf(argv[10], "%d", &pipe_for_server_target[PIPE_READ]);
    // sscanf(argv[11], "%d", &pipe_for_server_target[PIPE_WRITE]);
    // sscanf(argv[12], "%d", &pipe_for_target_server[PIPE_READ]);
    // sscanf(argv[13], "%d", &pipe_for_target_server[PIPE_WRITE]);
    // sscanf(argv[14], "%d", &pipe_for_server_world[PIPE_READ]);
    // sscanf(argv[15], "%d", &pipe_for_server_world[PIPE_WRITE]);
    // sscanf(argv[16], "%d", &pipe_for_world_server[PIPE_READ]);
    // sscanf(argv[17], "%d", &pipe_for_world_server[PIPE_WRITE]);
    // sscanf(argv[18], "%d", &pipe_for_server_drone_target[PIPE_READ]);
    // sscanf(argv[19], "%d", &pipe_for_server_drone_target[PIPE_WRITE]);
    // sscanf(argv[20], "%d", &pipe_for_input_server[PIPE_READ]);
    // sscanf(argv[21], "%d", &pipe_for_input_server[PIPE_WRITE]);
}

void print_status_of_pipes()
{

    printf("number_of_process ====> %d\n", number_of_process);

    for (int i = 0; i < 10; ++i)

    {

        printf("the (%s) ====> %d\n", name_of_pipes[i], pipes[i][PIPE_READ]);
        printf("the (%s) ====>%d\n", name_of_pipes[i], pipes[i][PIPE_WRITE]);
    }

}

void log_process_into_file()
{

    process_id = getpid();

    char *fnames[NUMBER_OF_PROCESS] = NAME_FOR_PROCESS_ID_FILE;
    // open a file
    FILE *pid_fp = fopen(fnames[number_of_process], "w");
    fprintf(pid_fp, "%d", process_id);
    fclose(pid_fp);

    printf("Published pid %d \n", process_id);
}

void log_whatchdog_process_into_file()
{
    FILE *watchdog_fp = NULL;

    watchdog_fp = fopen(NAME_FOR_WATCHDOG_PEOCESS_ID_FILE, "r");

    fscanf(watchdog_fp, "%d", &processId_of_watchdog);
    printf("watchdog pid %d \n", processId_of_watchdog);
    fclose(watchdog_fp);
}

int status_of_buffer()
{
    /* Call stat to retrieve information about the file, fill stat buffer, and validate success */
    if (stat(NAME_FOR_WATCHDOG_PEOCESS_ID_FILE, &buffer_of_stat) == -1)
    {
        perror("there is an error in stat"); // Print an error message if stat fails
        return -1;                           // Return -1 to indicate failure
    }
    // Wait until the file has data
    while (buffer_of_stat.st_size <= 0)
    {
        // Call stat again to check if the file has data
        if (stat(NAME_FOR_WATCHDOG_PEOCESS_ID_FILE, &buffer_of_stat) == -1)
        {
            perror("there is an error in stat"); // Print an error message if stat fails
            return -1;                           // Return -1 to indicate failure
        }
        usleep(50000); // Sleep for 50 milliseconds
    }
}
