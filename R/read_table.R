
#' @export
#' @importFrom utils read.table
read_table <- function(filepath) {
    options <- infer_format(filepath)
    binary <- options$binary
    compression <- options$compression

    if (!binary & compression == 0)
        read.table(filepath,
                   header = TRUE,
                   sep = "\x1f",
                   comment.char = "",
                   stringsAsFactors = FALSE)
    else {
        .Call(C_serializr_read_table, filepath, binary, compression)
    }
}
