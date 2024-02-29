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
#include "../target_dependency/targetVar.h"
#include "../target_dependency/targetConst.h"
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
    set_signals(); // Set up signal handling for the program

    switch (argc)
    {
    case 6:
        scanf_of_arguments(argv); // If there are 6 arguments, read them
        break;
    default:
        printf("there is an error in target arguments\n"); // Print an error message if the argument count is not as expected
        exit(EXIT_FAILURE);                                // Exit the program with a failure status
        break;
    }

    // Log the process ID into a file
    log_process_into_file();
    // Check and print the status of the buffer
    status_of_buffer();
    // Log the process ID related to the watchdog into a file
    log_whatchdog_process_into_file();

    // Read the sleep time for the process
    int time_of_sleep[NUMBER_OF_PROCESS] = PROCESS_SLEEPS_US;
    int time_of_sleep2 = time_of_sleep[number_of_process];
    // Array of names for each process
    char *name_of_processes[NUMBER_OF_PROCESS] = NAME_OF_PROCESS_LIST;
    // Get the name of the current process for logging
    name_of_process = name_of_processes[number_of_process];
    // Assign the name of the current process for logging purposes

    FILE *readFile = fopen("variable_files.txt", "r"); // Open the file "variable_files.txt" for reading

    // get requireed parameters from file
    get_parameters_from_file(readFile);

   // Declare a structure variable 'world' of type 'World' to hold game world information
struct World world;

// Initialize variables for managing targets
int number_of_targets_now = NUM_TARGETS;
struct Target current_targets[number_of_targets_now];

// Initialize variables for time management and border tracking
time_t last_spawn_time = 0;
int border_of_top,start ,precision_of_border = 0;

// Initialize targets' status
for (int i = 0; i < number_of_targets_now; i++) {
    // Make the start target active and visible, others inactive but visible
    if (i == 0) {
        current_targets[i].is_active = true;
        current_targets[i].is_visible = true;
    } else {
        current_targets[i].is_active = false;
        current_targets[i].is_visible = true;
    }
}

// Continuously run the game loop
while (1) {
    // Read data from the pipe connected to the server for targets
    if (read(pipr_for_server_target[PIPE_READ], &world, sizeof(world)) == -1) {
        perror("read");
        continue; // Continue to the next iteration if there's an error in reading
    } else {
        // Calculate the total number of borders
        border_of_top = 2 * (world.screen.height - 2) + 2 * (world.screen.width - 2);

        // Check if the total number of borders has changed
        if (border_of_top != precision_of_border) {
            start = 2; // Reset the 'start' variable to indicate changes in borders
        }
        precision_of_border = border_of_top; // Update the border precision

        // Get the current time
        time_t current_time = time(NULL);

        // Iterate through each target
        for (int i = 0; i < number_of_targets_now; i++) {
            // If the target is visible and it's time to refresh or it's the start iteration
            if (current_targets[i].is_visible && (current_time - last_spawn_time >= refresh_time_targets || start > 0)) {
                // Generate random position for the target, ensuring it's not at the drone's position
                do {
                    current_targets[i].x = rand() % (world.screen.width - 4) + 2;
                    current_targets[i].y = rand() % (world.screen.height - 4) + 2;
                } while (current_targets[i].x == world.drone.x && current_targets[i].y == world.drone.y);

                // Assign a symbol to the target based on its index
                current_targets[i].symbol = '0' + i;

                // Copy the current target to the world's target array
                world.target[i] = current_targets[i];
            }

            // Print drone and target positions for debugging
            printf("drone.x: %d, drone.y: %d\n", world.drone.x, world.drone.y);
            if (current_targets[i].is_active) {
                printf("target.x: %d, target.y: %d\n", world.target[i].x, world.target[i].y);
            }

            // If the drone collides with an active target
            if ((int)world.drone.x == (int)world.target[i].x && (int)world.drone.y == (int)world.target[i].y && current_targets[i].is_active) {
                // Deactivate and make the current target invisible
                current_targets[i].is_active = false;
                current_targets[i].is_visible = false;

                // If there's a next target, make it active
                if (i + 1 < number_of_targets_now) {
                    current_targets[i + 1].is_active = true;
                }

                // Update the world's target array
                world.target[i] = current_targets[i];
            }
        }

        // Update the last spawn time if it's time to refresh targets or it's the start iteration
        if (current_time - last_spawn_time >= refresh_time_targets || start > 0) {
            last_spawn_time = current_time;
        }

        // Decrement the 'start' variable
        start--;

        // Write the updated target data to the pipe connected to the target server
        write(pipe_for_target_server[PIPE_WRITE], &world.target, sizeof(world.target));
        fsync(pipe_for_target_server[PIPE_WRITE]); // Synchronize data to ensure it's written
    }
}

// Return 0 upon successful execution
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
void scanf_of_arguments(char *argv[]) {
    // Extract and store the number of processes from the first argument
    sscanf(argv[1], "%d", &number_of_process);

    // Extract and store the file descriptors for reading and writing from the server to target process
    sscanf(argv[2], "%d", &pipr_for_server_target[PIPE_READ]); // Read file descriptor from argv[2]
    sscanf(argv[3], "%d", &pipr_for_server_target[PIPE_WRITE]); // Read file descriptor from argv[3]

    // Extract and store the file descriptors for reading and writing from the target process to server
    sscanf(argv[4], "%d", &pipe_for_target_server[PIPE_READ]); // Read file descriptor from argv[4]
    sscanf(argv[5], "%d", &pipe_for_target_server[PIPE_WRITE]); // Read file descriptor from argv[5]
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
// Function to log the process ID into a file
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

int get_parameters_from_file(FILE *read_file)
{
    // Loop through each line in the file
    while (fgets(line, sizeof(line), read_file))
    {
        // Check if the line matches the format "NUM_TARGETS = %d"
        if (sscanf(line, "NUM_TARGETS = %d", &NUM_TARGETS) == 1)
        {
            // If a line matching the format "NUM_TARGETS = %d" is found, continue to the next line
            continue;
        }
        // Check if the line matches the format "refresh_time_targets = %d"
        else if (sscanf(line, "refresh_time_targets = %d", &refresh_time_targets) == 1)
        {
            // If a line matching the format "refresh_time_targets = %d" is found, exit the loop
            break;
        }
    }

    fclose(read_file); // Close the file after reading

    printf("NUM_TARGETS = %d\n", NUM_TARGETS);                   // Print the value of NUM_TARGETS
    printf("refresh_time_targets = %d\n", refresh_time_targets); // Print the value of refresh_time_targets
}
