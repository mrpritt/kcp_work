import pandas as pd

# Load CSV files
cfs600 = pd.read_csv("C15-CFS600.csv", sep=";")
mcqdw600 = pd.read_csv("C15-mcqdw600.csv", sep=";")
mcqdw600['time'] = mcqdw600['time'] / 1000
insidrug600 = pd.read_csv("C15-insidrug600.csv", sep=";")
insidrug600['time'] = insidrug600['time'] / 1000
insidrug600['instance'] = insidrug600['instance'].apply(lambda instance: ".".join(instance.split('.')[:-2]))

# Count solved instances
solved_cfs = (cfs600["weight"] > 0).sum()
solved_mcqdw = (mcqdw600["weight"] > 0).sum()
solved_insidrug = (insidrug600["weight"] > 0).sum()
print(f"Solved instances in CFS600: {solved_cfs}")
print(f"Solved instances in MCQDW600: {solved_mcqdw}")
print(f"Solved instances in INSIDRUG600: {solved_insidrug}")

# Merge dataframes on 'instance'
merged = pd.merge(cfs600, mcqdw600, on="instance", suffixes=("_cfs", "_mcqdw"))

# Assert weight consistency
assert all(
    (merged["weight_cfs"] == merged["weight_mcqdw"]) | (merged["weight_cfs"] == 0) | (merged["weight_mcqdw"] == 0)
), "Mismatch in weights when both are supposed to be > 0"

# Find instances where weights are different
mismatched_instances = merged[merged["weight_cfs"] != merged["weight_mcqdw"]]["instance"]
print("Instances where CFS/MCQDW weights differ:")
print(mismatched_instances.to_list())

# Find instances where weights are different
merge_mcqdw_insi = pd.merge(mcqdw600, insidrug600, on="instance", suffixes=("_mcqdw", "_insidrug"))
mismatched_instances = merge_mcqdw_insi[merge_mcqdw_insi["weight_insidrug"] != merge_mcqdw_insi["weight_mcqdw"]]["instance"]
print("Instances where MCQDW/INSIDRUG weights differ:")
print(mismatched_instances.to_list())

# Merge dataframes
merge_cfs_insi = pd.merge(cfs600, insidrug600, on="instance", suffixes=("_cfs", "_insidrug"))

# Compare times
cfs_faster_mcqdw = merged[merged["time_cfs"] < merged["time_mcqdw"]]
mcqdw_faster_cfs = merged[merged["time_cfs"] > merged["time_mcqdw"]]
insi_faster_mcqdw = merge_mcqdw_insi[merge_mcqdw_insi["time_mcqdw"] > merge_mcqdw_insi["time_insidrug"]]
mcqdw_faster_insi = merge_mcqdw_insi[merge_mcqdw_insi["time_mcqdw"] < merge_mcqdw_insi["time_insidrug"]]
insi_faster_cfs = merge_cfs_insi[merge_cfs_insi["time_cfs"] > merge_cfs_insi["time_insidrug"]]
cfs_faster_insi = merge_cfs_insi[merge_cfs_insi["time_cfs"] < merge_cfs_insi["time_insidrug"]]


print(f"Instances where CFS is faster than MCQDW: {len(cfs_faster_mcqdw)}")
print(f"Instances where MCQDW is faster than CFS: {len(mcqdw_faster_cfs)}")
print(f"Instances where INSIDRUG is faster than MCQDW: {len(insi_faster_mcqdw)}")
print(f"Instances where MCQDW is faster than INSIDRUG: {len(mcqdw_faster_insi)}")
print(f"Instances where INSIDRUG is faster than CFS: {len(insi_faster_cfs)}")
print(f"Instances where CFS is faster than INSIDRUG: {len(cfs_faster_insi)}")

# Compute average speedup factor
cfs_mcqdw_speedup_cfs = (cfs_faster_mcqdw["time_mcqdw"] / cfs_faster_mcqdw["time_cfs"]).mean()
mcqdw_speedup_cfs = (mcqdw_faster_cfs["time_cfs"] / mcqdw_faster_cfs["time_mcqdw"]).replace([float("inf"), -float("inf")], pd.NA).dropna().mean()
insi_speedup_mcqdw = (insi_faster_mcqdw["time_mcqdw"] / insi_faster_mcqdw["time_insidrug"]).replace([float("inf"), -float("inf")], pd.NA).dropna().mean()
mcqdw_speedup_insi = (mcqdw_faster_insi["time_insidrug"] / mcqdw_faster_insi["time_mcqdw"]).replace([float("inf"), -float("inf")], pd.NA).dropna().mean()
insi_speedup_cfs = (insi_faster_cfs["time_cfs"] / insi_faster_cfs["time_insidrug"]).replace([float("inf"), -float("inf")], pd.NA).dropna().mean()
cfs_speedup_insi = (cfs_faster_insi["time_insidrug"] / cfs_faster_insi["time_cfs"]).mean()


print(f"When CFS is faster than MCQDW, it is {cfs_mcqdw_speedup_cfs:.2f} times faster on average.")
print(f"When MCQDW is faster than CFS, it is {mcqdw_speedup_cfs:.2f} times faster on average.")
print(f"When INSIDRUG is faster than MCQDW, it is {insi_speedup_mcqdw:.2f} times faster on average.")
print(f"When MCQDW is faster than INSIDRUG, it is {mcqdw_speedup_insi:.2f} times faster on average.")
print(f"When INSIDRUG is faster than CFS, it is {insi_speedup_cfs:.2f} times faster on average.")
print(f"When CFS is faster than INSIDRUG, it is {cfs_speedup_insi:.2f} times faster on average.")

