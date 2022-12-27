#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

/**
 *///CS149 assignment#4 helper code.
// See the TODO's in the comments below! You need to implement those.


/**
 */// TRACE_NODE_STRUCT is a linked list of
// pointers to function identifiers
// TRACE_TOP is the head of the list is the top of the stack

struct TRACE_NODE_STRUCT {
    char* functionid;                // ptr to function identifier (a function name)
    struct TRACE_NODE_STRUCT* next;  // ptr to next frama
};
typedef struct TRACE_NODE_STRUCT TRACE_NODE;
static TRACE_NODE* TRACE_TOP = NULL;       // ptr to the top of the stack

struct commandStruct{
    char* command;
    int index;
    struct commandStruct* next;
};
typedef struct  commandStruct NODE;
static NODE* TOP = NULL;

/* --------------------------------*/
/* function PUSH_TRACE */
/*
 * The purpose of this stack is to trace the sequence of function calls,
 * just like the stack in your computer would do.
 * The "global" string denotes the start of the function call trace.
 * The char *p parameter is the name of the new function that is added to the call trace.
 * See the examples of calling PUSH_TRACE and POP_TRACE below
 * in the main, make_extend_array, add_column functions.
**/
void PUSH_TRACE(char* p)          // push p on the stack
{
    TRACE_NODE* tnode;
    static char glob[]="global";

    if (TRACE_TOP==NULL) {

        // initialize the stack with "global" identifier
        TRACE_TOP=(TRACE_NODE*) malloc(sizeof(TRACE_NODE));

        // no recovery needed if allocation failed, this is only
        // used in debugging, not in production
        if (TRACE_TOP==NULL) {
            printf("PUSH_TRACE: memory allocation error\n");
            exit(1);
        }

        TRACE_TOP->functionid = glob;
        TRACE_TOP->next=NULL;
    }//if

    // create the node for p
    tnode = (TRACE_NODE*) malloc(sizeof(TRACE_NODE));

    // no recovery needed if allocation failed, this is only
    // used in debugging, not in production
    if (tnode==NULL) {
        printf("PUSH_TRACE: memory allocation error\n");
        exit(1);
    }//if

    tnode->functionid=p;
    tnode->next = TRACE_TOP;  // insert fnode as the first in the list
    TRACE_TOP=tnode;          // point TRACE_TOP to the first node

}/*end PUSH_TRACE*/

/* --------------------------------*/
/* function POP_TRACE */
/* Pop a function call from the stack */
void POP_TRACE()    // remove the op of the stack
{
    TRACE_NODE* tnode;
    tnode = TRACE_TOP;
    TRACE_TOP = tnode->next;
    free(tnode);

}/*end POP_TRACE*/



/* ---------------------------------------------- */
/* function PRINT_TRACE prints out the sequence of function calls that are on the stack at this instance */
/* For example, it returns a string that looks like: global:funcA:funcB:funcC. */
/* Printing the function call sequence the other way around is also ok: funcC:funcB:funcA:global */
char* PRINT_TRACE()
{
    int depth = 50; //A max of 50 levels in the stack will be combined in a string for printing out.
    int i, length, j;
    TRACE_NODE* tnode;
    static char buf[100];

    if (TRACE_TOP==NULL) {     // stack not initialized yet, so we are
        strcpy(buf,"global");   // still in the `global' area
        return buf;
    }

    /* peek at the depth(50) top entries on the stack, but do not
       go over 100 chars and do not go over the bottom of the
       stack */

    sprintf(buf,"%s",TRACE_TOP->functionid);
    length = strlen(buf);                  // length of the string so far
    for(i=1, tnode=TRACE_TOP->next;
        tnode!=NULL && i < depth;
        i++,tnode=tnode->next) {
        j = strlen(tnode->functionid);             // length of what we want to add
        if (length+j+1 < 100) {              // total length is ok
            sprintf(buf+length,"%d:%s",i - 1,tnode->functionid);
            length += j+1;
        }else                                // it would be too long
            break;
    }
    return buf;
} /*end PRINT_TRACE*/

// -----------------------------------------
// function REALLOC calls realloc
// TODO REALLOC should also print info about memory usage.
// TODO For this purpose, you need to add a few lines to this function.
// For instance, example of print out:
// "File tracemem.c, line X, function F reallocated the memory segment at address A to a new size S"
// Information about the function F should be printed by printing the stack (use PRINT_TRACE)
void* REALLOC(void* p,int t,char* file,int line)
{
    FILE* fp;
    fp = fopen("mem_tracer.out", "a");
    p = realloc(p,t);
    fprintf(fp, "File %s, line %d, function %s reallocated the memory segment at address %p to a new size %d\n", file, line, PRINT_TRACE(), &p, t);
    fflush(fp);
    fclose(fp);
    return p;
}

// -------------------------------------------
// function MALLOC calls malloc
// TODO MALLOC should also print info about memory usage.
// TODO For this purpose, you need to add a few lines to this function.
// For instance, example of print out:
// "File tracemem.c, line X, function F allocated new memory segment at address A to size S"
// Information about the function F should be printed by printing the stack (use PRINT_TRACE)
void* MALLOC(int t,char* file,int line)
{
    void* p;
    FILE* fp;
    fp = fopen("mem_tracer.out", "a");
    p = malloc(t);
    fprintf(fp, "File %s, line %d, function %s allocated new memory segment at address %p to a new size %d\n", file, line, PRINT_TRACE(), &p, t);
    fflush(fp);
    fclose(fp);
    return p;
}

// ----------------------------------------------
// function FREE calls free
// TODO FREE should also print info about memory usage.
// TODO For this purpose, you need to add a few lines to this function.
// For instance, example of print out:
// "File tracemem.c, line X, function F deallocated the memory segment at address A"
// Information about the function F should be printed by printing the stack (use PRINT_TRACE)
void FREE(void* p,char* file,int line)
{
    FILE* fp;
    fp = fopen("mem_tracer.out", "a");
    free(p);
    fprintf(fp, "File %s, line %d, function %s deallocated the memory segment at address %p\n", file, line, PRINT_TRACE(), &p);
    fflush(fp);
    fclose(fp);

}

#define realloc(a,b) REALLOC(a,b,__FILE__,__LINE__)
#define malloc(a) MALLOC(a,__FILE__,__LINE__)
#define free(a) FREE(a,__FILE__,__LINE__)


// -----------------------------------------
// function add_column will add an extra column to a 2d array of ints.
// This function is intended to demonstrate how memory usage tracing of realloc is done
// Returns the number of new columns (updated)
int add_column(int** array,int rows,int columns)
{
    PUSH_TRACE("add_column");
    int i;

    for(i=0; i<rows; i++) {
        array[i]=(int*) realloc(array[i],sizeof(int)*(columns+1));
        array[i][columns]=10*i+columns;
    }//for
    POP_TRACE();
    return (columns+1);
}// end add_column


// ------------------------------------------
// function make_extend_array
// Example of how the memory trace is done
// This function is intended to demonstrate how memory usage tracing of malloc and free is done
void make_extend_array()
{
    PUSH_TRACE("make_extend_array");
    int i, j;
    int **array;
    int ROW = 4;
    int COL = 3;

    //make array
    array = (int**) malloc(sizeof(int*)*4);  // 4 rows
    for(i=0; i<ROW; i++) {
        array[i]=(int*) malloc(sizeof(int)*3);  // 3 columns
        for(j=0; j<COL; j++)
            array[i][j]=10*i+j;
    }//for

    //display array
    for(i=0; i<ROW; i++)
        for(j=0; j<COL; j++)
            printf("array[%d][%d]=%d\n",i,j,array[i][j]);

    // and a new column
    int NEWCOL = add_column(array,ROW,COL);

    // now display the array again
    for(i=0; i<ROW; i++)
        for(j=0; j<NEWCOL; j++)
            printf("array[%d][%d]=%d\n",i,j,array[i][j]);

    //now deallocate it
    for(i=0; i<ROW; i++)
        free((void*)array[i]);
    free((void*)array);

    POP_TRACE();
    return;
}//end make_extend_array

void PrintNodes(NODE* node){

    if(node == NULL){
        return;
    }
    if(node != NULL){
        fprintf(stdout, "Line Index: %d, Command: %s\n", node->index, node->command);

        PrintNodes(node->next);
    }
}

void setCommand(char* command, NODE* node){
    node->command = command;
}

void setIndex(int index, NODE* node){
    node->index = index;
}

// ----------------------------------------------
// function main
int main()
{

    PUSH_TRACE("main");
    char** commands;

    TOP = NULL;
    NODE* curr = NULL;
    NODE* prev = malloc(sizeof(NODE));
    free(prev);


    commands = malloc(sizeof(char*) * 10);

    char* str = malloc(sizeof(char)*100);
    int i = 0;
    int size = 10;
    while(fgets(str, 100, stdin) != NULL) {

        commands[i] = strdup(str);
        printf("Command: %s", commands[i]);

        curr = malloc(sizeof(NODE));
        curr->next = NULL;
        setCommand(commands[i], curr);
        setIndex(i, curr);
        prev->next = curr;
        prev = curr;

        if(i == 0){
            TOP = curr;
        }

        i++;

        if(i == size){
            size += 100;
            commands = realloc(commands, sizeof(char*) * size);
        }

    }

    PrintNodes(TOP);

    while(TOP != NULL){
        NODE* temp = TOP;
        TOP = TOP->next;
        free(temp);
    }

    free(str);

    for(int j = 0; j < i; j++){
        free(commands[j]);
    }
    free(commands);
    POP_TRACE();
}// end main
