#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// Expands the list if there are more than 2 records added
void resizeList(RecordList *list) {
    list->capacity *= 2;
    list->records = realloc(list->records, list->capacity * sizeof(Record));

    if (list->records == NULL){
        printf("Memory reallocation failed\n");
        exit(1);
    }
}

// Creates a new record with a given category and amount depending on user input
void addRecord(RecordList *list){
    if(list->size == list->capacity){
        resizeList(list);
    }

    printf("Enter category: ");
    fgets(list->records[list->size].category, 50, stdin);

    list->records[list->size].category[strcspn(list->records[list->size].category, "\n")] = '\0';

    printf("Enter amount: ");
    scanf("%f", &list->records[list->size].amount);
    getchar();

    list->size++;
    printf("Record added!\n");
}

// Prints out all records in dynamic memory
void viewRecords(RecordList *list){
    if(list->size == 0){
        printf("No records yet.\n");
        return;
    }

    for(int i = 0; i < list->size; i++){
        printf("%d. %s - %.2f\n", i + 1, list->records[i].category, list->records[i].amount);
    }
}

// Saves the records that are stored in dynamic memory and stores them in the records.txt file
void saveToFile(RecordList *list){
    FILE *file = fopen("records.txt", "w");

    if(file == NULL){
        printf("Error opening file\n");
        return;
    }

    for(int i = 0; i < list->size; i++){
        fprintf(file, "%s %.2f\n", list->records[i].category, list->records[i].amount);
    }

    fclose(file);
    printf("Records saved to file successfully.\n");
}

// Opens the saved file, reads each record one by one, and then stores it back into dynamic memory
void loadFromFile(RecordList *list){
    FILE *file = fopen("records.txt", "r");

    if(file == NULL){
        printf("Error opening file\n");
        return;
    }

    list->size = 0;

    while(fscanf(file, "%49s %f", list->records[list->size].category, &list->records[list->size].amount) == 2){
        list->size++;

        if(list->size == list->capacity){
            resizeList(list);
        }
    }

    fclose(file);
    printf("Records loaded from file successfully.\n");
}

int main(){
    RecordList list;
    initList(&list);

    int choice;

    do{
        printf("\n1. Add Record\n");
        printf("2. View Record\n");
        printf("3. Save to File\n");
        printf("4. Load from File\n");
        printf("5. Exit\n");
        printf("Choose: ");
        scanf("%d", &choice);
        getchar();

        if(choice == 1){
            addRecord(&list);
        }
        else if(choice == 2){
            viewRecords(&list);
        }
        else if(choice == 3){
            saveToFile(&list);
        }
        else if(choice == 4){
            loadFromFile(&list);
        }
    } while(choice != 5);

    free(list.records); // Prevents memory leaks
    return 0;
}