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
#include "../include/structures.h"
#include "../obstacle_dependency/obstacleVar.h"
#include "../obstacle_dependency/obstacleConst.h"
#include <sys/ipc.h>

void log_into_file(struct timeval tv);
void handle_watchdog_process(int sig, siginfo_t *info, void *context);
int set_signals();
void scanf_of_arguments(char *argv[]);
void log_process_into_file();
void log_whatchdog_process_into_file();
int status_of_buffer();
int get_parameters_from_file(FILE *read_file);
int main(int argc, char *argv[])
{

    set_signals(); // Set up signal handlers

    switch (argc) // Begin a switch-case based on the number of command-line arguments
    {
    case 6:                                                      // If there are 6 arguments
        scanf_of_arguments(argv);                                // Parse the arguments using a custom function
        break;                                                   // Exit the switch statement
    default:                                                     // If there's a different number of arguments
        printf("there is an error in arguments in obstacles\n"); // Print an error message
        exit(EXIT_FAILURE);                                      // Terminate the program with failure status
        break;                                                   // This line is redundant as the control has already left the switch statement
    }

    printf("number of process is ===========>,%d \n", number_of_process); // Print the number of processes

    log_process_into_file(); // Log process information into a file

    // Read watchdog pid

    // Log the watchdog process information into a file
    log_whatchdog_process_into_file();

    status_of_buffer(); // Check the status of a buffer

    // Read how long to sleep process for
    int time_of_sleep[NUMBER_OF_PROCESS] = PROCESS_SLEEPS_US;
    int time_of_sleep2 = time_of_sleep[number_of_process];
    char *name_of_processes[NUMBER_OF_PROCESS] = NAME_OF_PROCESS_LIST;
    name_of_process = name_of_processes[number_of_process]; // added to logfile for readability

    // leggere da file variable_files numero di ostacoli
    FILE *readFile = fopen("variable_files.txt", "r");

    // struct Obstacle obstacles[NUMBER_OF_OBSTACLES]; // Assume obstacles are initialized
    // struct Screen screen;

    get_parameters_from_file(readFile);
    // Create a new window

    int borders_of_top;                                            // Variable to store the total number of borders
    time_t previous_spawn_time = 0;                                // Variable to store the time of the previous obstacle spawn
    int start, precision_of_border = 0, number_of_targets_now = 9; // Variables for controlling obstacle generation

    while (1)
    {
        // Read data from the pipe connected to the server for obstacles
        if (read(pipe_for_server_obstacle[PIPE_READ], &world, sizeof(world)) == -1)
        {
            perror("read");
            continue; // Continue to the next iteration if there's an error in reading
        }
        else
        {
            // Calculate the total number of borders
            borders_of_top = 2 * (world.screen.height - 2) + 2 * (world.screen.width - 2);

            // Check if the total number of borders has changed
            borders_of_top != precision_of_border ? start = 2 : printf("not same");
            // If the number of borders has changed, reset the start variable to 2
            precision_of_border = borders_of_top;

            int counter = 0;

            // Generate random obstacles
            time_t time_of_now = time(NULL);
            if (time_of_now - previous_spawn_time >= time_for_refreshing_obstacles || start > 0)
            {
                // for (; i < NUMBER_OF_OBSTACLES; i++)
                while (counter < NUMBER_OF_OBSTACLES)
                {
                    int position_equality;
                    do
                    {
                        position_equality = 0;
                        // Generate random coordinates for the obstacle within the screen bounds
                        world.obstacle[counter].x = rand() % (world.screen.width - 4) + 2;
                        world.obstacle[counter].y = rand() % (world.screen.height - 4) + 2;

                        // Check if the obstacle is in the same position as the drone
                        if (world.obstacle[counter].x == world.drone.x && world.obstacle[counter].y == world.drone.y)
                        {
                            position_equality = 1;
                        }

                        // Check if the obstacle is in the same position as any of the targets
                        for (int j = 0; j < number_of_targets_now; j++)
                        {
                            if (world.obstacle[counter].x == world.target[j].x && world.obstacle[counter].y == world.target[j].y)
                            {
                                position_equality = 1;
                                break;
                            }
                        }
                    } while (position_equality);

                    world.obstacle[counter].symbol = '#'; // Assign a symbol to the obstacle
                    counter++;
                }
                previous_spawn_time = time_of_now; // Update the previous spawn time
            }
            start--; // Decrement the start variable
            //----------------------------------------
            printf("borders_of_top ==========> %d\n", borders_of_top); // Print the total number of borders
            printf("counter: %d\n", counter);                          // Print the value of counter
            // Write obstacles to the pipe connected to the server
            write(pipe_for_obstacle_server[PIPE_WRITE], &world.obstacle, sizeof(world.obstacle));
            fsync(pipe_for_obstacle_server[PIPE_WRITE]); // Synchronize data to ensure it's written
            //-----------------------------------------
        }
    }

    // Pass obstacles to the drone via pipe
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
    // Extract an integer value from the second command-line argument (argv[1])
    // and store it in the variable number_of_process.
    sscanf(argv[1], "%d", &number_of_process);

    // Extract an integer value from the third command-line argument (argv[2])
    // and store it in the corresponding index of the pipe_for_server_obstacle array
    // for reading operations.
    sscanf(argv[2], "%d", &pipe_for_server_obstacle[PIPE_READ]);

    // Extract an integer value from the fourth command-line argument (argv[3])
    // and store it in the corresponding index of the pipe_for_server_obstacle array
    // for writing operations.
    sscanf(argv[3], "%d", &pipe_for_server_obstacle[PIPE_WRITE]);

    // Extract an integer value from the fifth command-line argument (argv[4])
    // and store it in the corresponding index of the pipe_for_obstacle_server array
    // for reading operations.
    sscanf(argv[4], "%d", &pipe_for_obstacle_server[PIPE_READ]);

    // Extract an integer value from the sixth command-line argument (argv[5])
    // and store it in the corresponding index of the pipe_for_obstacle_server array
    // for writing operations.
    sscanf(argv[5], "%d", &pipe_for_obstacle_server[PIPE_WRITE]);
}

void log_process_into_file()
{
    process_id = getpid(); // Get the process ID

    // Array of file names for storing process IDs
    char *names_of_file[NUMBER_OF_PROCESS] = NAME_FOR_PROCESS_ID_FILE;

    // Open the file corresponding to the current process
    FILE *pid_fp = fopen(names_of_file[number_of_process], "w");
    if (pid_fp == NULL)
    {
        perror("fopen"); // Print an error message if opening the file fails
        return;
    }

    // Write the process ID to the file
    fprintf(pid_fp, "%d", process_id);

    // Close the file
    fclose(pid_fp);

    // Print a message indicating the published process ID
    printf("process id is ==========>: %d\n", process_id);
}
// Function to log the watchdog process ID into a file
void log_whatchdog_process_into_file()
{
    FILE *file_pointer_of_watchdog = NULL; // Declare a file pointer for the watchdog process file

    file_pointer_of_watchdog = fopen(NAME_FOR_WATCHDOG_PEOCESS_ID_FILE, "r"); // Open the file containing the watchdog process ID in read mode

    // Read the watchdog process ID from the file
    fscanf(file_pointer_of_watchdog, "%d", &processId_of_watchdog);

    // Print the watchdog process ID
    printf("process id of watchdog is =========>: %d\n", processId_of_watchdog);

    fclose(file_pointer_of_watchdog); // Close the file
}

// Function to check the status of a buffer
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

int get_parameters_from_file(FILE *read_file)
{
    if (read_file == NULL)
    {
        printf("there is an error in opening file\n");
        return -1;
    }
    while (fgets(line, sizeof(line), read_file))
    {
        if (sscanf(line, "NUMBER_OF_OBSTACLES = %d", &NUMBER_OF_OBSTACLES) == 1)
        {
            // Se abbiamo trovato una riga che corrisponde al formato "NUMBER_OF_OBSTACLES = %d",
            // interrompiamo il ciclo
            continue;
        }
        else if (sscanf(line, "time_for_refreshing_obstacles = %d", &time_for_refreshing_obstacles) == 1)
        {
            break;
        }
    }

    fclose(read_file);

    printf("NUMBER_OF_OBSTACLES = %d\n", NUMBER_OF_OBSTACLES);

    //-------------------------------------------------------------------------
    printf("time_for_refreshing_obstacles = %d\n", time_for_refreshing_obstacles);
}