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
sem_t room_in_museum;
sem_t room_in_giftshop;
sem_t tickets;

sem_t needTicket;
sem_t wakeupDriver;
sem_t ticketReady;
sem_t buyTicket;

sem_t getPassenger;
sem_t seatTaken;
sem_t passengerSeated;

sem_t gMailbox;

pthread_mutex_t getTicketMutex;
pthread_mutex_t needDriverMutex;

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
    pthread_mutex_init(&getTicketMutex, NULL);
    pthread_mutex_init(&needDriverMutex, NULL);

    sem_init(&room_in_park, 0, 20);
    sem_init(&room_in_museum, 0, 3);
    sem_init(&room_in_giftshop, 0, 3);
    sem_init(&tickets, 0, MAX_TICKETS);
    sem_init(&needTicket, 0, MAX_TICKETS);
    sem_init(&wakeupDriver, 0, NUM_DRIVERS);
    sem_init(&ticketReady, 0, MAX_TICKETS);
    sem_init(&needTicket, 0, MAX_TICKETS);
    sem_init(&buyTicket, 0, MAX_TICKETS);

    sem_init(&getPassenger, 0, 0);    // Signal semaphore
    sem_init(&seatTaken, 0, 0);       // Signal semaphore
    sem_init(&passengerSeated, 0, 0); // Signal semaphore
    sem_init(&wakeupDriver, 0, 0);    // Signal semaphore

    sem_init(&needTicket, 0, MAX_TICKETS);
    sem_init(&wakeupDriver, 0, NUM_DRIVERS);
    sem_init(&ticketReady, 0, MAX_TICKETS);
    sem_init(&needTicket, 0, MAX_TICKETS);
    sem_init(&buyTicket, 0, MAX_TICKETS);

    // wait for park to get initialized...
    while (!begin)
    {
    }
    printf("\n\nWelcome to Jurassic Park\n\n");

    //?? create car, driver, and visitor tasks here
    int visitors[NUM_VISITORS];
    for (int i = 0; i < NUM_VISITORS; i++)
    {
        pthread_create((void *)&visitors[i], NULL, visitorTask, NULL);
        sleep(randomNumber(1, 2));
    }

    int drivers[NUM_DRIVERS];
    for (int i = 0; i < NUM_DRIVERS; i++)
    {
        pthread_create((void *)&drivers[i], NULL, driverTask, (void *)&i);
    }

    int cars[NUM_CARS];
    for (int i = 0; i < NUM_CARS; i++)
    {
        pthread_create((void *)&cars[i], NULL, carTask, (void *)&i);
    }

    pthread_join(parkTask, NULL);

    return 0;
}

// Car task and driver task should be wrapped in a do while (while there are visitors in park; numVisited < 60)
void *carTask(void *args)
{
    // Sem_t array of 3
    // For loop 3 times
    // 3rd time, get the driver
    // Mailbox
    // After ride done, signals 3 passenger semaphore and driver semaphore

    int carID = *(int *)args;
    sem_t passengerSems[3];

    do
    {
        for (int i = 0; i < 3; i++)
        {
            pthread_mutex_lock(&fillSeat[carID]);
            sem_post(&getPassenger);
            // Get visitors local semaphore
            sem_wait(&seatTaken);

            sem_post(&passengerSeated);

            pthread_mutex_lock(&needDriverMutex);
            sem_post(&wakeupDriver);

            pthread_mutex_unlock(&needDriverMutex);
            pthread_mutex_unlock(&fillSeat[carID]);
        }
    } while (myPark.numExitedPark < 60);

    return 0;
}

void *driverTask(void *args)
{
    // int driverID = *(int *)args;
    // Pass semaphore to car and have car pass carID to driver
    do
    {
        sem_wait(&wakeupDriver);
    } while (myPark.numExitedPark < 60);

    return 0;
}

void *visitorTask(void *args)
{
    sem_t mySem;
    // Add visitors to the outside of park
    pthread_mutex_lock(&parkMutex);
    myPark.numOutsidePark++;
    pthread_mutex_unlock(&parkMutex);

    sleep(randomNumber(1, 3));

    // Check to see if the park is full
    sem_wait(&room_in_park);
    pthread_mutex_lock(&parkMutex);
    myPark.numOutsidePark--;
    myPark.numInPark++;
    myPark.numInTicketLine++;
    pthread_mutex_unlock(&parkMutex);

    // Get ticket
    pthread_mutex_lock(&getTicketMutex);
    sem_post(&needTicket);
    sem_post(&wakeupDriver);
    sem_wait(&ticketReady);
    sem_wait(&needTicket);
    sem_post(&buyTicket);
    pthread_mutex_unlock(&getTicketMutex);

    sleep(randomNumber(1, 3));

    pthread_mutex_lock(&parkMutex);
    myPark.numInTicketLine--;
    myPark.numTicketsAvailable--;
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
    myPark.numInMuseum--;
    myPark.numInCarLine++;
    pthread_mutex_unlock(&parkMutex);
    sem_post(&room_in_museum);

    sleep(randomNumber(1, 3));

    // Wait for the car to go around
    pthread_mutex_lock(&parkMutex);
    myPark.numInCarLine--;
    myPark.numInCars++;
    myPark.numTicketsAvailable++;
    myPark.numRidesTaken++;
    pthread_mutex_unlock(&parkMutex);
    sem_post(&tickets);

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
    myPark.numInGiftShop--;
    myPark.numExitedPark++;
    myPark.numInPark--;
    pthread_mutex_unlock(&parkMutex);
    sem_post(&room_in_giftshop);

    sem_post(&room_in_park);

    return 0;
}
