library("ggplot2")

# === Parameters to change ===
file <- "benchmark60.csv"
algo1 <- "rlx_ub"
algo2 <- "mcqdw"
metric1 <- "time"
metric2 <- metric1
# ============================

# Load data
df <- read.csv(file)

# Reshape wide by algorithm
df <- reshape(df, idvar = "instance", timevar = "algo", direction = "wide")

# Dynamically build column names
col1 <- paste0(metric1, ".", algo1)
col2 <- paste0(metric2, ".", algo2)

# Filter out zeros in both metrics (to avoid log10 issues)
df <- df[df[[col1]] != 0 & df[[col2]] != 0, ]

# Plot comparison
ggplot(df, aes(x = .data[[col1]], y = .data[[col2]])) +
  geom_point() +
  geom_abline() +
  scale_x_log10(limits = c(1, NA)) +
  scale_y_log10(limits = c(1, NA)) +
  coord_fixed() +
  xlab(paste(metric1, algo1, sep = " - ")) +
  ylab(paste(metric2, algo2, sep = " - ")) +
  ggtitle(paste("Comparison of", metric1, "and", metric2, "between", algo1, "and", algo2))

# Compute ratio and show
df$ratio <- (df[[col1]] - df[[col2]]) / df[[col1]]
i_by_ratio <- df[, c("instance", "ratio")]
i_by_ratio
mean(i_by_ratio$ratio)

################################################################################

# library("ggplot2")
# df <- read.csv("benchmark.csv")
# df <- reshape(df, idvar="instance", timevar="algo", direction="wide")
# df <- df[df$weight.combo != 0, ]
#
# df$r.combo_ub = (df$n_cut_weight.combo_ub/df$nodes.combo_ub)
# df$r.mcqdw = (df$n_cut_weight.mcqdw/df$nodes.mcqdw)
#
# df <- df[df$r.combo_ub != 0 & df$r.mcqdw != 0, ]
#
# ggplot(df, aes(x=r.mcqdw, y=r.combo_ub)) +
#     geom_point() +
#     geom_abline() +
#     # scale_x_log10(limits = c(1, NA)) +   # Start from log10(1) = 0
#     # scale_y_log10(limits = c(1, NA)) +    # Ditto
#     coord_fixed()
#
# df$ratio = df$r.combo_ub / df$r.mcqdw
#
# df <- df[, c("instance", "ratio")]
# df
# mean(df$ratio)
#
