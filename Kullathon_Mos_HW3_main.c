/**************************************************************
 * Class::  CSC-415-03 Spring 2024
 * Name::  Mos Kullathon
 * Student ID::  921425216
 * GitHub-Name::  mosguinz
 * Project::  Assignment 3 – Simple Shell with Pipes
 *
 * File::  Kullathon_Mos_HW3_main.c
 *
 * Description::  Emulate the functionality of a simple shell,
 *                including pipes.
 *
 **************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_INPUT_SIZE 177

/**
 * Tokenize provided input into an array of strings,
 * so that it may be provided to `execvp`.
 */
char **tokenize_command(char *cmd)
{
    int token_count = 0;

    // Remove newline character.
    if (cmd[strlen(cmd) - 1] == '\n')
    {
        cmd[strlen(cmd) - 1] = '\0';
    }

    // Create double pointer to store array of tokens.
    char **tokens = NULL;
    char *token = strtok(cmd, " ");
    while (token != NULL)
    {
        // For each token found, reallocate memory and save token.
        tokens = realloc(tokens, (token_count + 1) * sizeof(char *));
        tokens[token_count] = token;
        token_count++;
        token = strtok(NULL, " ");
    }
    // Reallocate memory once more to store the null element.
    tokens = realloc(tokens, (token_count + 1) * sizeof(char *));
    tokens[token_count] = NULL;

    return tokens;
}

/**
 * Helper to print process ID and display error message from `execvp`.
 */
void print_pid(pid_t pid, int status)
{
    printf("Process %d finished with %d\n", pid, WEXITSTATUS(status));
    if (errno = WEXITSTATUS(status))
    {
        perror("execvp");
    }
}

int main(int argc, char const *argv[])
{
    // Per requirement, display custom prompt, if provided.
    const char *prompt = argc > 1 ? argv[1] : "> ";

    while (1)
    {
        // Display prompt.
        printf("%s", prompt);
        char input[MAX_INPUT_SIZE];

        // Check for EOF. Exit shell if reached.
        if (!fgets(input, MAX_INPUT_SIZE, stdin))
        {
            printf("EOF reached, exiting\n");
            exit(0);
        }

        // Split the provided input on pipes.
        char *input_ptr = NULL;
        char *cmd = strtok_r(input, "|", &input_ptr);
        size_t length = strlen(cmd);

        // Once again, remove the newline character at the end.
        if (cmd[length - 1] == '\n')
        {
            cmd[length - 1] = '\0';
        }

        // Exit shell, when prompted.
        if (strcmp(input, "exit") == 0)
        {
            printf("Exiting...\n");
            exit(0);
        }

        // For each command found - i.e., for the number of pipes found.
        while (cmd != NULL)
        {
            // Tokenize the command, to be passed to `execvp`.
            char **tokens = tokenize_command(cmd);

            // Read in the next command to run.
            cmd = strtok_r(NULL, "|", &input_ptr);

            // If there is only one command to run, then no piping needed.
            if (cmd == NULL)
            {
                // Create a child process.
                pid_t pid = fork();
                int status;
                if (pid == 0)
                {
                    // In the child process, execute command and return.
                    int code = execvp(tokens[0], tokens);
                    exit(errno);
                }
                else
                {
                    // In the parent, wait the process to execute and print PID.
                    pid_t child_pid = wait(&status);
                    if (WIFEXITED(status))
                    {
                        print_pid(child_pid, status);
                    }
                }
            }
            else
            {
                // single pipe
                int fd[2];
                pipe(fd);

                pid_t pid = fork();
                int status;
                if (pid == 0)
                {
                    dup2(fd[1], STDOUT_FILENO);
                    close(fd[0]);
                    close(fd[1]);
                    execvp(tokens[0], tokens);
                    exit(errno);
                }
                else
                {
                    pid_t child_pid = wait(&status);
                    char **tokens = tokenize_command(cmd);

                    if (WIFEXITED(status))
                    {
                        print_pid(child_pid, status);
                    }

                    pid_t pid = fork();
                    int status;
                    if (pid == 0)
                    {
                        dup2(fd[0], STDIN_FILENO);
                        close(fd[0]);
                        close(fd[1]);
                        execvp(tokens[0], tokens);
                        exit(errno);
                    }
                    else
                    {
                        close(fd[0]);
                        close(fd[1]);
                        pid_t child_pid = wait(&status);
                        if (WIFEXITED(status))
                        {
                            print_pid(child_pid, status);
                        }
                    }
                }
            }
            cmd = strtok_r(NULL, "|", &input_ptr);
        }
    }
    return 0;
}
