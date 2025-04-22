import pandas as pd
import re
import os
from glob import glob

def extract_type(name):
    match = re.search(r'BPPC_(\d+)_\d+_(\d+)\.txt_(\d+\.\d+)\.bz2', name)
    if match:
        return int(match.group(1))
    else:
        raise ValueError(f"Invalid instance name format: {name}")

def process_file(file_path, algo_name, class_name):
    df = pd.read_csv(file_path, sep=None, engine='python')  # auto-detects delimiter

    # Determine column keys based on available columns
    cols = df.columns.str.lower()
    if 'steps' in cols:
        columns_map = {'weight': 'weight', 'time': 'time', 'steps': 'steps'}
    elif 'nodes' in cols:
        columns_map = {'weight': 'weight', 'time': 'time', 'steps': 'nodes'}
    else:
        raise ValueError(f"Can't find steps or nodes in {file_path}")

    df['type'] = df['instance'].apply(extract_type)
    df['solved'] = df[columns_map['weight']] > 0
    df['steps'] = df[columns_map['steps']]
    if algo_name == 'rlx':
        df['time'] = df[columns_map['time']]/1000
    else:
        df['time'] = df[columns_map['time']]

    grouped = df.groupby('type').agg(
        solved=('solved', 'sum'),
        time=('time', 'mean'),
        steps=('steps', 'mean')
    ).reset_index()

    grouped['algo'] = algo_name
    grouped['class'] = class_name
    return grouped[['class', 'type', 'solved', 'time', 'steps', 'algo']]

def extract_info_from_filename(filename):
    base = os.path.basename(filename)
    match = re.match(r'(C\d+)-([A-Za-z_]+)\d*\.csv', base)
    if match:
        class_name = match.group(1)
        algo_name = match.group(2)
        return class_name, algo_name
    else:
        raise ValueError(f"Filename does not match expected pattern: {filename}")

# 🔍 Get all CSVs in current directory
csv_files = glob('*.csv')

# 📦 Process all files and collect results
results = []
for file in csv_files:
    try:
        class_name, algo_name = extract_info_from_filename(file)
        df = process_file(file, algo_name, class_name)
        results.append(df)
    except Exception as e:
        print(f"⚠️ Skipping {file}: {e}")

# 📝 Save final combined CSV
final_df = pd.concat(results)
final_df.to_csv('table11.csv', index=False)

############################# LATEX ######################################
df = final_df.copy()
# Pivot the table so we get a multi-column view per algorithm
pivot = df.pivot_table(
    index=["class", "type"],
    columns="algo",
    values=["solved", "time", "steps"]
)

# Flatten the MultiIndex columns
pivot.columns = [f"{metric}-{algo}" for metric, algo in pivot.columns]
pivot = pivot.reset_index()

# Round values for presentation (like the image: int for solved/steps, 1 decimal for time)
for col in pivot.columns:
    if col.startswith("solved") or col.startswith("steps"):
        pivot[col] = pivot[col].round(0).astype(int)
    elif col.startswith("time"):
        pivot[col] = pivot[col].round(1)

# Reorder columns (adjust based on your actual algorithm names)
ordered_cols = [
    "class", "type",
    "solved-CFS", "time-CFS", "steps-CFS",
    "solved-rlx", "time-rlx", "steps-rlx"
]
pivot = pivot[[col for col in ordered_cols if col in pivot.columns]]

# Generate LaTeX code
latex = pivot.to_latex(
    index=False,
    escape=False,
    column_format="ll" + "rrr" * ((len(pivot.columns) - 2) // 3),
    float_format='%.1f',
    multicolumn=True,
    multicolumn_format="c"
)
with open("table.tex", "w") as f:
    f.write(latex)

