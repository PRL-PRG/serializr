#include "r_streamr.h"
#include "reader.h"
#include "writer.h"

SEXP r_streamr_read_table(SEXP r_filepath,
                            SEXP r_binary,
                            SEXP r_compression) {
    const std::string filepath = CHAR(asChar(r_filepath));
    bool binary = asLogical(r_binary);
    int compression = asInteger(r_compression);

    return streamr::read_data_table(filepath, binary, compression);
}

SEXP r_streamr_write_table(SEXP r_data_frame,
                             SEXP r_filepath,
                             SEXP r_truncate,
                             SEXP r_binary,
                             SEXP r_compression) {
    const std::string filepath = CHAR(asChar(r_filepath));
    bool truncate = asLogical(r_truncate);
    bool binary = asLogical(r_binary);
    int compression = asInteger(r_compression);

    return streamr::write_data_table(
        r_data_frame, filepath, truncate, binary, compression);
}
