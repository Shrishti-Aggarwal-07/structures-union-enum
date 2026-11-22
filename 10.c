#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TYPE_EMPTY,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING
} DataType;

typedef union {
    int integer;
    float decimal;
    char character;
    char *string;
} Data;

typedef struct {
    Data data;
    DataType type;
} Container;

/* Initialize container */
void container_init(Container *c) {
    if (c == NULL) {
        return;
    }

    c->data.string = NULL;
    c->type = TYPE_EMPTY;
}

/* Clear container and release memory */
void container_clear(Container *c) {
    if (c == NULL) {
        return;
    }

    if (c->type == TYPE_STRING) {
        free(c->data.string);
    }

    c->data.string = NULL;
    c->type = TYPE_EMPTY;
}

/* Set integer */
int container_set_int(Container *c, int value) {
    if (c == NULL) {
        return 0;
    }

    container_clear(c);

    c->data.integer = value;
    c->type = TYPE_INT;

    return 1;
}

/* Set float */
int container_set_float(Container *c, float value) {
    if (c == NULL) {
        return 0;
    }

    container_clear(c);

    c->data.decimal = value;
    c->type = TYPE_FLOAT;

    return 1;
}

/* Set character */
int container_set_char(Container *c, char value) {
    if (c == NULL) {
        return 0;
    }

    container_clear(c);

    c->data.character = value;
    c->type = TYPE_CHAR;

    return 1;
}

/* Set dynamically allocated string */
int container_set_string(Container *c, const char *value) {
    if (c == NULL || value == NULL) {
        return 0;
    }

    size_t length = strlen(value);

    if (length == (size_t)-1) {
        return 0;
    }

    char *new_string = malloc(length + 1);

    if (new_string == NULL) {
        return 0;
    }

    memcpy(new_string, value, length + 1);

    /* Allocate first, then replace the old value */
    container_clear(c);

    c->data.string = new_string;
    c->type = TYPE_STRING;

    return 1;
}

/* Get integer */
int container_get_int(const Container *c, int *output) {
    if (c == NULL || output == NULL ||
        c->type != TYPE_INT) {
        return 0;
    }

    *output = c->data.integer;
    return 1;
}

/* Get float */
int container_get_float(const Container *c, float *output) {
    if (c == NULL || output == NULL ||
        c->type != TYPE_FLOAT) {
        return 0;
    }

    *output = c->data.decimal;
    return 1;
}

/* Get character */
int container_get_char(const Container *c, char *output) {
    if (c == NULL || output == NULL ||
        c->type != TYPE_CHAR) {
        return 0;
    }

    *output = c->data.character;
    return 1;
}

/* Get string */
int container_get_string(const Container *c,
                         const char **output) {
    if (c == NULL || output == NULL ||
        c->type != TYPE_STRING ||
        c->data.string == NULL) {
        return 0;
    }

    *output = c->data.string;
    return 1;
}

/* Return readable type name */
const char *container_type_name(const Container *c) {
    if (c == NULL) {
        return "Invalid";
    }

    switch (c->type) {
        case TYPE_EMPTY:
            return "Empty";

        case TYPE_INT:
            return "Integer";

        case TYPE_FLOAT:
            return "Float";

        case TYPE_CHAR:
            return "Character";

        case TYPE_STRING:
            return "String";

        default:
            return "Unknown";
    }
}

/* Print active value */
void container_print(const Container *c) {
    if (c == NULL) {
        printf("Invalid container.\n");
        return;
    }

    switch (c->type) {
        case TYPE_EMPTY:
            printf("Empty container.\n");
            break;

        case TYPE_INT:
            printf("Integer: %d\n", c->data.integer);
            break;

        case TYPE_FLOAT:
            printf("Float: %.2f\n", c->data.decimal);
            break;

        case TYPE_CHAR:
            printf("Character: %c\n", c->data.character);
            break;

        case TYPE_STRING:
            if (c->data.string != NULL) {
                printf("String: %s\n", c->data.string);
            } else {
                printf("Invalid string.\n");
            }
            break;

        default:
            printf("Unknown data type.\n");
    }
}

/* Deep copy */
int container_copy(Container *destination,
                   const Container *source) {
    if (destination == NULL || source == NULL) {
        return 0;
    }

    if (destination == source) {
        return 1;
    }

    if (source->type == TYPE_STRING) {
        if (source->data.string == NULL) {
            return 0;
        }

        size_t length = strlen(source->data.string);
        char *new_string = malloc(length + 1);

        if (new_string == NULL) {
            return 0;
        }

        memcpy(new_string, source->data.string, length + 1);

        container_clear(destination);

        destination->data.string = new_string;
        destination->type = TYPE_STRING;

        return 1;
    }

    if (source->type != TYPE_EMPTY &&
        source->type != TYPE_INT &&
        source->type != TYPE_FLOAT &&
        source->type != TYPE_CHAR) {
        return 0;
    }

    DataType source_type = source->type;
    Data source_data = source->data;

    container_clear(destination);

    destination->type = source_type;

    if (source_type == TYPE_INT) {
        destination->data.integer = source_data.integer;
    } else if (source_type == TYPE_FLOAT) {
        destination->data.decimal = source_data.decimal;
    } else if (source_type == TYPE_CHAR) {
        destination->data.character = source_data.character;
    }

    return 1;
}

int main(void) {
    const int SIZE = 6;

    Container *records = calloc(SIZE, sizeof(Container));

    if (records == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    Container copied_record;
    container_init(&copied_record);

    /* Initialize all containers */
    for (int i = 0; i < SIZE; i++) {
        container_init(&records[i]);
    }

    printf("===== STORING DIFFERENT DATA TYPES =====\n");

    container_set_int(&records[0], 10);
    container_set_float(&records[1], 3.14f);
    container_set_char(&records[2], 'A');
    container_set_string(&records[3], "Hello, World!");
    container_set_int(&records[4], 0);
    container_set_string(&records[5], "Data");

    /* Display all records */
    for (int i = 0; i < SIZE; i++) {
        printf("\nRecord %d\n", i);
        printf("Type: %s\n",
               container_type_name(&records[i]));

        container_print(&records[i]);
    }

    /* Test type-safe getter */
    printf("\n===== TESTING GETTERS =====\n");

    int number;

    if (container_get_int(&records[0], &number)) {
        printf("Retrieved integer: %d\n", number);
    }

    if (!container_get_float(&records[0], &(float){0})) {
        printf("Wrong-type getter rejected successfully.\n");
    }

    /* Test deep copy */
    printf("\n===== DEEP COPY TEST =====\n");

    if (container_copy(&copied_record, &records[5])) {
        printf("Copied value: ");
        container_print(&copied_record);
    }

    container_set_string(&records[5], "Modified Data");

    printf("Original after modification: ");
    container_print(&records[5]);

    printf("Copied record remains: ");
    container_print(&copied_record);

    /* Test type transitions */
    printf("\n===== TYPE TRANSITIONS =====\n");

    container_set_string(&records[0], "First String");
    container_print(&records[0]);

    container_set_int(&records[0], 100);
    container_print(&records[0]);

    container_set_string(&records[0], "Second String");
    container_print(&records[0]);

    container_set_float(&records[0], 5.75f);
    container_print(&records[0]);

    /* Test self-copy */
    printf("\n===== SELF COPY TEST =====\n");

    if (container_copy(&records[1], &records[1])) {
        printf("Self-copy handled safely.\n");
    }

    /* Clean up */
    for (int i = 0; i < SIZE; i++) {
        container_clear(&records[i]);
    }

    container_clear(&copied_record);
    container_clear(&copied_record);

    free(records);
    records = NULL;

    printf("\nAll allocated memory released successfully.\n");

    return 0;
}