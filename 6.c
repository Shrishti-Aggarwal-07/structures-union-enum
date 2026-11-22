#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define TEXT_SIZE 20
#define RECORD_COUNT 5

typedef struct {
    int integer;
    float decimal;
    char text[TEXT_SIZE];
} StructData;

typedef union {
    int integer;
    float decimal;
    char text[TEXT_SIZE];
} UnionData;

typedef enum {
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_TEXT
} ValueType;

typedef struct {
    UnionData data;
    ValueType type;
} TypedRecord;

/* Display memory layout */
void memory_analysis(void) {
    StructData s;
    UnionData u;

    printf("\n===== MEMORY ANALYSIS =====\n");

    printf("\nStructure size: %zu bytes\n", sizeof(StructData));
    printf("Union size: %zu bytes\n", sizeof(UnionData));

    printf("\nStructure alignment: %zu bytes\n",
           (size_t)_Alignof(StructData));

    printf("Union alignment: %zu bytes\n",
           (size_t)_Alignof(UnionData));

    printf("\nStructure member addresses:\n");
    printf("Integer: %p\n", (void *)&s.integer);
    printf("Float: %p\n", (void *)&s.decimal);
    printf("Text: %p\n", (void *)s.text);

    printf("\nStructure member offsets:\n");
    printf("Integer: %zu\n", offsetof(StructData, integer));
    printf("Float: %zu\n", offsetof(StructData, decimal));
    printf("Text: %zu\n", offsetof(StructData, text));

    printf("\nUnion member addresses:\n");
    printf("Integer: %p\n", (void *)&u.integer);
    printf("Float: %p\n", (void *)&u.decimal);
    printf("Text: %p\n", (void *)u.text);

    printf("\nMemory required for 100 records:\n");
    printf("Structures: %zu bytes\n",
           100 * sizeof(StructData));

    printf("Unions: %zu bytes\n",
           100 * sizeof(UnionData));
}

/* Set integer value */
void set_integer(TypedRecord *record, int value) {
    if (record == NULL) {
        return;
    }

    record->data.integer = value;
    record->type = VALUE_INT;
}

/* Set float value */
void set_float(TypedRecord *record, float value) {
    if (record == NULL) {
        return;
    }

    record->data.decimal = value;
    record->type = VALUE_FLOAT;
}

/* Set text value */
int set_text(TypedRecord *record, const char *text) {
    if (record == NULL || text == NULL ||
        strlen(text) >= TEXT_SIZE) {
        return 0;
    }

    strcpy(record->data.text, text);
    record->type = VALUE_TEXT;

    return 1;
}

/* Display active member only */
void display_record(const TypedRecord *record) {
    if (record == NULL) {
        printf("Invalid record.\n");
        return;
    }

    switch (record->type) {
        case VALUE_INT:
            printf("Integer: %d\n", record->data.integer);
            break;

        case VALUE_FLOAT:
            printf("Float: %.2f\n", record->data.decimal);
            break;

        case VALUE_TEXT:
            printf("Text: %s\n", record->data.text);
            break;

        default:
            printf("Invalid record type.\n");
    }
}

/* Return type name */
const char *type_name(ValueType type) {
    switch (type) {
        case VALUE_INT:
            return "Integer";

        case VALUE_FLOAT:
            return "Float";

        case VALUE_TEXT:
            return "Text";

        default:
            return "Invalid";
    }
}

int main(void) {
    TypedRecord records[RECORD_COUNT] = {0};

    memory_analysis();

    printf("\n===== TAGGED UNION RECORDS =====\n");

    set_integer(&records[0], 10);
    set_float(&records[1], 220.5f);

    if (!set_text(&records[2], "Union Programming")) {
        printf("Unable to store text.\n");
    }

    set_integer(&records[3], 500);

    if (!set_text(&records[4], "C Language")) {
        printf("Unable to store text.\n");
    }

    for (int i = 0; i < RECORD_COUNT; i++) {
        printf("\nRecord %d\n", i + 1);
        printf("Type: %s\n", type_name(records[i].type));
        display_record(&records[i]);
    }

    printf("\n===== TAGGED RECORD SIZE =====\n");
    printf("Tagged record: %zu bytes\n", sizeof(TypedRecord));
    printf("Plain union: %zu bytes\n", sizeof(UnionData));
    printf("Plain structure: %zu bytes\n", sizeof(StructData));

    printf("\n===== TYPE TRANSITION =====\n");

    set_integer(&records[0], 100);
    display_record(&records[0]);

    if (set_text(&records[0], "Hello")) {
        display_record(&records[0]);
    }

    set_float(&records[0], 3.14f);
    display_record(&records[0]);

    return 0;
}