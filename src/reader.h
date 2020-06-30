#ifndef STREAMR_READER_HPP
#define STREAMR_READER_HPP

#include <string>
#include <Rinternals.h>

namespace streamr {

SEXP read_data_table(const std::string& filepath, bool binary, int compression);

}

#endif /* STREAMR_READER_HPP */
