#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CMDLINE_MAX 512
#define ARG_MAX 16
#define PATH_MAX 1024

struct Command {
        char *args[ARG_MAX];
        int numArgs;
};

void changeDir(struct Command *command) {
        int errorCode = chdir(command->args[1]);
        if (errorCode != 0) {
                perror("cd failed\n");
        }
}

void printWorkingDir() {
        char currentWorkingDir[PATH_MAX];
        char* output = getcwd(currentWorkingDir, sizeof(currentWorkingDir));
        if (output != NULL) {
                printf("%s\n", output);
        }
}

int executeBuiltInCommand(struct Command* command) {
        if (strcmp(command->args[0], "cd") == 0) {
                changeDir(command);
                return 0;
        }
        else if (strcmp(command->args[0], "pwd") == 0) {
                printWorkingDir();
                return 0;
        }
        else {
                return 1;
        }
}

void execute(struct Command *command) {
        if (executeBuiltInCommand(command) == 0) {
                return;
        }
        pid_t pid;
        pid = fork();
        // Execute the command if we are the child process
        if (pid == 0) {
                execvp(command->args[0],command->args);
                exit(1);
        }
        // Print our completion message if we are the parent process
        else if (pid > 0) {
                int status;
                waitpid(pid, &status, 0);
                fprintf(stderr, "Completed '%s' [%d]\n", command->args[0], WEXITSTATUS(status));
        }
        // Simply exit if there was a forking error 
        else {
                exit(1);
        }
}

void parseInput(char *cmd, struct Command *command) {
        int argCount = 0;
        char *token = strtok(cmd, " ");
        if (token != NULL) {
                command -> args[argCount] = token;
        }
        while (token != NULL) {
                token = strtok(NULL, " ");
                argCount++;
                command->args[argCount] = token;
        }
        command->numArgs = argCount;
}


int main(void)
{
        char cmd[CMDLINE_MAX];

        while (1) {
                char *nl;

                /* Print prompt */
                printf("sshell$ ");
                fflush(stdout);

                /* Get command line */
                fgets(cmd, CMDLINE_MAX, stdin);

                /* Print command line if stdin is not provided by terminal */
                if (!isatty(STDIN_FILENO)) {
                        printf("%s", cmd);
                        fflush(stdout);
                }

                /* Remove trailing newline from command line */
                nl = strchr(cmd, '\n');
                if (nl)
                        *nl = '\0';

                /* Handle exit */
                if (!strcmp(cmd, "exit")) {
                        fprintf(stderr, "Completed '%s' [%d]\n", cmd, 0);
                        fprintf(stderr, "Bye...\n");
                        break;
                }

                /* Regular command */
                struct Command command;
                parseInput(cmd, &command);
                execute(&command);
        }

        return EXIT_SUCCESS;
}
