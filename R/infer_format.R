
infer_format <- function(filepath) {

    if (endsWith(filepath, ".csv")) {
        binary <- FALSE
        compression <- 0
    }
    else if (endsWith(filepath, ".bin")) {
        binary <- TRUE
        remaining <- substr(filepath, 1, nchar(filepath) - 4)
        last_dot <- regexpr("\\.[^\\.]*$", remaining)[1]

        if (last_dot == -1) {
            message <- sprintf("%s does not specify compression in its extension", filepath)
            stop(message)
        }

        compression <- substr(remaining, last_dot + 1, nchar(remaining))
        compression <- as.numeric(compression)

        if (is.na(compression)) {
            message <- sprintf("%s does not specify numeric compression value in its extension", filepath)
            stop(message)
        }
    }
    else {
        message <- sprintf("%s does not have correct extension", filepath)
        stop(message)
    }

    list(binary = binary, compression = compression)
}
