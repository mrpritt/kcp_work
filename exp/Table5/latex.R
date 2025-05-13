library(tidyverse)
library(knitr)
library(kableExtra)

df <- read_csv("table5.csv")

df <- df |>
  mutate(
    lp2 = abs(lp2 - ilp2)/ilp2,
    ub_l2 = abs(ub_l2 - ilp2)/ilp2,
    ub_l2g = abs(ub_l2g - ilp2)/ilp2,
    )

summary_df <- df %>%
  group_by(class, mult, type) %>%
  summarise(
    lp2 = 100 * mean(lp2),
    ub_l2 = 100 * mean(ub_l2),
    ub_l2g = 100 * mean(ub_l2g),
    .groups = "drop"
  )

summary_df

kable(summary_df, na = "-", format = "latex", digits = 1, booktabs = TRUE, linesep = c("", "", "", "\\addlinespace")) %>%
  kable_styling(latex_options = c("hold_position", "scale_down")) %>%
  writeLines("table.tex")

