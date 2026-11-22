#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>

#define MAX_DESCRIPTION 100
#define INITIAL_CAPACITY 2
#define MAX_CAPACITY 10000

typedef enum {
    PENDING,
    DONE
} TaskStatus;

typedef enum {
    ADD,
    DELETE,
    VIEW,
    UPDATE,
    COMPLETE,
    FIND,
    HELP,
    EXIT,
    UNKNOWN
} CommandType;

typedef struct {
    int id;
    char description[MAX_DESCRIPTION];
    TaskStatus status;
} Task;

/* Safe input */
int read_line(char *buffer, size_t size) {
    if (fgets(buffer, size, stdin) == NULL) {
        return 0;
    }

    size_t len = strlen(buffer);

    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    } else {
        int ch = getchar();

        if (ch != '\n' && ch != EOF) {
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }

            return -1;
        }
    }

    return 1;
}

/* Convert command into uppercase */
void uppercase(char *text) {
    for (int i = 0; text[i] != '\0'; i++) {
        text[i] = (char)toupper((unsigned char)text[i]);
    }
}

/* Convert string to enum */
CommandType get_command(const char *input) {
    char command[100];

    if (strlen(input) >= sizeof(command)) {
        return UNKNOWN;
    }

    strcpy(command, input);
    uppercase(command);

    if (strcmp(command, "ADD") == 0) {
        return ADD;
    }

    if (strcmp(command, "DELETE") == 0) {
        return DELETE;
    }

    if (strcmp(command, "VIEW") == 0) {
        return VIEW;
    }

    if (strcmp(command, "UPDATE") == 0) {
        return UPDATE;
    }

    if (strcmp(command, "COMPLETE") == 0) {
        return COMPLETE;
    }

    if (strcmp(command, "FIND") == 0) {
        return FIND;
    }

    if (strcmp(command, "HELP") == 0) {
        return HELP;
    }

    if (strcmp(command, "EXIT") == 0) {
        return EXIT;
    }

    return UNKNOWN;
}

/* Read positive integer */
int read_id(const char *message, int *id) {
    char input[100];
    char *end;

    printf("%s", message);

    if (read_line(input, sizeof(input)) != 1) {
        printf("Invalid input.\n");
        return 0;
    }

    errno = 0;
    long value = strtol(input, &end, 10);

    while (isspace((unsigned char)*end)) {
        end++;
    }

    if (input == end || *end != '\0' ||
        errno == ERANGE || value <= 0 || value > INT_MAX) {
        printf("Enter a valid positive ID.\n");
        return 0;
    }

    *id = (int)value;
    return 1;
}

/* Find task by ID */
int find_task(const Task tasks[], int count, int id) {
    for (int i = 0; i < count; i++) {
        if (tasks[i].id == id) {
            return i;
        }
    }

    return -1;
}

/* Expand task array */
int expand_tasks(Task **tasks, int *capacity) {
    if (*capacity >= MAX_CAPACITY) {
        printf("Maximum task capacity reached.\n");
        return 0;
    }

    int new_capacity = (*capacity > MAX_CAPACITY / 2)
                       ? MAX_CAPACITY
                       : (*capacity * 2);

    if (new_capacity <= 0 ||
        (size_t)new_capacity > SIZE_MAX / sizeof(Task)) {
        return 0;
    }

    Task *temp = realloc(
        *tasks,
        (size_t)new_capacity * sizeof(Task)
    );

    if (temp == NULL) {
        printf("Memory allocation failed.\n");
        return 0;
    }

    *tasks = temp;
    *capacity = new_capacity;

    return 1;
}

/* Add task */
void add_task(Task **tasks, int *count, int *capacity) {
    if (*count >= MAX_CAPACITY) {
        printf("Maximum task capacity reached.\n");
        return;
    }

    if (*count == *capacity) {
        if (!expand_tasks(tasks, capacity)) {
            return;
        }
    }

    Task t;

    if (!read_id("Enter task ID: ", &t.id)) {
        return;
    }

    if (find_task(*tasks, *count, t.id) != -1) {
        printf("Task ID already exists.\n");
        return;
    }

    printf("Enter task description: ");

    if (read_line(t.description, sizeof(t.description)) != 1 ||
        strlen(t.description) == 0) {
        printf("Invalid or overlong description.\n");
        return;
    }

    t.status = PENDING;

    (*tasks)[*count] = t;
    (*count)++;

    printf("Task %d added successfully.\n", t.id);
}

/* View tasks */
void view_tasks(const Task tasks[], int count) {
    if (count == 0) {
        printf("Task list is empty.\n");
        return;
    }

    printf("\n%-10s %-35s %-12s\n",
           "ID", "Description", "Status");

    for (int i = 0; i < count; i++) {
        printf("%-10d %-35s %-12s\n",
               tasks[i].id,
               tasks[i].description,
               tasks[i].status == PENDING ? "PENDING" : "DONE");
    }
}

/* Find and display a task */
void find_one_task(const Task tasks[], int count) {
    int id;

    if (!read_id("Enter task ID: ", &id)) {
        return;
    }

    int index = find_task(tasks, count, id);

    if (index == -1) {
        printf("Task not found.\n");
        return;
    }

    printf("ID: %d\n", tasks[index].id);
    printf("Description: %s\n", tasks[index].description);
    printf("Status: %s\n",
           tasks[index].status == PENDING ? "PENDING" : "DONE");
}

/* Delete task */
void delete_task(Task tasks[], int *count) {
    int id;

    if (!read_id("Enter task ID to delete: ", &id)) {
        return;
    }

    int index = find_task(tasks, *count, id);

    if (index == -1) {
        printf("Task not found.\n");
        return;
    }

    for (int i = index; i < *count - 1; i++) {
        tasks[i] = tasks[i + 1];
    }

    (*count)--;

    printf("Task %d deleted successfully.\n", id);
}

/* Update description */
void update_task(Task tasks[], int count) {
    int id;

    if (!read_id("Enter task ID to update: ", &id)) {
        return;
    }

    int index = find_task(tasks, count, id);

    if (index == -1) {
        printf("Task not found.\n");
        return;
    }

    char description[MAX_DESCRIPTION];

    printf("Enter new description: ");

    if (read_line(description, sizeof(description)) != 1 ||
        strlen(description) == 0) {
        printf("Invalid or overlong description.\n");
        return;
    }

    strcpy(tasks[index].description, description);

    printf("Task %d updated successfully.\n", id);
}

/* Mark task as completed */
void complete_task(Task tasks[], int count) {
    int id;

    if (!read_id("Enter task ID: ", &id)) {
        return;
    }

    int index = find_task(tasks, count, id);

    if (index == -1) {
        printf("Task not found.\n");
        return;
    }

    tasks[index].status = DONE;

    printf("Task %d marked as DONE.\n", id);
}

/* Help menu */
void show_help(void) {
    printf("\nAvailable commands:\n");
    printf("ADD       - Add a new task\n");
    printf("DELETE    - Delete a task\n");
    printf("VIEW      - Display all tasks\n");
    printf("UPDATE    - Update task description\n");
    printf("COMPLETE  - Mark task as completed\n");
    printf("FIND      - Search task by ID\n");
    printf("HELP      - Display commands\n");
    printf("EXIT      - Exit the program\n");
}

int main(void) {
    int count = 0;
    int capacity = INITIAL_CAPACITY;

    Task *tasks = malloc((size_t)capacity * sizeof(Task));

    if (tasks == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    char input[100];
    CommandType command;

    printf("TASK MANAGEMENT SYSTEM\n");
    show_help();

    while (1) {
        printf("\nEnter command: ");

        int status = read_line(input, sizeof(input));

        if (status == 0) {
            printf("\nInput ended. Exiting.\n");
            break;
        }

        if (status == -1) {
            printf("Command too long.\n");
            continue;
        }

        command = get_command(input);

        switch (command) {
            case ADD:
                add_task(&tasks, &count, &capacity);
                break;

            case DELETE:
                delete_task(tasks, &count);
                break;

            case VIEW:
                view_tasks(tasks, count);
                break;

            case UPDATE:
                update_task(tasks, count);
                break;

            case COMPLETE:
                complete_task(tasks, count);
                break;

            case FIND:
                find_one_task(tasks, count);
                break;

            case HELP:
                show_help();
                break;

            case EXIT:
                printf("Exiting task manager.\n");
                free(tasks);
                return 0;

            default:
                printf("Unknown command. Type HELP for instructions.\n");
        }
    }

    free(tasks);
    tasks = NULL;

    return 0;
}