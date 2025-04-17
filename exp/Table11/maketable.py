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
    df['time'] = df[columns_map['time']]

    grouped = df.groupby('type').agg(
        solved=('solved', 'sum'),
        time=('time', 'sum'),
        steps=('steps', 'sum')
    ).reset_index()

    grouped['algo'] = algo_name
    grouped['class'] = class_name
    return grouped[['class', 'type', 'solved', 'time', 'steps', 'algo']]

def extract_info_from_filename(filename):
    base = os.path.basename(filename)
    match = re.match(r'(C\d+)-([A-Za-z]+)\d*\.csv', base)
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
final_df.to_csv('aggregated_all_algos_with_class.csv', index=False)
