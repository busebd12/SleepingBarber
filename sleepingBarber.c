#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "ezipc.h"
 
int *customerCount;
int *customerHair;
int *numHaircuts;
 
void hairCut();
int main(int argc, char *argv[]) 
{
 
        int number_of_chairs; 
        int maximum_number_customers;
        pid_t customer;
        srand(time(NULL));

        if (argc < 3) 
        {
                printf("You forgot to pass the number of chairs and maximum number of customers (in that order) in the terminal.\nProgram terminated.\n");
                exit(1);
        } 
        else 
        {
                number_of_chairs = strtol(argv[1], NULL, 0);
                maximum_number_customers = strtol(argv[2], NULL, 0);
        }
 
 
        SETUP(); 
 
        int theBarber = SEMAPHORE(SEM_BIN, 1);
        int waitingRoomSeats = SEMAPHORE(SEM_CNT, number_of_chairs); 
 
        customerCount = SHARED_MEMORY(20); 
        *customerCount = 0;
        customerHair = SHARED_MEMORY(20); 
        *customerHair = 0;
        numHaircuts = SHARED_MEMORY(20); 
        *numHaircuts = 1;
 
        pid_t barber = fork(); 
 
        if (barber == 0) 
        {
 
                while(*customerCount < maximum_number_customers) 
                {
 
                        *customerCount+=1; 
                        customer = fork(); 
 
 
                        if (customer == 0) 
                        {
 
                                if (SHOW(waitingRoomSeats) == 0)
                                {
                                        printf("********Customer %d arrived, but leaves because there are %d seats available.\n", *customerCount, SHOW(waitingRoomSeats));
                                }
                                else 
                                {
                                        printf("\nCustomer %d arrived. There are %d seats available.\n", *customerCount, SHOW(waitingRoomSeats));
                                        P(waitingRoomSeats); 
                                        *customerHair+=1; 
                                }
 
 
                                if (*customerCount == maximum_number_customers) 
                                {
                                        printf("######## Reached maximum customers: %d\n", *customerCount);
                                }
 
                                exit(1);
                        }
 
                        sleep(1);
                }
 
        } 
        else 
        { 
 
                printf("Barber process pid is: %d\n", barber);
                printf("Number of Chairs: %d\nMaximum number of Customers: %d\n\n", number_of_chairs, maximum_number_customers);
 
             	sleep(1);
 
                while(1) 
                {
 
                        if (*customerHair < maximum_number_customers) 
                        {
                                printf("Barber awakened, or there was a customer waiting there.\n");
                                P(theBarber); //Decrement barber
                                V(waitingRoomSeats); //Increment waitRM seats after barber is done cutting
                                hairCut(); //Barber does a haircut
                                V(theBarber);
                        }
 
                        if ((*customerCount) == maximum_number_customers)
                        {
                                break;
                        }
                }

                printf("Barber awakened, or there was a customer waiting there.\n");
                while(number_of_chairs-SHOW(waitingRoomSeats) != 0) 
                {
                        hairCut();
                        V(waitingRoomSeats);
                }
                printf("\n\n******************************************************************************\n");
                printf("In total, there were %d customers today, with %d leaving because the shop ran out of space.\nThat means the barber gave %d haircuts today before closing the shop and going home.\nDay well spent!\n", *customerCount, *customerCount-(*numHaircuts-1), *numHaircuts-1);
        }
 
 
        return 0;
 
}
 
 
void hairCut() 
{
 
        srand(time(NULL));
        int random;
        printf("HAAIIRRR CUUTTTTTT. Customer %d is getting a haircut.\n", *numHaircuts); //*customerCount
        random = (rand() % 2) + 2; //So cutting time = 2 to 3 seconds
        sleep(random); //Sleep for 2 to 3 seconds
        printf("###Finished giving haircut to customer. That is haircut number %d today... a busy day\n", *numHaircuts);
        *numHaircuts+=1; //Increment # of numHaircuts done
        return;
}