#ifndef SERIALIZR_SERIALIZR_H
#define SERIALIZR_SERIALIZR_H

#include <R.h>
//#include <R_ext/Error.h>
#include <Rdefines.h>

extern "C" {
SEXP r_serializr_read_table(SEXP r_filepath, SEXP r_binary, SEXP r_compression);

SEXP r_serializr_write_table(SEXP r_data_frame,
                             SEXP r_filepath,
                             SEXP r_truncate,
                             SEXP r_binary,
                             SEXP r_compression);
}

#endif /* SERIALIZR_SERIALIZR_H */
