/**
 * @file UVash.c
 * @brief A minimalist Unix shell written in C supporting parallel execution and redirection.
 * * Features:
 * - Interactive and batch modes.
 * - Built-in commands: 'cd' and 'exit'.
 * - Output redirection using '>'.
 * - Parallel command execution using '&'.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>

#define MAX_COMMANDS 100
#define MAX_ARGS 100

/**
 * @brief Prints the standard uniform error message to stderr.
 */
void print_error(void) {
    char error_message[30] = "An error has occurred\n";
    fprintf(stderr, "%s", error_message);
}

/**
 * @brief Removes leading and trailing whitespace from a string via pointer manipulation.
 * @param str The string to trim.
 * @return A pointer to the first non-whitespace character.
 */
char *trim_whitespace(char *str) {
    if (str == NULL) return NULL;

    // Trim leading space
    while (isspace((unsigned char)*str)) {
        str++;
    }

    if (*str == '\0') return str;

    // Trim trailing space
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }
    
    end[1] = '\0';
    return str;
}

/**
 * @brief Parses and validates output redirection for a command.
 * @param command The raw command string.
 * @param outfile Pointer to store the extracted output filename.
 * @return 0 on success, -1 if syntax is invalid.
 */
int parse_redirection(char *command, char **outfile) {
    char *redir_ptr = strchr(command, '>');
    *outfile = NULL;

    if (redir_ptr != NULL) {
        // Multiple '>' operators are not allowed
        if (strchr(redir_ptr + 1, '>') != NULL) {
            return -1;
        }

        // Split command and filename
        *redir_ptr = '\0';
        redir_ptr++;
        
        char *file_str = trim_whitespace(redir_ptr);
        
        // Ensure the filename doesn't contain internal spaces (e.g., "> file1 file2")
        for (int i = 0; file_str[i] != '\0'; i++) {
            if (isspace((unsigned char)file_str[i])) {
                return -1;
            }
        }

        if (file_str[0] == '\0') {
            return -1; // Missing filename after '>'
        }

        *outfile = file_str;
    }
    return 0;
}

/**
 * @brief Executes a single command (either built-in or external).
 * @param command The parsed, trimmed command string.
 * @param line_buffer Reference to the original line buffer (for safe freeing on exit).
 * @return The PID of the child process if forked, or 0 if built-in/empty.
 */
pid_t execute_command(char *command, char *line_buffer) {
    if (command[0] == '>') {
        print_error();
        return 0;
    }

    char *outfile = NULL;
    if (parse_redirection(command, &outfile) < 0) {
        print_error();
        return 0;
    }

    // Tokenize command into arguments
    char *args[MAX_ARGS];
    int arg_count = 0;
    char *arg = strtok(command, " \t");
    
    while (arg != NULL && arg_count < MAX_ARGS - 1) {
        args[arg_count++] = arg;
        arg = strtok(NULL, " \t");
    }
    args[arg_count] = NULL;

    if (arg_count == 0) return 0;

    // --- Built-in Commands ---
    if (strcmp(args[0], "exit") == 0) {
        if (arg_count != 1) {
            print_error();
        } else {
            free(line_buffer);
            exit(0);
        }
        return 0;
    }

    if (strcmp(args[0], "cd") == 0) {
        if (arg_count != 2 || chdir(args[1]) != 0) {
            print_error();
        }
        return 0;
    }

    // --- External Commands (Process Creation) ---
    pid_t pid = fork();
    if (pid < 0) {
        print_error();
        return 0;
    } else if (pid == 0) { // Child process
        if (outfile != NULL) {
            int fout = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (fout < 0 || dup2(fout, STDOUT_FILENO) < 0 || dup2(fout, STDERR_FILENO) < 0) {
                print_error();
                exit(1);
            }
            close(fout);
        }

        execvp(args[0], args);
        
        // execvp only returns if it fails
        print_error();
        exit(1);
    }

    // Parent process returns the child's PID
    return pid;
}

int main(int argc, char *argv[]) {
    FILE *input = NULL;
    char *line = NULL;
    size_t len = 0;

    if (argc > 2) {
        print_error();
        exit(1);
    }

    if (argc == 2) {
        input = fopen(argv[1], "r");
        if (input == NULL) {
            print_error();
            exit(1);
        }
    } else {
        input = stdin;
    }

    while (1) {
        if (input == stdin) {
            printf("UVash> ");
            fflush(stdout);
        }

        ssize_t read_bytes = getline(&line, &len, input);
        if (read_bytes == -1) {
            break; // EOF reached
        }

        char *trimmed_line = trim_whitespace(line);
        if (trimmed_line[0] == '&') {
            print_error();
            continue;
        }

        // Parse parallel commands separated by '&'
        char *commands[MAX_COMMANDS];
        int commands_count = 0;
        char *divided_command = strtok(trimmed_line, "&");

        while (divided_command != NULL && commands_count < MAX_COMMANDS) {
            commands[commands_count++] = divided_command;
            divided_command = strtok(NULL, "&");
        }

        pid_t child_pids[MAX_COMMANDS];
        int child_count = 0;

        // Execute commands concurrently
        for (int i = 0; i < commands_count; i++) {
            char *command = trim_whitespace(commands[i]);
            if (strlen(command) == 0) continue;

            pid_t pid = execute_command(command, line);
            if (pid > 0) {
                child_pids[child_count++] = pid;
            }
        }

        // Parent process waits for all actively spawned children in this line
        for (int i = 0; i < child_count; i++) {
            int status;
            waitpid(child_pids[i], &status, 0);
        }
    }

    free(line);
    if (input != stdin) fclose(input);
    return 0;
}