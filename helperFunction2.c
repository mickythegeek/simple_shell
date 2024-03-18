#include "shell.h"


/**
 * funcExitStatus - Handle exit status in shell
 * @stat: Exit status value
 * @args: Array of command arguments
 * @token: Pointer to command token
 * @status: Pointer to status variable
 *
 * Description: This function handles the exit status in the shell.
 * It checks the exit status value and takes appropriate actions based on it.
 */


void funcExitStatus(int stat, char **args, char **token, int *status)
{
    if (stat == -1 || (stat == 0 && args[1][0] != '0') || stat < 0)
    {
        writeExitError(args[1]);
        *status = 2;
    }
    else
    {
        free(*token);
        freeArgs(args);
        exit(stat);
    }
}






/**
 * exitCustom - Handle custom exit in shell
 * @stat: Exit status value
 * @args: Array of command arguments
 * @prompt: Pointer to command prompt
 * @status: Pointer to status variable
 *
 * Description: This function handles a custom exit in the shell.
 * It checks the exit status value and takes appropriate actions based on it.
 */


void exitCustom(int stat, char **args, char *prompt, int *status)
{
    if (stat == -1 || (stat == 0 && args[1][0] != '0') || stat < 0)
    {
        writeExitError(args[1]);
        *status = 2;
    }
    else
    {
        free(prompt);
        freeArgs(args);
        exit(stat);
    }
}


/**
 * getPromptFail - Handle failure in getting command prompt
 * @prompt: Pointer to command prompt
 *
 * Description: This function handles
 * the failure in getting the command prompt.
 * It prints an error message, frees the
 * allocated memory, and exits with failure status.
 */


void getPromptFail(char *prompt)
{
    perror("getline");
    free(prompt);
    prompt = NULL;
    exit(EXIT_FAILURE);
}


/**
 * printEnviron - Print environment variables to standard output
 *
 * Description: This function prints environment
 * variables to the standard output.
 */
void printEnviron(void)
{
    char **env;


    env = environ;
    while (*env != NULL)
    {
        write(STDOUT_FILENO, *env, strlen(*env));
        write(STDOUT_FILENO, "\n", 1);
        env++;
    }
}
