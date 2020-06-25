#ifndef SERIALIZR_UTILITIES_H
#define SERIALIZR_UTILITIES_H

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <Rinternals.h>
#undef error

namespace serializr {

extern const char UNIT_SEPARATOR;
extern const char RECORD_SEPARATOR;

struct memory_block_t {
    void* origin;
    std::size_t size;
};

#define failwith(format, ...) \
    failwith_impl(__FILE__, __LINE__, format, __VA_ARGS__)

#define failwith_impl(file, line, format, ...)     \
    do {                                           \
        fprintf(stderr,                            \
                "serializr error [%s:%d] " format, \
                file,                              \
                line,                              \
                __VA_ARGS__);                      \
        exit(EXIT_FAILURE);                        \
    } while (0)

inline void* malloc_or_die(std::size_t size) {
    void* data = std::malloc(size);
    if (data == nullptr) {
        failwith("memory allocation error: unable to allocate %lu bytes.\n",
                 size);
    }
    return data;
}

inline void* calloc_or_die(std::size_t num, std::size_t size) {
    void* data = std::calloc(num, size);
    if (data == nullptr) {
        failwith("memory allocation error: unable to allocate %lu bytes.\n",
                 size);
    }
    return data;
}

inline void* realloc_or_die(void* ptr, std::size_t size) {
    void* data = std::realloc(ptr, size);
    if (data == nullptr) {
        failwith("memory allocation error: unable to reallocate %lu bytes.\n",
                 size);
    }
    return data;
}

int parse_integer(const char* buffer, const char** end, std::size_t bytes = 4);

bool can_parse_integer(const char* buffer,
                       const char* end,
                       std::size_t bytes = 4);

double parse_real(const char* buffer, const char** end);

bool can_parse_real(const char* buffer, const char* end);

int parse_logical(const char* buffer, const char** end);

bool can_parse_logical(const char* buffer, const char* end);

SEXPTYPE parse_sexptype(const char* buffer, const char** end);

SEXP parse_character(const char* buffer,
                     const char** end,
                     char** dest,
                     std::size_t* dest_size);

bool can_parse_character(const char* buffer, const char* end);

std::string sexptype_to_string(SEXPTYPE sexptype);

int open_file(const std::string& filepath, int flags, mode_t mode = 0666);

void close_file(int fd, const std::string& filepath);

memory_block_t map_to_memory(const std::string& filepath);

void unmap_memory(void* data, std::size_t size);

} // namespace serializr

#endif /* SERIALIZR_UTILITIES_H */
