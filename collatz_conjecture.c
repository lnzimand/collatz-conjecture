#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/**********************************************************************************
 ******************************** Install gcc *************************************
 **********************************************************************************/

/**********************************************************************************
 *   Compile commands: gcc -pthread collatz_conjecture -o collatz_conjecture      *
 **********************************************************************************/

 /*********************************************************************************
 ********************** running program: ./collatz_conjecture *********************
 **********************************************************************************/
  
#define NUM_THREADS 4
#define list_size(list) ((list)->size)

typedef enum {
    UNDONE = 0,
    IN_PROGRESS = 1,
    DONE = 2
} STATE;

typedef struct number_details {
    unsigned long long int num;
    int totalSteps;
    unsigned long long int maxValue;
    STATE state;
    struct number_details *next;
} number_details;

typedef struct number_sequence {
    int size;
    STATE state;
    number_details *head;
} number_sequence;

int     list_ins_next(number_sequence *list, number_details *element)
{
    number_details      *current = list->head;

    if (list->head == NULL)
    {
        list->head = element;
    }
    else
    {
        while(current->next != NULL)
        {
            current = current->next;
        }
        current->next = element;
    }

    list->size++;
    return (0);
}

unsigned long long int oddNum(unsigned long long int num) {
  return (num * 3) + 1;
}

unsigned long long int evenNum(unsigned long long int num) {
  return num / 2;
}

void error_handler(char *str)
{
    printf("%s\n", str);
    exit(1);
}

number_sequence *list_init() {
    number_sequence *list;
    
    if (!(list = (number_sequence*)malloc(sizeof(number_sequence))))
    {
        error_handler("Not enough memory");
    }
    list->head = NULL;
    list->size = 0;
    list->state = UNDONE;
    return list;
}

void print_number_sequence(number_sequence *num_sequence) {
    number_details *head = num_sequence->head;
    while (head->next != NULL)
    {
        printf("NUM: %llu\t\t\t\tTOTAL STEPS: %d\t\t\tMAX VALUE: %llu\n", head->num, head->totalSteps, head->maxValue);
        head = head->next;
    }
}

number_details *details_add(unsigned long long int num) {
    number_details *num_details;
    
    if (!(num_details = (number_details*)malloc(sizeof(number_details))))
    {
        error_handler("Not enough memory");
    }
    num_details->num = num;
    num_details->state = UNDONE;
    num_details->next = NULL;
    return num_details;
}

void stateStatus(number_sequence *num_sequence)
{
    while (num_sequence->state == UNDONE)
    {
        number_details *head = num_sequence->head;
        int all_done = list_size(num_sequence) - 1;
        
        while(head->next != NULL)
        {
            if (head->state == DONE) all_done--;
            head = head->next;
        }
        if (all_done == 0) num_sequence->state = DONE;
    }
}

void solve(number_details *num_details)
{
    unsigned long long int maxValue = 0;
    int totalSteps = 0;
    unsigned long long int num = num_details->num;
    
    while(num != 1)
    {
        if (num > maxValue) maxValue = num;
        num = num % 2 == 0 ? evenNum(num) : oddNum(num);
        totalSteps++;
    }
    num_details->maxValue = maxValue;
    num_details->totalSteps = totalSteps;
    num_details->state = DONE;
}

void collatz_conjecture(number_sequence *num_sequence)
{
    while(num_sequence->state != DONE)
    {
        number_details *head = num_sequence->head;
        while(head->next != NULL)
        {
            if (head->state != DONE && head->state != IN_PROGRESS)
            {
                head->state = IN_PROGRESS;
                solve(head);
            }
            head = head->next;
        }
    }
}

void *threading(void *vargp)
{
    collatz_conjecture((number_sequence*) vargp);
}

void startingPoint(unsigned long long int num) {
  number_sequence *num_sequence = list_init();
  pthread_t threads[NUM_THREADS];
  
  while (num != 1)
  {
      number_details *number = details_add(num);
      list_ins_next(num_sequence, number);
      num = num % 2 == 0 ? evenNum(num) : oddNum(num);
  }
  
  for(int i = 0; i < NUM_THREADS; i++)
  {
      pthread_create(&threads[i], NULL, threading, (void*)num_sequence);
  }

  stateStatus(num_sequence);
  
  print_number_sequence(num_sequence);
}
   
int main()
{
    unsigned long long int num;
    printf("Enter an integer: ");
    if (scanf("%llu", &num) == 1)
    {
        startingPoint(num);
    }
    else
    {
        printf("Please enter a valid number\n");
    }
    return 0;
}