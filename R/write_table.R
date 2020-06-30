
#' @export
write_table <- function(object,
                        filepath,
                        extension = infer_extension(binary, compression),
                        truncate = TRUE,
                        binary = TRUE,
                        compression = 1L) {

    stopifnot(is_data_frame(object))
    stopifnot(is_scalar_character(filepath))
    stopifnot(is_scalar_logical(truncate))
    stopifnot(is_scalar_logical(binary))
    stopifnot(is_scalar_integer(compression) || is_scalar_real(compression))

    compression <- as.integer(compression)

    stopifnot(is_scalar_character(extension))

    filepath <- paste(filepath, extension, sep = ".")

    ## if file exists and should not be truncated, then we throw an error
    if (file.exists(filepath) && !truncate) {
        message <- sprintf("%s already exists and cannot be overwritten because truncate is FALSE", filepath)
        stop(message)
    }

    invisible(.Call(C_streamr_write_table,
                    object,
                    filepath,
                    truncate,
                    binary,
                    compression))
}
