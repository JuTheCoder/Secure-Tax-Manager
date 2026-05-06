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

/*
 * Function: initList
 * Purpose: Initializes a RecordList structure and allocates memory
 * for storing records dynamically.
 *
 * Parameters:
 * - list: Pointer to the RecordList being initialized.
 *
 * Return Value:
 * - None.
 *
 * Side Effects:
 * - Allocates heap memory for records.
 * - Sets the initial size and capacity.
 */
void initList(RecordList *list){
    list->size = 0;
    list->capacity = INITIAL_CAPACITY;
    list->records = malloc(INITIAL_CAPACITY * sizeof(Record)); // Allocates memory for 2 records

    // Vulnerability ID: 4-1 Failure to Handle Errors Correctly
    // Resolution: Checks if malloc() failed before using list->records,
    // preventing a NULL pointer dereference.
    if(list->records == NULL){
        printf("Memory allocation failed\n");
        exit(1);
    }
}

/*
 * Function: resizeList
 * Purpose: Expands the dynamic array when the current capacity is full.
 *
 * Parameters:
 * - list: Pointer to the RecordList being resized.
 *
 * Return Value:
 * - None.
 *
 * Side Effects:
 * - Reallocates memory for the records array.
 * - Updates list capacity.
 */
void resizeList(RecordList *list) {
    int newCapacity = list->capacity * 2;

    // Vulnerability ID: 3-1 Integer Overflows
    // Resolution: Verifies the new capacity is greater than the old capacity
    // before reallocating memory.
    if (newCapacity <= list->capacity) {
        printf("Capacity overflow detected\n");
        exit(1);
    }

    Record *temp = realloc(list->records, newCapacity * sizeof(Record));

    // Vulnerability ID: 4-2 Failure to Handle Errors Correctly
    // Resolution: Stores realloc() in a temporary pointer and checks for failure
    // before replacing the original records pointer.
    if (temp == NULL){
        printf("Memory reallocation failed\n");
        exit(1);
    }

    list->records = temp;
    list->capacity = newCapacity;
}

/*
 * Function: removeNewline
 * Purpose: Removes the newline character left by fgets().
 *
 * Parameters:
 * - input: String to modify.
 *
 * Return Value:
 * - None.
 *
 * Side Effects:
 * - Modifies the original string.
 */
void removeNewline(char *input) {
    input[strcspn(input, "\n")] = '\0';
}

/*
 * Function: toLowerCase
 * Purpose: Converts every character in a string to lowercase.
 *
 * Parameters:
 * - str: String to convert.
 *
 * Return Value:
 * - None.
 *
 * Side Effects:
 * - Modifies the original string.
 */
void toLowerCase(char *str) {
    if (str == NULL) {
        return;
    }

    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

/*
 * Function: addRecord
 * Purpose: Allows the user to add a new income or expense record
 * to the RecordList.
 *
 * Parameters:
 * - list: Pointer to the RecordList where the new record
 *   will be stored.
 *
 * Return Value:
 * - None.
 *
 * Side Effects:
 * - Modifies the RecordList by adding a new record.
 * - May resize the dynamic array if capacity is reached.
 * - Prints prompts and validation messages to the terminal.
 */
void addRecord(RecordList *list){
    char input[MAX_INPUT_SIZE];

    // Expands the dynamic array if the list is full
    // before adding another record.
    if(list->size == list->capacity){
        resizeList(list);
    }

    printf("Enter type of record (income or expense): ");

    // Vulnerability ID: 1-1 Buffer Overruns
    // Resolution: fgets() limits input to the size of the input buffer,
    // which prevents the user from entering more characters than the array can hold.
    if(fgets(input, sizeof(input), stdin) == NULL){
        printf("Error reading type");
        return;
    }

    removeNewline(input);

    // Vulnerability ID: 5-2 Poor Usability
    // Resolution: Validates the record type and provides
    // a clear message for invalid input.
    if(!equalsIgnoreCase(input, "Income") && !equalsIgnoreCase(input, "Expense")){
        printf("Invalid type. Please enter Income or Expense.\n");
        return;
    }

    // Vulnerability ID: 1-2 Buffer Overruns
    // Resolution: strncpy() copies only up to the destination size minus one,
    // and the last character is manually set to '\0' to safely terminate the string.
    if(equalsIgnoreCase(input, "Income")){
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

    // Capitalizes the first letter of the category
    // to keep formatting consistent.
    input[0] = toupper((unsigned char)input[0]);

    // Remove newline character safely
    removeNewline(input);

    // Vulnerability ID: 5-2 Poor Usability
    // Resolution: Prevents empty categories from being saved.
    if(strlen(input) == 0){
        printf("Category cannot be empty\n");
        return;
    }

    // Capitalizes the first letter of the category
    // to keep formatting consistent.
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

    // Vulnerability ID: 4-3 Failure to Handle Errors Correctly
    // Resolution: Verifies that the entered amount is a valid float value
    // before storing it.
    if(sscanf(input, "%f", &list->records[list->size].amount) != 1){
        printf("Invalid amount\n");
        return;
    }

    // Vulnerability ID: 3-2 Integer Overflows
    // Resolution: Rejects negative or extremely large amounts
    // before storing them.
    if (list->records[list->size].amount < 0 || list->records[list->size].amount > MAX_AMOUNT) {
        printf("Invalid amount range\n");
        return;
    }

    // Increases the number of stored records
    // after successfully adding a new record.
    list->size++;
    printf("Record added!\n");
}

/*
 * Function: viewRecords
 * Purpose: Displays all stored income and expense records
 * currently saved in dynamic memory.
 *
 * Parameters:
 * - list: Pointer to the RecordList containing stored records.
 *
 * Return Value:
 * - None.
 *
 * Side Effects:
 * - Prints all records to the terminal.
 */
void viewRecords(RecordList *list){
    if(list->size == 0){
        printf("No records yet.\n");
        return;
    }

    printf("\nRecords:\n");

    // Loops through every stored record
    // and prints the record information.
    for(int i = 0; i < list->size; i++){

        // Vulnerability ID: 2-1 Format String Problems
        // Resolution: Uses a fixed printf() format string
        // instead of allowing user input to control formatting.
        printf("%d. %s: %s - %.2f\n", i + 1, list->records[i].type, list->records[i].category, list->records[i].amount);
    }
}

/*
 * Function: saveToFile
 * Purpose: Saves all stored records from dynamic memory
 * into a text file.
 *
 * Parameters:
 * - list: Pointer to the RecordList containing records.
 * - filename: Name of the file where records will be saved.
 *
 * Return Value:
 * - None.
 *
 * Side Effects:
 * - Creates or overwrites the specified file.
 * - Writes all record data into the file.
 * - Prints status messages to the terminal.
 */
void saveToFile(const RecordList *list, const char *filename){
    FILE *file = fopen(filename, "w");

    // Vulnerability ID: 4-4 Failure to Handle Errors Correctly
    // Resolution: Checks whether the file opened successfully
    // before writing records.
    if(file == NULL){
        printf("Error opening file\n");
        return;
    }

    for(int i = 0; i < list->size; i++){
        // Vulnerability ID: 2-2 Format String Problems
        // Resolution: Uses a fixed fprintf() format string
        // when saving records to the file.
        fprintf(file, "%s %s %.2f\n", list->records[i].type, list->records[i].category, list->records[i].amount);
    }

    fclose(file);
    printf("Records saved to file successfully.\n");
}

/*
 * Function: loadFromFile
 * Purpose: Loads records from a text file and stores them
 * into dynamic memory.
 *
 * Parameters:
 * - list: Pointer to the RecordList where loaded records
 *   will be stored.
 * - filename: Name of the file to load records from.
 *
 * Return Value:
 * - None.
 *
 * Side Effects:
 * - Clears the current list size before loading records.
 * - May resize the dynamic array if additional space is needed.
 * - Prints status and validation messages to the terminal.
 */
void loadFromFile(RecordList *list, const char *filename){
    FILE *file = fopen(filename, "r");

    if(file == NULL){
        printf("Error opening file\n");
        return;
    }

    list->size = 0;

    while(1){
        // Expands the dynamic array if more records
        // are loaded than the current capacity allows.
        if (list->size == list->capacity) {
            resizeList(list);
        }

        // Vulnerability ID: 1-3 Buffer Overruns
        // Resolution: Uses width limits (%49s) to prevent
        // reading too many characters into fixed-size arrays.
        int result = fscanf(file, "%49s %49s %f", list->records[list->size].type, list->records[list->size].category, &list->records[list->size].amount);

        if (result == EOF) {
            break;
        }

        // Vulnerability ID: 4-5 Failure to Handle Errors Correctly
        // Resolution: Validates that each file record
        // contains exactly three valid values.
        if (result != 3) {
            printf("Invalid record format in file\n");
            break;
        }

        list->size++;
    }

    fclose(file);
    printf("Records loaded from file successfully.\n");
}

/*
 * Function: deleteRecord
 * Purpose: Removes a selected record from the RecordList.
 *
 * Parameters:
 * - list: Pointer to the RecordList containing records.
 *
 * Return Value:
 * - None.
 *
 * Side Effects:
 * - Removes a record from dynamic memory.
 * - Shifts remaining records left to fill the empty position.
 * - Decreases the total number of stored records.
 * - Prints prompts and validation messages to the terminal.
 */
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

    // Shifts all records after the deleted record
    // one position to the left.
    for (int i = index; i < list->size - 1; i++) {
        list->records[i] = list->records[i + 1];
    }

    list->size--;

    printf("Record deleted successfully.\n");
}

/*
 * Function: editRecord
 * Purpose: Allows the user to modify an existing record's
 * type, category, and amount.
 *
 * Parameters:
 * - list: Pointer to the RecordList containing records.
 *
 * Return Value:
 * - None.
 *
 * Side Effects:
 * - Modifies an existing record in dynamic memory.
 * - Prints prompts, validation messages, and update results
 *   to the terminal.
 */
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

    if(fgets(input, sizeof(input), stdin) == NULL){
        printf("Error reading record number.\n");
        return;
    }

    if(sscanf(input, "%d", &recordNumber) != 1){
        printf("Invalid record number.\n");
        return;
    }

    if(recordNumber < 1 || recordNumber > list->size){
        printf("Record number out of range.\n");
        return;
    }

    index = recordNumber - 1;

    printf("Enter new type Income or Expense: ");

    if(fgets(input, sizeof(input), stdin) == NULL){
        printf("Error reading type.\n");
        return;
    }

    input[strcspn(input, "\n")] = '\0';

    if(input[0] != '\0'){
        input[0] = toupper((unsigned char)input[0]);
    }

    if(strcmp(input, "Income") != 0 && strcmp(input, "Expense") != 0){
        printf("Invalid type. Must be Income or Expense.\n");
        return;
    }

    // Vulnerability ID: 1-4 Buffer Overruns
    // Resolution: Uses strncpy() with size limits
    // and manual null termination.
    strncpy(list->records[index].type, input, sizeof(list->records[index].type) - 1);
    list->records[index].type[sizeof(list->records[index].type) - 1] = '\0';

    printf("Enter new category: ");

    if(fgets(input, sizeof(input), stdin) == NULL){
        printf("Error reading category.\n");
        return;
    }

    input[strcspn(input, "\n")] = '\0';

    if(strlen(input) == 0){
        printf("Category cannot be empty.\n");
        return;
    }

    // Vulnerability ID: 1-4 Buffer Overruns
    // Resolution: Uses strncpy() with size limits
    // and manual null termination.
    strncpy(list->records[index].category, input, sizeof(list->records[index].category) - 1);
    list->records[index].category[sizeof(list->records[index].category) - 1] = '\0';

    printf("Enter new amount: ");

    if(fgets(input, sizeof(input), stdin) == NULL){
        printf("Error reading amount.\n");
        return;
    }

    if(sscanf(input, "%f", &newAmount) != 1){
        printf("Invalid amount.\n");
        return;
    }

    // Vulnerability ID: 3-3 Integer Overflows
    // Resolution: Prevents invalid or extremely large
    // edited amounts from being stored.
    if(newAmount < 0 || newAmount > MAX_AMOUNT){
        printf("Invalid amount range.\n");
        return;
    }

    list->records[index].amount = newAmount;

    printf("Record updated successfully.\n");
}

/*
 * Function: calculateTotal
 * Purpose: Calculates total income, total expenses,
 * and the net total from all stored records.
 *
 * Parameters:
 * - list: Pointer to the RecordList containing records.
 *
 * Return Value:
 * - None.
 *
 * Side Effects:
 * - Prints calculated totals to the terminal.
 */
void calculateTotal(const RecordList *list){
    float incomeTotal = 0.0f;
    float expenseTotal = 0.0f;

    // Loops through all records and separates income totals from expense totals.
    for(int i = 0; i < list->size; i++){
        if(strncmp(list->records[i].type, "Income", sizeof("Income")) == 0){
            incomeTotal += list->records[i].amount;
        } else if(strncmp(list->records[i].type, "Expense", sizeof("Expense")) == 0) {
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

    // Validates the number of command line arguments before starting the program.
    if(argc > 2){
        printf("Usage: %s [records_file]\n", argv[0]);
        return 1;
    }

    // Allows the user to provide a custom records file through the command line.
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
        
        // Vulnerability ID: 5-1 Poor Usability
        // Resolution: Validates menu input before
        // executing any menu action.
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

    // Releases dynamically allocated memory before the program exits.
    free(list.records); 
    return 0;
}