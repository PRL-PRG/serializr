#include "r_streamr.h"
#include <R_ext/Rdynload.h>

#include <stdio.h>

extern "C" {
static const R_CallMethodDef CallEntries[] = {
    /* instrumentr */
    {"streamr_read_table", (DL_FUNC) &r_streamr_read_table, 5},
    {"streamr_write_table", (DL_FUNC) &r_streamr_write_table, 3},

    {NULL, NULL, 0}};

void R_init_streamr(DllInfo* dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
}
