
infer_extension <- function(binary, compression) {
    if (!binary) {
        "csv"
    }
    else {
        paste(compression, "bin", sep = ".")
    }
}
