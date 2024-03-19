#include "shell.h"

/**
 * readUserInput - Read user input from standard input
 * @args: Pointer to store the user input
 * @size_args: Pointer to the size of the input buffer
 *
 * Description: This function displays the shell prompt, reads user input from
 * standard input, and stores it in the provided buffer.
 *
 * Return: The number of characters read, or -1 on failure.
 */
ssize_t readUserInput(char **args, size_t *size_args)
{
	write(STDOUT_FILENO, "myshell$ ", 10);
	return (getline(args, size_args, stdin));
}

/**
 * writeError - Write error message for command not found
 * @input: Command that was not found
 *
 * Description: This function writes an error message to standard error output
 * when a command is not found.
 */

void writeError(char *input)
{
	write(STDERR_FILENO, "./hsh: 1: ", 10);
	write(STDERR_FILENO, input, _strlen(input));
	write(STDERR_FILENO, ": not found\n", 12);
}

/**
 * writeExitError - Write error message for illegal exit number
 * @nomber: Exit number that is not valid
 *
 * Description: This function writes an error message to standard error output
 * when an illegal exit number is provided.
 */

void writeExitError(char *nomber)
{
	write(STDERR_FILENO, "./hsh: 1: exit: Illegal number: ", 32);
	write(STDERR_FILENO, nomber, _strlen(nomber));
	write(STDERR_FILENO, "\n", 1);
}

/**
 * freeArgs - Free memory allocated for an array of strings
 * @args: Pointer to the array of strings to be freed
 *
 * Description: This function frees the memory
 * allocated for an array of strings.
 */
void freeArgs(char **args)
{
	int i;

	if (!args)
		return;
	for (i = 0; args[i] != NULL; i++)
	{
		free(args[i]);
	}
	free(args);
}
