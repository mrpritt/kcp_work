library(tidyverse)

cfs <- read_csv("C1-CFS600.csv") |>
  separate_wider_delim(
    instance,
    delim = "_",
    names = c(NA, "type", NA, "subtype", "density")
  ) |>
  mutate(
    class = "C",
    mult = 1,
    type = as.integer(type),
    subtype = as.integer(str_remove(subtype, ".txt")),
    density = as.numeric(str_remove(density, ".bz2"))
  ) |>
  filter(nodes > 0)
bb <- read_csv("bb.csv")
join_cols <- c("class", "mult", "type", "subtype", "density")
merge <- cfs |> inner_join(bb, by = join_cols)

ggplot(merge, aes(x = nodes.x, y = nodes.y)) +
  geom_point() +
  scale_x_log10() +
  scale_y_log10() +
  geom_abline()
