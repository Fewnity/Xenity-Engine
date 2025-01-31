#!/bin/bash

folder="./shaders_to_compile"

echo "Compile Shaders..."

success=true

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

        # Check if compilation was successful
        if [ $? -ne 0 ]; then
            echo "Compilation failed for: $file"
            success=false
        fi
    fi
done

# Exit with error if any shader failed to compile
if [ "$success" = false ]; then
    exit 1
else
    echo "All shaders compiled successfully."
    exit 0
fi