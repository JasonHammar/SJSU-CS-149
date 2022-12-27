#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
/**
 * Description: This program will read in commands given by the user, and it will then execute those commands by calling execvp.
 * If the command is valid and the arguments are valid, they will be executed and the output,
 * as well at the pid and parent pid will be as well as the command number will be written to an output file which is named after the child pid.
 * If the command is invalid or the arguments are invalid, then an error message is sent to the error file detailing what the error was.
 * If the program exited, whether it exited with 0 or a number other than 0, then the exit code will be written to the error file.
 * Otherwise, if the process was killed by a signal, then the termination signal will be sent to the error file.
 * The error files will also be named with PID.err, just like the PID.out files.
 * Author names: Jason Hammar, Ravi Gadgil
 * Author emails: jason.hammar@sjsu.edu, ravi.gadgil@sjsu.edu
 * Last modified date: 12 October 2022
 * Creation date: 5 October 2022
 *
 */

 // This variable is here to store the command number.
static int commandNumber = 0;


/**
* This main is responsible for reading in commands from the user, which is from the standard input line.
* The main method will be reading in an undisclosed amount of commands and arguments from the user.
* The main method will read the commands and arguments into a buffer and they will be separated into tokens.
* A child process is then created using fork(), which will help to execute new commands given by the user with every new process.
* It will check if the child_id variable is equal to 0, if so, the last character is set to 0.
* It will check for the number of tokens, and if the number of tokens is greater than or equal to 1,
* then the method will create an argument list.
* A File Pointer will then be used to create a new file where the output of execvp will be written to.
* From there a file descriptor is used to give execvp the ability to be written to the .out file.
* The .out file will be titles PID.out with PID being the process ID of the child.
* There will also be another file descriptor made which will be used to write an error message to an error file.
* The error file will be titled PID.err, with PID being the Process ID of the child.
* If execvp is successful, it's output will be put in the PID.out file.
* If execvp is not successful, then an error message will be sent to the PID.err file.
* If an invalid command is given, then the method exits with exit code 2. If there are no errors, it will exit with 0.
* If there are no commands given, either the user returns or enters a whitespace, the main method will exit with code 0, and put the command number in the PID.out file.
* The parent process will wait for the child to terminate, and then the parent will use file descriptors writing to the output file that the process is finished.
* The parent will also write to the error files the exit code of the child process, or the termination signal if the process was killed by a signal.
* Assumption: The user will enter at most 30 characters.
* Input Parameters: none
* Return type: Integer
**/
int main(void)
{
    char buf[30];

    int numTokens = 0;
    int childPid;
    int status;
    int fileDesc = 0;
    char* token;
    char* command;
    char* fileName = "";
    FILE* fp;
    char* tokens[30];

    /*
    * This while loop is primarily responsible for reading in commands and arguments from the user and then executing them through execvp.
    * Every time the program goes through this while loop, a new child process is created, as are an output file and an error file.
    * This while loop will also increment commandNumber every time it's called and checks to make sure that child_id equals 0.
    * If child_id does equal 0, it will separate the command and arguments given by the user into tokens, and then count how many tokens there are.
    * If there are one or more tokens, the while loop will then execute the command and arguments. If there are no errors, the output is put into a PID.out file.
    * If there is an error, the error is written to a PID.err file and exit with exit code 2.
    * If there are no tokens, the while loop will then write the command number and PID and PPID to the output file and exit with 0
    */

    while (fgets(buf, 30, stdin) != NULL) {
        commandNumber++;
        int child_id = fork();

        /*
        * This if statement checks if the child_id variable is equal to 0.
        * If it is, it will then set the end of buf to 0 if it's equal to the newline character.
        * Afterwards, it will separate the command and arguments given by the user into tokens. The number of tokens will then be checked.
        * If there are one or more tokens, the commands and arguments will be executed through execvp. If there are no errors, the output is put into a PID.out file.
        * If there is an error, the error is written to a PID.err file and the process will exit with exit code 2.
        * If there are no tokens, the program will then write the command number and PID and PPID to the output file and exit with 0
        */
        if(child_id == 0) {

            //This if statement will check if the las character in buf is a newline character. If so, the last characters is set equal to 0.
            if (buf[strlen(buf) - 1] == '\n') {
                buf[strlen(buf) - 1] = 0;
            }

            token = strtok(buf, " ");

            //This while loop will put the command and arguments into the tokens array until token ids null. numTokens is incremented each iteration through.
            // The command and arguments are also separated by Whitespace as well.
            while(token != NULL) {
                tokens[numTokens++] = token;
                token = strtok(NULL, " ");
            }
            command = tokens[0];

            /*
            * This if statement checks if the number of tokens are greater than or equal to one.
            * If that is the case, the program then puts the tokens into an argument list so that they can be accessed and executed later.
            * The process id is the stored in the pid variable using getpid(). The variable mypid is then used to store the pid in string format by using sprintf().
            * The file name is generated by setting fileName to the concatenation of mypid and ".out".
            * A file pointer and a file descriptor are then opened using the file name generated so that the file can be written to, appended, and created.
            * Using fprintf() the command number, the process id and the parent process id are written to the PID.out file.
            * The file pointer and file descriptor are then closed.
            * The file pointer and file descriptor are opened up again using the file name generated so that the file can be written to, appended, and created. This will be used to create the PID.err file.
            * The file pointer and file descriptor are then closed.
            * After that execvp is called, and if it is successful, basically executes without errors, then the output of execvp is written to the PID.out file and the process exits with 0.
            * If there is an error however, the error message is written to the PID.err file and the process exits with 2 if the command is invalid, one otherwise.
            *
            * If numTokens is not greater than or equal to 1,
            * then the process id is the stored in the pid variable using getpid(). The variable mypid is then used to store the pid in string format by using sprintf().
            * The file name is generated by setting fileName to the concatenation of mypid and ".out".
            * A file pointer and a file descriptor are then opened using the file name generated so that the file can be written to, appended, and created.
            * Using fprintf() the command number, the process id and the parent process id are written to the PID.out file.
            * The file pointer and file descriptor are then closed and the process exits with exitcode 0.
            */
            if(numTokens >= 1) {
                char* argumentList[numTokens + 1];

                // This for loop puts the command and arguments into the argument list to be used later.
                for(int i = 0; i < numTokens; i++) {
                    argumentList[i] = tokens[i];
                }

                argumentList[numTokens] = NULL;

                int pid = getpid();
                char* mypid = malloc(15);
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
            }
            else {
                int pid = getpid();
                char* mypid = malloc(15);
                sprintf(mypid, "%d", pid);

                fileName = strcat(mypid, ".out");
                fp = fopen(fileName, "w");
                fileDesc = open(fileName, O_WRONLY | O_APPEND, O_CREAT);

                dup2(fileDesc, 1);
                fprintf(fp, "Starting command %d: child %d pid of parent %d\n", commandNumber++, getpid(), getppid());

                fflush(fp);
                close(fileDesc);
                fclose(fp);
            }
            exit(0);
        }
    }

    /*
    * This while loop represents the parent process, as it calls wait until the child process has terminated.
    * This loop will open the file pointer and file descriptor up again using the child_pid variable and sprintf(). The file is set to append instead of write so the file isn't overwritten.
    * Once they are opened, the message "Finished child PID pid of parent PPID" will be appended to the PID.out file, with the PID being the process id and PPID being parent process id.
    * The file pointer and file descriptor are then closed.
    * Afterwards, the loop opens up the file pointer and descriptor again but this time for the PID.err file.
    * The program then checks if the child process exited normally or was killed by a signal.
    * If the child process exited, then the exit code is appended to the PID.err file.
    * otherwise, if the child process was killed, the kill signal is appended to the PID.err file.
    */
    while((childPid = wait(&status)) > 0) {
        int pid = childPid;
        char* mypid = malloc(15);
        sprintf(mypid, "%d", pid);

        fileName = strcat(mypid, ".out");
        fp = fopen(fileName, "a");
        fileDesc = open(fileName, O_WRONLY | O_APPEND);

        dup2(fileDesc, 1);
        fprintf(fp, "Finished child %d pid of parent %d\n", pid, getpid());

        close(fileDesc);
        fclose(fp);

        sprintf(mypid, "%d", pid);
        fileName = strcat(mypid, ".err");
        fp = fopen(fileName, "a");
        fileDesc = open(fileName, O_WRONLY | O_APPEND);

        dup2(fileDesc, 2);

        // This if statement checks if the child process exited. If so, the exit code is appended to the PID.err file.
        // Otherwise, if the child process was killed, then the signal it was killed with is appended to the PID.err file.
        if(WIFEXITED(status)) {
            fprintf(fp, "Exited with exitcode = %d\n", WEXITSTATUS(status));
        }else if(WIFSIGNALED(status)){
            fprintf(fp, "Killed with signal %d", WTERMSIG(status));
        }

        close(fileDesc);
        fclose(fp);
    }
}
