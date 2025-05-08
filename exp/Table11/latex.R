library(tidyverse)
library(knitr)
library(kableExtra)

df <- read_csv("table11.csv")

df <- df %>%
  mutate(solved = ifelse(weight > 0, 1, 0),
         time = ifelse(weight == 0, time_limit, time))

summary_df <- df %>%
  group_by(class, mult, type, algo) %>%
  summarise(
    N = sum(solved),
    t = round(mean(time), 1),
    nn = round(mean(nodes)),
    .groups = "drop"
  )

wide_df <- summary_df %>%
  pivot_wider(
    names_from = algo,
    values_from = c(N, t, nn),
    names_glue = "{algo}.{.value}"
  )

wide_df <- wide_df %>%
  select(class, mult, type, matches("rlx\\..*"), matches("rlx_mt\\..*"), matches("CFS\\..*"))

custom_colnames <- c(
  "Class", "Mult", "Type",
  "Solved", "Time", "Nodes",
  "Solved", "Time", "Nodes",
  "Solved", "Time", "Nodes"
)

kable(wide_df, format = "latex", booktabs = TRUE, col.names = custom_colnames, linesep = c("", "", "", "", "", "", "", "\\addlinespace")) %>%
  kable_styling(latex_options = c("hold_position", "scale_down")) %>%
  add_header_above(c(" " = 3, "rlx" = 3, "rlx_mt" = 3, "CFS" = 3)) %>%
  writeLines("table.tex")

