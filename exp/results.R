library(tidyverse)
library(here)

cfs60 = read_csv2("results-C15-CFS60.csv") %>% select(-`...5`)
ins60 = read_csv2("results-C15-insidrug60.csv") %>% select(-`...5`) %>% mutate(instance=str_remove(instance, "^c_"))

df =  cfs60 %>% inner_join(ins60, by="instance") %>%
    as_tibble()

df %>% summarize(N.cfs = sum(valid.x), N.ins = sum(valid.y))

ggplot(df, aes(x=weight.x, y=weight.y)) + geom_point()
