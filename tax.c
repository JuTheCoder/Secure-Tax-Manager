#include <stdio.h>
#include <stdlib.h>

// Defines one tax record
typedef struct {
    char category[50];
    float amount;
} Record;

// Dynamic list for records
typedef struct {
    Record *records;
    int size;
    int capacity;
} RecordList;

// Sets up the list so it can store records (allows space for 2 records initially)
void initList(RecordList *list){
    list->size = 0;
    list->capacity = 2;
    list->records = malloc(list->capacity * sizeof(Record)); // Allocates memory for 2 records

    // Checking if memory allocation failed
    if(list->records == NULL){
        printf("Memory allocation failed\n");
        exit(1);
    }
}

int main(){
    RecordList list;
    initList(&list);

    printf("Program started successfully.\n");

    free(list.records); // Prevents memory leaks
    return 0;
}