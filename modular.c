#include "shell.h"


void startMyshell(void);
/**
 * startMyshell - Initiates the custom shell process.
 *
 * Description: This function starts the custom shell
 * It displays the shell prompt,
 * reads user input, processes commands, and executes them until the user exits
 * the shell. It handles built-in commands such as 'exit', 'env', 'cd',
 *'setenv',
 * and 'unsetenv',
 * as well as executing external commands using executeCommands().
 * It also manages changing directories, setting environment variables,
 * and displays
 * appropriate error messages.
 */
void startMyshell(void)
{
    char    *prompt = NULL, **args, *envp[] = {NULL};
    size_t  size_prompt;
    ssize_t numRead;
    int     stat;
    char    *previousDir;
    char    cwd[1024];


    size_prompt = 0;
    stat = 0;
    while (1)
    {


        write(STDOUT_FILENO, "myshell$ ", 11); /*Display the shell prompt*/


        numRead = getline(&prompt, &size_prompt, stdin); /*Read user input*/
        if (numRead == -1)
        {
            perror("getline");
            free(prompt);
            prompt = NULL;
            exit(EXIT_FAILURE);
        }


        args = funcTokenize(prompt, " \n\t");
        /*Tokenize user input into command arguments*/


        if (args[0])
        {
            if (!_strcmp(args[0], "exit"))
            {
                /* Handle 'exit' command */
                if (args[1])
                {
                    stat = _atoi(args[1]);
                }
                else
                {
                    free(prompt);
                    freeArgs(args);
                    exit(stat);
                }
            }
            else if (!_strcmp(args[0], "env"))
            {
                printEnviron();
                stat = 0;
            }
            else if (!_strcmp(args[0], "cd"))
            {
                /* Handle 'cd' command */
                if (args[1] == NULL)
                    /* No arguments provided, change to home directory */
                    chdir(_getenv("HOME"));
                else if (!_strcmp(args[1], "-"))
                {
                    /* Handle "cd -" to change to the previous directory */
                    previousDir = _getenv("OLDPWD");
                    if (previousDir)
                        chdir(previousDir);
                }
                else
                {
                    /* Change to the specified directory */
                    if (chdir(args[1]) != 0)
                        perror("cd");


                    else
                    {
                /* Update PWD environment variable */
                    if (getcwd(cwd, sizeof(cwd)) != NULL)
                        setenv("PWD", cwd, 1);
                    else
                        perror("getcwd");
                    }
                }
                stat = 0;
            }
            else if (!_strcmp(args[0], "setenv"))
            {
                /* Handle 'setenv' command */
                if (args[1] && args[2])
                {
                    setEnvironmentVariable(args[1], args[2]);
                    stat = 0;
                }
                else
                {
                    /* Invalid usage of 'setenv' command,display error message */
                    write(STDERR_FILENO, "Usage: setenv VARIABLE VALUE\n", 29);
                    stat = 1;
                }
            }
            else if (!_strcmp(args[0], "unsetenv"))
            {
                /* Handle 'unsetenv' command */
                if (args[1])
                {
                    unsetEnvironmentVariable(args[1]);
                    stat = 0;
                }
                else
                {
                    /* Invalid usage of 'unsetenv' command,display error message */
                    write(STDERR_FILENO, "Usage: unsetenv VARIABLE\n", 25);
                    stat = 1;
                }
            }
            else
            {
                /* Execute other commands */
                executeCommands(args, envp, &stat);
            }
        }
        /* Free allocated memory for arguments */
        freeArgs(args);
    }
}


void nonInteractMode(char *token, int *status);


/**
 * nonInteractMode - Executes shell commands,
 *  in non-interactive mode.
 * @token: User input token containing the command.
 * @status: Pointer to the status variable indicating,
 * the command's exit status.
 *
 * Description: This function handles shell commands
 * provided in non-interactive mode.
 * It tokenizes the input command, processes built-in functions
 * such as 'exit', 'env',
 * 'cd', 'setenv', and 'unsetenv', or executes
 * external commands using executeCommands().
 * The function also handles changing directories,
 *  setting environment variables, and
 * displaying appropriate error messages.
 */
void nonInteractMode(char *token, int *status)
{
    char    **single_command;
    char    *envp[] = {NULL};
    char    *previousDir;
    char    cwd[1024];


    token[_strlen(token) - 1] = '\0';
    single_command = funcTokenize(token, " \t");
    if (single_command[0])
    {
        if (!_strcmp(single_command[0], "exit"))
        {
            if (single_command[1])
            {
                *status = _atoi(single_command[1]);


            }
            free(token);
            freeArgs(single_command);
            exit(*status);
        }
        else if (!_strcmp(single_command[0], "env"))
        {
            printEnviron();
            *status = 0;
        }
        else if (!_strcmp(single_command[0], "cd"))
        {
            /* Handle cd command */
            if (single_command[1] == NULL)
            {
                /* No arguments provided, change to home directory */
                chdir(_getenv("HOME"));
            }
            else if (!_strcmp(single_command[1], "-"))
            {
                /* Handle "cd -" to change to the previous directory */
                previousDir = _getenv("OLDPWD");
                if (previousDir)
                {
                    chdir(previousDir);
                }
            }
            else
            {
                /* Change to the specified directory */
                if (chdir(single_command[1]) != 0)
                {
                    perror("cd");
                }
                else
                {
                    /* Update PWD environment variable */
                    if (getcwd(cwd, sizeof(cwd)) != NULL)
                    {
                        setenv("PWD", cwd, 1);
                    }
                    else
                    {
                        perror("getcwd");
                    }
                }
            }
            *status = 0; /* Set status to success for cd command */
        }
        else if (!_strcmp(single_command[0], "setenv"))
        {
            if (single_command[1] && single_command[2])
            {
                setEnvironmentVariable(single_command[1], single_command[2]);
                *status = 0;
            }
            else
            {
                write(STDERR_FILENO, "Usage: setenv VARIABLE\n", 29);
                *status = 1;
            }
        }
        else if (!_strcmp(single_command[0], "unsetenv"))
        {
            if (single_command[1])
            {
                unsetEnvironmentVariable(single_command[1]);
                *status = 0;
            }
            else
            {
                write(STDERR_FILENO, "Usage: unsetenv VARIABLE\n", 25);
                *status = 1;
            }
        }
        else
        {
            executeCommands(single_command, envp, status);
        }
    }
    freeArgs(single_command);
}


/**
 * setEnvironmentVariable -  Sets the specified
 * environment variable with a value.
 * @variable: Name of the environment variable to set.
 * @value: Value to assign to the environment variable.
 *
 * Description: Attempts to set the environment
 * variable specified by 'variable'
 * to the value specified by 'value'. Prints an
 * error message to stderr on failure.
 */
void setEnvironmentVariable(char *variable, char *value)
{
    const char  *error_message = "Failed to set environment variable: ";
    const char  *equals = "=";
    const char  *newline = "\n";


    if (setenv(variable, value, 1) != 0)
    {
        /*Write the error message to stderr character by character*/
        write(STDERR_FILENO, error_message, _strlen(error_message));
        write(STDERR_FILENO, variable, _strlen(variable));
        write(STDERR_FILENO, equals, _strlen(equals));
        write(STDERR_FILENO, value, _strlen(value));
        write(STDERR_FILENO, newline, _strlen(newline));
    }
}


/**
 * unsetEnvironmentVariable - Unsets the specified environment variable.
 * @variable: The name of the environment variable to be unset.
 *
 * Description: This function unsets the specified environment variable.
 * If unsetting the variable fails, it prints an error message to stderr.
 */


void unsetEnvironmentVariable(char *variable)
{
    const char  *error_message = "Failed to unset environment variable: ";
    const char  *newline = "\n";


    if (unsetenv(variable) != 0)
    {
        /* Write the error message to stderr character by character*/
        write(STDERR_FILENO, error_message, _strlen(error_message));
        write(STDERR_FILENO, variable, _strlen(variable));
        write(STDERR_FILENO, newline, _strlen(newline));
    }
}
