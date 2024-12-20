library(tidyverse)
library(here)

wei = read_table("wei.dat", col_names=c("instance", "wei"))
our = read_csv2("results_hl-C1.csv", locale = locale(grouping_mark = ",",decimal_mark = "."))
all = merge(wei,our,by="instance") %>%
    mutate(hl_v=hl_v) %>%
    mutate(rd=rd(hl_v,wei)) %>%
    as_tibble()

p = ggplot(data=all, aes(x=wei,y=hl_v)) + geom_point() + geom_abline()

p
p + scale_x_log10() + scale_y_log10()

ggplot(data=all %>% filter(wei!=hl_v), aes(x=rd)) + geom_histogram()

nrd = all %>% filter(wei!=hl_v)

table(cut(nrd$rd,breaks=c(-70,-30,-10,-5,-3,-2,-1,0)))

ggplot(data=all, aes(x=hl_t)) + geom_histogram()
