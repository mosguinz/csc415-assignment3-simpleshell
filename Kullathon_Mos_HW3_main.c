/**************************************************************
 * Class::  CSC-415-03 Spring 2024
 * Name::  Mos Kullathon
 * Student ID::  921425216
 * GitHub-Name::  mosguinz
 * Project::  Assignment 3 – Simple Shell with Pipes
 *
 * File::  Kullathon_Mos_HW3_main.c
 *
 * Description::  bruh
 *
 **************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 177

int main(int argc, char const *argv[])
{
    const char *prompt = argc > 1 ? argv[1] : "$ ";

    while (1)
    {
        printf("%s", prompt);
        char input[MAX_INPUT_SIZE];
        if (!fgets(input, MAX_INPUT_SIZE, stdin))
        {
            printf("EOF reached, exiting\n");
            exit(0);
        }

        char **tokens = NULL;
        int token_count = 0;
        size_t length = strlen(input);

        if (input[length - 1] == '\n')
        {
            input[length - 1] = '\0';
        }

        char *token = strtok(input, " ");
        while (token != NULL)
        {
            tokens = realloc(tokens, (token_count + 1) * sizeof(char *));
            tokens[token_count] = token;
            token_count++;
            token = strtok(NULL, " ");
        }
        tokens = realloc(tokens, (token_count + 1) * sizeof(char *));
        tokens[token_count] = NULL;

        if (strcmp(input, "exit") == 0)
        {
            exit(0);
        }

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
                printf("Process %d finished with %d\n", child_pid,
                       WEXITSTATUS(status));
            }
        }
    }
    return 0;
}
