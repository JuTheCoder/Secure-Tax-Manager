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

    if (list == NULL || list->records == NULL) {
        printf("Error: Invalid record list.\n");
        return;
    }

    if (list->size == 0) {
        printf("No records available to calculate tax.\n");
        return;
    }

    for (int i = 0; i < list->size; i++) {
        if (equalsIgnoreCase(list->records[i].type, "Income")) {
            incomeTotal += list->records[i].amount;
        }
        else if (equalsIgnoreCase(list->records[i].type, "Expense")) {
            expenseTotal += list->records[i].amount;

            if (isDeductibleCategory(list->records[i].category)) {
                deductibleTotal += list->records[i].amount;
            }
        }

        if (incomeTotal > MAX_TOTAL ||
            expenseTotal > MAX_TOTAL ||
            deductibleTotal > MAX_TOTAL) {
            printf("Error: totals are too large. Possible overflow risk.\n");
            return;
        }
    }

    taxableIncome = incomeTotal - expenseTotal;

    if (taxableIncome <= 0) {
        printf("\n-- Tax Estimate --\n");
        printf("Total Income: %.2f\n", incomeTotal);
        printf("Total Expenses: %.2f\n", expenseTotal);
        printf("Deductible Expenses: %.2f\n", deductibleTotal);
        printf("No taxable income.\n");
        return;
    }

    remaining = taxableIncome;

    printf("\n-- Tax Breakdown --\n");

    if (remaining > 0) {
        taxedAmount = remaining > 10000.0f ? 10000.0f : remaining;
        taxPart = taxedAmount * 0.10f;
        taxOwed += taxPart;
        printf("10%% on %.2f = %.2f\n", taxedAmount, taxPart);
        remaining -= taxedAmount;
    }

    if (remaining > 0) {
        taxedAmount = remaining > 30000.0f ? 30000.0f : remaining;
        taxPart = taxedAmount * 0.12f;
        taxOwed += taxPart;
        printf("12%% on %.2f = %.2f\n", taxedAmount, taxPart);
        remaining -= taxedAmount;
    }

    if (remaining > 0) {
        taxPart = remaining * 0.22f;
        taxOwed += taxPart;
        printf("22%% on %.2f = %.2f\n", remaining, taxPart);
    }

    if (expenseTotal > 0) {
        deductiblePercent = (deductibleTotal / expenseTotal) * 100.0f;
    }

    if (incomeTotal > 0) {
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

    if (expenseTotal > incomeTotal) {
        printf("Warning: Expenses exceed income.\n");
    }

    if (deductibleTotal > incomeTotal * 0.80f) {
        printf("Warning: Deductible expenses are very high compared to income.\n");
    }

    if (deductibleTotal > incomeTotal * 0.90f) {
        printf("Alert: Possible suspicious deduction pattern detected.\n");
    }

    if (incomeTotal > 0 && deductibleTotal > incomeTotal * 0.80f) {
        printf("Warning: Deductible expenses are very high compared to income.\n");
    }   

    FILE *report = fopen("tax_report.txt", "w");

    if (report == NULL) {
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