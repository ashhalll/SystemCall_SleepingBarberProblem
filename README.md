# SystemCall_SleepingBarberProblem
The well-known sleeping barber dilemma, implemented using pthreads, semaphores, and mutexes in C.

Input:

Number of available seats
Total number of customers
Output:

Average waiting time for customers in milliseconds
Number of customers who had to leave without being served
Barber: pthread
Customers: pthreads

In the realm of computer science, the sleeping barber problem stands as a quintessential challenge in inter-process communication and synchronization across multiple operating system processes. The scenario echoes the task of maintaining a barber's workflow: attending to customers when they are present, resting when the shop is empty, and managing this process with order.

This scenario is modeled around a hypothetical barber shop with a single barber. The barber has a chair for cutting hair in one room and a waiting room equipped with several chairs. After completing a haircut, the barber checks the waiting room for more customers. If there are any, he invites one to the cutting chair for service. If the waiting room is empty, he returns to the cutting chair and awaits the next customer.

Customers, upon arrival, observe the barber's status. If the barber is sleeping, the customer wakes him and takes a seat in the cutting room. If the barber is already serving, the customer waits in the waiting room. If there's an available chair in the waiting room, the customer occupies it, awaiting their turn. If no chairs are free, the customer leaves.

On the surface, this setup seems to ensure the smooth operation of the shop, with the barber attending to each arriving customer until none remain, then resting until the next arrival. However, in practice, various issues can arise, shedding light on broader scheduling challenges.
