#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>
#include "tokens.h"
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>

#define MAXLINE 255 /* maximum size of a single line */

int strcmpIgnoreCase(char *a, char *b) {
    for (; *a != '\0' && *b != '\0'; a++, b++) {
        if (tolower(*a) != tolower(*b)) {
            return tolower(*a) < tolower(*b) ? -1 : 1;
        }
    }
    return *a == '\0' && *b == '\0' ? 0 : *a == '\0' ? -1 : 1;
}


static void processCommand(char input[], int *argc, char *argv[], char** file_input, char** file_output, int *pipe, int *bg) {
    fgets(input, MAXLINE, stdin);

    char **tokens = get_tokens(input);

    char **current = tokens;

    assert(tokens != NULL);

    int i = 0;

    while (*current != NULL) {
        if (strcmp(*current, "|") == 0) {
            *pipe = 1;
        } else if (strcmp(*current, "<") == 0) {
	    ++current;
            *file_input = *current;
        } else if (strcmp(*current, ">") == 0) {
	    ++current;
            *file_output = *current;
        } else if (strcmpIgnoreCase(*current, ";") == 0) {
            *bg = 1;
            break;
        } else {
            argv[i++] = *current;
        }
        ++current;
    }
    argv[i] = NULL;
    *argc = i;
    //free_tokens(tokens);
}

void exec(int argc, char *argv[], char *file_input, char *file_output, int isPipe, char *prevCommand) {
    int file_des;
    int pipe_fds[2];
    pid_t child1, child2;
    int read_fd = pipe_fds[0];
    int write_fd = pipe_fds[1];


    if (isPipe) {
        if (pipe(pipe_fds) == - 1) {
            perror("pipe error.");
            exit(1);
        }
    }

    child1 = fork();

    if (child1 == -1) {
        perror("fork error.");
        exit(1);
    } else if (child1 > 0 && isPipe) {
        child2 = fork();
    }

    if (child1 == 0) { // in the first child
        if (isPipe) { // if pipe is required
            if (close(STDOUT_FILENO) == -1) {
                perror("Error closing stdout.");
            }

            file_des = dup(pipe_fds[write_fd]);

            if (file_des == -1) {
                perror("dup write error.");
            } else if (file_des != STDOUT_FILENO) {
                perror("Write pipe not set correctly.");
            }

            close(pipe_fds[read_fd]);
            close(pipe_fds[write_fd]);

            argv[1] = NULL;

        } else if (file_input != NULL) { // if there is a file input
            if (close(STDIN_FILENO) == -1) {
                perror("Error closing stdin.");
            }

            file_des = open(file_input, O_RDONLY);

            if (file_des == -1) {
                perror("Error reading file input.");
            }
        } else if (file_output != NULL) { // if there is a file output
            if (close(STDOUT_FILENO) == -1) {
                perror("Error closing stdout.");
            }

            file_des = open(file_output, O_WRONLY | O_CREAT | O_TRUNC, 0666);

            if (file_des == -1) {
                perror("Error writing file output.");
            }
        }

	if (strcmp(argv[0], "prev") == 0) {
            printf("Previous command: %s\n", prevCommand);
            execvp(prevCommand, argv);
        } else if (strcmp(argv[0], "cd") == 0) {
            if (chdir(argv[1]) == -1) {
                perror("Error changing directory.");
            }
        } else if (strcmp(argv[0], "help") == 0) {
	    printf("Help:\n");
	    printf("quit/exit - to quit \n");
	    printf("cd - change directory \n");
	    printf("prev - prints previous command and executes it. \n");
	    printf("source - executes a script \n");
        } else if (strcmp(argv[0], "source") == 0) {
            FILE *fp = fopen(argv[1], "r");
            char line[255];
            while (fgets(line, sizeof(line), fp)) {
                execvp(line, argv);
            }
        } else if (execvp(argv[0], argv) == -1) { // execute command
            printf("[%s]: command not found\n", argv[0]);
            exit(1);
        }
    } else if (child2 == 0 && isPipe) { // in second child
	if (close(STDIN_FILENO) == -1) {
            perror("Error closing stdin.");
        }

        file_des = dup(pipe_fds[read_fd]);

        if (file_des == -1) {
            perror("dup read error.");
        } else if (file_des != STDIN_FILENO) {
            perror("Read pipe not set correctly.");
        }

        close(pipe_fds[read_fd]);
        close(pipe_fds[write_fd]);

        argv[0] = argv[1];
        argv[1] = NULL;

        if (strcmp(argv[0], "prev") == 0) {
            printf("Previous command: %s\n", prevCommand);
            execvp(prevCommand, argv);
        } else if (strcmp(argv[0], "cd") == 0) {
            if (chdir(argv[1]) == -1) {
                perror("Error changing directory.");
            }
	} else if (strcmp(argv[0], "help") == 0) {
            printf("Help:\n");
            printf("quit/exit - to quit \n");
            printf("cd - change directory \n");
            printf("prev - prints previous command and executes it. \n");
            printf("source - executes a script \n");
        } else if (strcmp(argv[0], "source") == 0) {
            FILE *fp = fopen(argv[1], "r");
            char line[255];
            while (fgets(line, sizeof(line), fp)) {
                execvp(line, argv);
            }
	} else if (execvp(argv[0], argv) == -1) {
            printf("[%s]: command not found\n", argv[0]);
            exit(1);
        }
    } else { // in parent
        if (waitpid(child1, NULL, 0) == -1) {
            perror("Error waiting for child 1.");
        }

        if (isPipe) {
            close(pipe_fds[read_fd]);
            close(pipe_fds[write_fd]);

            if (waitpid(child2, NULL, 0) == -1) {
              perror("Error waiting for child 2.");
            }
        }
    }
    return;
}


int main(int argc, char *argv[]) {
    printf("Welcome to mini-shell.\n");

    char input[MAXLINE];

    int background = 0;
    int pipe = 0;
    pid_t backgroundPID;
    char* file_input = NULL;
    char* file_output = NULL;
    char* processargv[20];
    char* prevCommand = "";
    int processargc;


    while(1) {
        printf("shell $ ");

        fflush(stdout);

        processCommand(input, &processargc, processargv, &file_input, &file_output, &pipe, &background);

        if (processargc > 0 && (strcmpIgnoreCase(processargv[0], "exit") == 0 || strcmpIgnoreCase(processargv[0], "quit") == 0)) {
	    printf("Bye bye.\n");
            exit(0);
        } else if (processargc > 0) {
            if (background) {
                backgroundPID = fork();

                if (backgroundPID == 0) {
                    exec(processargc, processargv, file_input, file_output, pipe, prevCommand);
		    prevCommand = processargv[0];
                    break;
                }
            } else {
                exec(processargc, processargv, file_input, file_output, pipe, prevCommand);
		prevCommand = processargv[0];
                file_input = NULL;
                file_output = NULL;
                pipe = 0;
                background = 0;
            }
        }
    }

  return 0;
}




