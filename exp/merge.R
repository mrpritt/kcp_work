# List of algorithm names
algorithms <- c("mcqdw", "combo", "combo_cum", "combo_ub")

# Initialize an empty list to store dataframes
data_list <- list()

# Loop through each algorithm, read its corresponding CSV, and add the 'algo' column
for (algo in algorithms) {
  file_name <- paste0("benchmark_", algo, "60.csv")
  df <- read.csv(file_name, sep=";")
  df$algo <- algo
  data_list[[algo]] <- df
}

# Combine all dataframes into one
merged_df <- do.call(rbind, data_list)

# Optionally, write to a new CSV
write.csv(merged_df, "benchmark.csv", row.names = FALSE, quote=FALSE)

