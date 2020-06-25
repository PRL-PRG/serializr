#ifndef SERIALIZR_READER_HPP
#define SERIALIZR_READER_HPP

#include <string>
#include <Rinternals.h>

namespace serializr {

SEXP read_data_table(const std::string& filepath, bool binary, int compression);

}

#endif /* SERIALIZR_READER_HPP */
