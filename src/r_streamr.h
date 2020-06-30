#ifndef STREAMR_STREAMR_H
#define STREAMR_STREAMR_H

#include <R.h>
//#include <R_ext/Error.h>
#include <Rdefines.h>

extern "C" {
SEXP r_streamr_read_table(SEXP r_filepath, SEXP r_binary, SEXP r_compression);

SEXP r_streamr_write_table(SEXP r_data_frame,
                             SEXP r_filepath,
                             SEXP r_truncate,
                             SEXP r_binary,
                             SEXP r_compression);
}

#endif /* STREAMR_STREAMR_H */
