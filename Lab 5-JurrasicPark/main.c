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
extern volatile int begin;
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
sem_t needPassenger;
sem_t needDriver;

sem_t mailboxReady;
sem_t mailAcquired;
sem_t *gMailbox;
int carMailbox;

pthread_mutex_t getTicketMutex;
pthread_mutex_t needDriverMutex;
pthread_mutex_t mailboxMutex;

int randomNumber(int lower, int upper)
{
    int num = (rand() % (upper - lower + 1)) + lower;
    return num;
}

int main(int argc, char *argv[])
{
    begin = 0;
    srand(time(0));
    // start park
    pthread_t parkTask;
    pthread_create(&parkTask, NULL, jurassicTask, NULL);
    pthread_mutex_init(&getTicketMutex, NULL);
    pthread_mutex_init(&needDriverMutex, NULL);
    pthread_mutex_init(&mailboxMutex, NULL);

    sem_init(&room_in_park, 0, MAX_IN_PARK);
    sem_init(&room_in_museum, 0, MAX_IN_MUSEUM);
    sem_init(&room_in_giftshop, 0, MAX_IN_GIFTSHOP);
    sem_init(&tickets, 0, MAX_TICKETS);

    sem_init(&getPassenger, 0, 0);    // Signal semaphore
    sem_init(&seatTaken, 0, 0);       // Signal semaphore
    sem_init(&passengerSeated, 0, 0); // Signal semaphore
    sem_init(&wakeupDriver, 0, 0);    // Signal semaphore
    sem_init(&mailboxReady, 0, 0);    // Signal semaphore
    sem_init(&mailAcquired, 0, 0);    // Signal semaphore
    sem_init(&needPassenger, 0, 0);   // Signal semaphore
    sem_init(&needTicket, 0, 0);      // Signal semaphore
    sem_init(&ticketReady, 0, 0);     // Signal semaphore
    sem_init(&buyTicket, 0, 0);       // Signal semaphore
    sem_init(&needDriver, 0, 0);      // Signal semaphore

    // wait for park to get initialized...
    while (!begin)
    {
    }
    printf("\n\nWelcome to Jurassic Park\n\n");

    //?? create car, driver, and visitor tasks here
    pthread_t visitors[NUM_VISITORS];
    int visitorsID[NUM_VISITORS];
    for (int i = 0; i < NUM_VISITORS; i++)
    {
        visitorsID[i] = i;
        pthread_create(&visitors[i], NULL, visitorTask, &visitorsID[i]);
    }

    pthread_t drivers[NUM_DRIVERS];
    int driversID[NUM_DRIVERS];
    for (int i = 0; i < NUM_DRIVERS; i++)
    {
        driversID[i] = i;
        pthread_create(&drivers[i], NULL, driverTask, &driversID[i]);
    }

    pthread_t cars[NUM_CARS];
    int carsID[NUM_CARS];
    for (int i = 0; i < NUM_CARS; i++)
    {
        carsID[i] = i;
        pthread_create(&cars[i], NULL, carTask, &carsID[i]);
    }

    pthread_join(parkTask, NULL);

    return 0;
}

void *carTask(void *args)
{
    int carID = *(int *)args;
    sem_t *passengerSems[3];
    sem_t *driverSem;

    do
    {
        for (int i = 0; i < 3; i++)
        {
            pthread_mutex_lock(&fillSeat[carID]);
            sem_post(&needPassenger);
            sem_wait(&mailboxReady);
            passengerSems[i] = gMailbox;
            sem_post(&mailAcquired);
            sem_wait(&seatTaken);
            pthread_mutex_lock(&parkMutex);
            myPark.numTicketsAvailable++;
            pthread_mutex_unlock(&parkMutex);
            sem_post(&tickets);

            if (i == 2) // Car is full
            {
                pthread_mutex_lock(&needDriverMutex);
                carMailbox = carID;
                sem_post(&wakeupDriver);
                sem_post(&needDriver);
                sem_wait(&mailboxReady);
                driverSem = gMailbox;
                sem_post(&mailAcquired);
                pthread_mutex_unlock(&needDriverMutex);
            }
            pthread_mutex_unlock(&seatFilled[carID]);
        }
        pthread_mutex_lock(&rideOver[carID]);

        for (int i = 0; i < 3; i++)
        {
            sem_post(passengerSems[i]);
        }
        sem_post(driverSem);
    } while (myPark.numExitedPark < 60);

    return 0;
}

void *driverTask(void *args)
{
    int driverID = *(int *)args;
    // Pass semaphore to car and have car pass carID to driver
    sem_t mySem;
    sem_init(&mySem, 0, 0);
    int carID;
    do
    {
        sem_wait(&wakeupDriver);
        if (sem_trywait(&needTicket) == 0) // Need to sell tickets
        {
            // assign myPark.driver[id] = -1 (Sell tickets)
            pthread_mutex_lock(&parkMutex);
            myPark.drivers[driverID] = -1;
            pthread_mutex_unlock(&parkMutex);

            // simulate time to print ticket with sleep
            sleep(randomNumber(1, 3));

            // post to ticket ready
            sem_post(&ticketReady);
            // wait on buy ticket
            sem_wait(&buyTicket);

            // assign myPark.driver[id] = 0 (Go back to sleep)
            pthread_mutex_lock(&parkMutex);
            myPark.drivers[driverID] = 0;
            pthread_mutex_unlock(&parkMutex);
        }
        else if (sem_trywait(&needDriver) == 0) // Need to drive car
        {
            carID = carMailbox;
            pthread_mutex_lock(&parkMutex);
            myPark.drivers[driverID] = carID + 1;
            pthread_mutex_unlock(&parkMutex);

            pthread_mutex_lock(&mailboxMutex);
            gMailbox = &mySem;
            sem_post(&mailboxReady);
            sem_wait(&mailAcquired);
            pthread_mutex_unlock(&mailboxMutex);

            sem_wait(&mySem);

            pthread_mutex_lock(&parkMutex);
            myPark.drivers[driverID] = 0;
            pthread_mutex_unlock(&parkMutex);
        }

    } while (myPark.numExitedPark < 60);

    return 0;
}

void *visitorTask(void *args)
{
    sem_t mySem;
    sem_init(&mySem, 0, 0);
    sleep(randomNumber(1, 30));

    /*
 Add visitors to outside of park
*/
    pthread_mutex_lock(&parkMutex);
    myPark.numOutsidePark++;
    pthread_mutex_unlock(&parkMutex);

    sleep(randomNumber(1, 3));

    /*
 Check to see if park is full
*/
    sem_wait(&room_in_park);
    pthread_mutex_lock(&parkMutex);
    myPark.numOutsidePark--;
    myPark.numInPark++;
    myPark.numInTicketLine++;
    pthread_mutex_unlock(&parkMutex);

    /*
 Get Ticket
*/
    sem_wait(&tickets);
    pthread_mutex_lock(&getTicketMutex);
    sem_post(&needTicket);
    sem_post(&wakeupDriver);
    sem_wait(&ticketReady);
    sem_post(&buyTicket);
    pthread_mutex_unlock(&getTicketMutex);

    pthread_mutex_lock(&parkMutex);
    myPark.numInTicketLine--;
    myPark.numTicketsAvailable--;
    myPark.numInMuseumLine++;
    pthread_mutex_unlock(&parkMutex);

    sleep(randomNumber(1, 3));

    /*
 Check to see if museum is full
*/
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

    /*
 Car section
*/

    /*
 Hand ticket and get into car
*/

    sem_wait(&needPassenger);
    pthread_mutex_lock(&mailboxMutex);
    gMailbox = &mySem;
    sem_post(&mailboxReady);
    sem_wait(&mailAcquired);
    pthread_mutex_unlock(&mailboxMutex);

    pthread_mutex_lock(&parkMutex);
    myPark.numInCarLine--;
    myPark.numInCars++;
    pthread_mutex_unlock(&parkMutex);
    sem_post(&seatTaken);

    sem_wait(&mySem);

    pthread_mutex_lock(&parkMutex);
    myPark.numInCars--;
    myPark.numInGiftLine++;
    pthread_mutex_unlock(&parkMutex);

    sleep(randomNumber(1, 3));

    /*
 Check to see if gift shop is full
*/
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
