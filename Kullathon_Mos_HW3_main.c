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

#define MAX_INPUT_SIZE 177

char **getTokens();

int main(int argc, char const *argv[])
{
    const char *prompt = argv[0];

    char input[MAX_INPUT_SIZE];
    fgets(input, MAX_INPUT_SIZE, stdin);

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

    for (int i = 0; i < token_count; i++)
    {
        printf("%d. %s\n", i, tokens[i]);
    }

    pid_t pid = fork();
    int code;
    if (pid == 0)
    {
        int code = execvp(tokens[0], tokens);
        free(tokens);
        printf("Something went wrong!\n");
    }
    else
    {
        printf("Done running!");
        return 1;
    }
    return 0;
}

char **getTokens()
{
    char input[MAX_INPUT_SIZE];
    fgets(input, MAX_INPUT_SIZE, stdin);

    char **tokens = NULL;
    int token_count = 0;

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
    return tokens;
}
