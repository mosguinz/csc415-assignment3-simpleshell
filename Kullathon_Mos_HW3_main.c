/**************************************************************
 * Class::  CSC-415-03 Spring 2024
 * Name::  Mos Kullathon
 * Student ID::  921425216
 * GitHub-Name::  mosguinz
 * Project::  Assignment 3 – Simple Shell with Pipes
 *
 * File::  Kullathon_Mos_HW3_main.c
 *
 * Description::  Emulate the functionality of a simple shell.
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

    if (cmd[strlen(cmd) - 1] == '\n')
    {
        cmd[strlen(cmd) - 1] = '\0';
    }

    char **tokens = NULL;
    char *token = strtok(cmd, " ");
    while (token != NULL)
    {
        tokens = realloc(tokens, (token_count + 1) * sizeof(char *));
        tokens[token_count] = token;
        (token_count)++;
        token = strtok(NULL, " ");
    }
    tokens = realloc(tokens, (token_count + 1) * sizeof(char *));
    tokens[token_count] = NULL;

    return tokens;
}

int main(int argc, char const *argv[])
{
    // Per requirement, display custom prompt, if provided.
    const char *prompt = argc > 1 ? argv[1] : "> ";

    while (1)
    {
        printf("%s", prompt);
        char input[MAX_INPUT_SIZE];
        if (!fgets(input, MAX_INPUT_SIZE, stdin))
        {
            printf("EOF reached, exiting\n");
            exit(0);
        }
        char *inputPtr = NULL;
        char *cmd = strtok_r(input, "|", &inputPtr);
        size_t length = strlen(cmd);
        if (cmd[length - 1] == '\n')
        {
            cmd[length - 1] = '\0';
        }

        if (strcmp(input, "exit") == 0)
        {
            printf("Exiting...\n");
            exit(0);
        }

        while (cmd != NULL)
        {
            char **tokens = tokenize_command(cmd);
            cmd = strtok_r(NULL, "|", &inputPtr);

            if (cmd == NULL)
            {
                // no pipes
                pid_t pid = fork();
                int status;
                if (pid == 0)
                {
                    int code = execvp(tokens[0], tokens);
                    free(tokens);
                    printf("Something went wrong!\n");
                }
                else
                {
                    pid_t child_pid = wait(&status);
                    if (WIFEXITED(status))
                    {
                        printf("Process %d finished with %d\n", child_pid, WEXITSTATUS(status));
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
                        printf("Process %d finished with %d\n", child_pid, WEXITSTATUS(status));
                        errno = WEXITSTATUS(status);
                        perror("execvp");
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
                            printf("Process %d finished with %d\n", child_pid, WEXITSTATUS(status));
                            errno = WEXITSTATUS(status);
                            perror("execvp");
                        }
                    }
                }
            }
            cmd = strtok_r(NULL, "|", &inputPtr);
        }
    }
    return 0;
}
