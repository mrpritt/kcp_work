library(tidyverse)

bb <- read_csv("bb.csv") |> mutate(profit = as.integer(profit))
ilp2 <- read_csv("C1.csv") |> mutate(ilp2 = as.integer(round(ilp2)))

join_cols <- c("class", "mult", "type", "subtype", "density")
bb |>
  inner_join(ilp2, by = join_cols) |>
  select(all_of(join_cols), profit, ilp2, LBi, valid) |>
  filter(profit != ilp2)
