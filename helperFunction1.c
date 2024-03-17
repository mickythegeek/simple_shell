#include "shell.h"


/**
 * executeCommands - Execute shell commands
 * @args: Array of command arguments
 * @envp: Array of environment variables
 * @stat: Pointer to status variable
 */
void executeCommands(char **args, char **envp, int *stat)
{
    char    *full_path;
    int     pid;


    full_path = NULL;
    if (access(args[0], X_OK) == 0)
    {
        pid = fork();
        if (pid == 0)
            execve(args[0], args, envp);
        else
            waitChildprocess(stat);
    }
    else if (findFullPath(args[0], &full_path))
    {
        pid = fork();
        if (pid == 0)
            execve(full_path, args, envp);
        else
            waitChildprocess(stat);
        free(full_path);
    }
    else
    {
        *stat = 127;
        writeError(args[0]);
    };
}
/**
 * findFullPath - Find the full path of a command
 * @prompt: Command prompt
 * @full_path: Pointer to store full path
 * Return: 1 if found, 0 otherwise
 */


int findFullPath(char *prompt, char **full_path)
{
    int     found;
    char    *path_env_copy;


    char *token, *path_env;


    found = 0;
    path_env = _getenv("PATH");
    if (path_env != NULL)
    {
        path_env_copy = strdup(path_env);
        token = strtok(path_env_copy, ":");
        while (token != NULL && !found)
        {
            *full_path = malloc(strlen(token) + strlen(prompt) + 2);
            if (*full_path != NULL)
            {
                strcpy(*full_path, token);
                strcat(*full_path, "/");
                strcat(*full_path, prompt);
                if (access(*full_path, X_OK) == 0)
                {
                    found = 1;
                }
                if (!found)
                    free(*full_path);
            }
            token = strtok(NULL, ":");
        }
        free(path_env_copy);
    }
    return (found);
}


/**
 * waitChildprocess - Wait for child process to complete
 * @stat: Pointer to status variable
 */


void waitChildprocess(int *stat)
{
    int child_status;


    if (wait(&child_status) == -1)
    {
        perror("wait");
        exit(EXIT_FAILURE);
    }
    if (WIFEXITED(child_status))
    {
        *stat = WEXITSTATUS(child_status);
    }
}




/**
 * funcTokenize - Tokenize input string based on delimiters
 * @str: Input string
 * @delim: Delimiter string
 * Return: Array of tokens
 */
char    **funcTokenize(char *str, char *delim)
{
    int     amount;
    char    *token;
    char    **result;


    amount = 0;
    result = malloc(20 * sizeof(char *));
    if (result == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    token = strtok(str, delim);
    while (token != NULL)
    {
        result[amount] = _strdup(token);
        amount++;
        token = strtok(NULL, delim);
    }
    while (amount < 20)
    {
        result[amount] = NULL;
        amount++;
    }
    return (result);
}
