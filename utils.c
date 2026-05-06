#include <string.h>
#include <ctype.h>
#include "utils.h"

/*
 * Function: equalsIgnoreCase
 * Purpose: Compares two strings without
 * considering uppercase or lowercase letters.
 *
 * Parameters:
 * - a: First string.
 * - b: Second string.
 *
 * Return Value:
 * - Returns 1 if strings match ignoring case.
 * - Returns 0 if they do not match or are NULL.
 *
 * Side Effects:
 * - None.
 */
int equalsIgnoreCase(const char *a, const char *b) {
    // Prevent NULL pointer errors.
    if (a == NULL || b == NULL) {
        return 0;
    }

    // Compare characters one at a time.
    while (*a && *b) {

        // Convert both characters to lowercase before comparing.
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return 0;
        }
        a++;
        b++;
    }

    // Strings are equal only if both end together.
    return *a == '\0' && *b == '\0';
}

/*
 * Function: isDeductibleCategory
 * Purpose: Checks if a category is considered
 * tax deductible.
 *
 * Parameters:
 * - category: Category name to check.
 *
 * Return Value:
 * - Returns 1 if deductible.
 * - Returns 0 if not deductible or NULL.
 *
 * Side Effects:
 * - None.
 */
int isDeductibleCategory(const char *category) {

    // Prevent NULL pointer errors.
    if (category == NULL) {
        return 0;
    }

    // Compare against valid deductible categories.
    if (equalsIgnoreCase(category, "School") ||
        equalsIgnoreCase(category, "Medical") ||
        equalsIgnoreCase(category, "Business") ||
        equalsIgnoreCase(category, "Charity")) {
        return 1;
    }

    return 0;
}