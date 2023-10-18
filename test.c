#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE 80
#define MAX_HISTORY_SIZE 10

void invoke_error(char *a) {
    printf("osh: %s >_< \n", a);
}

int fetch_input(char *a) {
    char p;
    int num = 0;

    while (((p = getchar()) != '\n') && (num < MAX_LINE)) {
        a[num++] = p;
    }

    if (num == MAX_LINE && p != '\n') {
        invoke_error("Command exceeds maximum command length");
        num = -1;
    } else {
        a[num] = 0;
    }
    while (p != '\n') p = getchar();
    return num;
}

void display_history(char history[][MAX_LINE + 1], int history_count) {
    if (history_count == 0) {
        invoke_error("No history yet");
        return;
    }

    int i, j = 10;
    for (i = history_count; i > 0 && j > 0; i--, j--) {
        printf("%4d\t%s\n", i, history[i % 10]);
    }
}

int split_command(char *input, char *arguments[]) {
    int arg_count = 0, last = -1, i;
    arguments[0] = NULL;

    for (i = 0; i <= MAX_LINE; i++) {
        if (input[i] != ' ' && input[i] != '\t' && input[i]) {
            continue;
        } else {
            if (last != i - 1) {
                arguments[arg_count] = (char *)malloc(i - last);
                if (arguments[arg_count] == NULL) {
                    invoke_error("Unable to allocate memory");
                    return 1;
                }
                memcpy(arguments[arg_count], &input[last + 1], i - last - 1);
                arguments[arg_count][i - last] = 0;
                arguments[++arg_count] = NULL;
            }
            last = i;
        }
    }
    return arg_count;
}

int to_number(char *a) {
    int length = strlen(a), i, answer = 0;
    for (i = 0; i < length; i++) {
        if (a[i] > '9' || a[i] < '0') return -1;
        answer = answer * 10 + a[i] - '0';
    }
    return answer;
}

int main(void) {
    char *args[MAX_LINE/2 + 1];
    int should_run = 1;

    char history[MAX_HISTORY_SIZE][MAX_LINE + 1];
    int history_count = 0;

    char buffer[MAX_LINE + 1];
    memset(buffer, 0, sizeof(buffer));
    int length, args_num;

    while (should_run) {
        printf("osh>");
        fflush(stdout);

        length = fetch_input(buffer);
        if (length == -1) continue;

        if (strcmp(buffer, "!!") == 0) {
            if (history_count > 0) {
                memcpy(buffer, history[history_count % MAX_HISTORY_SIZE], MAX_LINE + 1);
                length = strlen(buffer);
            } else {
                invoke_error("No commands in history");
                continue;
            }
        }

        args_num = split_command(buffer, args);

        if (args_num == 0) continue;

        if (strcmp(args[0], "!") == 0) {
            int temp = to_number(args[1]);
            if (temp <= 0 || temp < history_count - (MAX_HISTORY_SIZE - 1) || temp > history_count) {
                invoke_error("No such command in history");
                continue;
            } else {
                memcpy(buffer, history[temp % MAX_HISTORY_SIZE], MAX_LINE + 1);
                length = strlen(buffer);
                args_num = split_command(buffer, args);
            }
        }

        if (strcmp(args[0], "exit") == 0) {
            should_run = 0;
            continue;
        }

        if (strcmp(args[0], "history") == 0) {
            display_history(history, history_count);
            continue;
        }

        history_count++;
        memcpy(history[history_count % MAX_HISTORY_SIZE], buffer, MAX_LINE + 1);

        int background = 0;
        if (strcmp(args[args_num - 1], "&") == 0) {
            background = 1;
            args[--args_num] = NULL;
        }

        pid_t pid = fork();
        if (pid < 0) {
            invoke_error("Fork process error");
            return 1;
        }

        int status;

        if (pid == 0) {
            status = execvp(args[0], args);
            if (status == -1) {
                invoke_error("Fail to execute the command");
            }
            return 0;
        } else {
            if (background) {
                printf("pid #%d running in the background: %s\n", pid, buffer);
            } else {
                wait(&status);
            }
        }
    }

    return 0;
}