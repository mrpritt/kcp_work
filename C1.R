library(tidyverse)

df <- read_csv("C1.csv") |>
    mutate(lp2 = (lp2 - ilp2)/ilp2 * 100,
            lp1 = (lp1 - ilp2)/ilp2 * 100)

t4 <- df |> filter(density == 0.1 | density == 0.3 | density == 0.6 | density == 0.9) |> 
    group_by(subtype, density) |>
    summarize(across(c("lp1", "lp2"), mean))

## subtype 10 matches table 4
t4 |> filter(subtype == 10)


t5 <- df |> filter(type == 1 | type == 4 | type == 5 | type == 8) |> 
    group_by(subtype, type) |>
    summarize(across(c("lp2"), mean))

# t5 |> write_csv("test.csv")
t5 |> filter(subtype == 10)


t6 <- df |> filter(density == 0.1 | density == 0.3 | density == 0.6 | density == 0.9) |> 
    group_by(subtype, density) |>
    summarize(across(c("lp2"), mean))

# t6 |> write_csv("test.csv")
t6 |> filter(subtype == 10)

