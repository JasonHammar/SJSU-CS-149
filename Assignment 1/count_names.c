#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
/**
 * Description: This program will take a file given from the user, count the number of times a name is found in the file
 * and also output the error message as well when a line in the file is empty.
 * Author names: Jason Hammar, Ravi Gadgil
 * Author emails: jason.hammar@sjsu.edu, ravi.gadgil@sjsu.edu
 * Last modified date: 5 September 2022
 * Creation date: 5 September 2022
 *
 */


/**
 * This method will be responsible for reading all of the names in the text file, counting how many times a name appears,
 * and then printing it out on the screen. This method will also print out an error message explaining
 * that a line is empty.
 *
 */
int main(int argc, char *argv[]) {
    char names [100][30];
    char name[30];
    char namesUsed [100][30];
    int numNames = 0;

    /*if(argc == 1){
        return 0;
    }*/
    FILE *fp;
    fp = fopen("numbers.txt", "r");
    if(fp == NULL){
        printf("cannot open file\n");
        exit(1);
    }
    int i = 0;

    while(fgets(name,30, fp)){// This while loop will read in all of the names into an array.
        if(name[0] == '\n'){
            continue;
        }else{
            for(int j = 0; j < 30; j++){// This for loop reads in the names by reading each character.

                if(name[j] != '\n' && name[j] != NULL){
                    names[i][j] = name[j];

                }

            }
            numNames++;
        }

        if(name[0] == '\n'){// If the line is empty, an error message is printed out.
            fprintf(stderr, "Warning - Line %d is empty.\n", i + 1);
        }
        i++;

    }


    for(int j = 0; j < numNames; j++){
        // This for loop's overall responsibility is to print out the number of times
        // each name is found in the file/
        int counter = 0;

        if(j == 0){
            for(int k = 0; k < numNames; k++){// This for loop determines if the first name has any duplicates
                if(strcmp(names[j], names[k]) == 0){
                    counter++;
                }
            }
            for(int k = 0; k < 30; k++){// This loop puts the name that has been used into the used names category.
                namesUsed[j][k] = names[j][k];
                if(names[j][k] != NULL){
                    //fprintf(stdout, "%c", names[j][k]);
                }
            }

            fprintf(stdout, "%s: %d\n", names[j], counter);
        }else if(names[j][0] != '\n'){
            bool duplicate = false;
            for(int k = 0; k < numNames; k++){// This for loop checks if the name has already been counted.
                if(strcmp(names[j], namesUsed[k]) == 0){
                    duplicate = true;
                    break;
                }
            }
            if(duplicate == true){
                duplicate = false;
                continue;
            }
            for(int k = 0; k < numNames; k++){// This loop counts how many times the name is found in the file.
                if(strcmp(names[j], names[k]) == 0){
                    counter++;
                }
            }
            for(int k = 0; k < 30; k++){// Prints out the name
                namesUsed[j][k] = names[j][k];
                if(names[j][k] != '\n' && names[j][k] != NULL){
                    //fprintf(stdout, "%c", names[j][k]);
                }
            }

            fprintf(stdout, "%s: %d\n", names[j], counter);

        }
    }
    /*for(int j = 0; j < numNames; j++){
        printf("(%s)", names[j]);
    }*/

    return 0;
}
