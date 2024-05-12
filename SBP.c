#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include <pthread.h>
#include <semaphore.h>
#define MAX_CUSTOMERS 25
void *customer(void *num);
void *barber(void *);

int remaining;

// waitingRoom Limits the # of customers allowed to enter the waiting room at one time.
sem_t waitingRoom;
// barberChair ensures mutually exclusive access to the barber chair.
sem_t barberChair[MAX_CUSTOMERS];
// barberPillow is used to allow the barber to sleep until a customer arrives.
sem_t barberPillow[MAX_CUSTOMERS];
// seatBelt is used to make the customer to wait until the barber is done cutting his/her hair.
sem_t seatBelt[MAX_CUSTOMERS];
// Flag to stop the barber thread when all customers have been serviced.
pthread_mutex_t allDoneLock = PTHREAD_MUTEX_INITIALIZER;
int allDone = 0;
int numChairs = 0;

int main(int argc, char *argv[]) {

  pthread_t tid[MAX_CUSTOMERS];
  int i, x, numCustomers;
  int Number[MAX_CUSTOMERS];

  printf("Welcome to our Barber Shop!\nMaximum number of customers can only be 25\n");
  printf("Enter Number of customers: \n");
  scanf("%d", &x);
  numCustomers = x;
  remaining = x;
  printf("Enter Number of Chairs: \n");
  scanf("%d", &x);
  numChairs = x;

  pthread_t btid[numChairs];

  if (numCustomers > MAX_CUSTOMERS) {
    printf("The maximum number of Customers is %d.\n", MAX_CUSTOMERS);
    return 0;
  }
  printf("A solution to the sleeping barber problem using semaphores.\n");
  for (i = 0; i < MAX_CUSTOMERS; i++) {
    Number[i] = i;
  }
  // Initialize the semaphores with initial values...

  sem_init(&waitingRoom, 0, numChairs);
  for (int i = 0; i < numChairs; i++)
    sem_init(&barberChair[i], 0, 1);

  for (int i = 0; i < numChairs; i++)
    sem_init(&barberPillow[i], 0, 0);

  for (int i = 0; i < numChairs; i++)
    sem_init(&seatBelt[i], 0, 0);
  // Create the barber.
  for (int i = 0; i < numChairs; i++)
    pthread_create(&btid[i], NULL, barber, &Number[i]);
  // Create the customers.
  for (i = 0; i < numCustomers; i++) {
    pthread_create(&tid[i], NULL, customer, (void *)&Number[i]);
  }
  // Join each of the threads to wait for them to finish.
  sleep(10);
  for (i = 0; i < numCustomers; i++) {
    pthread_join(tid[i], NULL);
  }

  for (i = 0; i < numChairs; i++) {
    pthread_join(btid[i], NULL);
  }

  printf("All barbers done for the day.\n");

  for (int i = 0; i < numChairs; i++)
    sem_destroy(&barberChair[i]);

  for (int i = 0; i < numChairs; i++)
    sem_destroy(&barberPillow[i]);

  for (int i = 0; i < numChairs; i++)
    sem_destroy(&seatBelt[i]);

  return 0;
}
void *customer(void *number) {
  int num = *(int *)number; // Leave for the shop and take some random amount of
                            // time to arrive.

  syscall(335, "Leaving for Barber Shop.\n", num);
  sleep(5);
  // Wait for space to open up in the waiting room...
  syscall(335, "Arrived at Barber Shop.\n", num);
  sem_wait(&waitingRoom);
  // Wait for the barber chair to become free.
  syscall(335, "Entering the Barber Shop.\n", num);
  int temp = 0;
  while (1) {
    temp %= numChairs;
    if (sem_trywait(&barberChair[temp]) == 0) {
      // The chair is free so give up your spot in the waiting room.
      sem_post(&waitingRoom); // Wake up the barber...
      break;
    }
    temp++;
  }

  printf("Customer %d waking the barber %d.\n", num, temp);
  syscall(335, "Waking the barber.\n", num);
  sem_post(
      &barberPillow[temp]);  // Wait for the barber to finish cutting your hair.
  sem_wait(&seatBelt[temp]); // Give up the chair.
  sem_post(&barberChair[temp]);
  syscall(335, "Leaving Barber Shop.\n", num);
}

void *barber(void *arg) {
  int num = *(int *)arg;
  // While there are still customers to be serviced... Our barber is omnicient
  // and can tell if there are
  // customers still on the way to his shop.
  while (!allDone) { // Sleep until someone arrives and wakes you.
    printf("Barber %d is sleeping\n", num);
    if (!allDone) { // Take a random amount of time to cut the customer's hair.
      while (1) {
        if (allDone == 1) {
          printf("Barber %d done for the day.\n", num);
          pthread_exit(NULL);
        }
        if (sem_trywait(&barberPillow[num]) == 0) {
          break;
        }
      }
      printf("Barber %d is cutting hair\n", num);
      sleep(3);
      printf("Barber %d has finished cutting hair.\n",
             num); // Release the customer when done cutting
      remaining--;
      pthread_mutex_lock(&allDoneLock);
      if (remaining == 0) {
        allDone = 1;
      }
      pthread_mutex_unlock(&allDoneLock);
      sem_post(&seatBelt[num]);
    }
  }
  printf("Barber %d done for the day.\n", num);
  pthread_exit(NULL);
}