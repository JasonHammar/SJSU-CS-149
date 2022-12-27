#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 * Description: This program will take multiple files given from the user, count the number of times a name is found in each of the files and output that information to the screen.
 * The program will also output an error message when a line in a particular file is empty, and the file they were found in.
 * This program exits if the user does not give a path to a text file.
 * Author names: Jason Hammar, Ravi Gadgil
 * Author emails: jason.hammar@sjsu.edu, ravi.gadgil@sjsu.edu
 * Last modified date: 22 September 2022
 * Creation date: 19 September 2022
 *
 */



struct NameCounts {// This struct keeps track of each distinct name, and keeps track of each time that name appears in each file.
    char name[30];
    int count;
    int nunNames;// This variable keeps track of a total number of distinct elements in the file (The first element of the array of NameCounts does this.)
};

/**
* This main method is responsible for reading in more than one text file with an undisclosed number of names.
* If a line is empty, an stderr message is printed saying that a line in a certain text file is empty.
* If there are no files present or the main method can't find them, the program will exit.
* This method uses a data structure called nameCounter to keep track of the statistics of each individual file. (The statistics include each distinct name in the file, the number of times that distinct name appeared in the file, and the total number of distinct names in the file)
* This method uses a data structure called aggregateNameCount to keep track of the statistics across all files. (The statistics include each distinct name in all of the files, the number of times that distinct name appeared in all of the files, and the total number of distinct names in all of the files)
* This method uses fork() to start different child processes that collect the statistics on each particular file using the nameCounter data structure.
* This method uses pipe() and a pipe array to send the statistics on each particular file to the parent process, so the results/statistics of each individual file stored in the nameCounter data structure can be aggregated to the aggregatedNameCount data structure.
* This method uses wait() to enable the parent to wait for each child process to finish before aggregating the statistics of each file into the aggregatedCountNames data structure.
* Finally, the method prints out the contents of the aggregateNameCount data structure.
* Assumption: There are at most 100 different names with at most 30 characters. The input file used for testing won't have names of more than 25 chars length (According to the Professor).
* Input Parameters: argc, *argv
* Return type: Integer
**/

int main(int argc, char *argv[]) {
    int pfds[2];
    char name[30];
    struct NameCounts nameCounter[100];
    struct NameCounts aggregateNameCount[100];
    int lineCounter = 0;

    //This if statement checks if the user gave a text file. If not the program exits
    if(argc == 1){
        return 0;
    }

    // This for loop sets count in each element in the nameCounter data structure to 0.
    for (int j = 0; j < 100; j++) {
        nameCounter[j].count = 0;
    }
    nameCounter[0].nunNames = 0;

    pipe(pfds);

    // This for loop iterates through each file and uses a different child process for each file. During each iteration,
    // the statistics for each individual file are collated into the nameCounter data structure. After this,
    // the nameCounter data structure will be sent to the parent process via a pipe. Finally,
    // each individual child process for each individual file will exit to prevent the children from having more children.
    for(int i=1; i<argc; i++) {
        int childid = fork();

        // This if statement will check if childid equals 0. If it does, it will count the names in one of the specified files.
        if(childid==0) {
            FILE *fp;
            fp = fopen(argv[i], "r");

            // This checks if the file is null. If so, it tells the user that the file can't be opened and the program exits.
            if (fp == NULL) {
                printf("cannot open file\n");
                exit(1);
            }

            // This while loop will traverse through each line in the file. If it encounters a blank line, a stderr message will be printed.
            // Then the loop will check if there are any duplicate names. If the line being read in is a duplicate, the count for that particular name will be updated (each individual name refers to an index or an instance of NameCount struct in the nameCounter data structure).
            // If the line being read in is not a duplicate, a new index or NameCount struct in the nameCounter array will be used to track the new distinct name.
            // This while loop also keeps track of the total number of distinct names and the current line number for each individual file.
            while (fgets(name, 30, fp)) {
                bool duplicate = false;

                // This statement checks if a line is empty.
                // If so, an error message is printed stating that the line is empty, the line number that is empty, and the file in which the empty line was found in.
                if (name[0] == '\n' || name[1] == '\n') {
                    lineCounter++;
                    fprintf(stderr, "Warning - file %s line %d is empty.\n", argv[i], lineCounter);
                }

                    // If the above if statement is false, then the program checks for duplicates and counts how many times a name appears.
                    // The program will also check for newline characters and change them to null characters.
                    // If a duplicate is found, it will update the count variable of the element of the nameCounter data structure with the duplicate name to indicate that another instance of that name has been found.
                    // If there is no duplicate found, then that name is added to the nameCounter data structure and the count variable of that nameCounter is updated.
                else {

                    // This for loop changes new line characters into null characters.
                    for (int j = 0; j < 30; j++) {

                        // This if statement checks if a character equals a new line character.
                        // If it does, it is changed to a null character.
                        if (name[j] == '\n') {
                            name[j] = '\0';
                        }
                    }

                    // This for loop will go through the nameCounter data structure so it can check for duplicates.
                    for (int j = 0; j < nameCounter[0].nunNames; j++) {

                        // This if statement checks if a name has already been used.
                        // If it has been used, it will update the count variable of the element of the nameCounter data structure with the duplicate name.
                        // This if statement will also let the program know it was a duplicate and then move on.
                        if (strcmp(name, nameCounter[j].name) == 0) {
                            nameCounter[j].count++;
                            duplicate = true;
                            break;
                        }
                    }

                    // This if statement checks if the name has been labeled as a duplicate.
                    // If it is not a duplicate, it will update the count variable of the new element of the nameCounter data structure and put the new name in the nameCounter data structure.
                    if (!duplicate) {
                        nameCounter[nameCounter[0].nunNames].count++;

                        // This for loop reads in the names by reading each character, and putting them into the name variable in the nameCounter data structure.
                        for (int j = 0; j < 30; j++) {
                            nameCounter[nameCounter[0].nunNames].name[j] = name[j];
                        }
                        nameCounter[0].nunNames++;
                    }
                    lineCounter++;
                }
            }

            write(pfds[1], nameCounter, sizeof(nameCounter));

            exit(0);
        }
    }

    // The parent waits for each child to finish.
    // This while loop will also read from the child process into the currentNameCount, which represents the statistics for the current file.
    // From there, the while loop will go through the number of names and compare the names from the currentNameCount data structure to the aggregateNameCount data structure.
    // If a duplicate is found, it will add the count variable of the element in currentNameCount to the count variable of the element in aggregateNameCount.
    // If there is not a duplicate, the name in the element of aggregateNameCount is set equal to the name of the element in currentNameCount, as are the count variables. The numNames variable of the current element of the aggregate nameCount is incremented as well.
    while ((wait(NULL)) > 0) {
        struct NameCounts currentNameCount[100];

        read(pfds[0], currentNameCount, sizeof(currentNameCount));

        // This for loop will go through every element of the currentNameCount data structure to check if there are any duplicates.
        // The for loop will go through and compare the name in the currentNameCount element and the name in the aggregateNameElement.
        // If they are duplicates, the count variable of the element in currentNameCount is added to the count element in aggregateNameCount.
        // If there are no duplicates, the name in the element of aggregateNameCount is set equal to the name in the element of currentNameCount.
        for(int i=0; i<currentNameCount[0].nunNames; i++) {
            bool duplicate = false;

            // This for loop goes through each name in the aggregateNameCount data structure to check for duplicates.
            for(int j=0; j<aggregateNameCount[0].nunNames; j++) {

                // This if statement checks if the name variable in the element of currentNameCount is equal to the name variable in the element aggregatNameCount
                // If they are equal, the count variable of the element in the currentNameCount is added to the count variable of the element in the aggregateNameCount.
                if(strcmp(currentNameCount[i].name, aggregateNameCount[j].name)==0) {
                    duplicate = true;
                    aggregateNameCount[j].count+=currentNameCount[i].count;
                }
            }

            // This if statement checks if there are no duplicates
            // If there are no duplicates, the name variable of the element in aggregateNameCount is set equal to the name variable in currentNameCount.
            // The count variable of the element in aggregateNameCount is set equal to the count variable in currentNameCount.
            // The numNames variable of the element in aggregateNameCount is incremented by one.
            if(!duplicate) {

                // This for loop sets the name variable of the element in aggregateNameCount is set equal to the name variable in currentNameCount.
                for (int k = 0; k < 30; k++) {
                    aggregateNameCount[aggregateNameCount[0].nunNames].name[k] = currentNameCount[i].name[k];
                }
                aggregateNameCount[aggregateNameCount[0].nunNames].count = currentNameCount[i].count;
                aggregateNameCount[0].nunNames++;
            }
        }
    }

    // This for loop prints out all of the names and how many times those names appeared.
    for(int i=0; i<aggregateNameCount[0].nunNames; i++) {
        fprintf(stdout, "%s: %d\n", aggregateNameCount[i].name, aggregateNameCount[i].count);
    }

    return 0;
}
