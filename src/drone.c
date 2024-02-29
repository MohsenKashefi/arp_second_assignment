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
#include "../drone_dependecy/droneConst.h"
#include "../drone_dependecy/drone_functions.h"
#include "../drone_dependecy/droneVar.h"
#include <sys/ipc.h>
#include <math.h>
#include <float.h>

struct timeval timeval_instance;
float M, K;
struct World world;
char label[256];
void log_into_file(struct timeval timeval_instance);
void handle_watchdog_process(int sig, siginfo_t *signal_info, void *context);
int set_signals();
void scanf_of_arguments(char *argv[]);
void log_process_into_file();
void print_status_of_pipes();
void log_whatchdog_process_into_file();
int get_parameters_from_file(FILE *read_file);
void print_parameters();
int status_of_buffer();

int *pipes[] = {
    pipe_for_drone_input,
    pipe_for_server_drone,
    pipe_for_drone_server,
    pipe_for_drone_server_target,
    pipe_for_drone_server_screen};

int main(int argc, char *argv[])
{
    set_signals();

    switch (argc)
    {
    case 12:
        scanf_of_arguments(argv);
        break;

    default:
        printf("there is an error in arguments\n");
        return -1;
        break;
    }

    print_status_of_pipes();

    // process id
    log_process_into_file();

    // buffer status
    status_of_buffer();
    // setup process id of watchdog
    log_whatchdog_process_into_file();

    // Read how long to sleep process for
    int time_of_sleep[NUMBER_OF_PROCESS] = PROCESS_SLEEPS_US;
    int time_of_sleep2 = time_of_sleep[number_of_process];
    char *name_of_processes[NUMBER_OF_PROCESS] = NAME_OF_PROCESS_LIST;
    name_of_process = name_of_processes[number_of_process]; // added to logfile for readability

    FILE *readFile = fopen("variable_files.txt", "r");

    get_parameters_from_file(readFile);
    print_parameters();

    // Write the drone data to the pipe for communication with the drone server
    write(pipe_for_drone_server[PIPE_WRITE], &world.drone, sizeof(world.drone));

    // Declare variables to store forces in x and y directions
    double fx, fy;

    // Store the previous position of the drone
    double prev_x = world.drone.x, prev_y = world.drone.y;

    // Initialize static variables to store previous velocities
    static double prev_vx = 0, prev_vy = 0;

    // Initialize Fx and Fy with values of fx and fy (These are uninitialized at this point)
    double Fx = fx;
    double Fy = fy;

    // Initialize variables for distance and changes in position
    double distance = 0;
    double dx = 0;
    double dy = 0;

    // Initialize a variable to store distance on the board
    float distance_board = 0;

    // Write a comment explaining that this line calculates the distance on the board based on the drone's position, which will likely be updated later in the code
    // However, since there is no calculation here, it might be a placeholder or an oversight.
    // Consider revisiting this part to ensure correctness or completeness.

    while (1)
    {
        fx = 0;
        fy = 0;

        char command = '\0';
        printf("Reading from pipe\n");

        int board_x0 = 0, board_y0 = 0;
        int board_x1 = world.screen.width, board_y1 = world.screen.height;

        float dist_board_x_y = world.drone.y, dist_board_y_x = world.drone.x;

        float dis[] = {0, 0, 0, 0};

        dis[0] = dis_calculation(world.drone.x, world.drone.y, board_x0, dist_board_x_y);
        dis[1] = dis_calculation(world.drone.x, world.drone.y, board_x1, dist_board_x_y);
        dis[2] = dis_calculation(world.drone.x, world.drone.y, dist_board_y_x, board_y0);
        dis[3] = dis_calculation(world.drone.x, world.drone.y, dist_board_y_x, board_y1);

        float distance_board = dis[0];
        int i = 0;
        distance_board = (dis[1] < distance_board) ? dis[1] : distance_board;
        i = (dis[1] < distance_board) ? 1 : i;

        distance_board = (dis[2] < distance_board) ? dis[2] : distance_board;
        i = (dis[2] < distance_board) ? 2 : i;

        distance_board = (dis[3] < distance_board) ? dis[3] : distance_board;
        i = (dis[3] < distance_board) ? 3 : i;
        //---------------------------------------------------------
        printf("dis1 =>>>>>>>>>>>>>> %f\n", dis[0]);
        printf("dis2 =>>>>>>>>>>>>>> %f\n", dis[1]);
        printf("dis3 =>>>>>>>>>>>>>> %f\n", dis[2]);
        printf("dis4 =>>>>>>>>>>>>>> %f\n", dis[3]);
        printf("dis_of_plane =>>>>>> %f\n", distance_board);
        //----------------------------------------------------------

        // read pipes for server _drone related to ===========> obstacles
        if (read(pipe_for_server_drone[PIPE_READ], &world.obstacle, sizeof(world.obstacle)) == -1)
        {
            perror("there is an error in reading obstacles");
            continue;
        }
        int index_of_nearest_obstacle = -1;
        double minimum_dis = DBL_MAX;
        int counter = 0;     // Initialize a counter variable to 0 for iterating through the obstacles.
        while (counter < 20) // Execute the following block of code as long as the counter is less than 20 (assuming there are 20 obstacles).
        {
            dx = world.drone.x - world.obstacle[counter].x; // Calculate the difference in x-coordinates between the drone and the current obstacle.
            dy = world.drone.y - world.obstacle[counter].y; // Calculate the difference in y-coordinates between the drone and the current obstacle.
            distance = sqrt(dx * dx + dy * dy);             // Calculate the Euclidean distance between the drone and the current obstacle using the Pythagorean theorem.
                                                            // Use absolute value to ensure a positive distance.

            if (distance < minimum_dis) // Check if the calculated distance is less than the current minimum distance.
            {
                minimum_dis = distance;              // If the distance is smaller than the current minimum distance, update the minimum distance.
                index_of_nearest_obstacle = counter; // Update the index of the nearest obstacle to the current counter value.
            }
            ++counter; // Increment the counter to move to the next obstacle.
        }

        // Calculate the difference in x-coordinates between the drone and the closest obstacle.
        dx = world.drone.x - world.obstacle[index_of_nearest_obstacle].x;

        // Calculate the difference in y-coordinates between the drone and the closest obstacle.
        dy = world.drone.y - world.obstacle[index_of_nearest_obstacle].y;

        // Calculate the Euclidean distance between the drone and the closest obstacle using the Pythagorean theorem.
        distance = sqrt(dx * dx + dy * dy); // Use absolute value to ensure a positive distance.

        // Check if the distance to the board (distance_board) is less than the current closest obstacle distance (distance).
        if (distance_board < distance)
        {
            distance = distance_board; // Update the closest obstacle distance to be the distance to the board.

            // Depending on the value of 'i', calculate the difference in x and y coordinates between the drone and the closest board edge.
            switch (i)
            {
            case 0:
                dx = world.drone.x - board_x0; // Calculate the difference in x-coordinate for the left board edge.
                dy = 0;                        // No change in y-coordinate as the board edge is along the x-axis.
                break;
            case 1:
                dx = world.drone.x - board_x1; // Calculate the difference in x-coordinate for the right board edge.
                dy = 0;                        // No change in y-coordinate as the board edge is along the x-axis.
                break;
            case 2:
                dx = 0;                        // No change in x-coordinate as the board edge is along the y-axis.
                dy = world.drone.y - board_y0; // Calculate the difference in y-coordinate for the bottom board edge.
                break;
            case 3:
                dx = 0;                        // No change in x-coordinate as the board edge is along the y-axis.
                dy = world.drone.y - board_y1; // Calculate the difference in y-coordinate for the top board edge.
                break;
            default:
                printf("there is an error"); // Print an error message if 'i' has an unexpected value.
                break;
            }
        }

        // Print the value of the closest obstacle distance for debugging purposes.
        printf("value of distance is ===========> = %f\n", distance);
        refresh();
        double repulsion = 0;   // Initialize the repulsion force to zero.
        double drone_angle = 0; // Initialize the angle of the drone with respect to the obstacle to zero.
        if (distance < 4)       // Check if the obstacle is within the circle of radius 2.
        {
            // Calculate the repulsion force exerted by the obstacle on the drone.
            repulsion = REPLUSION_FORCE_FOR_OBSTACLE * ((1 / distance) - (1 / 4)) * (1 / (distance * distance));
            drone_angle = atan2(dy, dx); // Calculate the angle between the drone and the obstacle.

            // Apply the repulsion force in the opposite direction of the drone's angle.
            Fx = fx + repulsion * cos(drone_angle); // Update the force in the x-direction.
            Fy = fy + repulsion * sin(drone_angle); // Update the force in the y-direction.

            refresh(); // Refresh the display.
        }
        else
        {
            // If the obstacle is not close enough, maintain the original force components.
            Fx = fx;
            Fy = fy;
        }

        // If there is non-zero repulsion force, print the repulsion and drone angle values for debugging.
        if (repulsion != 0)
        {
                printf("repulsion_force  ========> %f\n", repulsion);
                printf("drone_angle ========> %f\n", drone_angle);
            refresh(); // Refresh the display.
        }

        // Read the target position from the pipe for the drone server target.
        if (read(pipe_for_drone_server_target[PIPE_READ], &world.target, sizeof(world.target)) == -1)
        {
            perror("there is an error related to targets"); // Print an error message if reading from the pipe fails.
            continue;                                       // Continue to the next iteration of the loop.
        }

        int index_of_nearest_target = -1; // Initialize the index of the nearest target to -1.
        double minimum_dis_t = DBL_MAX;   // Initialize the minimum distance to a very large value.

        // Find the closest target by iterating through all the target points.
        int counter2 = 0;    // Initialize a counter variable for iterating through the target points.
        while (counter2 < 9) // Iterate through all target points assuming there are 9 targets.
        {
            // Check if the target is both active and visible.
            if (world.target[counter2].is_active && world.target[counter2].is_visible)
            {
                double dx = world.drone.x - world.target[counter2].x; // Calculate the difference in x-coordinates between the drone and the current target.
                double dy = world.drone.y - world.target[counter2].y; // Calculate the difference in y-coordinates between the drone and the current target.
                double distance = sqrt(dx * dx + dy * dy);            // Calculate the Euclidean distance between the drone and the current target.
                                                                      // Use absolute value to ensure a positive distance.

                if (distance < minimum_dis_t) // Check if the distance to the current target is less than the current minimum distance.
                {
                    minimum_dis_t = distance;           // If the distance is smaller than the current minimum distance, update the minimum distance.
                    index_of_nearest_target = counter2; // Update the index of the nearest target.
                }
            }
            // Print information for debugging purposes.
            printf("world.target[%d].x ==================>%d\n", counter2, world.target[counter2].x);
            printf("world.target[%d].y ===================> %d\n", counter2, world.target[counter2].y);
            printf("world.target[%d].is_active ============> %d\n", i, world.target[i].is_active);
            printf("world.target[%d].is_visible ===========> %d\n", i, world.target[index_of_nearest_target].is_visible);
            ++counter2; // Increment the counter to move to the next target point.
        }

        double force_of_attractive = 0; // Initialize the attractive force to zero.
        double drone_angleA = 0;        // Initialize the angle of the drone with respect to the nearest target to zero.

        // Check if the nearest target is both active and visible.
        if (world.target[index_of_nearest_target].is_active && world.target[index_of_nearest_target].is_visible)
        {
            // Calculate the difference in x and y coordinates between the drone and the nearest target.
            double dx = world.drone.x - world.target[index_of_nearest_target].x;
            double dy = world.drone.y - world.target[index_of_nearest_target].y;
            double distance_target = sqrt(dx * dx + dy * dy); // Calculate the distance to the nearest target.

            // Check if the nearest target is within the circle of radius 2 but not within radius 1.
            if (distance_target < 4 && distance_target > 1)
            {
                // Calculate the attractive force exerted by the target on the drone.
                force_of_attractive = -ATTRACTION_FORCE_FOR_TARGETS * (distance_target);
                drone_angleA = atan2(dy, dx); // Calculate the angle between the drone and the nearest target.

                // Apply the attractive force towards the target.
                fx = fx + force_of_attractive * cos(drone_angleA); // Update the force in the x-direction.
                fy = fy + force_of_attractive * sin(drone_angleA); // Update the force in the y-direction.

                minimum_dis_t = DBL_MAX;                               // Reset the minimum distance to a large value.
                index_of_nearest_target = index_of_nearest_target + 1; // Move to the next target index.
                refresh();                                             // Refresh the display.
            }
            else
            {
                fx = Fx; // Maintain the original force in the x-direction.
                fy = Fy; // Maintain the original force in the y-direction.
            }

            // If there is non-zero attractive force, print the force and drone angle values for debugging.

            if (force_of_attractive != 0)
            {
                printf("force_of_attractive ========> %f\n", force_of_attractive);
                printf("drone_angleA ===============> %f\n", drone_angleA);
                refresh(); // Refresh the display.
            }

            refresh(); // Refresh the display.
        }

        int required_bytes_for_reading = read(pipe_for_drone_input[PIPE_READ], &command, sizeof(char));
        if (required_bytes_for_reading > 0)
        {
            switch (command)
            {
            case 'x':
                fy += STRENGTH__OF_INPUT_FORCE; // Force towards USx
                fx -= STRENGTH__OF_INPUT_FORCE;
                break;
            case 'c':
                fy += STRENGTH__OF_INPUT_FORCE; // Force towards U
                break;
            case 'v':
                fy += STRENGTH__OF_INPUT_FORCE; // Force towards UDx
                fx += STRENGTH__OF_INPUT_FORCE;
                break;
            case 's':
                fx -= STRENGTH__OF_INPUT_FORCE; // Force towards Sx
                break;
            case 'd':
                fy = 0; // Nullify force
                fx = 0;
                break;
            case 'f':
                fx += STRENGTH__OF_INPUT_FORCE; // Force towards Dx
                break;
            case 'w':
                fy -= STRENGTH__OF_INPUT_FORCE; // Force towards DSx
                fx -= STRENGTH__OF_INPUT_FORCE;
                break;
            case 'e':
                fy -= STRENGTH__OF_INPUT_FORCE; // Force towards D
                break;
            case 'r':
                fy -= STRENGTH__OF_INPUT_FORCE; // Force towards DDx
                fx += STRENGTH__OF_INPUT_FORCE;
                break;
            case 'b':
                fx = 0; // Nullify force
                fy = 0;
                vx = 0; // Nullify velocity
                vy = 0;
                prev_x = world.drone.x; // Nullify position
                prev_y = world.drone.y;
                prev_vx = 0; // Nullify velocity
                prev_vy = 0;
                break;
            case 'a':
                fx = 0; // Nullify force
                fy = 0;
                vx = 0; // Nullify velocity
                vy = 0;
                world.drone.x = 10; // Reset position
                world.drone.y = 10;
                prev_x = world.drone.x; // Nullify position
                prev_y = world.drone.y;
                prev_vx = 0; // Nullify velocity
                prev_vy = 0;
                break;
            case '\0':
                command = '\0'; // Invalid command
                break;
            }

            printf("invalid comment===========>  %c\n", command);
            printf("please enter a correct command\n");
        }
        else if (required_bytes_for_reading == 0)
        {
            printf("there is no byte for reading\n");
        }
        else
        {
            perror("there is an error related to input");
        }


        // Update velocity and position using the equations of motion
        double ax = (fx / M) - (K * vx); // Calculate acceleration in the x-direction
        double ay = (fy / M) - (K * vy); // Calculate acceleration in the y-direction
        vx = prev_vx + ax * TIME_STEP;           // Update velocity in the x-direction
        vy = prev_vy + ay * TIME_STEP;           // Update velocity in the y-direction

        // Limit the velocity to a maximum of 100 and a minimum of -100
        vx = fmax(fmin(vx, 100), -100);
        vy = fmax(fmin(vy, 100), -100);

        // Calculate the new position based on the updated velocity
        double new_x = prev_x + vx * TIME_STEP; // Calculate new x-coordinate
        double new_y = prev_y + vy * TIME_STEP; // Calculate new y-coordinate

        // Store the current position and velocity for the next iteration
        prev_x = new_x;
        prev_y = new_y;
        prev_vx = vx;
        prev_vy = vy;

        // Update the drone's position in the world structure
        world.drone.x = (int)new_x;                      // Update the x-coordinate of the drone
        world.drone.y = (int)new_y;                      // Update the y-coordinate of the drone
        world.drone.symbol = world.drone.symbol;         // Retain the symbol of the drone
        world.drone.color_pair = world.drone.color_pair; // Retain the color pair of the drone

        // Write the updated drone information to the pipe for the drone server
        write(pipe_for_drone_server[PIPE_WRITE], &world.drone, sizeof(world.drone));
        fsync(pipe_for_drone_server[PIPE_WRITE]); // Ensure data is written to the pipe immediately

        refresh();
    }
    endwin();
    return 0;
}

// Function to append time information into a log file
void log_into_file(struct timeval timeval_instance)
{
    FILE *file_for_log = fopen(name_of_log_file, "a");                                                        // Open the log file in append mode
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
    for (int i = 0; i < 5; ++i)

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

    printf("pipe_for_drone_input[0] =====> %d, pipe_for_drone_input[1] =======> %d\n", pipe_for_drone_input[PIPE_READ], pipe_for_drone_input[PIPE_WRITE]);
    printf("pipe_for_server_drone[0] =======> %d, pipe_for_server_drone[1] =======> %d\n", pipe_for_server_drone[PIPE_READ], pipe_for_server_drone[PIPE_WRITE]);
    printf("pipe_for_drone_server[0] =======> %d, pipe_for_drone_server[1] ======> %d\n", pipe_for_drone_server[PIPE_READ], pipe_for_drone_server[PIPE_WRITE]);
    printf("pipe_for_drone_server_target[0] =======> %d, pipe_for_drone_server_target[1] =======> %d\n", pipe_for_drone_server_target[PIPE_READ], pipe_for_drone_server_target[PIPE_WRITE]);
    printf("pipe_for_drone_server_screen[0] =======> %d, pipe_for_drone_server_screen[1] =======> %d\n", pipe_for_drone_server_screen[PIPE_READ], pipe_for_drone_server_screen[PIPE_WRITE]);
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

// Function to extract parameters from a file
int get_parameters_from_file(FILE *read_file)
{
    // Check if the file pointer is valid
    if (read_file == NULL)
    {
        printf("Error: Unable to open file.\n"); // Print an error message if the file cannot be opened
        return -1;                               // Return -1 to indicate failure
    }

    // Loop through each line in the file
    while (fscanf(read_file, "%s", label) != EOF)
    {
        // Check the label and extract corresponding parameters
        if (strcmp(label, "drone.symbol") == 0)
        {
            fscanf(read_file, " = '%c'", &world.drone.symbol); // Read the drone symbol
        }
        else if (strcmp(label, "drone.color_paire") == 0)
        {
            fscanf(read_file, " = %hd", &world.drone.color_pair); // Read the drone color pair
        }
        else if (strcmp(label, "M") == 0)
        {
            fscanf(read_file, " = %f", &M); // Read the M parameter
        }
        else if (strcmp(label, "K") == 0)
        {
            fscanf(read_file, " = %f", &K); // Read the K parameter
        }
        else if (strcmp(label, "drone.x") == 0)
        {
            fscanf(read_file, " = %d", &world.drone.x); // Read the drone x-coordinate
        }
        else if (strcmp(label, "drone.y") == 0)
        {
            fscanf(read_file, " = %d", &world.drone.y); // Read the drone y-coordinate
        }
    }

    fclose(read_file); // Close the file
}

//------------------------ print parameters ------------------------------
void print_parameters()
{
    printf("drone.symbol =>>>>>>>>> %c\n", world.drone.symbol);
    printf("drone.color_paire =>>>>>>> %hd\n", world.drone.color_pair);
    printf("Mass =>>>>>>>> %f\n", M);
    printf("K =>>>>>>>>> %f\n", K);
    printf("drone.x =>>>>>>>>>>> %d\n", world.drone.x);
    printf("drone.y =>>>>>>>>>>>>>>> %d\n", world.drone.y);
}
//------------------------------------------------------

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
