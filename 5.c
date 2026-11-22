#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>

#define MAX_CAPACITY 1000
#define EPSILON 0.000001

typedef struct {
    int id;
    double width;
    double height;
    double area;
    double perimeter;
} Rectangle;

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

int read_integer(const char *message, int *value) {
    char input[100];
    char *end;

    printf("%s", message);

    if (read_line(input, sizeof(input)) != 1) {
        printf("Invalid input.\n");
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
        printf("Invalid integer.\n");
        return 0;
    }

    *value = (int)number;
    return 1;
}

int read_positive_double(const char *message, double *value) {
    char input[100];
    char *end;

    printf("%s", message);

    if (read_line(input, sizeof(input)) != 1) {
        printf("Invalid input.\n");
        return 0;
    }

    errno = 0;
    double number = strtod(input, &end);

    while (isspace((unsigned char)*end)) {
        end++;
    }

    if (input == end || *end != '\0' ||
        errno == ERANGE || !isfinite(number) ||
        number <= 0.0) {
        printf("Enter a finite positive number.\n");
        return 0;
    }

    *value = number;
    return 1;
}

/* Calculate area and perimeter */
void calculate_geometry(Rectangle *r) {
    r->area = r->width * r->height;
    r->perimeter = 2.0 * (r->width + r->height);
}

/* Find rectangle using ID */
int find_rectangle(const Rectangle rectangles[],
                   int count, int id) {
    for (int i = 0; i < count; i++) {
        if (rectangles[i].id == id) {
            return i;
        }
    }

    return -1;
}

/* Expand allocated memory */
int expand_capacity(Rectangle **rectangles, int *capacity) {
    if (*capacity >= MAX_CAPACITY) {
        printf("Maximum capacity reached.\n");
        return 0;
    }

    int new_capacity = (*capacity > MAX_CAPACITY / 2)
                       ? MAX_CAPACITY
                       : (*capacity * 2);

    if (new_capacity <= 0 ||
        (size_t)new_capacity > SIZE_MAX / sizeof(Rectangle)) {
        return 0;
    }

    Rectangle *temp = realloc(
        *rectangles,
        (size_t)new_capacity * sizeof(Rectangle)
    );

    if (temp == NULL) {
        printf("Memory reallocation failed.\n");
        return 0;
    }

    *rectangles = temp;
    *capacity = new_capacity;

    printf("Capacity increased to %d.\n", *capacity);
    return 1;
}

/* Add rectangle */
void add_rectangle(Rectangle **rectangles,
                   int *count, int *capacity) {
    if (*count >= MAX_CAPACITY) {
        printf("Maximum number of rectangles reached.\n");
        return;
    }

    if (*count == *capacity) {
        if (!expand_capacity(rectangles, capacity)) {
            return;
        }
    }

    Rectangle r;

    if (!read_integer("Enter rectangle ID: ", &r.id) ||
        r.id <= 0) {
        printf("ID must be positive.\n");
        return;
    }

    if (find_rectangle(*rectangles, *count, r.id) != -1) {
        printf("Duplicate rectangle ID.\n");
        return;
    }

    if (!read_positive_double("Enter width: ", &r.width) ||
        !read_positive_double("Enter height: ", &r.height)) {
        return;
    }

    if (!isfinite(r.width * r.height) ||
        !isfinite(2.0 * (r.width + r.height))) {
        printf("Dimensions are too large.\n");
        return;
    }

    calculate_geometry(&r);

    (*rectangles)[*count] = r;
    (*count)++;

    printf("Rectangle added successfully.\n");
}

/* Display all rectangles */
void display_rectangles(const Rectangle rectangles[], int count) {
    if (count == 0) {
        printf("No rectangles available.\n");
        return;
    }

    printf("\n%-8s %-12s %-12s %-12s %-12s %-12s\n",
           "ID", "Width", "Height", "Area",
           "Perimeter", "Type");

    for (int i = 0; i < count; i++) {
        const char *type =
            fabs(rectangles[i].width - rectangles[i].height)
            <= EPSILON ? "Square" : "Non-square";

        printf("%-8d %-12.2f %-12.2f %-12.2f %-12.2f %-12s\n",
               rectangles[i].id,
               rectangles[i].width,
               rectangles[i].height,
               rectangles[i].area,
               rectangles[i].perimeter,
               type);
    }
}

/* Search rectangle */
void search_rectangle(const Rectangle rectangles[], int count) {
    int id;

    if (!read_integer("Enter rectangle ID: ", &id)) {
        return;
    }

    int index = find_rectangle(rectangles, count, id);

    if (index == -1) {
        printf("Rectangle not found.\n");
        return;
    }

    const Rectangle *r = &rectangles[index];

    printf("\nID: %d\n", r->id);
    printf("Width: %.2f\n", r->width);
    printf("Height: %.2f\n", r->height);
    printf("Area: %.2f\n", r->area);
    printf("Perimeter: %.2f\n", r->perimeter);
}

/* Update rectangle dimensions */
void update_rectangle(Rectangle rectangles[], int count) {
    int id;

    if (!read_integer("Enter rectangle ID to update: ", &id)) {
        return;
    }

    int index = find_rectangle(rectangles, count, id);

    if (index == -1) {
        printf("Rectangle not found.\n");
        return;
    }

    double width, height;

    if (!read_positive_double("Enter new width: ", &width) ||
        !read_positive_double("Enter new height: ", &height)) {
        return;
    }

    if (!isfinite(width * height) ||
        !isfinite(2.0 * (width + height))) {
        printf("Dimensions are too large.\n");
        return;
    }

    rectangles[index].width = width;
    rectangles[index].height = height;

    calculate_geometry(&rectangles[index]);

    printf("Rectangle updated successfully.\n");
}

/* Delete rectangle */
void delete_rectangle(Rectangle rectangles[],
                      int *count) {
    int id;

    if (!read_integer("Enter rectangle ID to delete: ", &id)) {
        return;
    }

    int index = find_rectangle(rectangles, *count, id);

    if (index == -1) {
        printf("Rectangle not found.\n");
        return;
    }

    for (int i = index; i < *count - 1; i++) {
        rectangles[i] = rectangles[i + 1];
    }

    (*count)--;

    printf("Rectangle deleted successfully.\n");
}

/* Area statistics */
void area_summary(const Rectangle rectangles[], int count) {
    if (count == 0) {
        printf("No rectangles available for analysis.\n");
        return;
    }

    int largest = 0;
    int smallest = 0;
    double total = 0.0;

    for (int i = 0; i < count; i++) {
        total += rectangles[i].area;

        if (rectangles[i].area > rectangles[largest].area) {
            largest = i;
        }

        if (rectangles[i].area < rectangles[smallest].area) {
            smallest = i;
        }
    }

    printf("\nLargest area: ID %d (%.2f)\n",
           rectangles[largest].id,
           rectangles[largest].area);

    printf("Smallest area: ID %d (%.2f)\n",
           rectangles[smallest].id,
           rectangles[smallest].area);

    printf("Total area: %.2f\n", total);
    printf("Average area: %.2f\n", total / count);
}

/* Sorting comparator */
int compare_rectangles(const void *a, const void *b) {
    const Rectangle *r1 = a;
    const Rectangle *r2 = b;

    if (r1->area < r2->area) {
        return -1;
    }

    if (r1->area > r2->area) {
        return 1;
    }

    if (r1->id < r2->id) {
        return -1;
    }

    if (r1->id > r2->id) {
        return 1;
    }

    return 0;
}

/* Display sorted rectangles */
void sort_rectangles(const Rectangle rectangles[], int count) {
    if (count == 0) {
        printf("No rectangles to sort.\n");
        return;
    }

    Rectangle *copy = malloc((size_t)count * sizeof(Rectangle));

    if (copy == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    memcpy(copy, rectangles, (size_t)count * sizeof(Rectangle));

    qsort(copy, count, sizeof(Rectangle), compare_rectangles);

    printf("\nRectangles sorted by ascending area:\n");
    display_rectangles(copy, count);

    free(copy);
}

/* Filter rectangles using area threshold */
void filter_rectangles(const Rectangle rectangles[], int count) {
    double threshold;

    if (!read_positive_double("Enter area threshold: ", &threshold)) {
        return;
    }

    int found = 0;

    printf("\nRectangles with area greater than %.2f:\n",
           threshold);

    for (int i = 0; i < count; i++) {
        if (rectangles[i].area > threshold) {
            printf("ID: %d | Area: %.2f\n",
                   rectangles[i].id,
                   rectangles[i].area);
            found = 1;
        }
    }

    if (!found) {
        printf("No matching rectangles found.\n");
    }
}

int main(void) {
    int count = 0;
    int capacity;

    if (!read_integer("Enter initial number of rectangles (1-1000): ",
                      &capacity) ||
        capacity < 1 || capacity > MAX_CAPACITY) {
        printf("Invalid capacity.\n");
        return 1;
    }

    Rectangle *rectangles =
        malloc((size_t)capacity * sizeof(Rectangle));

    if (rectangles == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    int choice;

    do {
        printf("\n====== RECTANGLE MANAGEMENT ======\n");
        printf("1. Add Rectangle\n");
        printf("2. Display Rectangles\n");
        printf("3. Search Rectangle\n");
        printf("4. Update Rectangle\n");
        printf("5. Delete Rectangle\n");
        printf("6. Area Summary\n");
        printf("7. Sort by Area\n");
        printf("8. Filter by Area\n");
        printf("0. Exit\n");

        if (!read_integer("Enter choice: ", &choice)) {
            continue;
        }

        switch (choice) {
            case 1:
                add_rectangle(&rectangles, &count, &capacity);
                break;

            case 2:
                display_rectangles(rectangles, count);
                break;

            case 3:
                search_rectangle(rectangles, count);
                break;

            case 4:
                update_rectangle(rectangles, count);
                break;

            case 5:
                delete_rectangle(rectangles, &count);
                break;

            case 6:
                area_summary(rectangles, count);
                break;

            case 7:
                sort_rectangles(rectangles, count);
                break;

            case 8:
                filter_rectangles(rectangles, count);
                break;

            case 0:
                printf("Exiting program.\n");
                break;

            default:
                printf("Invalid choice.\n");
        }

    } while (choice != 0);

    free(rectangles);
    rectangles = NULL;

    return 0;
}