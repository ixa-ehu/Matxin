#include "matxin_string_utils.h"
#include <wctype.h>
#include <assert.h>

/*
 * zenbaki positiboak testutik int-era pasatzeko
 */
int watoi(const wchar_t *buffer) {
    return wcstol(buffer, NULL, 10);
}
