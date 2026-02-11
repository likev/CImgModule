#!/bin/bash

TEST_DIR="tests"
INCLUDE_DIR="."
FAIL_COUNT=0
PASS_COUNT=0

echo "Starting tests..."

for test_file in "$TEST_DIR"/*.cpp; do
    test_name=$(basename "$test_file" .cpp)
    exe_name="$TEST_DIR/$test_name.exe"
    
    # Compile
    # Using -w to suppress warnings because snippets might be incomplete or use unused variables
    g++ -w -o "$exe_name" "$test_file" -I"$INCLUDE_DIR" -lpthread -lX11
    compile_status=$?
    
    if [ $compile_status -eq 0 ]; then
        # Run
        # Timeout to prevent hanging tests (e.g. display loops)
        # Using `timeout` command if available, otherwise just run
        if command -v timeout >/dev/null 2>&1; then
             timeout 5s "$exe_name" > /dev/null 2>&1
             run_status=$?
        else
             "$exe_name" > /dev/null 2>&1
             run_status=$?
        fi

        if [ $run_status -eq 0 ]; then
            echo "[PASS] $test_name"
            PASS_COUNT=$((PASS_COUNT + 1))
        else
            # timeout returns 124
            if [ $run_status -eq 124 ]; then
                 echo "[TIMEOUT] $test_name"
            else
                 echo "[FAIL] $test_name (Runtime error)"
            fi
            FAIL_COUNT=$((FAIL_COUNT + 1))
        fi
        rm -f "$exe_name"
    else
        echo "[FAIL] $test_name (Compile error)"
        FAIL_COUNT=$((FAIL_COUNT + 1))
    fi
done

echo "------------------------------------------------"
echo "Tests Passed: $PASS_COUNT"
echo "Tests Failed: $FAIL_COUNT"
echo "Total Tests:  $((PASS_COUNT + FAIL_COUNT))"

if [ $FAIL_COUNT -eq 0 ]; then
    exit 0
else
    exit 1
fi
