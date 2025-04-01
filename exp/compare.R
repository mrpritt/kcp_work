library("ggplot2")
df <- read.csv("test.csv", sep=";")
df <- reshape(df, idvar="instance", timevar="alg", direction="wide")
df <- df[df$weight.combo != 0, ]

ggplot(df, aes(x=nodes.mcqdw, y=nodes.combo))
	+ geom_point()
	+ geom_abline() 
	+ scale_x_log10()
	+ scale_y_log10()
df$ratio = df$nodes.combo / df$nodes.mcqdw
df[, c("instance", "ratio")]

