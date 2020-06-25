#ifndef SERIALIZR_WRITER_HPP
#define SERIALIZR_WRITER_HPP

#include <string>
#include <Rinternals.h>

namespace serializr {

SEXP write_data_table(SEXP r_data_frame,
                      const std::string& filepath,
                      bool truncate,
                      bool binary,
                      int compression);

}

#endif /* SERIALIZR_WRITER_HPP */
