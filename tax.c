#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "record.h"
#include "tax_logic.h"
#include "utils.h"

#define INITIAL_CAPACITY 5
#define MAX_TEXT_SIZE 50
#define MAX_INPUT_SIZE 100
#define MAX_AMOUNT 1000000000.0f
#define TAX_RATE 0.10f
#define MAX_TOTAL 1000000000.0f

// Sets up the list so it can store records (allows space for 2 records initially)
void initList(RecordList *list){
    list->size = 0;
    list->capacity = INITIAL_CAPACITY;
    list->records = malloc(INITIAL_CAPACITY * sizeof(Record)); // Allocates memory for 2 records

    // Checking if malloc faild to prevent dereferencing a NULL pointer
    // which can cause a crash or undefined behavior
    if(list->records == NULL){
        printf("Memory allocation failed\n");
        exit(1);
    }
}

// Expands the list if there are more than 2 records added
void resizeList(RecordList *list) {
    int newCapacity = list->capacity * 2;

    if (newCapacity <= list->capacity) {
        printf("Capacity overflow detected\n");
        exit(1);
    }

    Record *temp = realloc(list->records, newCapacity * sizeof(Record));

    if (temp == NULL){
        printf("Memory reallocation failed\n");
        exit(1);
    }

    list->records = temp;
    list->capacity = newCapacity;
}

void removeNewline(char *input) {
    input[strcspn(input, "\n")] = '\0';
}

void toLowerCase(char *str) {
    if (str == NULL) {
        return;
    }

    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

// Creates a new record with a given category and amount depending on user input
void addRecord(RecordList *list){
    char input[MAX_INPUT_SIZE];

    if(list->size == list->capacity){
        resizeList(list);
    }

    printf("Enter type of record (income or expense): ");

    if(fgets(input, sizeof(input), stdin) == NULL){
        printf("Error reading type");
        return;
    }

    removeNewline(input);

    if (!equalsIgnoreCase(input, "Income") && !equalsIgnoreCase(input, "Expense")) {
        printf("Invalid type. Please enter Income or Expense.\n");
        return;
    }

    if (equalsIgnoreCase(input, "Income")) {
        strncpy(list->records[list->size].type, "Income", sizeof(list->records[list->size].type) - 1);
    }
    else{
        strncpy(list->records[list->size].type, "Expense", sizeof(list->records[list->size].type) - 1);
    }
    list->records[list->size].type[sizeof(list->records[list->size].type) - 1] = '\0';

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
    removeNewline(input);

    // Prevent empty input which could cause logic errors later
    if(strlen(input) == 0){
        printf("Category cannot be empty\n");
        return;
    }

    input[0] = toupper((unsigned char)input[0]);

    // Copying the users input string into the category field (-1 to prevent buffer overflow)
    strncpy(list->records[list->size].category, input, sizeof(list->records[list->size].category) - 1);
    list->records[list->size].category[ sizeof(list->records[list->size].category) - 1] = '\0';

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

    if (list->records[list->size].amount < 0 || list->records[list->size].amount > MAX_AMOUNT) {
        printf("Invalid amount range\n");
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

    printf("\nRecords:\n");

    for(int i = 0; i < list->size; i++){
        printf("%d. %s: %s - %.2f\n", i + 1, list->records[i].type, list->records[i].category, list->records[i].amount);
    }
}

// Saves the records that are stored in dynamic memory and stores them in the records.txt file
void saveToFile(const RecordList *list, const char *filename){
    FILE *file = fopen(filename, "w");

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
void loadFromFile(RecordList *list, const char *filename){
    FILE *file = fopen(filename, "r");

    if(file == NULL){
        printf("Error opening file\n");
        return;
    }

    list->size = 0;

    while(1){
        if (list->size == list->capacity) {
            resizeList(list);
        }

        int result = fscanf(file, "%49s %49s %f", list->records[list->size].type, list->records[list->size].category, &list->records[list->size].amount);

        if (result == EOF) {
            break;
        }

        if (result != 3) {
            printf("Invalid record format in file\n");
            break;
        }

        list->size++;
    }

    fclose(file);
    printf("Records loaded from file successfully.\n");
}

void deleteRecord(RecordList *list) {
    char input[100];
    int recordNumber;

    if (list == NULL || list->records == NULL) {
        printf("Error: Invalid record list.\n");
        return;
    }

    if (list->size == 0) {
        printf("No records to delete.\n");
        return;
    }

    viewRecords(list);

    printf("Enter record number to delete: ");

    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("Error reading record number.\n");
        return;
    }

    if (sscanf(input, "%d", &recordNumber) != 1) {
        printf("Invalid record number.\n");
        return;
    }

    if (recordNumber < 1 || recordNumber > list->size) {
        printf("Record number out of range.\n");
        return;
    }

    int index = recordNumber - 1;

    for (int i = index; i < list->size - 1; i++) {
        list->records[i] = list->records[i + 1];
    }

    list->size--;

    printf("Record deleted successfully.\n");
}

void editRecord(RecordList *list) {
    char input[100];
    int recordNumber;
    int index;
    float newAmount;

    if (list == NULL || list->records == NULL) {
        printf("Error: Invalid record list.\n");
        return;
    }

    if (list->size == 0) {
        printf("No records to edit.\n");
        return;
    }

    viewRecords(list);

    printf("Enter record number to edit: ");

    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("Error reading record number.\n");
        return;
    }

    if (sscanf(input, "%d", &recordNumber) != 1) {
        printf("Invalid record number.\n");
        return;
    }

    if (recordNumber < 1 || recordNumber > list->size) {
        printf("Record number out of range.\n");
        return;
    }

    index = recordNumber - 1;

    printf("Enter new type Income or Expense: ");

    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("Error reading type.\n");
        return;
    }

    input[strcspn(input, "\n")] = '\0';

    if (input[0] != '\0') {
        input[0] = toupper((unsigned char)input[0]);
    }

    if (strcmp(input, "Income") != 0 && strcmp(input, "Expense") != 0) {
        printf("Invalid type. Must be Income or Expense.\n");
        return;
    }

    strncpy(list->records[index].type, input,
            sizeof(list->records[index].type) - 1);
    list->records[index].type[
        sizeof(list->records[index].type) - 1
    ] = '\0';

    printf("Enter new category: ");

    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("Error reading category.\n");
        return;
    }

    input[strcspn(input, "\n")] = '\0';

    if (strlen(input) == 0) {
        printf("Category cannot be empty.\n");
        return;
    }

    strncpy(list->records[index].category, input,
            sizeof(list->records[index].category) - 1);
    list->records[index].category[
        sizeof(list->records[index].category) - 1
    ] = '\0';

    printf("Enter new amount: ");

    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("Error reading amount.\n");
        return;
    }

    if (sscanf(input, "%f", &newAmount) != 1) {
        printf("Invalid amount.\n");
        return;
    }

    if (newAmount < 0 || newAmount > MAX_AMOUNT) {
        printf("Invalid amount range.\n");
        return;
    }

    list->records[index].amount = newAmount;

    printf("Record updated successfully.\n");
}

// Calculates the overall total of each record
void calculateTotal(const RecordList *list){
    float incomeTotal = 0.0f;
    float expenseTotal = 0.0f;

    for (int i = 0; i < list->size; i++) {
        if (strncmp(list->records[i].type, "Income", sizeof("Income")) == 0) {
            incomeTotal += list->records[i].amount;
        } else if (strncmp(list->records[i].type, "Expense", sizeof("Expense")) == 0) {
            expenseTotal += list->records[i].amount;
        }
    }

    printf("\nIncome Total: %.2f\n", incomeTotal);
    printf("Expense Total: %.2f\n", expenseTotal);
    printf("Net Total: %.2f\n", incomeTotal - expenseTotal);
}

int main(int argc, char *argv[]){
    RecordList list;
    const char *filename = "records.txt";
    initList(&list);
    char input[MAX_INPUT_SIZE];
    int choice = 0;

    if(argc > 2){
        printf("Usage: %s [records_file]\n", argv[0]);
        return 1;
    }

    if(argc == 2){
        filename = argv[1];
    }

    do{
        printf("\nUsing file: %s\n", filename);
        printf("\n1. Add Record\n");
        printf("2. View Record\n");
        printf("3. Save to File\n");
        printf("4. Load from File\n");
        printf("5. Delete Record\n");
        printf("6. Edit Record\n");
        printf("7. Calculate Total\n");
        printf("8. Calculate Tax\n");
        printf("9. Exit\n");
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
            saveToFile(&list, filename);
        }
        else if(choice == 4){
            loadFromFile(&list, filename);
        }
        else if(choice == 5){
            deleteRecord(&list);
        }
        else if(choice == 6){
            editRecord(&list);
        }
        else if(choice == 7){
            calculateTotal(&list);
        }
        else if(choice == 8){
            calculateTax(&list);
        }
    } while(choice != 9);

    free(list.records); // Prevents memory leaks
    return 0;
}