#include "shell.h"

/**
 * main - Entry point of the shell program
 *
 * Description: This function is the entry point of the shell program.
 * It reads user input and processes the commands either interactively
 * or in non-interactive mode based on whether the standard input is a TTY.
 *
 * Return: Always 0 to indicate successful execution.
 */
int main(void)
{
	size_t size_prompt;

	char *prompt;

	int status;

	size_prompt = 0;

	prompt = NULL;

	status = 0;

	if (!isatty(0))
	{
		while (getline(&prompt, &size_prompt, stdin) != -1)
		{
			nonInteractMode(prompt, &status);
		}
		if (prompt)
		{
			free(prompt);
			prompt = NULL;
		}
		return (status);
	}
	startMyshell();
	return (0);
}
