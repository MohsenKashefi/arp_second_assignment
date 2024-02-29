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
#include "../window_dependency/windowVar.h"
#include "../window_dependency/windowConst.h"
#include <ncurses.h>
#include <stdbool.h>
#include <sys/ipc.h>

int *pipes[] = {
    pipe_for_server_world,
    pipe_for_world_server,
    pipe_for_server_drone_screen,
};

void log_into_file(struct timeval tv);
void handle_watchdog_process(int sig, siginfo_t *info, void *context);
int set_signals();
void print_status_of_pipes();
void log_process_into_file();
void log_whatchdog_process_into_file();
int status_of_buffer();
void scanf_of_arguments(char *argv[]);
int main(int argc, char *argv[])
{
    // Set signal handlers for the program
    set_signals();

    // Check the number of command line arguments
    switch (argc)
    {
    // If there are 8 arguments, proceed
    case 8:
        // Parse the command line arguments
        scanf_of_arguments(argv);
        break;

    // If the number of arguments is not 8, display an error message and exit
    default:
        printf("there is an error in arguments of world\n");
        return -1;
        break;
    }

    // Print the status of pipes
    print_status_of_pipes();

    // Log the process into a file
    log_process_into_file();

    // Check the status of buffer
    status_of_buffer();

    // Log the watchdog process into a file
    log_whatchdog_process_into_file();

    // Initialize sleep times for processes
    int time_of_sleep[NUMBER_OF_PROCESS] = PROCESS_SLEEPS_US;
    int time_of_sleep2 = time_of_sleep[number_of_process];

    // Initialize names of processes for logging
    char *name_of_processes[NUMBER_OF_PROCESS] = NAME_OF_PROCESS_LIST;
    name_of_process = name_of_processes[number_of_process]; // added to logfile for readability

    // Initialize ncurses window
    WINDOW *win;
    win = initscr();

    // Start color mode
    start_color();

    // Hide cursor
    curs_set(0);

    // Set timeout for input functions
    timeout(100);

    // Seed random number generator
    srand(time(NULL));

    // Initialize color pairs
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);

    // Define a struct for the world
    struct World world;

    // Variables to store window dimensions
    int height, width;
    while (1)
    {
        clear();

        // Read data from the pipe
        if (read(pipe_for_server_world[PIPE_READ], &world.drone, sizeof(world.drone)) == -1)
        {
            printf("error in reading drone!!!!\n");
            printf("loading\n");
            continue;
        }
        if (read(pipe_for_server_world[PIPE_READ], &world.obstacle, sizeof(world.obstacle)) == -1)
        {
            printf("error in reading obstacle!!!!\n");
            printf("loading\n");
            continue;
        }
        if (read(pipe_for_server_world[PIPE_READ], &world.target, sizeof(world.target)) == -1)
        {
            printf("error in reading targets!!!!\n");
            printf("loading\n");
            continue;
        }

        getmaxyx(win, height, width);
        world.screen.height = height;
        world.screen.width = width;

        // Draw border around the window

        clear();
        box(win, 0, 0);
        // Print the target symbols at their positions if they're within the window dimensions
        for (int i = 0; i < 9; i++)
        {
            if ((world.target[i].y < height && world.target[i].x < width) && world.target[i].is_visible)
            {
                attron(COLOR_PAIR(4));
                mvprintw(world.target[i].y, world.target[i].x, "%c", world.target[i].symbol);
                attroff(COLOR_PAIR(4));
            }
        }

        for (int i = 0; i < 20; i++)
        {
            if (world.obstacle[i].y < height && world.obstacle[i].x < width)
            {
                attron(COLOR_PAIR(3));
                mvprintw(world.obstacle[i].y, world.obstacle[i].x, "%c", world.obstacle[i].symbol);
                attroff(COLOR_PAIR(3));
            }
        }

        if (world.drone.y < height && world.drone.x < width)
        {
            attron(COLOR_PAIR(world.drone.color_pair));
            mvprintw(world.drone.y, world.drone.x, "%c", world.drone.symbol);
            attroff(COLOR_PAIR(world.drone.color_pair));
        }

        refresh();

        // Write the screen dimensions to the pipe
        if (write(pipe_for_world_server[PIPE_WRITE], &world.screen, sizeof(world.screen)) == -1)
        {
            perror("write screen");
            continue;
        }
        else
        {
            fsync(pipe_for_world_server[PIPE_WRITE]) == -1;
        }

        if (write(pipe_for_server_drone_screen[PIPE_WRITE], &world.screen, sizeof(world.screen)) == -1)
        {
            perror("write screen");
            continue;
        }
        else
        {
            fsync(pipe_for_server_drone_screen[PIPE_WRITE]) == -1;
        }

        refresh(); // Refresh the screen to show the changes
    }
    endwin();
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

    sscanf(argv[1], "%d", &number_of_process); // Read the number of processes

    // Loop over argv starting from index 2
    int counter = 2;
    int counter2 = 3;
    for (int i = 0; i < 3; ++i)

    {

        // Read the file descriptors for the current pipe
        printf("counter =========>%d\n", counter);
        printf("counter2 =========>%d\n", counter2);
        sscanf(argv[counter], "%d", &pipes[i][PIPE_READ]);
        sscanf(argv[counter2], "%d", &pipes[i][PIPE_WRITE]);

        counter += 2;
        counter2 += 2;
    }

}

void print_status_of_pipes()
{
    printf("number of process =====> %d \n", number_of_process);
    printf("pipe for server_world (READ) =======> %d \n", pipe_for_server_world[PIPE_READ]);
    printf("pipe for server_world (WRITE) =======> %d \n", pipe_for_server_world[PIPE_WRITE]);
    printf("pipe for wolrd_server (READ) ========>%d \n", pipe_for_world_server[PIPE_READ]);
    printf("pipe for world_Server (WRITE) %d \n", pipe_for_world_server[PIPE_WRITE]);
    printf("pipe for server drone screen(READ) %d \n", pipe_for_server_drone_screen[PIPE_READ]);
    printf("pipe for server drone screen (WRITE) %d \n", pipe_for_server_drone_screen[PIPE_WRITE]);
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

int status_of_buffer() // Function definition for status_of_buffer
{
    /* call stat, fill stat buffer, validate success */
    if (stat(NAME_FOR_WATCHDOG_PEOCESS_ID_FILE, &buffer_of_stat) == -1) // Attempt to retrieve information about a file and store it in the buffer_of_stat structure. Check for failure.
    {
        perror("error-stat"); // Print an error message related to the failure of the stat function.
        return -1; // Return -1 to indicate an error.
    }
    // waits until the file has data
    while (buffer_of_stat.st_size <= 0) // Enter a loop while the size of the file indicated by buffer_of_stat is less than or equal to 0.
    {
        if (stat(NAME_FOR_WATCHDOG_PEOCESS_ID_FILE, &buffer_of_stat) == -1) // Attempt to retrieve information about the file again if the previous attempt failed.
        {
            perror("error-stat"); // Print an error message related to the failure of the stat function.
            return -1; // Return -1 to indicate an error.
        }
        usleep(50000); // Sleep for 50000 microseconds (50 milliseconds).
    }
}
