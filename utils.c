#include <string.h>
#include <ctype.h>
#include "utils.h"

int equalsIgnoreCase(const char *a, const char *b) {
    if (a == NULL || b == NULL) {
        return 0;
    }

    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return 0;
        }
        a++;
        b++;
    }

    return *a == '\0' && *b == '\0';
}

int isDeductibleCategory(const char *category) {
    if (category == NULL) {
        return 0;
    }

    if (equalsIgnoreCase(category, "School") ||
        equalsIgnoreCase(category, "Medical") ||
        equalsIgnoreCase(category, "Business") ||
        equalsIgnoreCase(category, "Charity")) {
        return 1;
    }

    return 0;
}