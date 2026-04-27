#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Defines one tax record
typedef struct {
    char type[50];
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

    // Checking if malloc faild to prevent dereferencing a NULL pointer
    // which can cause a crash or undefined behavior
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
    char input[100];
    char test1[] = "Income";
    char test2[] = "Expense";

    if(list->size == list->capacity){
        resizeList(list);
    }

    printf("Enter type of record (income or expense): ");

    if(fgets(input, sizeof(input), stdin) == NULL){
        printf("Error reading type");
        return;
    }

    input[0] = toupper((unsigned char)input[0]);

    input[strcspn(input, "\n")] = '\0';

    if((strncmp(test1, input, sizeof(test1) - 1) == 0) || (strncmp(test2, input, sizeof(test2) - 1) == 0)){
        strncpy(list->records[list->size].type, input, sizeof(list->records[list->size].type) - 1);
        list->records[list->size].type[strcspn(list->records[list->size].type, "\n")] = '\0';
    }

    printf("Enter category: ");

    // Using fgets + sscanf instead of scanf to prevent input issues
    // scanf can leave leftover characters in the buffer or crash on invalid input
    // fgets reads the full line safely, and sscanf lets us validate the input
    if(fgets(input, sizeof(input), stdin) == NULL){
        printf("Error reading category\n");
        return;
    }

    input[0] = toupper((unsigned char)input[0]);

    // Remove newline character safely
    input[strcspn(input, "\n")] = '\0';

    // Prevent empty input which could cause logic errors later
    if(strlen(input) == 0){
        printf("Category cannot be empty\n");
        return;
    }

    // Copying the users input string into the category field (-1 to prevent buffer overflow)
    strncpy(list->records[list->size].category, input, sizeof(list->records[list->size].category) - 1);
    list->records[list->size].category[strcspn(list->records[list->size].category, "\n")] = '\0';

    printf("Enter amount: ");
    // fgets is used instead of unsafe functions like gets() to prevent buffer overflow
    // It limits input size and avoids writing past allocated memory
    if(fgets(input, sizeof(input), stdin) == NULL){
        printf("Error reading amount\n");
        return;
    }

    // Validate numeric input to prevent invalid data
    // sscanf returns 1 only if a valid float is read
    if(sscanf(input, "%f", &list->records[list->size].amount) != 1){
        printf("Invalid amount\n");
        return;
    }

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
        printf("%d. %s: %s - %.2f\n", i + 1, list->records[i].type, list->records[i].category, list->records[i].amount);
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
        fprintf(file, "%s %s %.2f\n", list->records[i].type, list->records[i].category, list->records[i].amount);
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

    while(fscanf(file, "%49s %49s %f", list->records[list->size].type, list->records[list->size].category, &list->records[list->size].amount) == 3){
        list->size++;

        if(list->size == list->capacity){
            resizeList(list);
        }
    }

    fclose(file);
    printf("Records loaded from file successfully.\n");
}

// Calculates the overall total of each record
void calculateTotal(RecordList *list){
    float total = 0;

    for(int i = 0; i < list->size; i++){
        total += list->records[i].amount;
    }
    printf("Total amount: %.2f\n", total);
}

int main(){
    RecordList list;
    initList(&list);
    char input[100];
    int choice;

    do{
        printf("\n1. Add Record\n");
        printf("2. View Record\n");
        printf("3. Save to File\n");
        printf("4. Load from File\n");
        printf("5. Calculate Total\n");
        printf("6. Exit\n");
        printf("Choose: ");
        
        // Using fgets and sscanf to safely read and validate user input, preventing buffer issues and invalid input crashes.
        if(fgets(input, sizeof(input), stdin) == NULL) {
            printf("Error reading menu choice\n");
            continue;
        }

        if(sscanf(input, "%d", &choice) != 1){
            printf("Invalid menu choice\n");
            continue;
        }

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
        else if(choice == 5){
            calculateTotal(&list);
        }
    } while(choice != 6);

    free(list.records); // Prevents memory leaks
    return 0;
}