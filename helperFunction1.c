#include "shell.h"


/**
 * executeCommands - Execute shell commands
 * @args: Array of command arguments
 * @envp: Array of environment variables
 * @stat: Pointer to status variable
 */
void    executeCommands(char **args, char **envp, int *stat)
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


void    waitChildprocess(int *stat)
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









File name : modular.c
Content :
#include "shell.h"


void    startMyshell(void);
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
void    startMyshell(void)
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
void    nonInteractMode(char *token, int *status)
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
void    setEnvironmentVariable(char *variable, char *value)
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


void    unsetEnvironmentVariable(char *variable)
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



File name : helperFunction2.c
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
