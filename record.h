#ifndef RECORD_H
#define RECORD_H

#define MAX_TEXT_SIZE 50

// Defines the structure of one record
typedef struct {
    char type[MAX_TEXT_SIZE];
    char category[MAX_TEXT_SIZE];
    float amount;
} Record;

// Stores multiple records. Uses dynamic memory so the list can grow as more records are added.
typedef struct {
    Record *records;
    int size;
    int capacity;
} RecordList;

#endif