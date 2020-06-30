#ifndef STREAMR_WRITER_HPP
#define STREAMR_WRITER_HPP

#include <string>
#include <Rinternals.h>

namespace streamr {

SEXP write_data_table(SEXP r_data_frame,
                      const std::string& filepath,
                      bool truncate,
                      bool binary,
                      int compression);

}

#endif /* STREAMR_WRITER_HPP */
