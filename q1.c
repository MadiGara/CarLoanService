/* 
 * Madison Gara
 * October 21, 2022
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

struct CarNode {
    char * licensePlate;
    int mileage;
    int expectedReturnDay, expectedReturnMonth, expectedReturnYear;
    struct CarNode * next;
    struct CarNode * prev;
};
typedef struct CarNode CarNode;

//function prototypes
void push(CarNode ** head);
void printList(CarNode * node, char rented);
CarNode * split(CarNode * head);
CarNode * merge(CarNode * first, CarNode * second, char availableOrRented);
CarNode * mergeSort(CarNode *head, char availableOrRented);
void moveNode (CarNode ** headSource, CarNode ** headDestination);
CarNode * checkList(CarNode * node, char * licensePlate);
void saveList(CarNode * listHead, char whichList);
CarNode * readList(char filename[]);

//run the menu and associated car dealership functions
int main() {
    int choice, numberRead, i, licenseCheck, input; 
    double totalIncome;
    char * inputPtr, * licensePtr;
    char ** date;
    numberRead = 0;
    choice = 0;
    input = 0;
    totalIncome = 0.00;
    inputPtr = malloc(7);
    licensePtr = malloc(20);

    CarNode * availableHead = NULL;
    CarNode * rentedHead = NULL;
    CarNode * repairHead = NULL;
    CarNode * returnedCar = NULL;

    //get lists from their files
    availableHead = readList("availableCars.txt");
    rentedHead = readList("rentedCars.txt");
    repairHead = readList("repairCars.txt");

    //menu loop until user exits
    while (choice != 7) {
        printf("\nWelcome to Carl's Cars. Please pick a service to use:\n");
        printf("1. Add a new car to the available-for-rent list\n");
        printf("2. Add a returned car to the available-for-rent list\n");
        printf("3. Add a returned car to the repair list\n");
        printf("4. Transfer a car from the repair list to the available-for-rent list\n");
        printf("5. Rent the first available car\n");
        printf("6. Print the available-for-rent, repair, and rented car lists\n");
        printf("7. Save and Exit\n\n");
        printf("Your Selection: ");

        //get input within menu bounds
        numberRead = scanf("%d", &choice);
        while (numberRead != 1 || ((numberRead == 1) && (choice <=0 || choice > 7))) {
            //read past the existing input until the newline character to get new input
            scanf("%*[^\n]"); 
            printf("Please enter a number between 1 and 7: ");
            numberRead = scanf("%d", &choice);
        }

        numberRead = 0;

        switch(choice) {
            //add car to available list, sort by mileage
            case 1:
                push(&availableHead);
                printf("Car license plate %s is now available.\n", availableHead->licensePlate);
                availableHead = mergeSort(availableHead, 'a');
                break;

            //add returned car to available list, sort by mileage
            case 2:
                printf("Please enter the license plate of the car you'd like to return:\n");
                scanf("%20s", licensePtr);
                returnedCar = checkList(rentedHead, licensePtr);

                if (returnedCar != NULL) {
                    //get mileage (total, assuming works like an odometer in a car)
                    printf("Please enter the current mileage: ");
                    numberRead = scanf("%d", &input);
                    while (numberRead != 1) {
                        scanf("%*[^\n]");
                        printf("Please enter the mileage as a number: ");
                        numberRead = scanf("%d", &input);
                    }
                    returnedCar->mileage = input;
                    printf("\n");

                    //move returned car to available list - must make it the head of rented list first
                    if (returnedCar != rentedHead) {
                        returnedCar->prev->next = returnedCar->next;
                        if (returnedCar->next != NULL) {
                            returnedCar->next->prev = returnedCar->prev;
                        }
                        returnedCar->next = rentedHead;
                        returnedCar->prev = NULL;
                        if (rentedHead != NULL) {
                            rentedHead->prev = returnedCar;
                        } 
                        rentedHead = returnedCar;
                    }

                    moveNode(&availableHead, &rentedHead);
                    printf("Rented car %s is now available.\n", availableHead->licensePlate);

                    if (availableHead->mileage <= 200) {
                        totalIncome += 80.00;
                    } else {
                        totalIncome += ((15.0 * availableHead->mileage)/100.0);
                    }

                    availableHead = mergeSort(availableHead, 'a');

                } else {
                    printf("That license plate is not rented out.\n\n");
                }
                break;

            //add returned car to repair list
            case 3: 
                printf("Please enter the license plate of the car you'd like to have repaired:\n");
                scanf("%20s", licensePtr);
                returnedCar = checkList(rentedHead, licensePtr);

                if (returnedCar != NULL) {
                    //get total current mileage
                    printf("Please enter the current mileage: ");
                    numberRead = scanf("%d", &input);
                    while (numberRead != 1) {
                        scanf("%*[^\n]");
                        printf("Please enter the mileage as a number: ");
                        numberRead = scanf("%d", &input);
                    }
                    returnedCar->mileage = input;

                    //move returned car to repair list - must make it the head of rented list first
                    if (returnedCar != rentedHead) {
                        returnedCar->prev->next = returnedCar->next;
                        if (returnedCar->next != NULL) {
                            returnedCar->next->prev = returnedCar->prev;
                        }
                        returnedCar->next = rentedHead;
                        returnedCar->prev = NULL;
                        if (rentedHead != NULL) {
                            rentedHead->prev = returnedCar;
                        } 
                        rentedHead = returnedCar;
                    }
                    moveNode(&repairHead, &rentedHead);
                    printf("Rented car %s has been put up for repairs.\n", repairHead->licensePlate);

                    if (repairHead->mileage <= 200) {
                        totalIncome += 80.00;
                    } else {
                        totalIncome += ((15.0 * repairHead->mileage)/100.0);
                    }
                } else {
                    printf("That license plate is not rented out.\n");
                }
                break;

            //transfer car from repair list to available list, sort by mileage
            case 4:
                printf("Please enter the license plate of the repaired car:\n");
                scanf("%20s", licensePtr);
                printf("\n");
                returnedCar = checkList(repairHead, licensePtr);

                if (returnedCar != NULL) {
                    //move returned car to available list - must make it the head of rented list first
                    if (returnedCar != repairHead) {
                        returnedCar->prev->next = returnedCar->next;
                        if (returnedCar->next != NULL) {
                            returnedCar->next->prev = returnedCar->prev;
                        }
                        returnedCar->next = repairHead;
                        returnedCar->prev = NULL;
                        if (repairHead != NULL) {
                            repairHead->prev = returnedCar;
                        } 
                        repairHead = returnedCar;
                    }
                    moveNode(&availableHead, &repairHead);
                    printf("Repaired car %s is now available.\n", availableHead->licensePlate);
                    availableHead = mergeSort(availableHead, 'a');
                } else {
                    printf("That license plate is not rented out.\n\n");
                }
                break;

            //rent first available car
            case 5:
                printf("Please enter your expected return date (yymmdd):\n");
                //only accepts 6 chars (all that should be needed for date)
                scanf("%6s", inputPtr);
                while ((isdigit(*inputPtr) == 0) || (strlen(inputPtr) != 6)) {
                    scanf("%*[^\n]");
                    printf("Input is not a number of appropriate length. Please enter a number date of format yymmdd:\n");
                    scanf("%6s", inputPtr);
                }
                //get rid of extra before the newline that users may have entered
                scanf("%*[^\n]");
                printf("\n");

                //parse date string
                size_t length = strlen(inputPtr);
                date = malloc(sizeof(*date) * length/2);
                for (i = 0; i < (length/2); i++) {
                    date[i] = strndup(inputPtr + 2 * i, 2);
                }

                //assign to struct members - atoi conversions should work since isdigit checked earlier
                availableHead->expectedReturnYear = atoi(date[0]);
                availableHead->expectedReturnMonth = atoi(date[1]);
                availableHead->expectedReturnDay = atoi(date[2]);

                //transfer to rented list
                moveNode(&rentedHead, &availableHead);
                printf("Car %s has been rented out.\n", rentedHead->licensePlate);
                rentedHead = mergeSort(rentedHead, 'r');
                break;

            //print every car list
            case 6:
                printf("\nAvailable cars:\n");
                printList(availableHead, 'n');
                printf("Rented cars:\n");
                printList(rentedHead, 'y');
                printf("Cars in repair:\n");
                printList(repairHead, 'n');
                break;

            //exit and save lists to their own text files
            case 7:
                saveList(availableHead, 'a');
                saveList(rentedHead, 'r');
                saveList(repairHead, 'p');
                printf("\nYour lists have been saved.\nTotal income for this session: $%.2f\n", totalIncome);
                printf("\nThank you for using Carl's Cars. We wish you a pleasant day.\n");
                break;

            default:
                printf("Error: invalid choice entered\n");
                break;
        }
    }
    return 0;
}

/*
* Helper functions for the menu
*/

//push new car to start of list
void push(CarNode ** head) {
    int numberRead, input;
    numberRead = 0;
    CarNode * temp = *head;

    CarNode * newCar = (CarNode *)malloc(sizeof(CarNode));
    newCar->licensePlate = malloc(20);
 
    printf("\nPlease enter car license plate: ");
    scanf("%s", newCar->licensePlate);
    
    //check for duplicate license plates - *only works in current list*
    while (temp != NULL) {
        while (strcmp(temp->licensePlate, newCar->licensePlate) == 0) {
            printf("License plate %s already exists. Please enter another license plate: ", temp->licensePlate);
            scanf("%s", newCar->licensePlate);
            temp = *head;
        }
        temp = temp->next;
    }

    printf("Please enter the current mileage: ");
    numberRead = scanf("%d", &input);
    while (numberRead != 1) {
        scanf("%*[^\n]");
        printf("Please enter the mileage as a number: ");
        numberRead = scanf("%d", &input);
    }
    newCar->mileage = input;

    newCar->expectedReturnDay = 00;
    newCar->expectedReturnMonth = 00;
    newCar->expectedReturnYear = 00;
    printf("\n");

    newCar->next = (*head);
    newCar->prev = NULL;
    if ((*head) != NULL) {
        (*head)->prev = newCar;
    }
    (*head) = newCar;
}

//print list starting at given node
void printList(CarNode * node, char rented) {
    CarNode * last;
    while (node != NULL) {
        printf("License plate %s, ", node->licensePlate);
        if (rented == 'y') {
            printf("expected return date %02d%02d%02d, ", node->expectedReturnYear, node->expectedReturnMonth, node->expectedReturnDay);
        }
        printf("%d miles\n", node->mileage);
        last = node;
        node = node->next;
    }
}

//traverse list to look for given license plate
CarNode * checkList(CarNode * head, char * licensePlate) {
    CarNode * ptr;
    int car, i, flag;
    i = 0;

    ptr = head;

    if (ptr == NULL) {
        printf("There are no cars in the list.\n");
        return NULL;
    } else {
        while (ptr != NULL) {
            if (strcmp(ptr->licensePlate, licensePlate) == 0) {
                return ptr;
            }
            ptr = ptr->next;
        }
        return NULL;
    }
}

//move head node of source list to front of destination list
void moveNode (CarNode ** headDestination, CarNode ** nodeSource) {
    if (*nodeSource == NULL) {
        printf("There are no cars available to rent.\n\n");
        return;
    }

    CarNode * carMoving = *nodeSource;
    *nodeSource = (*nodeSource)->next;
    carMoving->next = *headDestination;
    //set new car node to head of destination list
    *headDestination = carMoving;
}

/*
* Merge sort and its helper functions
*/

//merge sort function to order linked list by ascending mileage
CarNode * mergeSort(CarNode * head, char availableOrRented) {
    if (!head || !head->next) {
        return head;
    }
    CarNode * secondHalf = split(head);

    //cover left and right halves
    head = mergeSort(head, availableOrRented);
    secondHalf = mergeSort(secondHalf, availableOrRented);

    //merge the sorted halves back together
    return merge(head, secondHalf, availableOrRented);
}

//split linked list into two halves
CarNode * split(CarNode * head) {
    CarNode * first = head;
    CarNode * second = head;

    while (first->next && first->next->next) {
        first = first->next->next;
        second = second->next;
    }

    CarNode * temp = second->next;
    second->next = NULL;

    return temp;
}

//merge halves of a split list back togehter 
CarNode * merge(CarNode * firstHalf, CarNode * secondHalf, char availableOrRented) {
    //check if either half empty
    if(!firstHalf) {
        return secondHalf;
    }
    if(!secondHalf) {
        return firstHalf;
    }

    //if available, find smaller mileage value of the comparisons
    if (availableOrRented == 'a') {
        if(firstHalf->mileage < secondHalf->mileage) {
            firstHalf->next = merge(firstHalf->next, secondHalf, availableOrRented);
            firstHalf->next->prev = firstHalf;
            firstHalf->prev = NULL;
            return firstHalf;
        } else {
            secondHalf->next = merge(firstHalf, secondHalf->next, availableOrRented);
            secondHalf->next->prev = secondHalf;
            secondHalf->prev = NULL;
            return secondHalf;
        }
    }
    //if rented, find smallest date value (closer date) of the comparisons
    else {
        if(firstHalf->expectedReturnYear < secondHalf->expectedReturnYear) {
            firstHalf->next = merge(firstHalf->next, secondHalf, availableOrRented);
            firstHalf->next->prev = firstHalf;
            firstHalf->prev = NULL;
            return firstHalf;
        } else if (firstHalf->expectedReturnYear == secondHalf->expectedReturnYear
            && firstHalf->expectedReturnMonth < secondHalf->expectedReturnMonth) {
            firstHalf->next = merge(firstHalf->next, secondHalf, availableOrRented);
            firstHalf->next->prev = firstHalf;
            firstHalf->prev = NULL;
            return firstHalf;
        } else if (firstHalf->expectedReturnYear == secondHalf->expectedReturnYear
            && firstHalf->expectedReturnMonth == secondHalf->expectedReturnMonth
            && firstHalf->expectedReturnDay < secondHalf->expectedReturnDay) {
            firstHalf->next = merge(firstHalf->next, secondHalf, availableOrRented);
            firstHalf->next->prev = firstHalf;
            firstHalf->prev = NULL;
            return firstHalf;
        } else {
            secondHalf->next = merge(firstHalf, secondHalf->next, availableOrRented);
            secondHalf->next->prev = secondHalf;
            secondHalf->prev = NULL;
            return secondHalf;
        }
    }
}

/* 
* File writing and reading functions
*/

//write car list data to file
void saveList(CarNode * listHead, char whichList) {
    char savefileName[30];
    char mileage[20];
    char returnYear[3];
    char returnMonth[3];
    char returnDay[3];
    CarNode * temp = listHead;

    //save to appropriate file depending on list type
    if (whichList == 'a') {
        strcpy(savefileName, "availableCars.txt");
    } else if (whichList == 'r') {
        strcpy(savefileName, "rentedCars.txt");
    } else {
        //using 'p' for this indicator
        strcpy(savefileName, "repairCars.txt"); 
    }

    FILE * filePtr = fopen(savefileName, "w");
    if (filePtr == NULL) {
        printf("Error: file could not be opened.\n");
        return;
    }
    
    //outputs info in text form to file
    while (temp != NULL) {
        fprintf(filePtr, "%s %d %02d %02d %02d\n", temp->licensePlate, temp->mileage, 
        temp->expectedReturnYear, temp->expectedReturnMonth, temp->expectedReturnDay);
        temp = temp->next;
    }

    fclose(filePtr);
}

//read car list data from file
CarNode * readList(char filename[]) {
    FILE * filePtr;
    char wholeLine[30];
    char * parsedWord;
    CarNode * temp;
    CarNode * head;
    CarNode * currentCar;
    currentCar = NULL;
    head = NULL;

    filePtr = fopen(filename, "r");
    if (filePtr == NULL) {
        printf("Error: file could not be opened.\n");
        exit(1);
    }

    while (fgets(wholeLine, sizeof(wholeLine), filePtr)){
        temp = malloc(sizeof(CarNode));

        parsedWord = strtok(wholeLine, " ");
        temp->licensePlate = strdup(wholeLine);
        parsedWord = strtok(NULL, " ");
        temp->mileage = atoi(parsedWord);
        parsedWord = strtok(NULL, " ");
        temp->expectedReturnYear = atoi(parsedWord);
        parsedWord = strtok(NULL, " ");
        temp->expectedReturnMonth = atoi(parsedWord);
        parsedWord = strtok(NULL, " ");
        temp->expectedReturnDay = atoi(parsedWord);
       
        temp->next = NULL;
        if (head == NULL) {
            currentCar = head = temp;
        } else {
            currentCar = currentCar->next = temp;
        }
    }
    fclose(filePtr);

    return head;
}
