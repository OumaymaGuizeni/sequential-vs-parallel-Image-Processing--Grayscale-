#!/bin/bash

# Function to display welcome message
display_welcome() {
    echo "#############################################"
    echo "#                                           #"
    echo "#      Welcome to our Image Processing      #"
    echo "#            Sequential vs Parallel         #"
    echo "#                  Project!                 #"
    echo "#                                           #"
    echo "#############################################"
}

# Function to run sequential processing
run_sequential() {
    echo "Compiling Sequential Processing..."
    g++ -o test_seq test_seq.c `pkg-config --cflags --libs opencv4 starpu-1.2` -lpthread
    if [ $? -ne 0 ]; then
        echo "Compilation failed for Sequential Processing."
        exit 1
    fi
    echo "Running Sequential Processing..."
    ./test_seq > sequential_output.txt
    echo "Sequential Processing completed."
}

# Function to run parallel processing
run_parallel() {
    echo "Compiling Parallel Processing..."
    g++ -o test_para test_para.c `pkg-config --cflags --libs opencv4 starpu-1.2` -lpthread
    if [ $? -ne 0 ]; then
        echo "Compilation failed for Parallel Processing."
        exit 1
    fi
    echo "Running Parallel Processing..."
    ./test_para > parallel_output.txt
    echo "Parallel Processing completed."
}

# Function to run both sequential and parallel processing
run_both() {
    run_sequential
    run_parallel
}

# Function to generate HTML report
generate_report() {
    echo "Generating HTML report..."
    {
        echo "<html>"
        echo "<head>"
        echo "<title>Processing Report</title>"
        echo "<style>"
        echo "body { font-family: Arial, sans-serif; margin: 20px; }"
        echo "h1 { color: #000000; text-align: center; margin-top: 100px; }"
        echo "h2 { color: #808080; margin-top: 50px; }"  
        echo "table { width: 100%; border-collapse: collapse; margin: 20px 0; }"
        echo "table, th, td { border: 1px solid #ddd; }"
        echo "th, td { padding: 8px; text-align: left; }"
        echo "tr:nth-child(even) { background-color: #f2f2f2; }"
        echo "th { background-color: #0073e6; color: white; }"  
        echo "pre { background-color: #f4f4f4; padding: 10px; border-radius: 5px; }"
        echo ".logo-left { position: absolute; top: -30px; left: 10px;  }"
        echo ".logo-right { position: absolute; top: -60px; right: 10px;  }"
        echo "</style>"
        echo "</head>"
        echo "<body>"
        echo "<img src='https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcScTQdOHmC7eQRx_bc85kX_tis9mfliwNSCEg&s' alt='Logo Left' class='logo-left'>"
        echo "<img src='https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcQy7_XFUnz6xVIZZyRCCxn_tNTuSvf-5jaOUA&s' alt='Logo Right' class='logo-right'>"
        echo "<h1>Image Processing Report</h1>"
        
        echo "<h2>Sequential Processing Result</h2>"
        if [ -f sequential_output.txt ]; then
            echo "<table>"
            echo "<tr><th>Metric</th><th>Value</th></tr>"
            while IFS=: read -r metric value; do
                echo "<tr><td>$metric</td><td>$value</td></tr>"
            done < sequential_output.txt
            echo "</table>"
        else
            echo "<p>No data available for Sequential Processing.</p>"
        fi
        
        echo "<h2>Parallel Processing Result</h2>"
        if [ -f parallel_output.txt ]; then
            echo "<table>"
            echo "<tr><th>Metric</th><th>Value</th></tr>"
            while IFS=: read -r metric value; do
                echo "<tr><td>$metric</td><td>$value</td></tr>"
            done < parallel_output.txt
            echo "</table>"
        else
            echo "<p>No data available for Parallel Processing.</p>"
        fi
        
        echo "</body>"
        echo "</html>"
    } > report.html
    echo "HTML report generated as report.html"
    # Open the report in the default web browser
    xdg-open report.html
}

# Function to display results in terminal
display_results() {
    if [ -f sequential_output.txt ]; then
        echo "Sequential Processing Result:"
        cat sequential_output.txt
    fi
    
    if [ -f parallel_output.txt ]; then
        echo "Parallel Processing Result:"
        cat parallel_output.txt
    fi
}

# Main script
display_welcome

echo "Please choose an option:"
echo "1) Run Sequential Processing"
echo "2) Run Parallel Processing"
echo "3) Run Both Sequential and Parallel Processing"
read -p "Enter your choice (1/2/3): " choice

case $choice in
    1)
        run_sequential
        ;;
    2)
        run_parallel
        ;;
    3)
        run_both
        ;;
    *)
        echo "Invalid choice. Exiting."
        exit 1
        ;;
esac

read -p "Do you want to generate a report? (yes/no): " generate_report_choice

if [ "$generate_report_choice" == "yes" ]; then
    generate_report
else
    display_results
fi
