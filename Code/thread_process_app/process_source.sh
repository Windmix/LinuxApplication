#!/bin/bash

# Script: process_source.sh
# Purpose: Process source files by adding copyright headers, renaming extensions, and replacing float/double types
# Usage: ./process_source.sh <source_folder> <copyright_file> [--float-to-double|--double-to-float]

# Validate arguments
if [ "$#" -lt 2 ]; then
    echo "Usage: $0 <source_folder> <copyright_file> [--float-to-double|--double-to-float]"
    echo "Example: ./process_source.sh /home/user/project/src copyright.txt --float-to-double"
    exit 1
fi

SOURCE_FOLDER="$1"
COPYRIGHT_FILE="$2"
REPLACE_MODE="${3:---float-to-double}"  # Default to float-to-double

# Verify paths exist
if [ ! -d "$SOURCE_FOLDER" ]; then
    echo "Error: Source folder '$SOURCE_FOLDER' does not exist"
    exit 1
fi

if [ ! -f "$COPYRIGHT_FILE" ]; then
    echo "Error: Copyright file '$COPYRIGHT_FILE' does not exist"
    exit 1
fi

# Function to backup files
backup_files() 
{
    local backup_dir="/tmp/source_backup_$(date +%Y%m%d_%H%M%S)"
    echo "Creating backup in $backup_dir..."
    mkdir -p "$backup_dir"
    cp -r "$SOURCE_FOLDER" "$backup_dir"
}

# --- 1. Add copyright header ---
add_copyright() 
{
    echo "Adding copyright headers..."
    find "$SOURCE_FOLDER" -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.cc" -o -name "*.hpp" \) | while read -r file; do
        if ! grep -q "Copyright" "$file"; then
            # Create temp file with copyright header and original content
            echo "/*" > temp_file
            cat "$COPYRIGHT_FILE" >> temp_file
            echo "*/" >> temp_file
            cat "$file" >> temp_file
            mv temp_file "$file"
            echo "Added header to $file"
        else
            echo "Copyright exists in $file (skipped)"
        fi
    done
}

# --- 2. Rename file extensions ---
rename_extensions() 
{
    echo "Processing file extensions..."
    
    # Rename .cpp to .cc if any exist
    cpp_files=$(find "$SOURCE_FOLDER" -type f -name "*.cpp")
    if [ -n "$cpp_files" ]; then
        echo "Renaming .cpp to .cc..."
        find "$SOURCE_FOLDER" -type f -name "*.cpp" -exec bash -c 'mv "$0" "${0%.cpp}.cc"' {} \;
		
    else
        # Otherwise rename .h to .hpp
        echo "No .cpp files found, renaming .h to .hpp..."
        find "$SOURCE_FOLDER" -type f -name "*.h" -exec bash -c 'mv "$0" "${0%.h}.hpp"' {} \;
    fi
}

# --- 3. Replace float/double types ---
replace_types() 
{
    echo "Replacing floating-point types ($REPLACE_MODE)..."
    
    case "$REPLACE_MODE" in
        --float-to-double)
            find "$SOURCE_FOLDER" -type f \( -name "*.cc" -o -name "*.hpp" \) -exec sed -i 's/\bfloat\b/double/g' {} +
            ;;
        --double-to-float)
            find "$SOURCE_FOLDER" -type f \( -name "*.cc" -o -name "*.hpp" \) -exec sed -i 's/\bdouble\b/float/g' {} +
            ;;
        *)
            echo "Error: Invalid replacement mode '$REPLACE_MODE'"
            exit 1
            ;;
    esac
}

# --- Main execution ---
backup_files
add_copyright
rename_extensions
replace_types

echo "Processing complete!"