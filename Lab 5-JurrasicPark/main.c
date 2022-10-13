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
volatile int begin;
extern JPARK myPark;
extern pthread_mutex_t parkMutex;            // mutex park variable access
extern pthread_mutex_t fillSeat[NUM_CARS];   // (signal) seat ready to fill
extern pthread_mutex_t seatFilled[NUM_CARS]; // (wait) passenger seated
extern pthread_mutex_t rideOver[NUM_CARS];   // (signal) ride over

void *carTask(void *args);
void *driverTask(void *args);
void *visitorTask(void *args);

sem_t room_in_park;
sem_t room_in_giftshop;
sem_t room_in_museum;

int randomNumber(int lower, int upper)
{
    srand(time(0));
    int num = (rand() % (upper - lower + 1)) + lower;
    return num;
}

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
    int visitors[NUM_VISITORS];
    for (int i = 0; i < NUM_VISITORS; i++)
    {
        pthread_create(&visitors[i], NULL, visitorTask, NULL);
        sleep(randomNumber(1, 2));
    }

    int drivers[NUM_DRIVERS];
    for (int i = 0; i < NUM_DRIVERS; i++)
    {
        pthread_create(&drivers[i], NULL, driverTask, NULL);
    }

    int cars[NUM_CARS];
    for (int i = 0; i < NUM_CARS; i++)
    {
        pthread_create(&cars[i], NULL, carTask, NULL);
    }

    pthread_join(parkTask, NULL);
    return 0;
}

void *carTask(void *args)
{
    return 0;
}

void *driverTask(void *args)
{
    return 0;
}

void *visitorTask(void *args)
{
    sem_init(&room_in_park, 0, 20);
    sem_init(&room_in_museum, 0, 3);
    sem_init(&room_in_giftshop, 0, 3);

    // Add visitors to the outside of park
    pthread_mutex_lock(&parkMutex);
    myPark.numOutsidePark++;
    pthread_mutex_unlock(&parkMutex);

    sleep(randomNumber(1, 3));

    // Check to see if the park is full
    sem_wait(&room_in_park);
    printf("got here");
    pthread_mutex_lock(&parkMutex);
    myPark.numOutsidePark--;
    myPark.numInPark++;
    myPark.numInTicketLine++;
    pthread_mutex_unlock(&parkMutex);

    sleep(randomNumber(1, 3));

    pthread_mutex_lock(&parkMutex);
    myPark.numInTicketLine--;
    myPark.numInMuseumLine++;
    pthread_mutex_unlock(&parkMutex);

    sleep(randomNumber(1, 3));

    // Check to see if museum is full
    sem_wait(&room_in_museum);
    pthread_mutex_lock(&parkMutex);
    myPark.numInMuseumLine--;
    myPark.numInMuseum++;
    pthread_mutex_unlock(&parkMutex);

    sleep(randomNumber(1, 3));

    pthread_mutex_lock(&parkMutex);
    sem_post(&room_in_museum);
    myPark.numInMuseum--;
    myPark.numInCarLine++;
    pthread_mutex_unlock(&parkMutex);

    // Wait for the car to go around
    myPark.numInCarLine--;
    myPark.numInCars++;

    sleep(randomNumber(1, 3));

    pthread_mutex_lock(&parkMutex);
    myPark.numInCars--;
    myPark.numInGiftLine++;
    pthread_mutex_unlock(&parkMutex);

    sleep(randomNumber(1, 3));

    // Check to see if giftshop is full
    sem_wait(&room_in_giftshop);
    pthread_mutex_lock(&parkMutex);
    myPark.numInGiftLine--;
    myPark.numInGiftShop++;
    pthread_mutex_unlock(&parkMutex);

    sleep(randomNumber(1, 3));

    pthread_mutex_lock(&parkMutex);
    sem_post(&room_in_giftshop);
    myPark.numInGiftShop--;
    myPark.numExitedPark++;
    myPark.numInPark--;
    pthread_mutex_unlock(&parkMutex);

    return 0;
}
