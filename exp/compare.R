library("ggplot2")

# === Parameters to change ===
file <- "benchmark.csv"
algo1 <- "mcqdw"
algo2 <- "combo_ub"
metric1 <- "nodes"
metric2 <- metric1
# ============================

# Load data
df <- read.csv(file)

# Reshape wide by algorithm
df <- reshape(df, idvar = "instance", timevar = "algo", direction = "wide")

compare2 = function(df, algo1, algo2) {

  # Dynamically build column names
  col1 <- paste0(metric1, ".", algo1)
  col2 <- paste0(metric2, ".", algo2)
  
  # Filter out zeros in both metrics (to avoid log10 issues)
  ef <- df[df[[col1]] != 0 & df[[col2]] != 0, ]
  
  # Plot comparison
  p = ggplot(ef, aes(x = .data[[col1]], y = .data[[col2]])) +
    geom_point() +
    geom_abline() +
    scale_x_log10(limits = c(1, NA)) +
    scale_y_log10(limits = c(1, NA)) +
    coord_fixed() +
    xlab(paste(metric1, algo1, sep = " - ")) +
    ylab(paste(metric2, algo2, sep = " - ")) +
    ggtitle(paste("Comparison of", metric1, "and", metric2, "between", algo1, "and", algo2))
  
  # Compute ratio and show
  ef$ratio <- (ef[[col1]] - ef[[col2]]) / ef[[col1]]
  i_by_ratio <- ef[, c("instance", "ratio")]
  i_by_ratio
  print(mean(i_by_ratio$ratio))

  p
}

compare2(df, "mcqdw", "combo")

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
