#!/bin/bash

folder="./shaders_to_compile"

echo "Compile Shaders..."

for file in "$folder"/*; do
    # Check if it's a file
    if [ -f "$file" ]; then
        # Get file extension
        extension="${file##*.}"

        # Get file name
        fileName="${file%.*}"

        # Check file extension
        case "$extension" in
            "fcg")
                # Compile fragment shader
                cgcomp -f "$file" "${fileName}.fco"
                ;;
            "vcg")
                # Compile vertex shader
                cgcomp -v "$file" "${fileName}.vco"
                ;;
            *)
                # Wrong file
                echo "Cannot compile this file : $file"
                ;;
        esac
    fi
done