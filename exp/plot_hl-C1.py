import pandas as pd
import matplotlib.pyplot as plt
import os

# Function to extract profit and time from the result file
def extract_profit_time(result_file):
    profit, time = 0, 0
    with open(result_file, 'r') as file:
        lines = file.readlines()
        # Extract the total profit and time from the result file
        for line in lines:
            if line.startswith("TotalProfit"):
                params = [float(p.split('=')[1].strip()) for p in line.split(',')]
                profit = params[0]
                time = params[2]
    return profit, time

# Load the CSV data into a pandas DataFrame
csv_file = './results_hl-C1.csv'  # Path to your CSV file
df = pd.read_csv(csv_file, delimiter=';')

# Prepare lists to store the data for plotting
csv_profits = []
csv_times = []
result_profits = []
result_times = []


# Loop through each row in the CSV and find the corresponding result file
for index, row in df.iterrows():
    instance = row['instance']
    hl_v = row['hl_v']
    hl_t = row['hl_t']
    
    # Construct the result file name
    result_file = f"/home/gustavo/ic2024/data/set2/C1/{instance + '_result.txt'}" 
    
    # Check if the result file exists
    if os.path.exists(result_file):
        # Extract profit and time from the result file
        profit, time = extract_profit_time(result_file)
        
        # Append the CSV data
        csv_profits.append(hl_v)
        csv_times.append(hl_t)
        
        # Append the result file data
        result_profits.append(profit)
        result_times.append(time)
    else:
        print(f"Result file {result_file} not found.")


# Create subplots to compare profits and times
fig, ax = plt.subplots(1, 2, figsize=(14, 6))

# Plot Profits
ax[0].plot(result_profits, csv_profits, 'o', label='Profit Comparison', color='blue')
ax[0].plot([min(result_profits), max(result_profits)], [min(result_profits), max(result_profits)], 'r--', label='x = y line')
ax[0].set_title('Profit Comparison (Wei vs HL Profit)')
ax[0].set_xlabel('Wei Profit')
ax[0].set_ylabel('HL Profit (hl_v)')
ax[0].legend()
ax[0].grid(True)

# Plot Times
ax[1].plot(result_times, csv_times, 'o', label='Time Comparison', color='green')
ax[1].plot([min(result_times), max(result_times)], [min(result_times), max(result_times)], 'r--', label='x = y line')
ax[1].set_title('Time Comparison (Wei vs HL Time)')
ax[1].set_xlabel('Wei Time')
ax[1].set_ylabel('HL Time (hl_t)')
ax[1].legend()
ax[1].grid(True)

plt.tight_layout()
plt.show()

# Calculate relative distance (percentage) between CSV and result file (Wei)
relative_profit_distance = [(wei - csv) / wei for wei, csv in zip(result_profits, csv_profits)]
relative_time_distance = [(wei - csv) / wei for wei, csv in zip(result_times, csv_times)]

# Create subplots for relative profit and time distances
fig, ax = plt.subplots(1, 2, figsize=(14, 6))

# Plot Relative Profit Distance
ax[0].plot(result_profits, relative_profit_distance, 'o', label='Relative Profit Distance', color='blue')
ax[0].set_title('Relative Profit Distance (Wei vs CSV Profit)')
ax[0].set_xlabel('Result File Profit (Wei)')
ax[0].set_ylabel('Relative Profit Distance')
ax[0].grid(True)

# Plot Relative Time Distance
ax[1].plot(result_times, relative_time_distance, 'o', label='Relative Time Distance', color='green')
ax[1].set_title('Relative Time Distance (Wei vs CSV Time)')
ax[1].set_xlabel('Result File Time')
ax[1].set_ylabel('Relative Time Distance')
ax[1].grid(True)

plt.tight_layout()
plt.show()
