#include "reader.h"
#include "utilities.h"
#include "BinaryDataTableStream.h"
#include "TextDataTableStream.h"

namespace streamr {

static SEXP read_uncompressed_binary_data_table(const std::string& filepath);

static SEXP read_compressed_binary_data_table(const std::string& filepath,
                                              int compression_level);

static SEXP read_text_data_table(const std::string& filepath,
                                 int compression_level);

SEXP read_data_table(const std::string& filepath,
                     bool binary,
                     int compression) {
    if (binary) {
        if (compression == 0) {
            return read_uncompressed_binary_data_table(filepath);
        } else {
            return read_compressed_binary_data_table(filepath, compression);
        }

    } else {
        return read_text_data_table(filepath, compression);
    }
}

struct data_frame_t {
    SEXP object;
    std::size_t row_count;
    std::size_t column_count;
    std::vector<SEXP> columns;
    std::vector<DataTableStream::column_type_t> column_types;
};

static data_frame_t read_header(const char* buffer, const char** end) {
    data_frame_t data_frame{nullptr, 0, 0, {}, {}};
    std::size_t character_size = 1024 * 1024;
    char* character_value = static_cast<char*>(malloc(character_size));

    int row_count = parse_integer(buffer, end);
    int column_count = parse_integer(*end, end);

    data_frame.row_count = row_count;
    data_frame.column_count = column_count;

    data_frame.object = PROTECT(allocVector(VECSXP, column_count));
    SEXP column_names = PROTECT(allocVector(STRSXP, column_count));
    SEXP row_names = PROTECT(allocVector(STRSXP, row_count));

    data_frame.columns.reserve(column_count);
    data_frame.column_types.reserve(column_count);

    for (int column_index = 0; column_index < column_count; ++column_index) {
        SEXP name =
            parse_character(*end, end, &character_value, &character_size);
        SET_STRING_ELT(column_names, column_index, name);
        SEXPTYPE sexptype = parse_sexptype(*end, end);
        uint32_t size = parse_integer(*end, end);
        data_frame.column_types.push_back({sexptype, size});
        SEXP column = PROTECT(allocVector(sexptype, row_count));
        data_frame.columns.push_back(column);
        SET_VECTOR_ELT(data_frame.object, column_index, column);
    }

    for (int row_index = 0; row_index < row_count; ++row_index) {
        sprintf(character_value, "%d", row_index + 1);
        SET_STRING_ELT(row_names, row_index, mkChar(character_value));
    }

    setAttrib(data_frame.object, R_RowNamesSymbol, row_names);
    setAttrib(data_frame.object, R_NamesSymbol, column_names);
    setAttrib(data_frame.object, R_ClassSymbol, mkString("data.frame"));
    free(character_value);
    UNPROTECT(3);
    return data_frame;
}

static SEXP read_uncompressed_binary_data_table(const std::string& filepath) {
    memory_block_t block = map_to_memory(filepath);
    void* buf = block.origin;
    std::size_t buffer_size = block.size;
    const char* buffer = static_cast<const char*>(buf);
    const char* const end_of_buffer = buffer + buffer_size;
    const char* end = nullptr;
    data_frame_t data_frame{read_header(buffer, &end)};
    std::size_t character_size = 1024 * 1024;
    char* character_value = static_cast<char*>(malloc(character_size));

    int row_index = 0;
    int column_index = 0;
    while (row_index < data_frame.row_count) {
        switch (data_frame.column_types[column_index].first) {
        case LGLSXP:
            LOGICAL(data_frame.columns[column_index])
            [row_index] = parse_logical(end, &end);
            break;

        case INTSXP:
            INTEGER(data_frame.columns[column_index])
            [row_index] = parse_integer(
                end, &end, data_frame.column_types[column_index].second);
            break;

        case REALSXP:
            REAL(data_frame.columns[column_index])
            [row_index] = parse_real(end, &end);
            break;

        case STRSXP:
            SET_STRING_ELT(
                data_frame.columns[column_index],
                row_index,
                parse_character(end, &end, &character_value, &character_size));
            break;

        default:
            Rf_error("unhandled column type %d of column %d in %s ",
                     data_frame.column_types[column_index].first,
                     column_index,
                     filepath.c_str());
        }

        ++column_index;
        if (column_index == data_frame.column_count) {
            column_index = 0;
            ++row_index;
        }
    }

    UNPROTECT(data_frame.column_count);
    std::free(character_value);
    unmap_memory(buf, buffer_size);
    return data_frame.object;
}

static SEXP read_compressed_binary_data_table(const std::string& filepath,
                                              int compression_level) {
    memory_block_t block = map_to_memory(filepath);
    void* buf = block.origin;
    std::size_t input_buffer_size = block.size;

    const char* input_buffer = static_cast<const char*>(buf);

    const char* const input_buffer_end = input_buffer + input_buffer_size;

    const char* input_buffer_current = nullptr;

    data_frame_t data_frame{read_header(input_buffer, &input_buffer_current)};

    ZSTD_inBuffer input{
        input_buffer_current, input_buffer_end - input_buffer_current, 0};

    std::size_t character_size = 1024 * 1024;
    char* character_value = static_cast<char*>(malloc_or_die(character_size));

    std::size_t output_buffer_size = ZSTD_DStreamOutSize();
    char* output_buffer = static_cast<char*>(malloc_or_die(output_buffer_size));
    std::size_t current_output_buffer_size = output_buffer_size;

    ZSTD_DStream* decompression_stream = ZSTD_createDStream();

    if (decompression_stream == NULL) {
        fprintf(stderr, "ZSTD_createDStream() error \n");
        exit(EXIT_FAILURE);
    }

    const std::size_t init_result = ZSTD_initDStream(decompression_stream);

    if (ZSTD_isError(init_result)) {
        fprintf(stderr,
                "ZSTD_initDStream() error : %s \n",
                ZSTD_getErrorName(init_result));
        exit(EXIT_FAILURE);
    }

    std::size_t remaining_bytes = 0;
    int row_index = 0;
    int column_index = 0;
    bool have_enough_data = false;
    const char* output_buffer_cur = nullptr;
    const char* output_buffer_end = nullptr;

    /* run the loop while there is uncompressed data in input file */
    while (input.pos < input.size) {
        ZSTD_outBuffer output = {output_buffer + remaining_bytes,
                                 current_output_buffer_size - remaining_bytes,
                                 0};

        std::size_t decompressed_bytes =
            ZSTD_decompressStream(decompression_stream, &output, &input);

        have_enough_data = true;

        if (ZSTD_isError(decompressed_bytes)) {
            fprintf(stderr,
                    "ZSTD_decompressStream() error : %s \n",
                    ZSTD_getErrorName(decompressed_bytes));
            exit(EXIT_FAILURE);
        }

        /* reading starts from beginning of output buffer to ensure that
           the previously unread partial content is also read */
        output_buffer_cur = output_buffer;
        output_buffer_end = output_buffer + output.pos + remaining_bytes;

        while ((row_index < data_frame.row_count) && have_enough_data) {
            SEXP column = data_frame.columns[column_index];

            switch (data_frame.column_types[column_index].first) {
            case LGLSXP: {
                if (can_parse_logical(output_buffer_cur, output_buffer_end)) {
                    LOGICAL(column)
                    [row_index] =
                        parse_logical(output_buffer_cur, &output_buffer_cur);
                } else {
                    have_enough_data = false;
                }
            }

            break;

            case INTSXP: {
                auto int_bytes = data_frame.column_types[column_index].second;

                if (can_parse_integer(
                        output_buffer_cur, output_buffer_end, int_bytes)) {
                    INTEGER(column)
                    [row_index] = parse_integer(
                        output_buffer_cur, &output_buffer_cur, int_bytes);
                } else {
                    have_enough_data = false;
                }
            }

            break;

            case REALSXP: {
                if (can_parse_real(output_buffer_cur, output_buffer_end)) {
                    REAL(column)
                    [row_index] =
                        parse_real(output_buffer_cur, &output_buffer_cur);
                } else {
                    have_enough_data = false;
                }
            }

            break;

            case STRSXP: {
                if (can_parse_character(output_buffer_cur, output_buffer_end)) {
                    SET_STRING_ELT(column,
                                   row_index,
                                   parse_character(output_buffer_cur,
                                                   &output_buffer_cur,
                                                   &character_value,
                                                   &character_size));
                } else {
                    have_enough_data = false;
                }
            }

            break;

            default: {
                Rf_error("unhandled column type %d of column %d in %s ",
                         data_frame.column_types[column_index].first,
                         column_index,
                         filepath.c_str());
            } break;
            }

            /* The column is read iff there is enough data in the buffer.
               Only if the column is read, do we increment the column_index
               and increase the row_index.
               If there is not enough data in the buffer to read a complete
               column, we have to take care of the partial column data. A simple
               solution is to realloc the buffer to twice its size. This
               algorithm will always work, but may result in a few reallocs
               initially until the buffer size is big enough. */
            if (have_enough_data) {
                ++column_index;
                if (column_index == data_frame.column_count) {
                    column_index = 0;
                    ++row_index;
                }
            } else {
                remaining_bytes = output_buffer_end - output_buffer_cur;

                std::memmove(output_buffer, output_buffer_cur, remaining_bytes);

                if (remaining_bytes + output_buffer_size <=
                    current_output_buffer_size) {
                    /* do nothing because this means that there is enough space
                       left on the buffer for another frame to be decompressed
                     */
                } else {
                    current_output_buffer_size = 2 * current_output_buffer_size;
                    output_buffer = static_cast<char*>(realloc_or_die(
                        output_buffer, current_output_buffer_size));
                }
            }
        }
    }

    UNPROTECT(data_frame.column_count);
    std::free(character_value);
    std::free(output_buffer);
    unmap_memory(buf, input_buffer_size);

    if (row_index < data_frame.row_count) {
        Rf_error("STREAMR ERROR: read_compressed_binary_data_table: ",
                 "input file processed completely but all rows not read.");
    }

    return data_frame.object;
}

static SEXP read_text_data_table(const std::string& filepath,
                                 int compression_level) {
    return R_NilValue;
}

} // namespace streamr
