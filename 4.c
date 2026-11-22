#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<math.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include<stdint.h>

#define MAX_NAME 50
#define MAX_CAPACITY 10000

typedef struct {
    int roll_no;
    char name[MAX_NAME];
    int age;
    float gpa;
} Student;

/* Read a line safely */
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

/* Read a valid integer */
int read_integer(const char *message, int *value) {
    char input[100];
    char *end;

    printf("%s", message);

    int status = read_line(input, sizeof(input));

    if (status != 1) {
        return 0;
    }

    errno = 0;
    long number = strtol(input, &end, 10);

    while (isspace((unsigned char)*end)) {
        end++;
    }

    if (input == end || *end != '\0' ||
        errno == ERANGE || number < INT_MIN ||
        number > INT_MAX) {
        printf("Invalid integer input.\n");
        return 0;
    }

    *value = (int)number;
    return 1;
}

/* Read a valid GPA */
int read_gpa(float *gpa) {
    char input[100];
    char *end;

    printf("Enter GPA (0.00 - 4.00): ");

    if (read_line(input, sizeof(input)) != 1) {
        return 0;
    }

    errno = 0;
    double value = strtod(input, &end);

    while (isspace((unsigned char)*end)) {
        end++;
    }

    if (input == end || *end != '\0' ||
        errno == ERANGE || value < 0.0 || value > 4.0) {
        printf("Invalid GPA.\n");
        return 0;
    }

    *gpa = (float)value;
    return 1;
}

/* Search for a student using roll number */
int find_student(Student students[], int count, int roll) {
    for (int i = 0; i < count; i++) {
        if (students[i].roll_no == roll) {
            return i;
        }
    }

    return -1;
}

/* Expand array capacity */
int expand_capacity(Student **students, int *capacity) {
    if (*capacity >= MAX_CAPACITY) {
        printf("Maximum capacity reached.\n");
        return 0;
    }

    int new_capacity = (*capacity > MAX_CAPACITY / 2)
                       ? MAX_CAPACITY
                       : (*capacity * 2);

    if (new_capacity <= 0 ||
        (size_t)new_capacity > SIZE_MAX / sizeof(Student)) {
        return 0;
    }

    Student *temp = realloc(
        *students,
        (size_t)new_capacity * sizeof(Student)
    );

    if (temp == NULL) {
        printf("Memory reallocation failed.\n");
        return 0;
    }

    *students = temp;
    *capacity = new_capacity;

    printf("Capacity increased to %d.\n", *capacity);
    return 1;
}

/* Add a student */
void add_student(Student **students, int *count, int *capacity) {
    if (*count == MAX_CAPACITY) {
        printf("Maximum number of students reached.\n");
        return;
    }

    if (*count == *capacity) {
        if (!expand_capacity(students, capacity)) {
            return;
        }
    }

    Student s;

    if (!read_integer("Enter roll number: ", &s.roll_no) ||
        s.roll_no <= 0) {
        printf("Roll number must be positive.\n");
        return;
    }

    if (find_student(*students, *count, s.roll_no) != -1) {
        printf("Duplicate roll number. Student not added.\n");
        return;
    }

    char name[MAX_NAME];

    printf("Enter student name: ");

    if (read_line(name, sizeof(name)) != 1 ||
        strlen(name) == 0) {
        printf("Invalid or overlong name.\n");
        return;
    }

    strcpy(s.name, name);

    if (!read_integer("Enter age (16-100): ", &s.age) ||
        s.age < 16 || s.age > 100) {
        printf("Invalid age.\n");
        return;
    }

    if (!read_gpa(&s.gpa)) {
        return;
    }

    (*students)[*count] = s;
    (*count)++;

    printf("Student added successfully.\n");
}

/* Display all students */
void display_students(const Student students[], int count) {
    if (count == 0) {
        printf("No student records available.\n");
        return;
    }

    printf("\n%-10s %-25s %-10s %-10s\n",
           "Roll No", "Name", "Age", "GPA");

    for (int i = 0; i < count; i++) {
        printf("%-10d %-25s %-10d %-10.2f\n",
               students[i].roll_no,
               students[i].name,
               students[i].age,
               students[i].gpa);
    }
}

/* Search student */
void search_student(const Student students[], int count) {
    int roll;

    if (!read_integer("Enter roll number to search: ", &roll)) {
        return;
    }

    int index = find_student((Student *)students, count, roll);

    if (index == -1) {
        printf("Student not found.\n");
        return;
    }

    printf("\nRoll Number: %d\n", students[index].roll_no);
    printf("Name: %s\n", students[index].name);
    printf("Age: %d\n", students[index].age);
    printf("GPA: %.2f\n", students[index].gpa);
}

/* Update student details */
void update_student(Student students[], int count) {
    int roll;

    if (!read_integer("Enter roll number to update: ", &roll)) {
        return;
    }

    int index = find_student(students, count, roll);

    if (index == -1) {
        printf("Student not found.\n");
        return;
    }

    char name[MAX_NAME];

    printf("Enter new name: ");

    if (read_line(name, sizeof(name)) != 1 ||
        strlen(name) == 0) {
        printf("Invalid or overlong name.\n");
        return;
    }

    int age;
    if (!read_integer("Enter new age (16-100): ", &age) ||
        age < 16 || age > 100) {
        printf("Invalid age.\n");
        return;
    }

    float gpa;
    if (!read_gpa(&gpa)) {
        return;
    }

    strcpy(students[index].name, name);
    students[index].age = age;
    students[index].gpa = gpa;

    printf("Student updated successfully.\n");
}

/* Delete student */
void delete_student(Student students[], int *count) {
    int roll;

    if (!read_integer("Enter roll number to delete: ", &roll)) {
        return;
    }

    int index = find_student(students, *count, roll);

    if (index == -1) {
        printf("Student not found.\n");
        return;
    }

    for (int i = index; i < *count - 1; i++) {
        students[i] = students[i + 1];
    }

    (*count)--;

    printf("Student deleted successfully.\n");
}

/* Display GPA statistics */
void gpa_summary(const Student students[], int count) {
    if (count == 0) {
        printf("No records available for GPA analysis.\n");
        return;
    }

    int highest = 0;
    int lowest = 0;
    float total = 0.0f;

    for (int i = 0; i < count; i++) {
        total += students[i].gpa;

        if (students[i].gpa > students[highest].gpa) {
            highest = i;
        }

        if (students[i].gpa < students[lowest].gpa) {
            lowest = i;
        }
    }

    printf("\nHighest GPA: %s (%.2f)\n",
           students[highest].name, students[highest].gpa);

    printf("Lowest GPA: %s (%.2f)\n",
           students[lowest].name, students[lowest].gpa);

    printf("Average GPA: %.2f\n", total / count);
}

/* Sorting comparator */
int compare_students(const void *a, const void *b) {
    const Student *s1 = a;
    const Student *s2 = b;

    if (s1->gpa < s2->gpa) {
        return 1;
    }

    if (s1->gpa > s2->gpa) {
        return -1;
    }

    if (s1->roll_no < s2->roll_no) {
        return -1;
    }

    if (s1->roll_no > s2->roll_no) {
        return 1;
    }

    return 0;
}

/* Display students sorted by GPA */
void sort_students(const Student students[], int count) {
    if (count == 0) {
        printf("No records to sort.\n");
        return;
    }

    if ((size_t)count > SIZE_MAX / sizeof(Student)) {
        printf("Sorting allocation size is too large.\n");
        return;
    }

    Student *copy = malloc((size_t)count * sizeof(Student));

    if (copy == NULL) {
        printf("Unable to allocate memory for sorting.\n");
        return;
    }

    memcpy(copy, students, (size_t)count * sizeof(Student));

    qsort(copy, count, sizeof(Student), compare_students);

    printf("\nStudents sorted by GPA (descending):\n");
    display_students(copy, count);

    free(copy);
}

int main(void) {
    Student *students = NULL;
    int count = 0;
    int capacity;

    printf("STUDENT RECORDS AND ACADEMIC ANALYTICS\n");

    if (!read_integer("Enter initial capacity (1-10000): ",
                     &capacity) ||
        capacity < 1 || capacity > MAX_CAPACITY) {
        printf("Invalid initial capacity.\n");
        return 1;
    }

    students = malloc((size_t)capacity * sizeof(Student));

    if (students == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    int choice;

    do {
        printf("\n========== MENU ==========\n");
        printf("1. Add Student\n");
        printf("2. Display Students\n");
        printf("3. Search Student\n");
        printf("4. Update Student\n");
        printf("5. Delete Student\n");
        printf("6. GPA Summary\n");
        printf("7. Sort Students by GPA\n");
        printf("0. Exit\n");

        if (!read_integer("Enter choice: ", &choice)) {
            printf("Invalid choice. Try again.\n");
            continue;
        }

        switch (choice) {
            case 1:
                add_student(&students, &count, &capacity);
                break;

            case 2:
                display_students(students, count);
                break;

            case 3:
                search_student(students, count);
                break;

            case 4:
                update_student(students, count);
                break;

            case 5:
                delete_student(students, &count);
                break;

            case 6:
                gpa_summary(students, count);
                break;

            case 7:
                sort_students(students, count);
                break;

            case 0:
                printf("Exiting program.\n");
                break;

            default:
                printf("Invalid menu choice.\n");
        }

    } while (choice != 0);

    free(students);
    students = NULL;

    return 0;
}