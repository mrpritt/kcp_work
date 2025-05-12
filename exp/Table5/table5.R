library(tidyverse)

df <- read_csv("table5.csv") 
df_long <- df |> pivot_longer(cols = c(ub_l2, ub_l2g), names_to = "type", values_to = "value")

ggplot(df, aes(x = ub_l2, y = ub_l2g)) +
	geom_point() +
	scale_x_log10() +
	scale_y_log10() +
	geom_abline() +
	labs(x = "ub_l2", y = "ub_l2g") +
	theme_minimal()

ggsave("ubl2_ubl2g.png", width = 8, height = 6, dpi = 300)

ggplot(df_long, aes(x = lp2, y = value, color = type )) +
	geom_point() +
	scale_x_log10() +
	scale_y_log10() +
	geom_abline() +
	labs(x = "lp2", y = "ub", color = "bound") +
	theme_minimal()

ggsave("lp2_ub.png", width = 8, height = 6, dpi = 300)

