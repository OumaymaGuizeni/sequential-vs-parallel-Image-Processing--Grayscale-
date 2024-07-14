#!/bin/bash

# Function to run a script and capture specific metrics
run_script_and_capture_metrics() {
    script=$1
    output_file=$2
    
    # Run the script and capture the metrics
    output=$(python3 $script)
    echo "$output" > $output_file
}

# Command to run the sequential script and capture metrics
echo "Running sequential script..."
run_script_and_capture_metrics "seq.py" "sequential_metrics.txt" 

# Command to run the parallel script and capture metrics
echo "Running parallel script..."
run_script_and_capture_metrics "para.py" "parallel_metrics.txt"

# Extract and format metrics for sequential preprocessing
sequential_cpu=$(grep "CPU Usage:" sequential_metrics.txt | cut -d ":" -f 2)
sequential_temp=$(grep "Temperature:" sequential_metrics.txt | cut -d ":" -f 2)
sequential_time=$(grep "Sequential preprocessing time:" sequential_metrics.txt | cut -d ":" -f 2)

# Extract and format metrics for parallel preprocessing
parallel_cpu=$(grep "CPU Usage:" parallel_metrics.txt | cut -d ":" -f 2)
parallel_temp=$(grep "Temperature:" parallel_metrics.txt | cut -d ":" -f 2)
parallel_time=$(grep "Parallel preprocessing time:" parallel_metrics.txt | cut -d ":" -f 2)

# Store results in the comparison file
echo "Comparison of Sequential vs Parallel Execution:" > sequential_vs_parallel.txt
echo "----------------------------------------------" >> sequential_vs_parallel.txt
echo "Sequential Preprocessing Time: $sequential_time" >> sequential_vs_parallel.txt
echo "Sequential CPU Usage: $sequential_cpu" >> sequential_vs_parallel.txt
echo "Sequential Temperature: $sequential_temp" >> sequential_vs_parallel.txt
echo "" >> sequential_vs_parallel.txt
echo "----------------------------------------------" >> sequential_vs_parallel.txt
echo "Parallel Preprocessing Time: $parallel_time" >> sequential_vs_parallel.txt
echo "Parallel CPU Usage: $parallel_cpu" >> sequential_vs_parallel.txt
echo "Parallel Temperature: $parallel_temp" >> sequential_vs_parallel.txt

echo "Comparison completed. Results saved in sequential_vs_parallel.txt."
