library(tidyverse)


# Read files
files = list.files(pattern = "C.*\\.csv")

df = files |>
      set_names(basename) |>
      map(\(file) read_delim(file, delim = ";")) |>
      list_rbind(names_to = "filename") |>
      mutate(
        prefix     = str_extract(filename, "^[^-]+"),
        class      = str_sub(prefix, 1, 1),
        mult       = as.integer(str_sub(prefix, 2)),
        suffix     = str_remove(str_extract(filename, "(?<=-).+\\.csv"), ".csv"),
        algo       = str_extract(suffix, "^[^0-9]+"),
        time_limit = as.integer(str_extract(suffix, "[0-9]+"))
      ) |>
      separate_wider_delim(instance, delim="_", names=c(NA,"type",NA,NA,"density")) |>
      mutate(type = as.integer(type), density = as.numeric(str_sub(density, 0, 3))) |>
      mutate(nodes = coalesce(nodes, steps)) |>
      mutate(time = if_else(str_starts(algo, "rlx"), time / 1000, time)) |>
      select(class, mult, type, density, weight, nodes, time, algo, time_limit)

df
