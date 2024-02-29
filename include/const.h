#ifndef CONST_H
#define CONST_H


// Number of processes watchdog is tracking
#define NUMBER_OF_PROCESS 6

// Time each child process sleeps (in microseconds)
#define PROCESS_SLEEPS_US {100000, 500000, 300000, 350000, 300000, 300000}

// Number of process cycles before signaling watchdog
#define SIGNAL_PROCESS_BEFORE_WATCHDOG 5

// Watchdog sleep interval (in microseconds)
#define SLEEP_FOR_WATCHDOG 200000

// Time in seconds of process inactivity before watchdog kills all processes
#define TIME_OUT_FOR_PROCESS 10

// Filenames for simple process PIDs
#define NAME_FOR_PROCESS_ID_FILE {"/tmp/pid_file0", "/tmp/pid_file1", "/tmp/pid_file2", "/tmp/pid_file3","/tmp/pid_file4", "/tmp/pid_file5"}

// Filename for watchdog process PID
#define NAME_FOR_WATCHDOG_PEOCESS_ID_FILE "/tmp/pid_filew"

// Process names used for ncurses windows and in log file
#define NAME_OF_PROCESS_LIST {"server", "drone", "input", "window", "obstacles", "targets"}

// File path for watchdog log file
#define WATCHDOG_FILE_NAME "./log/watchdog/watchdog.txt"

#endif // !CONSTANTS_H
