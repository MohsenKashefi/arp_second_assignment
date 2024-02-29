#include <ncurses.h>
#include <string.h>
#include <time.h>
#include "../include/const.h"
#include "../watchdog_dependency/watchdogVar.h"
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

// passed time
double passedTime(struct timeval current, struct timeval previous)
{
    return (double)(current.tv_sec - previous.tv_sec) + (double)(current.tv_usec - previous.tv_usec) / 1000000;
}


// Terminates all watched processes
void terminate_all_watched_processes()
{
    for (int i = 0; i < NUMBER_OF_PROCESS; i++)
    {
        if (kill(sp_pids[i], SIGKILL) < 0)
        {
            perror("kill");
        }
    }
}
void logFile();
void watchdog_process_id();
int main(int argc, char *argv[])
{
    // related to releasing process id of watch dog
     watchdog_process_id();
    // Reading in pids for other processes
    FILE *file_pointer_for_process_id = NULL;
    struct stat buffer_of_stat;

    char *names_of_file[NUMBER_OF_PROCESS] = NAME_FOR_PROCESS_ID_FILE;

    for (int i = 0; i < NUMBER_OF_PROCESS; i++)
    {
        /* call stat, fill stat buffer, validate success */
        if (stat(names_of_file[i], &buffer_of_stat) == -1)
        {
            perror("error-stat");
            return -1;
        }

        while (buffer_of_stat.st_size <= 0)
        {
            if (stat(names_of_file[i], &buffer_of_stat) == -1)
            {
                perror("error-stat");
                return -1;
            }
            usleep(50000);
        }

        file_pointer_for_process_id = fopen(names_of_file[i], "r");

        fscanf(file_pointer_for_process_id, "%d", &sp_pids[i]);

        fclose(file_pointer_for_process_id);
    }

    // initializes/clears contents of logfile
    FILE *file_for_log = fopen(name_of_log_file, "w");
    fclose(file_for_log);

    refresh();
    // Get a start time to track total run time
    struct timeval process_start_time;
    gettimeofday(&process_start_time, NULL);
    int count;
    int signal_count = 0;

    while (1)
    {
        // Check log file for new entries
        logFile();

        if (signal_count == SIGNAL_PROCESS_BEFORE_WATCHDOG)
        {
            // Send new signal
            for (int i = 0; i < NUMBER_OF_PROCESS; i++)
            {
                if (kill(sp_pids[i], SIGUSR1) < 0)
                {
                    // perror("kill");  //This does weird things to the ncurses window if I leave it in
                }
            }
            signal_count = 0;
        }
        else
        {
            signal_count++;
        }

        usleep(SLEEP_FOR_WATCHDOG);
    }

    return 0;
}
void logFile()
{
    FILE *file = fopen(name_of_log_file, "r");
    char line[256];
    int i = 0;
    char name_of_process[80];
    int process_id;
    long seconds;
    long microseconds;
    int p_idx;

    // skip all data until new lines
    while (i < logfile_line)
    {
        i++;
        fgets(line, sizeof(line), file);
    }

    while (fgets(line, sizeof(line), file))
    {
        sscanf(line, "%d %ld %ld", &process_id, &seconds, &microseconds);

        // Get index of process from its id
        p_idx = 0;
        while (process_id != sp_pids[p_idx] && p_idx < NUMBER_OF_PROCESS)
        {
            p_idx++;
        }

        // Update prev ts
        prev_ts[p_idx].tv_sec = seconds;
        prev_ts[p_idx].tv_usec = microseconds;
        process_data_recieved[p_idx] = 1;
        i++;
    }
    logfile_line = i;
    fclose(file);
}

void watchdog_process_id() {
    // Get the process ID of the watchdog process
    pid_t processId_of_watchdog = getpid();

    // Open a file with write permission to store the process ID
    FILE *file_pointer_for_watchdog = fopen(NAME_FOR_WATCHDOG_PEOCESS_ID_FILE, "w");

    // Write the process ID to the file
    fprintf(file_pointer_for_watchdog, "%d", processId_of_watchdog);

    // Close the file
    fclose(file_pointer_for_watchdog);
}
