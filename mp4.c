#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>

#define MAX_INPUT 1024
#define MAX_ARGS  64

void clear_screen() {
    printf("\033[H\033[J"); // ANSI escape code for clearing screen
}

void print_help() {
    printf("Available commands:\n");
    printf("dir              - List files and directories\n");
    printf("cd <dir>         - Change directory\n");
    printf("cd..             - Move to parent directory\n");
    printf("mkdir <dir>      - Create new directory\n");
    printf("rmdir <dir>      - Remove empty directory\n");
    printf("del <file>       - Delete a file\n");
    printf("touch <file>     - Create a new file\n");
    printf("type <file>      - Show content of a file\n");
    printf("help             - Show help\n");
    printf("cls              - Clear screen\n");
    printf("date             - Display or modify date\n");
    printf("time             - Display or modify time\n");
    printf("exit             - Exit the shell\n");
}

void list_dir() {
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            printf("%s\n", dir->d_name);
        }
        closedir(d);
    } else {
        perror("opendir");
    }
}

void change_dir(char *path) {
    if (chdir(path) != 0) {
        perror("cd");
    } else {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            char *last = strrchr(cwd, '/');
        }
    }
}

void make_dir(char *name) {
    if (mkdir(name) != 0)
        perror("mkdir");
}

int is_directory_empty(const char *dirname) {
    int n = 0;
    struct dirent *d;
    DIR *dir = opendir(dirname);
    if (dir == NULL) return 1;
    while ((d = readdir(dir)) != NULL) {
        if (++n > 2) break;
    }
    closedir(dir);
    return (n <= 2);
}

void remove_dir(char *name) {
    if (!is_directory_empty(name)) {
        printf("rmdir: Directory not empty\n");
        return;
    }
    if (rmdir(name) != 0)
        perror("rmdir");
}


void delete_file(char *name) {
    if (unlink(name) != 0)
        perror("del");
}

void create_file(char *name) {
    int fd = open(name, O_CREAT | O_WRONLY, 0644);
    if (fd == -1) {
        perror("touch");
    } else {
        close(fd);
    }
}

void show_file(char *name) {
    FILE *file = fopen(name, "r");
    if (!file) {
        perror("type");
        return;
    }

    char ch;
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }
    fclose(file);
}

void display_date() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("Current date: %04d-%02d-%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

void display_time() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("Current time: %02d:%02d:%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
}

void set_date(char *new_date) {
    // Format: YYYY-MM-DD
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "sudo date -s \"%s\"", new_date);
    int ret = system(cmd);
    if (ret != 0) {
        perror("Failed to set date");
    }
}

void set_time_cmd(char *new_time) {
    // Format: HH:MM:SS
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "sudo date -s \"%s\"", new_time);
    int ret = system(cmd);
    if (ret != 0) {
        perror("Failed to set time");
    }
}


int main() {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];

    while (1) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            // Find last component of the path
            char *dir_name = strrchr(cwd, '/');  // for Unix-style paths
            if (dir_name)
                dir_name++;  // skip the slash
            else
                dir_name = cwd;  // fallback

            printf("<myshell/%s> ", dir_name);
        } else {
            perror("getcwd");
            printf("<myshell> ");
        }
        fgets(input, MAX_INPUT, stdin);
        input[strcspn(input, "\n")] = 0; // remove newline

        // Tokenize input
        int argc = 0;
        char *token = strtok(input, " ");
        while (token != NULL && argc < MAX_ARGS - 1) {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }
        args[argc] = NULL;

        // Handle empty input
        if (argc == 0) continue;

        // Command execution
        if (strcmp(args[0], "exit") == 0) break;
        else if (strcmp(args[0], "dir") == 0) list_dir();
        else if (strcmp(args[0], "cd") == 0 && argc > 1) change_dir(args[1]);
        else if (strcmp(args[0], "cd..") == 0) change_dir("..");
        else if (strcmp(args[0], "mkdir") == 0 && argc > 1) make_dir(args[1]);
        else if (strcmp(args[0], "rmdir") == 0 && argc > 1) remove_dir(args[1]);
        else if (strcmp(args[0], "del") == 0 && argc > 1) delete_file(args[1]);
        else if (strcmp(args[0], "touch") == 0 && argc > 1) create_file(args[1]);
        else if (strcmp(args[0], "type") == 0 && argc > 1) show_file(args[1]);
        else if (strcmp(args[0], "help") == 0) print_help();
        else if (strcmp(args[0], "cls") == 0) clear_screen();
        else if (strcmp(args[0], "date") == 0) {
            if (argc > 1)
                set_date(args[1]);
            else
                display_date();
        } else if (strcmp(args[0], "time") == 0) {
            if (argc > 1)
                set_time_cmd(args[1]);
            else
                display_time();
        }
        else printf("Invalid command. Type 'help' to see available commands.\n");
    }

    return 0;
}
