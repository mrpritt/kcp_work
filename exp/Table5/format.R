library(tidyverse)

df <- read_csv("data.csv")

df |> 
  separate_wider_delim(instance, delim="/", names=c("cm", "filename")) |>
  separate_wider_delim(filename, delim="_", names=c(NA, "type", NA, NA, "density")) |>
  mutate(type = as.integer(type), density = as.numeric(str_sub(density, 0, 3))) |>
  mutate(
    class = str_sub(cm, 1, 1),
    mult = as.integer(str_sub(cm, 2)),
    .before = 1
    ) |>
  select(-cm) |>
  write_csv("table5.csv")


