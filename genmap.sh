#!/bin/bash

# Default map dimensions
width=50
height=50

# Parse command line arguments
if [[ $# -gt 2 ]]; then
  echo "Error: Too many arguments. Usage: ./genmap.sh [width] [height]" >&2
  exit 1
elif [[ $# -eq 2 ]]; then
  width=$1
  height=$2
elif [[ $# -eq 1 ]]; then
  width=$1
  height=$1  # Use the same value for both width and height
fi

# Function to generate food (non-blank) characters
gen_food() {
  local random_num
  random_num=$((RANDOM % 4))
  case $random_num in
    0)
      echo -n "#";;
    1)
      echo -n "@";;
    2)
      echo -n "*";;
    3)
      echo -n "+";;
  esac
}

# Generate the map
iteration=0
while [[ $iteration -lt 10 ]]; do
  # Create temporary map file
  map_file=$(mktemp)

  # Generate random map
  for ((i=0; i<$height; i++)); do
    for ((j=0; j<$width; j++)); do
      gen_food >> $map_file
    done
    echo >> $map_file
  done

  # Count blank lines
  blank_lines=$(grep -cE "^[[:space:]]*$" $map_file)

  # Calculate percentage of non-blank lines
  non_blank_percentage=$((100 - (100 * blank_lines / height)))

  # Check if non-blank percentage is greater than 50%
  if [[ $non_blank_percentage -gt 50 ]]; then
    # Output map to STDOUT
    cat $map_file
    rm $map_file
    exit 0
  else
    # Increment iteration counter and delete temporary map file
    iteration=$((iteration + 1))
    rm $map_file
  fi
done

# Exit with error message if map generation failed
echo "Error: Failed to generate map after 10 iterations" >&2
exit 1
