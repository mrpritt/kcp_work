import pandas as pd
import matplotlib.pyplot as plt

# Load the data
file_path = './results.csv'  # Replace with your actual file path
df = pd.read_csv(file_path, delimiter=';')

# Extract parameters from instance names
df['type'] = df['instance'].str.split('-').str[0]
df['density'] = df['instance'].str.split('-').str[2].astype(float)
df['restrictiveness'] = df['instance'].str.split('-').str[3].astype(float)

# Define marker and color mappings
marker_map = {'uncorr': 'X', 'strcorr': 'o'}
color_map = {'hl': 'blue', 'hr': 'red'}

# Initialize the figure
fig, axes = plt.subplots(1, 2, figsize=(16, 8), constrained_layout=True)

# Scatter plot for value comparisons
for heuristic, color in color_map.items():
    for t_type, marker in marker_map.items():
        subset = df[df['type'] == t_type]
        axes[0].scatter(
            subset['exact_v'], subset[f'{heuristic}_v'],
            label=f'{heuristic} ({t_type})', alpha=0.7, color=color, marker=marker
        )
axes[0].plot(df['exact_v'], df['exact_v'], color='black', linestyle=':', label='y=x')
axes[0].set_title('Comparison of exact_v with heuristic values')
axes[0].set_xlabel('exact_v')
axes[0].set_ylabel('heuristic values')
axes[0].grid(True)
axes[0].legend()

# Scatter plot for time comparisons
for heuristic, color in color_map.items():
    for t_type, marker in marker_map.items():
        subset = df[df['type'] == t_type]
        axes[1].scatter(
            subset['exact_t'], subset[f'{heuristic}_t'],
            label=f'{heuristic} ({t_type})', alpha=0.7, color=color, marker=marker
        )
axes[1].plot(df['exact_t'], df['exact_t'], color='black', linestyle=':', label='y=x')
axes[1].set_title('Comparison of exact_t with heuristic times')
axes[1].set_xlabel('exact_t')
axes[1].set_ylabel('heuristic times (seconds)')
axes[1].grid(True)
axes[1].legend()

plt.show()
