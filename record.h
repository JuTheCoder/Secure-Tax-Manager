#ifndef RECORD_H
#define RECORD_H

#define MAX_TEXT_SIZE 50

typedef struct {
    char type[MAX_TEXT_SIZE];
    char category[MAX_TEXT_SIZE];
    float amount;
} Record;

typedef struct {
    Record *records;
    int size;
    int capacity;
} RecordList;

#endif