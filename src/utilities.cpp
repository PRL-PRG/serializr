#include "utilities.h"
#include <error.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>

namespace serializr {

int parse_integer(const char* buffer, const char** end, std::size_t bytes) {
    int value = 0;
    std::memcpy(&value, buffer, bytes);
    *end = buffer + bytes;
    return value;
}

bool can_parse_integer(const char* buffer, const char* end, std::size_t bytes) {
    return (buffer + bytes <= end);
}

double parse_real(const char* buffer, const char** end) {
    double value = 0.0;
    std::memcpy(&value, buffer, sizeof(double));
    *end = buffer + sizeof(double);
    return value;
}

bool can_parse_real(const char* buffer, const char* end) {
    return (buffer + sizeof(double) <= end);
}

int parse_logical(const char* buffer, const char** end) {
    int value = 0;
    std::memcpy(&value, buffer, sizeof(bool));
    *end = buffer + sizeof(bool);
    return value;
}

bool can_parse_logical(const char* buffer, const char* end) {
    return (buffer + sizeof(bool) <= end);
}

SEXPTYPE parse_sexptype(const char* buffer, const char** end) {
    std::uint32_t value = 0;
    std::memcpy(&value, buffer, sizeof(std::uint32_t));
    *end = buffer + sizeof(std::uint32_t);
    return value;
}

SEXP parse_character(const char* buffer,
                     const char** end,
                     char** dest,
                     std::size_t* dest_size) {
    std::uint32_t size = parse_integer(buffer, end, sizeof(std::uint32_t));

    if (size >= *dest_size) {
        free(*dest);
        *dest_size = 2 * size;
        *dest = static_cast<char*>(malloc_or_die(*dest_size));
    }

    std::memcpy(*dest, *end, size);
    *end = *end + size;
    (*dest)[size] = '\0';

    return mkChar(*dest);
}

bool can_parse_character(const char* buffer, const char* end) {
    std::size_t bytes = sizeof(std::uint32_t);

    if (!can_parse_integer(buffer, end, bytes)) {
        return false;
    }

    int character_size = 0;
    std::memcpy(&character_size, buffer, bytes);

    return (buffer + bytes + character_size <= end);
}

std::string sexptype_to_string(SEXPTYPE sexptype) {
    std::string str(type2char(sexptype));
    str[0] = std::toupper(str[0]);
    return str;
}

int open_file(const std::string& filepath, int flags, mode_t mode) {
    int fd;
    if ((fd = open(filepath.c_str(), flags, mode)) == -1) {
        error_at_line(1,
                      errno,
                      __FILE__,
                      __LINE__,
                      "unable to open '%s'",
                      filepath.c_str());
    }
    return fd;
}

void close_file(int fd, const std::string& filepath) {
    if (close(fd) == -1) {
        error_at_line(1,
                      errno,
                      __FILE__,
                      __LINE__,
                      "unable to close '%s'",
                      filepath.c_str());
    }
}

memory_block_t map_to_memory(const std::string& filepath) {
    int fd = open_file(filepath, O_RDONLY);

    struct stat file_info = {0};

    if (fstat(fd, &file_info) == -1) {
        error_at_line(1,
                      errno,
                      __FILE__,
                      __LINE__,
                      "unable to get size of '%s'",
                      filepath.c_str());
    }

    if (file_info.st_size == 0) {
        close_file(fd, filepath);
        return {NULL, 0};
    }

    void* data = mmap(
        NULL, file_info.st_size, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0);

    close_file(fd, filepath);

    if (data == MAP_FAILED) {
        error_at_line(1,
                      errno,
                      __FILE__,
                      __LINE__,
                      "failed to map '%s'",
                      filepath.c_str());
    }

    memory_block_t block = {data, static_cast<std::size_t>(file_info.st_size)};
    return block;
}

void unmap_memory(void* data, std::size_t size) {
    if (munmap(data, size)) {
        error_at_line(1, errno, __FILE__, __LINE__, "failed to unmap");
    }
}

} // namespace serializr
