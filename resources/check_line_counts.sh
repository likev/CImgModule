#!/bin/bash

THRESHOLD=3000
EXIT_CODE=0

echo "Checking line counts in module/ directory (threshold: $THRESHOLD lines)..."
echo "-----------------------------------------------------------------------"

# Find all .h files in module/
files=$(find module/ -name "*.h")

for file in $files; do
    lines=$(wc -l < "$file")
    if [ "$lines" -gt "$THRESHOLD" ]; then
        echo "⚠️  $file: $lines lines (OVER THRESHOLD)"
        EXIT_CODE=1
    else
        echo "✅ $file: $lines lines"
    fi
done

echo "-----------------------------------------------------------------------"
if [ $EXIT_CODE -eq 0 ]; then
    echo "All files are within the threshold."
else
    echo "Some files exceed the $THRESHOLD line threshold."
fi

exit $EXIT_CODE
