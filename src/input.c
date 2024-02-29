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
#include "../input_dependency/inputConst.h"
#include "../input_dependency/inputVar.h"

int height;
int width;
// Function to append time information into a log file

void update_shape(WINDOW *win, int y, int x, int height, int width, int count);
void log_whatchdog_process_into_file();
void paintShape(WINDOW *win, int y, int x, int height, int width, int count);
void related_to_konsole()
{
    WINDOW *win;
    win = initscr();
    // Initialize ncurses
    win == NULL ? fprintf(stderr, "Error initializing ncurses.\n") : printf("ok");
    win == NULL ? exit(1) : printf("ok");

    // Enable color
    if (has_colors() == FALSE)
    {
        endwin();
        fprintf(stderr, "Your terminal does not support color.\n");
        exit(1);
    }

    start_color();
    init_pair(1, COLOR_RED, COLOR_RED);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    getmaxyx(stdscr, maxRows, maxCols);
    getmaxyx(win, height, width);

    // Draw border around the window
    box(win, 0, 0);

    // Add a 2D array to keep track of count for each square
    int count[gridHeight][gridWidth];
    memset(count, 0, sizeof(count)); // Initialize all counts to 0

    // Calculate initial window position
    int startY = (maxRows - gridHeight * squareHeight) / 8;
    int startX = (maxCols - gridWidth * squareWidth) / 8;

    WINDOW *input_window = newwin(gridHeight * squareHeight, gridWidth * squareWidth, startY, startX);

    if (input_window == NULL)
    {
        endwin();
        fprintf(stderr, "there is an error related to masking board.\n");
        exit(1);
    }

    for (int i = 0; i < gridHeight; ++i)
    {
        for (int j = 0; j < gridWidth; ++j)
        {
            int y = i * squareHeight;
            int x = j * squareWidth;

            wattron(input_window, COLOR_PAIR(1));
            paintShape(input_window, y, x, squareHeight, squareWidth, count[i][j]);
            // Restore previous color
        }
    }
    mvprintw(30, 20, "drone movements");
    mvprintw(31, 20, "to direct drone use this key word");
    mvprintw(32, 20, "w e r s d");
    mvprintw(33, 20, "f x c a b q");

    // Main user interface loop
    while (1)
    {
        int ch = getch();
        char command = '\0';

        // Map keys to control the drone
        switch (ch)
        {
        case 'w':
            command = 'w'; // force towards USx
            if (count[2][2] > 0)
            {
                update_shape(input_window, 2 * squareHeight, 2 * squareWidth, squareHeight, squareWidth, --count[2][2]);
            }
            else
            {
                update_shape(input_window, 0, 0, squareHeight, squareWidth, ++count[0][0]);
            }
            break;
        case 'e':
            command = 'e'; // force towards U
            if (count[2][1] > 0)
            {
                update_shape(input_window, 2 * squareHeight, squareWidth, squareHeight, squareWidth, --count[2][1]);
            }
            else
            {
                update_shape(input_window, 0, squareWidth, squareHeight, squareWidth, ++count[0][1]);
            }
            break;
        case 'r':
            command = 'r'; // force towards UDx
            if (count[2][0] > 0)
            {
                update_shape(input_window, 2 * squareHeight, 0, squareHeight, squareWidth, --count[2][0]);
            }
            else
            {
                update_shape(input_window, 0, 2 * squareWidth, squareHeight, squareWidth, ++count[0][2]);
            }
            break;
        case 's':
            command = 's'; // force towards Sx
            if (count[1][0] > 0)
            {
                update_shape(input_window, squareHeight, 2 * squareWidth, squareHeight, squareWidth, --count[1][0]);
            }
            else
            {
                update_shape(input_window, squareHeight, 0, squareHeight, squareWidth, ++count[1][2]);
            }
            break;
        case 'd':
            command = 'd'; // cancel force
            memset(count, 0, sizeof(count));
            update_shape(input_window, squareHeight, squareWidth, squareHeight, squareWidth, ++count[1][1]);
            update_shape(input_window, 0, 0, squareHeight, squareWidth, count[0][0]);
            update_shape(input_window, 0, squareWidth, squareHeight, squareWidth, count[0][1]);
            update_shape(input_window, 0, 2 * squareWidth, squareHeight, squareWidth, count[0][2]);
            update_shape(input_window, squareHeight, 0, squareHeight, squareWidth, count[1][0]);
            update_shape(input_window, squareHeight, squareWidth, squareHeight, squareWidth, count[1][1]);
            update_shape(input_window, squareHeight, 2 * squareWidth, squareHeight, squareWidth, count[1][2]);
            update_shape(input_window, 2 * squareHeight, 0, squareHeight, squareWidth, count[2][0]);
            update_shape(input_window, 2 * squareHeight, squareWidth, squareHeight, squareWidth, count[2][1]);
            update_shape(input_window, 2 * squareHeight, 2 * squareWidth, squareHeight, squareWidth, count[2][2]);

            break;
        case 'f':
            command = 'f'; // force towards Dx
            if (count[1][2] > 0)
            {
                update_shape(input_window, squareHeight, 0, squareHeight, squareWidth, --count[1][2]);
            }
            else
            {
                update_shape(input_window, squareHeight, 2 * squareWidth, squareHeight, squareWidth, ++count[1][0]);
            }
            break;
        case 'x':
            command = 'x'; // force towards DSx
            if (count[0][2] > 0)
            {
                update_shape(input_window, 0, 2 * squareWidth, squareHeight, squareWidth, --count[0][2]);
            }
            else
            {
                update_shape(input_window, 2 * squareHeight, 0, squareHeight, squareWidth, ++count[2][0]);
            }
            break;
        case 'c':
            command = 'c'; // force towards D
            if (count[0][1] > 0)
            {
                update_shape(input_window, 0, squareWidth, squareHeight, squareWidth, --count[0][1]);
            }
            else
            {
                update_shape(input_window, 2 * squareHeight, squareWidth, squareHeight, squareWidth, ++count[2][1]);
            }
            break;
        case 'v':
            command = 'v'; // force towards DDx
            if (count[0][0] > 0)
            {
                update_shape(input_window, 0, 0, squareHeight, squareWidth, --count[0][0]);
            }
            else
            {
                update_shape(input_window, 2 * squareHeight, 2 * squareWidth, squareHeight, squareWidth, ++count[2][2]);
            }
            break;
        case 'a':
            command = 'a'; // easter egg
            break;
        case 'b':
            command = 'b'; // break
            break;
        case 'q':
            command = 'Q'; // Terminate the program
            break;
        default:
            command = '\0'; // Invalid command
            break;
        }

        wrefresh(input_window); // Add this line

        // Send command to the drone through the pipe
        if (command != '\0')
        {

            int retwrite = write(pipe_for_file_descriptor[PIPE_WRITE], &command, sizeof(char));
            fsync(pipe_for_file_descriptor[PIPE_WRITE]); // flush the pipe

            if (retwrite > 0)
            {
                if (command == 'Q')
                {
                    clear();
                    mvprintw(0, 0, "Termination in progress...\n");
                    refresh();
                    sleep(3);
                    break;
                }
                else
                {
                    mvprintw(5, 70, "the entered command is=====> %c\n", command);
                }
            }
            else if (retwrite < 0)
            {
                perror("write");
                mvprintw(0, 0, "Error writing to pipe\n");

                continue;
            }
            else if (retwrite == 0)
            {
                mvprintw(0, 0, "No bytes written to pipe\n");

                continue;
            }
        }
    }

    // Close ncurses
    endwin();
}

void scanf_of_arguments();
void log_process_into_file();
int set_signals();
int status_of_buffer();
void log_into_file(struct timeval timeval_instance);
void handle_watchdog_process(int sig, siginfo_t *signal_info, void *context);
int main(int argc, char *argv[])
{
    set_signals(); // Set up signal handlers

    switch (argc) // Begin a switch-case based on the number of command-line arguments
    {
    case 4:                                                 // If there are 4 arguments
        scanf_of_arguments(argv);                           // Parse the arguments using a custom function
        break;                                              // Exit the switch statement
    default:                                                // If there's a different number of arguments
        printf("there is an error related to arguments\n"); // Print an error message
        return -1;                                          // Exit the program with a return code indicating an error
        break;                                              // This line is redundant as the control has already left the switch statement
    }

    printf("pipe_for_file_descriptor[0] = %d, pipe_for_file_descriptor[1] = %d\n", pipe_for_file_descriptor[PIPE_READ], pipe_for_file_descriptor[PIPE_WRITE]); // Print the values of file descriptors

    log_process_into_file(); // Log process information into a file

    // Read watchdog pid
    status_of_buffer(); // Check the status of a buffer

    log_whatchdog_process_into_file(); // Log watchdog process information into a file

    // Read how long to sleep process for
    int time_of_sleep[NUMBER_OF_PROCESS] = PROCESS_SLEEPS_US;
    int time_of_sleep2 = time_of_sleep[number_of_process];
    char *name_of_processes[NUMBER_OF_PROCESS] = NAME_OF_PROCESS_LIST;
    name_of_process = name_of_processes[number_of_process]; // added to logfile for readability

    related_to_konsole();

    // the process of drone is completed
    kill(processId_of_drone, SIGKILL);

    return 0;
}
// counter for clicking
void update_shape(WINDOW *win, int y, int x, int height, int width, int count)
{
    char countStr[5];                 // Buffer
    sprintf(countStr, "%02d", count); // cast string to int

    // change the backround and add border
    wattron(win, COLOR_PAIR(2)); // color for counter
    paintShape(win, y, x, height, width, count);

    // border for selected square
    wattron(win, A_BOLD); // Make the border thicker
    box(win, 0, 0);
    wattroff(win, A_BOLD); // Restore previous attributes

    mvwprintw(win, y + height / 2, x + width / 2 - 2, "%s", countStr); // Print the count string at the center of the square
    wattroff(win, COLOR_PAIR(3));                                      // Restore previous attributes
    wrefresh(win);                                                     // Update the window
    usleep(80000);
    paintShape(win, y, x, height, width, count);
    wrefresh(win); // Update the window again
}

void paintShape(WINDOW *win, int y, int x, int height, int width, int count)
{

    // corners of the square
    mvwaddch(win, y, x, ACS_ULCORNER);                          // top left
    mvwaddch(win, y, x + width - 1, ACS_URCORNER);              // top right
    mvwaddch(win, y + height - 1, x, ACS_LLCORNER);             // bottom left
    mvwaddch(win, y + height - 1, x + width - 1, ACS_LRCORNER); // bottom right

    // horizontal sides
    for (int i = x + 1; i < x + width - 1; ++i)
    {
        mvwaddch(win, y, i, ACS_HLINE);              // top horizontal
        mvwaddch(win, y + height - 1, i, ACS_HLINE); // bottom horizantal
    }

    // vertical sidesl
    for (int i = y + 1; i < y + height - 1; ++i)
    {
        mvwaddch(win, i, x, ACS_VLINE);             // Left
        mvwaddch(win, i, x + width - 1, ACS_VLINE); // right
    }

    // top corners
    mvwaddch(win, y + 1, x + 1, ACS_ULCORNER);
    mvwaddch(win, y + 1, x + width - 2, ACS_URCORNER);

    // bottom corners
    mvwaddch(win, y + height - 2, x + 1, ACS_LLCORNER);
    mvwaddch(win, y + height - 2, x + width - 2, ACS_LRCORNER);

    // the curve section of top
    mvwaddch(win, y, x + 1, ACS_LTEE);
    mvwaddch(win, y, x + width - 2, ACS_RTEE);

    // the curve section of low
    mvwaddch(win, y + height - 1, x + 1, ACS_LTEE);
    mvwaddch(win, y + height - 1, x + width - 2, ACS_RTEE);

    char countStr[5];
    sprintf(countStr, "%02d", count);
    mvwprintw(win, y + height / 2, x + width / 2 - 2, "%s", countStr);
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
    // Extract an integer value from the first command-line argument (argv[1])
    // and store it in the variable number_of_process.
    sscanf(argv[1], "%d", &number_of_process);

    // Extract an integer value from the second command-line argument (argv[2])
    // and store it in the corresponding index of the pipe_for_file_descriptor array
    // for reading operations.
    sscanf(argv[2], "%d", &pipe_for_file_descriptor[PIPE_READ]);

    // Extract an integer value from the third command-line argument (argv[3])
    // and store it in the corresponding index of the pipe_for_file_descriptor array
    // for writing operations.
    sscanf(argv[3], "%d", &pipe_for_file_descriptor[PIPE_WRITE]);
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