library(tidyverse)

## Assumes there is a column named 'instance' with the following format
## `.../<class><mult>/BPPC_<type>_0_<subtype>.txt_<density>`
## for example `~/C15/BPPC_4_0_1.txt_0.3.bz2`

args <- commandArgs(trailingOnly = TRUE)
if (length(args) != 1) {
  stop("Usage: Rscript format.R <filename>")
}
filename <- args[1]

df <- read_csv(filename)
df |>
  mutate(instance = str_extract(instance, "[^/]+/[^/]+$")) |>
  separate_wider_delim(instance, delim = "/", names = c("folder", "file")) |>
  mutate(
    class = str_sub(folder, 1, 1),
    mult = as.integer(str_sub(folder, 2)),
    .before = everything()
  ) |>
  select(-folder) |>
  separate_wider_delim(
    file,
    delim = "_",
    names = c(NA, "type", NA, "subtype", "density")
  ) |>
  mutate(
    type = as.integer(type),
    subtype = as.integer(str_remove(subtype, ".txt")),
    density = as.numeric(str_remove(density, ".bz2")),
    .after = mult
  ) |>
  write_csv(filename)
