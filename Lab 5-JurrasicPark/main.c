// os345p3.c - Jurassic Park 07/27/2020
// ***********************************************************************
// **   DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER   **
// **                                                                   **
// ** The code given here is the basis for the CS345 projects.          **
// ** It comes "as is" and "unwarranted."  As such, when you use part   **
// ** or all of the code, it becomes "yours" and you are responsible to **
// ** understand any algorithm or method presented.  Likewise, any      **
// ** errors or problems become your responsibility to fix.             **
// **                                                                   **
// ** NOTES:                                                            **
// ** -Comments beginning with "// ??" may require some implementation. **
// ** -Tab stops are set at every 3 spaces.                             **
// ** -The function API's in "OS345.h" should not be altered.           **
// **                                                                   **
// **   DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER   **
// ***********************************************************************
#include <ctype.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "park.h"

// Jurassic Park
extern int begin;
extern JPARK myPark;
extern pthread_mutex_t parkMutex;            // mutex park variable access
extern pthread_mutex_t fillSeat[NUM_CARS];   // (signal) seat ready to fill
extern pthread_mutex_t seatFilled[NUM_CARS]; // (wait) passenger seated
extern pthread_mutex_t rideOver[NUM_CARS];   // (signal) ride over

void *carTask(void *args);
void *driverTask(void *args);
void *visitorTask(void *args);

int main(int argc, char *argv[])
{
    begin = 0;
    // start park
    pthread_t parkTask;
    pthread_create(&parkTask, NULL, jurassicTask, NULL);

    // wait for park to get initialized...
    while (!begin)
    {
    }
    printf("\n\nWelcome to Jurassic Park\n\n");

    //?? create car, driver, and visitor tasks here

    pthread_join(parkTask, NULL);
    return 0;
}
