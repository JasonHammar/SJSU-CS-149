#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>

static int commandNumber = 0;

/**
 * The reason for using a hashtable is to store the commands in hash slots by their pids.
 * This way you can lookup a command by a pid and retrieve all the info you need:
 *  command, index (this index means the line number in the text file you read),
 *  start time, and anything else you might need.
 *
 * A hashtable (as you might remember from CS146)
 * has slots and each slot contains a linked list of nodes
 * (these are the entries that contain all the command info).
 * Thus the hashtable (see hashtab array variable below) is
 * implemented as an array of nlists. Each array position is a
 * slot and a linked list of nlist nodes starts at each array slot.
 * Each array position is a hahstable slot.
 *
 * You find the hashtable slot for a pid by using a hash function,
 * which will map the pid to a hashtable slot (array position).
 *
 * You can copy this entire code directly in your .c code. No need to have
 * many files.
 *
 *
 * This nlist is a node, which stores one command's info in the hashtable.
 * TODO: You will need to adopt this code a little.
 *
 * The char *name and char *defn you can remove.
 * The nlist *next field is a pointer to the next node in the linked list.
 * There is one hashtable slot in each array position,
 * consequently there is one linked list of nlists under a hashtable slot.
 */



struct nlist { /* table entry: */
    struct nlist *next; /* next entry in chain */

    double starttime;
    double finishtime;
    int index; // this is the line index in the input text file */
    int pid;  // the process id. you can use the pid result of wait to lookup in the hashtable */
    char *command; // command. This is good to store for when you decide to restart a command */
};

#define HASHSIZE 101
static struct nlist *hashtab[HASHSIZE]; /* pointer table */

/* This is the hash function: form hash value for string s */
/* TODO change to: unsigned hash(int pid) */
/* TODO modify to hash by pid . */
/* You can use a simple hash function: pid % HASHSIZE */
unsigned hash(int pid)
{

    return pid % HASHSIZE;
}

/* lookup: look for s in hashtab */
/* TODO change to lookup by pid: struct nlist *lookup(int pid) */
/* TODO modify to search by pid, you won't need strcmp anymore */
/* This is traversing the linked list under a slot of the hash table. The array position to look in is returned by the hash function */
struct nlist *lookup(int pid)
{
    struct nlist *np;


    for (np = hashtab[hash(pid)]; np != NULL; np = np->next)
        if (pid == np->pid)
            return np; /* found */
    return NULL; /* not found */
}


/* insert: put (name, defn) in hashtab */
/* TODO: change this to insert in hash table the info for a new pid and its command */
/* TODO: change signature to: struct nlist *insert(char *command, int pid, int index). */
/* This insert returns a nlist node. Thus when you call insert in your main function  */
/* you will save the returned nlist node in a variable (mynode). */
/* Then you can set the starttime and finishtime from your main function: */
/* mynode->starttime = starttime; mynode->finishtime = finishtime; */
struct nlist *insert(char *command, int pid, int index, double starttime)
{
    struct nlist *np;
    unsigned hashval;
    //TODO change to lookup by pid. There are 2 cases:
    if ((np = lookup(pid)) == NULL) { /* case 1: the pid is not found, so you have to create it with malloc. Then you want to set the pid, command and index */
        np = (struct nlist *) malloc(sizeof(*np));

        hashval = hash(pid);
        np->pid = pid;
        np->index = index;
        if ((np->command = strdup(command)) == NULL) {
            return NULL;
        }
        np->starttime = starttime;
        np->next = hashtab[hashval];
        hashtab[hashval] = np;
    }
    return np;
}

int main() {
    char *buf = malloc(100);

    int numTokens = 0;
    int childPid;
    int status;
    int fileDesc = 0;
    char *token;
    char *command;
    char *fileName = "";
    char *mypid;
    FILE *fp;
    char *tokens[100];

    struct timespec start, finish;
    double elapsed;

    struct nlist *np = malloc(sizeof(*np));

    while (fgets(buf, 100, stdin) != NULL) {
        commandNumber++;

        int child_id = fork();

        if (child_id == 0) {
            if (buf[strlen(buf) - 1] == '\n') {
                buf[strlen(buf) - 1] = 0;
            }


            token = strtok(buf, " ");

            while (token != NULL) {
                tokens[numTokens++] = token;
                token = strtok(NULL, " ");
            }
            command = tokens[0];

            if (numTokens >= 1) {
                char *argumentList[numTokens + 1];
                for (int i = 0; i < numTokens; i++) {
                    argumentList[i] = tokens[i];
                }

                argumentList[numTokens] = NULL;

                int pid = getpid();
                mypid = malloc(15);
                sprintf(mypid, "%d", pid);


                fileName = strcat(mypid, ".out");
                fp = fopen(fileName, "w");
                fileDesc = open(fileName, O_WRONLY | O_APPEND, O_CREAT);

                dup2(fileDesc, 1);
                fprintf(fp, "Starting command %d: child %d pid of parent %d\n", commandNumber, getpid(), getppid());

                fflush(fp);
                close(fileDesc);
                fclose(fp);

                sprintf(mypid, "%d", pid);
                fileName = strcat(mypid, ".err");
                fp = fopen(fileName, "w");
                fileDesc = open(fileName, O_WRONLY | O_APPEND, O_CREAT);

                dup2(fileDesc, 2);

                close(fileDesc);
                fclose(fp);


                execvp(command, argumentList);

                fprintf(stderr, "%s is an invalid command.\n", command);
                exit(2);
            } else {
                int pid = getpid();
                mypid = malloc(15);
                sprintf(mypid, "%d", pid);

                fileName = strcat(mypid, ".out");
                fp = fopen(fileName, "w");
                fileDesc = open(fileName, O_WRONLY | O_APPEND, O_CREAT);

                dup2(fileDesc, 1);
                fprintf(fp, "Starting command %d: child %d pid of parent %d\n", commandNumber++, getpid(),
                        getppid());

                fflush(fp);
                close(fileDesc);
                fclose(fp);
            }
            exit(0);
        }else if(child_id > 0){
            clock_gettime(CLOCK_MONOTONIC, &start);
            insert(buf, child_id, commandNumber - 1, start.tv_sec);
        }
    }

    while ((childPid = wait(&status)) > 0) {
        clock_gettime(CLOCK_MONOTONIC, &finish);
        int pid = childPid;
        struct nlist *list = lookup(childPid);
        list->finishtime = finish.tv_sec;

        elapsed = (list->finishtime - list->starttime);

        mypid = malloc(15);
        sprintf(mypid, "%d", pid);

        fileName = strcat(mypid, ".out");
        fp = fopen(fileName, "a");

        fprintf(fp, "Finished child %d pid of parent %d\n", pid, getpid());
        fprintf(fp, "Finished at %ld, runtime duration %f\n", finish.tv_sec, elapsed);
        fflush(fp);
        fclose(fp);

        sprintf(mypid, "%d", pid);
        fileName = strcat(mypid, ".err");
        fp = fopen(fileName, "a");

        if (WIFEXITED(status)) {
            fprintf(fp, "Exited with exitcode = %d\n", WEXITSTATUS(status));
            if (elapsed <= 2) {
                fprintf(fp, "Spawning too fast!\n");
            }
        } else if (WIFSIGNALED(status)) {
            fprintf(fp, "Killed with signal %d", WTERMSIG(status));
            if (elapsed <= 2) {
                fprintf(fp, "Spawning too fast!\n");
            }
        }

        fclose(fp);

        if (elapsed > 2) {
            clock_gettime(CLOCK_MONOTONIC, &start);

            int child_id = fork();
            insert(list->command, child_id, commandNumber, start.tv_sec);

            if (child_id < 0) {
                printf("Fork Failed!");
            } else if (child_id == 0) {
                buf = list->command;
                if (buf[strlen(buf) - 1] == '\n') {
                    buf[strlen(buf) - 1] = 0;
                }


                token = strtok(buf, " ");

                while (token != NULL) {
                    tokens[numTokens++] = token;
                    token = strtok(NULL, " ");
                }
                command = tokens[0];

                if (numTokens >= 1) {
                    char *argumentList[numTokens + 1];
                    for (int i = 0; i < numTokens; i++) {
                        argumentList[i] = tokens[i];
                    }

                    argumentList[numTokens] = NULL;

                    int pid = getpid();

                    mypid = malloc(15);
                    sprintf(mypid, "%d", pid);


                    fileName = strcat(mypid, ".out");
                    fp = fopen(fileName, "w");
                    fileDesc = open(fileName, O_WRONLY | O_APPEND, O_CREAT);

                    dup2(fileDesc, 1);
                    fprintf(fp, "RESTARTING\nStarting command %d: child %d pid of parent %d\n", commandNumber, getpid(),
                            getppid());

                    fflush(fp);
                    close(fileDesc);
                    fclose(fp);

                    sprintf(mypid, "%d", pid);
                    fileName = strcat(mypid, ".err");
                    fp = fopen(fileName, "w");
                    fileDesc = open(fileName, O_WRONLY | O_APPEND, O_CREAT);

                    dup2(fileDesc, 2);

                    close(fileDesc);
                    fclose(fp);


                    execvp(command, argumentList);

                    fprintf(stderr, "%s is an invalid command.\n", command);
                    exit(2);
                }
            }//else if(child_id > 0){}
        }


    }
    free(mypid);
    free(np);
}
