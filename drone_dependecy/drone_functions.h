
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
#include <sys/ipc.h>
#include <math.h>
#include <float.h>




//function for calcualting distance
double dis_calculation(float x_point1, float y_point1, float x_point2, float y_point2)
{
    float dis = sqrt((x_point2 - x_point1) * (x_point2 - x_point1) + (y_point2 - y_point1) * (y_point2 - y_point1));
    return dis;
}
