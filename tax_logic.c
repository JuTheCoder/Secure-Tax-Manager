/*
 * This file calculates total income, total expenses, deductible expenses,
 * taxable income, tax owed, deductible percentage, and effective tax rate using 
 * the calculateTax() function
 * 
 * Parameters:
 * - list: Pointer to a RecordList containing income and expense records.
 *
 * Return Value:
 * - None.
 *
 * Side Effects:
 * - Prints the tax breakdown and tax estimate to the terminal.
 * - Creates or overwrites tax_report.txt.
 *
 */


#include <stdio.h>
#include "tax_logic.h"
#include "utils.h"

#define MAX_TOTAL 1000000000.0f

void calculateTax(RecordList *list) {
    float incomeTotal = 0.0f;
    float expenseTotal = 0.0f;
    float deductibleTotal = 0.0f;
    float taxableIncome = 0.0f;
    float taxOwed = 0.0f;
    float remaining;
    float taxedAmount;
    float taxPart;
    float deductiblePercent = 0.0f;
    float effectiveTaxRate = 0.0f;

    // Vulnerability ID: 4-5 Failure to Handle Errors Correctly
    // Resolution: Checks that the RecordList and records pointer are valid
    // before accessing record data.
    if (list == NULL || list->records == NULL) {
        printf("Error: Invalid record list.\n");
        return;
    }

    // Vulnerability ID: 5-4 Poor Usability
    // Resolution: Gives the user a clear message when there are no records
    // available instead of showing an empty or confusing tax result.
    if (list->size == 0) {
        printf("No records available to calculate tax.\n");
        return;
    }

    // Adds all income records together to calculate the user's total income and expenses
    for (int i = 0; i < list->size; i++) {
        if (equalsIgnoreCase(list->records[i].type, "Income")) {
            incomeTotal += list->records[i].amount;
        }
        else if (equalsIgnoreCase(list->records[i].type, "Expense")) {
            expenseTotal += list->records[i].amount;

            // Checks whether the expense category qualifies as a deductible expense for tax calculations.
            if (isDeductibleCategory(list->records[i].category)) {
                deductibleTotal += list->records[i].amount;
            }
        }

        // Vulnerability ID: 3-2 Integer Overflows
        // Resolution: Checks calculated totals against MAX_TOTAL and stops the
        // calculation if the values become too large.
        if (incomeTotal > MAX_TOTAL ||
            expenseTotal > MAX_TOTAL ||
            deductibleTotal > MAX_TOTAL) {
            printf("Error: totals are too large. Possible overflow risk.\n");
            return;
        }
    }

    // Taxable income is calculated by subtracting deductible expenses from total income.
    taxableIncome = incomeTotal - deductibleTotal;

    if(taxableIncome <= 0){
        printf("\n-- Tax Estimate --\n");
        printf("Total Income: %.2f\n", incomeTotal);
        printf("Total Expenses: %.2f\n", expenseTotal);
        printf("Deductible Expenses: %.2f\n", deductibleTotal);
        printf("No taxable income.\n");
        return;
    }

    // Remaining tracks how much taxable income still needs to be processed through tax brackets.
    remaining = taxableIncome;

    printf("\n-- Tax Breakdown --\n");

    // First tax bracket taxes up to the first $10,000 at 10%.
    if(remaining > 0){
        // If remaining income is greater than 10,000, only 10,000 is taxed in this bracket.
        taxedAmount = remaining > 10000.0f ? 10000.0f : remaining;
        taxPart = taxedAmount * 0.10f;
        taxOwed += taxPart;                                     // Adds this bracket's tax amount to the total tax owed.
        printf("10%% on %.2f = %.2f\n", taxedAmount, taxPart);
        remaining -= taxedAmount; // Removes the already-taxed amount before continuing to the next tax bracket.
    }

    // Second tax bracket taxes the next $30,000 at 12%.
    if(remaining > 0){
        taxedAmount = remaining > 30000.0f ? 30000.0f : remaining;
        taxPart = taxedAmount * 0.12f;
        taxOwed += taxPart;
        printf("12%% on %.2f = %.2f\n", taxedAmount, taxPart);
        remaining -= taxedAmount;
    }

    // Final tax bracket any remaining taxable income is taxed at 22%.
    if(remaining > 0){
        taxPart = remaining * 0.22f;
        taxOwed += taxPart;
        printf("22%% on %.2f = %.2f\n", remaining, taxPart);
    }

    if(expenseTotal > 0){
        // Calculates the percentage of expenses that were considered deductible.
        deductiblePercent = (deductibleTotal / expenseTotal) * 100.0f;
    }

    if(incomeTotal > 0){
        // Calculates the effective tax rate, showing what percentage of income was paid in taxes.
        effectiveTaxRate = (taxOwed / incomeTotal) * 100.0f;
    }

    printf("\n-- Tax Estimate --\n");
    printf("Total Income: %.2f\n", incomeTotal);
    printf("Total Expenses: %.2f\n", expenseTotal);
    printf("Deductible Expenses: %.2f\n", deductibleTotal);
    printf("Deductible Expense Percentage: %.2f%%\n", deductiblePercent);
    printf("Taxable Income: %.2f\n", taxableIncome);
    printf("Estimated Tax Owed: %.2f\n", taxOwed);
    printf("Effective Tax Rate: %.2f%%\n", effectiveTaxRate);

    // Vulnerability ID: 5-5 Poor Usability
    // Resolution: Warns the user when expenses exceed income, helping identify
    // unusual or possibly incorrect financial data.
    if(expenseTotal > incomeTotal){
        printf("Warning: Expenses exceed income.\n");
    }

    // Vulnerability ID: 5-6 Poor Usability
    // Resolution: Warns the user when deductible expenses are unusually high
    // compared to income.
    if(deductibleTotal > incomeTotal * 0.80f){
        printf("Warning: Deductible expenses are very high compared to income.\n");
    }

    // Vulnerability ID: 5-7 Poor Usability
    // Resolution: Alerts the user about a possible suspicious deduction pattern.
    if(deductibleTotal > incomeTotal * 0.90f){
        printf("Alert: Possible suspicious deduction pattern detected.\n");
    }

    FILE *report = fopen("tax_report.txt", "w");

    // Vulnerability ID: 4-4 Failure to Handle Errors Correctly
    // Resolution: Checks whether tax_report.txt opened successfully before
    // attempting to write the tax report.
    if(report == NULL){
        printf("Warning: Could not save tax report.\n");
        return;
    }

    fprintf(report, "-- Tax Report --\n");
    fprintf(report, "Total Income: %.2f\n", incomeTotal);
    fprintf(report, "Total Expenses: %.2f\n", expenseTotal);
    fprintf(report, "Deductible Expenses: %.2f\n", deductibleTotal);
    fprintf(report, "Deductible Expense Percentage: %.2f%%\n", deductiblePercent);
    fprintf(report, "Taxable Income: %.2f\n", taxableIncome);
    fprintf(report, "Estimated Tax Owed: %.2f\n", taxOwed);
    fprintf(report, "Effective Tax Rate: %.2f%%\n", effectiveTaxRate);

    fclose(report);

    printf("Tax report saved to tax_report.txt\n");
}