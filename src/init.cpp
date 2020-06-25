#include "r_serializr.h"
#include <R_ext/Rdynload.h>

#include <stdio.h>

extern "C" {
static const R_CallMethodDef CallEntries[] = {
    /* instrumentr */
    {"serializr_read_table", (DL_FUNC) &r_serializr_read_table, 5},
    {"serializr_write_table", (DL_FUNC) &r_serializr_write_table, 3},

    {NULL, NULL, 0}};

void R_init_serializr(DllInfo* dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
}
