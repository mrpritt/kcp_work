library(tidyverse)
library(knitr)
library(kableExtra)

options(knitr.kable.NA = '-') # Replace NA with '-'

inst_cols = c("class", "mult", "type", "subtype", "density")

ilp2 <- read_csv("ilp2.csv")
lp2 <- read_csv("lp2.csv")
ub_l2_v <- read_csv("ub_l2_v.csv")
ub_l2_v_ff <- read_csv("ub_l2_v_ff.csv")
ub_l2_i <- read_csv("ub_l2_i.csv")
ub_l2_i_ff <- read_csv("ub_l2_i_ff.csv")
ub_l2_v_ffr <- read_csv("ub_l2_v_ffr.csv")
ub_l2_weighted <- read_csv("weighted.csv")

table_weighted <- ilp2 |>
  inner_join(ub_l2_weighted, by = inst_cols) |>
  mutate(
    ub_l2 = (ub_l2 - ilp2) / ilp2 * 100,
    alpha = paste0("alpha_", str_replace(alpha, "-", "m"))
  )

tmp <- table_weighted |>
  filter(ascending == 1) |>
  pivot_wider(
    names_from = alpha,
    values_from = ub_l2,
  ) |>
  group_by(class, mult, type) |>
  summarise(
    across(starts_with("alpha_"), mean),
    .groups = "drop"
  )

kable(
  tmp,
  format = "latex",
  caption = "Weighted, ascending",
  label = "ub_l2_ascend",
  digits = 1,
  booktabs = TRUE,
  linesep = c("", "", "", "\\addlinespace")
) %>%
  kable_styling(latex_options = c("hold_position", "scale_down")) %>%
  writeLines("ub_l2_ascend.tex")

tmp <- table_weighted |>
  filter(ascending == 0) |>
  pivot_wider(
    names_from = alpha,
    values_from = ub_l2,
  ) |>
  group_by(class, mult, type) |>
  summarise(
    across(starts_with("alpha_"), mean),
    .groups = "drop"
  )

kable(
  tmp,
  format = "latex",
  caption = "Weighted, descending",
  label = "ub_l2_descend",
  digits = 1,
  booktabs = TRUE,
  linesep = c("", "", "", "\\addlinespace")
) %>%
  kable_styling(latex_options = c("hold_position", "scale_down")) %>%
  writeLines("ub_l2_descend.tex")


## Table \hat{V} = V, Coniglio non-decreasing pi/wi
table_v <- ilp2 |>
  inner_join(lp2, by = inst_cols) |>
  inner_join(ub_l2_v, by = inst_cols)


table_v <- table_v %>%
  mutate(
    lp2 = abs(lp2 - ilp2) / ilp2 * 100,
    across(starts_with("ub_"), ~ abs(.x - ilp2) / ilp2 * 100)
  )

table_v_summary <- table_v %>%
  group_by(class, mult, type) %>%
  summarise(
    lp2 = mean(lp2),
    across(starts_with("ub_"), mean),
    .groups = "drop"
  )

table_v_summary

kable(
  table_v_summary,
  format = "latex",
  caption = "$\\hat{V} = V$, Coniglio non-decreasing",
  label = "ub_l2_v",
  digits = 1,
  booktabs = TRUE,
  linesep = c("", "", "", "\\addlinespace")
) %>%
  kable_styling(latex_options = c("hold_position", "scale_down")) %>%
  writeLines("ub_l2_v.tex")


## Table \hat{V} = V, Greedy first-fit non-increasing pi/wi
table_v_ff <- ilp2 |>
  inner_join(lp2, by = inst_cols) |>
  inner_join(ub_l2_v_ff, by = inst_cols)


table_v_ff <- table_v_ff %>%
  mutate(
    lp2 = abs(lp2 - ilp2) / ilp2 * 100,
    across(starts_with("ub_"), ~ abs(.x - ilp2) / ilp2 * 100)
  )

table_v_ff_summary <- table_v_ff %>%
  group_by(class, mult, type) %>%
  summarise(
    lp2 = mean(lp2),
    across(starts_with("ub_"), mean),
    .groups = "drop"
  )

table_v_ff_summary

kable(
  table_v_ff_summary,
  format = "latex",
  caption = "$\\hat{V} = V$, Greedy first-fit non-increasing",
  label = "ub_l2_v_ff",
  digits = 1,
  booktabs = TRUE,
  linesep = c("", "", "", "\\addlinespace")
) %>%
  kable_styling(latex_options = c("hold_position", "scale_down")) %>%
  writeLines("ub_l2_v_ff.tex")

## Table \hat{V} = I^*, Coniglio non-decreasing pi/wi
table_i <- ub_l2_i

table_i <- table_i %>%
  mutate(
    across(starts_with("ub_"), ~ abs(.x - ilp2) / ilp2 * 100)
  )

table_i_summary <- table_i %>%
  group_by(class, mult, type) %>%
  summarise(
    across(starts_with("ub_"), mean),
    .groups = "drop"
  )

table_i_summary

kable(
  table_i_summary,
  caption = "$\\hat{V} = I^*$, Coniglio non-decreasing",
  label = "ub_l2_i",
  format = "latex",
  digits = 1,
  booktabs = TRUE,
  linesep = c("", "", "", "\\addlinespace")
) %>%
  kable_styling(latex_options = c("hold_position", "scale_down")) %>%
  writeLines("ub_l2_i.tex")

## Table \hat{V} = I^*, Greedy first fit non-increasing pi/wi
table_i_ff <- ub_l2_i_ff

table_i_ff <- table_i_ff %>%
  mutate(
    across(starts_with("ub_"), ~ abs(.x - ilp2) / ilp2 * 100)
  )

table_i_ff_summary <- table_i_ff %>%
  group_by(class, mult, type) %>%
  summarise(
    across(starts_with("ub_"), mean),
    .groups = "drop"
  )

table_i_ff_summary

kable(
  table_i_ff_summary,
  caption = "$\\hat{V} = I^*$, Greedy first fit non-increasing",
  label = "ub_l2_i_ff",
  format = "latex",
  digits = 1,
  booktabs = TRUE,
  linesep = c("", "", "", "\\addlinespace")
) %>%
  kable_styling(latex_options = c("hold_position", "scale_down")) %>%
  writeLines("ub_l2_i_ff.tex")


## Table \hat{V} = V, Greedy first-fit non-increasing pi/wi repeated at each iteration
table_v_ffr <- ilp2 |>
  inner_join(lp2, by = inst_cols) |>
  inner_join(ub_l2_v_ffr, by = inst_cols)


table_v_ffr <- table_v_ffr %>%
  mutate(
    lp2 = abs(lp2 - ilp2) / ilp2 * 100,
    across(starts_with("ub_"), ~ abs(.x - ilp2) / ilp2 * 100)
  )

table_v_ffr_summary <- table_v_ffr %>%
  group_by(class, mult, type) %>%
  summarise(
    lp2 = mean(lp2),
    across(starts_with("ub_"), mean),
    .groups = "drop"
  )

table_v_ffr_summary

kable(
  table_v_ffr_summary,
  format = "latex",
  caption = "$\\hat{V} = V$, Greedy first-fit non-increasing repeated at each iteration",
  label = "ub_l2_v_ffr",
  digits = 1,
  booktabs = TRUE,
  linesep = c("", "", "", "\\addlinespace")
) %>%
  kable_styling(latex_options = c("hold_position", "scale_down")) %>%
  writeLines("ub_l2_v_ffr.tex")

## Bound comparison
df <- read_csv("data.csv")

df <- df %>%
  mutate(
    lp2 = abs(lp2 - ilp2) / ilp2 * 100,
    across(ends_with(".ub_l2"), ~ abs(.x - ilp2) / ilp2 * 100)
  )

df_summ <- df %>%
  group_by(class, mult, type) %>%
  summarise(
    lp2 = mean(lp2),
    across(ends_with(".ub_l2"), mean),
    .groups = "drop"
  )

df_summ

kable(
  df_summ,
  format = "latex",
  caption = "$\\hat{V} = V$, differing sorting strategies comparison",
  label = "ub_l2_comp",
  digits = 1,
  booktabs = TRUE,
  linesep = c("", "", "", "\\addlinespace")
) %>%
  kable_styling(latex_options = c("hold_position", "scale_down")) %>%
  writeLines("ub_l2_comp.tex")
