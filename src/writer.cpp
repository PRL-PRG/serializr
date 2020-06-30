#include "writer.h"
#include "utilities.h"
#include "BinaryDataTableStream.h"
#include "TextDataTableStream.h"

namespace streamr {

DataTableStream* create_data_table(const std::string& filepath,
                                   const std::vector<std::string>& column_names,
                                   bool truncate,
                                   bool binary,
                                   int compression) {
    DataTableStream* stream = nullptr;
    if (binary) {
        stream = new BinaryDataTableStream(
            filepath, column_names, truncate, compression);
    } else {
        stream = new TextDataTableStream(
            filepath, column_names, truncate, compression);
    }
    return stream;
}

SEXP write_data_table(SEXP r_data_frame,
                      const std::string& filepath,
                      bool truncate,
                      bool binary,
                      int compression) {
    std::size_t column_count = LENGTH(r_data_frame);
    std::vector<std::string> column_names{column_count, ""};
    std::vector<SEXP> columns{column_count, R_NilValue};
    std::vector<DataTableStream::column_type_t> column_types{column_count,
                                                             {NILSXP, 0}};
    SEXP column_name_list = getAttrib(r_data_frame, R_NamesSymbol);
    SEXP column_name_str = R_NilValue;

    for (int column_index = 0; column_index < column_count; ++column_index) {
        columns[column_index] = VECTOR_ELT(r_data_frame, column_index);
        column_types[column_index] =
            std::make_pair((SEXPTYPE) TYPEOF(columns[column_index]), 0);

        if (column_name_list == R_NilValue) {
            continue;
        }

        column_name_str = STRING_ELT(column_name_list, column_index);

        if (TYPEOF(column_name_str) == CHARSXP) {
            column_names[column_index] = CHAR(column_name_str);
        }
    }

    std::size_t row_count = column_count == 0 ? 0 : LENGTH(columns[0]);

    DataTableStream* stream = create_data_table(
        filepath, column_names, truncate, binary, compression);

    for (int row_index = 0; row_index < row_count; ++row_index) {
        for (int column_index = 0; column_index < column_count;
             ++column_index) {
            switch (column_types[column_index].first) {
            case LGLSXP:
                stream->write_column(static_cast<bool>(
                    LOGICAL(columns[column_index])[row_index]));
                break;

            case INTSXP:
                stream->write_column(static_cast<int>(
                    INTEGER(columns[column_index])[row_index]));
                break;

            case REALSXP:
                stream->write_column(static_cast<double>(
                    REAL(columns[column_index])[row_index]));
                break;

            case STRSXP:
                stream->write_column(std::string(
                    CHAR(STRING_ELT(columns[column_index], row_index))));
                break;
            }
        }
    }

    delete stream;

    return R_NilValue;
}

} // namespace streamr
