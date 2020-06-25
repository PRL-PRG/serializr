is_scalar_character <- function(object) {
    is.character(object) && (length(object) == 1) && (nchar(object) != 0)
}

is_scalar_real <- function(object) {
    is.double(object) && (length(object) == 1)
}

is_vector_character <- function(object) {
    is.character(object)
}

is_scalar_logical <- function(object) {
    is.logical(object) && (length(object) == 1)
}

is_scalar_integer <- function(object) {
    is.integer(object) && (length(object) == 1)
}

is_environment <- function(object) {
    is.environment(object)
}

is_function <- function(object) {
    is.function(object)
}

is_null <- function(object) {
    is.null(object)
}

is_closure <- function(object) {
    typeof(object) == "closure"
}

is_language <- function(object) {
    is.language(object)
}

is_data_frame <- function(object) {
    is.data.frame(object)
}
