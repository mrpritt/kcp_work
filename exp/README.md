# What's here

We compare two algorithms:
1 .Coniglio's CFS algorithm (branch and bound)
2. Rozman maximum clique solver (called insidrug), on the complement graph, and otherwise with no modification, so the weight limit is ignored

- results-C15-CFS<T>.csv: CFS in T seconds, T \in {10,60};
- results-C15-insidrug<T>.csv: insidrug in T seconds, T \in {10,60}
- results-C15-insidrugkcp<T>.csv: a first (but buggy) modification to consider weights, essentially the same as results-C15-insidrug<T>.csv
