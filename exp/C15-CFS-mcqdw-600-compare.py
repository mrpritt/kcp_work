import pandas as pd

# Load CSV files
cfs600 = pd.read_csv("C15-CFS600.csv", sep=";")
mcqdw600 = pd.read_csv("C15-mcqdw600.csv", sep=";")

# Count solved instances
solved_cfs = (cfs600["weight"] > 0).sum()
solved_mcqdw = (mcqdw600["weight"] > 0).sum()
print(f"Solved instances in CFS600: {solved_cfs}")
print(f"Solved instances in MCQDW600: {solved_mcqdw}")

# Merge dataframes on 'instance'
merged = pd.merge(cfs600, mcqdw600, on="instance", suffixes=("_cfs", "_mcqdw"))

# Assert weight consistency
assert all(
    (merged["weight_cfs"] == merged["weight_mcqdw"]) | (merged["weight_cfs"] == 0) | (merged["weight_mcqdw"] == 0)
), "Mismatch in weights when both are supposed to be > 0"

# Find instances where weights are different
mismatched_instances = merged[merged["weight_cfs"] != merged["weight_mcqdw"]]["instance"]
print("Instances where weights differ:")
print(mismatched_instances.to_list())

# Convert mcqdw time from ms to seconds
merged["time_mcqdw"] = merged["time_mcqdw"] / 1000

# Compare times
cfs_faster = merged[merged["time_cfs"] < merged["time_mcqdw"]]
mcqdw_faster = merged[merged["time_cfs"] > merged["time_mcqdw"]]

print(f"Instances where CFS is faster: {len(cfs_faster)}")
print(f"Instances where MCQDW is faster: {len(mcqdw_faster)}")

# Compute average speedup factor
speedup_cfs = (cfs_faster["time_mcqdw"] / cfs_faster["time_cfs"]).mean()
speedup_mcqdw = (mcqdw_faster["time_cfs"] / mcqdw_faster["time_mcqdw"]).replace([float("inf"), -float("inf")], pd.NA).dropna().mean()


print(f"When CFS is faster, it is {speedup_cfs:.2f} times faster on average.")
print(f"When MCQDW is faster, it is {speedup_mcqdw:.2f} times faster on average.")

