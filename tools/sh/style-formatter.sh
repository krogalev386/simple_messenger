#!/bin/bash

cd "$(dirname "$0")"
cd ../..

code_files=$(find . | grep -e '.hpp$' -e '.cpp$')

for file in ${code_files[@]};
do
    clang-format -style=file $file > "$file"_temp
    chmod u+r "$file"_temp
    cat "$file"_temp > $file
    rm "$file"_temp
    echo "Processed: $file"
done
