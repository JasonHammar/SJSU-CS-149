#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

/*****************************************
//CS149 Fall 2022
//Template for assignment 6
//San Jose State University
//originally prepared by Bill Andreopoulos
*****************************************/


//thread mutex lock for access to the log index
//TODO you need to use this mutexlock for mutual exclusion
//when you print log messages from each thread
pthread_mutex_t tlock1 = PTHREAD_MUTEX_INITIALIZER;


//thread mutex lock for critical sections of allocating THREADDATA
//TODO you need to use this mutexlock for mutual exclusion
pthread_mutex_t tlock2 = PTHREAD_MUTEX_INITIALIZER;


//thread mutex lock for access to the name counts data structure
//TODO you need to use this mutexlock for mutual exclusion
pthread_mutex_t tlock3 = PTHREAD_MUTEX_INITIALIZER;


void* thread_runner(void*);
pthread_t tid1, tid2;

//struct points to the thread that created the object.
//This is useful for you to know which is thread1. Later thread1 will also deallocate.
struct THREADDATA_STRUCT
{
  pthread_t creator;
};
typedef struct THREADDATA_STRUCT THREADDATA;

THREADDATA* p=NULL;


//variable for indexing of messages by the logging function.
int logindex=0;
int *logip = &logindex;


//The name counts.
// You can use any data structure you like, here are 2 proposals: a linked list OR an array (up to 100 names).
//The linked list will be faster since you only need to lock one node, while for the array you need to lock the whole array.
//You can use a linked list template from A5. You should also consider using a hash table, like in A5 (even faster).
struct NAME_STRUCT
{
  char name[30];
  int count;
};
typedef struct NAME_STRUCT THREAD_NAME;

//array of 100 names
THREAD_NAME names_counts[100];

//node with name_info for a linked list
struct NAME_NODE
{
  THREAD_NAME name_count;
  struct NAME_NODE *next;
};

int numnames = 0;

/*********************************************************
// function main
*********************************************************/
int main(int argc, char* argv[])
{
  //TODO similar interface as A2: give as command-line arguments three filenames of numbers (the numbers in the files are newline-separated).
  if(argc != 3){
      printf("Error, you must enter two files as arguments only!");
      return 0;
  }
  for(int j = 0; j < 100; j++){
        names_counts[j].count = 0;
    }

  printf("\n==================== Log Message and Error Message =================\n");
  printf("create first thread\n");
  pthread_create(&tid1,NULL,thread_runner,argv[1]);

  printf("create second thread\n");
  pthread_create(&tid2,NULL,thread_runner,argv[2]);

  printf("wait for first thread to exit\n");
  pthread_join(tid1,NULL);
  printf("first thread exited\n");

  printf("wait for second thread to exit\n");
  pthread_join(tid2,NULL);
  printf("second thread exited\n");

    printf("\n==================== Name Count Result =================\n");
  //TODO print out the sum variable with the sum of all the numbers
    for(int i = 0; i < 100; i++){
        if(names_counts[i].name != NULL && strlen(names_counts[i].name) > 1){
            printf("%s: %d\n", names_counts[i].name, names_counts[i].count);
        }
    }

  exit(0);

}//end main


/**********************************************************************
// function thread_runner runs inside each thread
**********************************************************************/
void* thread_runner(void* x)
{

    int hours, minutes, seconds, day, month, year;

    time_t now;
    time(&now);

    struct tm *local = localtime(&now);

    hours = local->tm_hour;
    minutes = local->tm_min;
    seconds = local->tm_sec;

    day = local->tm_mday;
    month = local->tm_mon + 1;
    year = local->tm_year + 1900;

  pthread_t me;
  char * arg = (char *)x;
  me = pthread_self();


  pthread_mutex_lock(&tlock2); // critical section starts
  if (p==NULL) {
    p = (THREADDATA*) malloc(sizeof(THREADDATA));
    p->creator=me;
  }
  pthread_mutex_unlock(&tlock2);  // critical section ends

  pthread_mutex_lock(&tlock1);
  if(hours < 12)
    printf("Logindex: %d, thread: %ld, PID: %d, %02d/%02d/%02d %02d:%02d:%02d am ", ++logindex, me, getpid(), day, month, year, hours, minutes, seconds);
  else
      printf("Logindex: %d, thread: %ld, PID: %d, %02d/%02d/%02d %02d:%02d:%02d pm ", ++logindex, me, getpid(), day, month, year, hours, minutes, seconds);
  printf("This is thread %ld (p=%p)\n",me,p);
  pthread_mutex_unlock(&tlock1);

  if (p!=NULL && p->creator==me) {
      pthread_mutex_lock(&tlock1);
      if(hours < 12)
          printf("Logindex: %d, thread: %ld, PID: %d, %02d/%02d/%02d %02d:%02d:%02d am ", ++logindex, me, getpid(), day, month, year, hours, minutes, seconds);
      else
          printf("Logindex: %d, thread: %ld, PID: %d, %02d/%02d/%02d %02d:%02d:%02d pm ", ++logindex, me, getpid(), day, month, year, hours, minutes, seconds);
      printf("This is thread %ld and I created THREADDATA %p\n",me,p);
      pthread_mutex_unlock(&tlock1);
  } else {
      pthread_mutex_lock(&tlock1);
      if(hours < 12)
          printf("Logindex: %d, thread: %ld, PID: %d, %02d/%02d/%02d %02d:%02d:%02d am ", ++logindex, me, getpid(), day, month, year, hours, minutes, seconds);
      else
          printf("Logindex: %d, thread: %ld, PID: %d, %02d/%02d/%02d %02d:%02d:%02d pm ", ++logindex, me, getpid(), day, month, year, hours, minutes, seconds);
      printf("This is thread %ld and I can access the THREADDATA %p\n",me,p);
      pthread_mutex_unlock(&tlock1);
  }


  /**
   * //TODO implement any thread name counting functionality you need.
   * Assign one file per thread. Hint: you can either pass each argv filename as a thread_runner argument from main.
   * Or use the logindex to index argv, since every thread will increment the logindex anyway
   * when it opens a file to print a log message (e.g. logindex could also index argv)....
   * //Make sure to use any mutex locks appropriately
   */
    if(p != NULL && p->creator == me){
        pthread_mutex_lock(&tlock1);
        if(hours < 12)
            printf("Logindex: %d, thread: %ld, PID: %d, %02d/%02d/%02d %02d:%02d:%02d am ", ++logindex, me, getpid(), day, month, year, hours, minutes, seconds);
        else
            printf("Logindex: %d, thread: %ld, PID: %d, %02d/%02d/%02d %02d:%02d:%02d pm ", ++logindex, me, getpid(), day, month, year, hours, minutes, seconds);
        printf("Opening File: %s\n", arg);
        pthread_mutex_unlock(&tlock1);

        pthread_mutex_lock(&tlock3);
        FILE *fp;
        fp = fopen(arg, "r");

        int linecounter = 0;
        char name[30];
        if(fp == NULL){
            printf("range: cannot open file\n");
            exit(1);
        }


        while(fgets(name,30, fp)){
            bool duplicate = false;

            if(name[0] == '\n' || name[1] == '\n'){
                linecounter++;
                fprintf(stderr, "Warning - file %s line %d is empty\n", arg, linecounter);
            }else{
                for(int j = 0; j < 30; j++){
                    if(name[j] == '\n'){
                        name[j] = '\0';
                    }
                }
                for(int j = 0; j < numnames; j++){
                    if(strcmp(name, names_counts[j].name) == 0){
                        duplicate = true;
                        names_counts[j].count++;
                        break;
                    }
                }
                if(!duplicate){
                    names_counts[numnames].count++;
                    for(int j = 0; j < 30; j++){
                        names_counts[numnames].name[j] = name[j];
                    }
                    numnames++;
                }
                linecounter++;
            }
        }

        pthread_mutex_unlock(&tlock3);
    }else{
        pthread_mutex_lock(&tlock1);
        if(hours < 12)
            printf("Logindex: %d, thread: %ld, PID: %d, %02d/%02d/%02d %02d:%02d:%02d am ", ++logindex, me, getpid(), day, month, year, hours, minutes, seconds);
        else
            printf("Logindex: %d, thread: %ld, PID: %d, %02d/%02d/%02d %02d:%02d:%02d pm ", ++logindex, me, getpid(), day, month, year, hours, minutes, seconds);
        printf("Opening File: %s\n", arg);
        pthread_mutex_unlock(&tlock1);

        pthread_mutex_lock(&tlock3);
        FILE *fp;
        fp = fopen(arg, "r");

        int linecounter = 0;
        char name[30];
        if(fp == NULL){
            printf("range: cannot open file\n");
            exit(1);
        }


        while(fgets(name,30, fp)){
            bool duplicate = false;

            if(name[0] == '\n' || name[1] == '\n'){
                linecounter++;
                fprintf(stderr, "Warning - file %s line %d is empty\n", arg, linecounter);
            }else{
                for(int j = 0; j < 30; j++){
                    if(name[j] == '\n'){
                        name[j] = '\0';
                    }
                }
                for(int j = 0; j < numnames; j++){
                    if(strcmp(name, names_counts[j].name) == 0){
                        duplicate = true;
                        names_counts[j].count++;
                        break;
                    }
                }
                if(!duplicate){
                    names_counts[numnames].count++;
                    for(int j = 0; j < 30; j++){
                        names_counts[numnames].name[j] = name[j];
                    }
                    numnames++;
                }
                linecounter++;
            }
        }

        pthread_mutex_unlock(&tlock3);
    }

  // TODO use mutex to make this a start of a critical section
  pthread_mutex_lock(&tlock2);
  if (p!=NULL && p->creator==me) {
      pthread_mutex_lock(&tlock1);
      if(hours < 12)
          printf("Logindex: %d, thread: %ld, PID: %d, %02d/%02d/%02d %02d:%02d:%02d am ", ++logindex, me, getpid(), day, month, year, hours, minutes, seconds);
      else
          printf("Logindex: %d, thread: %ld, PID: %d, %02d/%02d/%02d %02d:%02d:%02d pm ", ++logindex, me, getpid(), day, month, year, hours, minutes, seconds);
      printf("This is thread %ld and I delete THREADDATA\n",me);
      pthread_mutex_unlock(&tlock1);
  /**
   * TODO Free the THREADATA object.
   * Freeing should be done by the same thread that created it.
   * See how the THREADDATA was created for an example of how this is done.
   */
    free(p);
  } else {
      pthread_mutex_lock(&tlock1);
      if(hours < 12)
          printf("Logindex: %d, thread: %ld, PID: %d, %02d/%02d/%02d %02d:%02d:%02d am ", ++logindex, me, getpid(), day, month, year, hours, minutes, seconds);
      else
          printf("Logindex: %d, thread: %ld, PID: %d, %02d/%02d/%02d %02d:%02d:%02d pm ", ++logindex, me, getpid(), day, month, year, hours, minutes, seconds);
      printf("This is thread %ld and I can access the THREADDATA\n",me);
      pthread_mutex_unlock(&tlock1);
  }
  // TODO critical section ends
    pthread_mutex_unlock(&tlock2);
  pthread_exit(NULL);
  return NULL;

}//end thread_runner
