#!/bin/bash

# Define the paths to the Python scripts
SCRIPT1="/home/${USER}/syscalls/parallel.py"
SCRIPT2="/home/${USER}/syscalls/sequentiel.py"

# Prompt the user to choose which script to run
echo "Please choose which script to run:"
echo "1) parallel.py"
echo "2) sequentiel.py"
read -p "Enter your choice (1 or 2): " choice

# Run the selected script based on the user's choice
if [ "$choice" -eq 1 ]; then
    echo "Running parallel.py..."
    cd /home/yasser_jemli/syscalls/
    python3 parallel.py
elif [ "$choice" -eq 2 ]; then
    echo "Running sequentiel.py..."
    python3 /home/yasser_jemli/syscalls/sequentiel.py
else
    echo "Invalid choice. Please run the script again and select either 1 or 2."
fi

