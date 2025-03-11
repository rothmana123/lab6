# Clear previous output.txt
> output.txt

# Run Test Case 1: Invalid key (should return nothing)
echo "Running Test Case 1: Invalid key" >> output.txt
./lab4 badkey ': ' /proc/meminfo >> output.txt
echo "" >> output.txt  # Newline for readability

# Run Test Case 2: Wrong delimiter (should not remove ': ' before key)
echo "Running Test Case 2: Wrong delimiter" >> output.txt
./lab4 MemAvailable 'baddelim' /proc/meminfo >> output.txt
echo "" >> output.txt  # Newline for readability

# Run Test Case 3: Correct key & delimiter (should return valid output)
echo "Running Test Case 3: Correct key & delimiter" >> output.txt
./lab4 MemAvailable ': ' /proc/meminfo >> output.txt
echo "" >> output.txt  # Newline for readability
