#!/bin/bash

TEST_DIR="tests"
INCLUDE_DIR="."
REPORT_FILE="PLAN-2-TEST.md"

# Initialize Report
echo "# Phase 8 Test Report" > "$REPORT_FILE"
echo "" >> "$REPORT_FILE"
echo "| Test Filename | Status | Error Info |" >> "$REPORT_FILE"
echo "|---|---|---|" >> "$REPORT_FILE"

PASS_COUNT=0
FAIL_COUNT=0

echo "Starting tests and generating report in $REPORT_FILE..."

for test_file in "$TEST_DIR"/*.cpp; do
    test_name=$(basename "$test_file" .cpp)
    exe_name="$TEST_DIR/$test_name.exe"
    error_log="$TEST_DIR/$test_name.log"
    
    # Compile
    g++ -w -o "$exe_name" "$test_file" -I"$INCLUDE_DIR" -lpthread -lX11 > "$error_log" 2>&1
    compile_status=$?
    
    if [ $compile_status -eq 0 ]; then
        # Run
        if command -v timeout >/dev/null 2>&1; then
             timeout 5s "$exe_name" > "$error_log" 2>&1
             run_status=$?
        else
             "$exe_name" > "$error_log" 2>&1
             run_status=$?
        fi

        if [ $run_status -eq 0 ]; then
            echo "| $test_name | PASS | - |" >> "$REPORT_FILE"
            PASS_COUNT=$((PASS_COUNT + 1))
            echo "[PASS] $test_name"
        else
            if [ $run_status -eq 124 ]; then
                 error_info="TIMEOUT"
            else
                 # Capture first line of runtime error or just say Runtime Error
                 error_info="Runtime Error (Exit: $run_status)"
                 # Append last few lines of log to error info, escaping pipes
                 log_tail=$(tail -n 1 "$error_log" | tr -d '\n' | sed 's/|/\\|/g')
                 if [ ! -z "$log_tail" ]; then
                    error_info="$error_info: $log_tail"
                 fi
            fi
            echo "| $test_name | FAIL | $error_info |" >> "$REPORT_FILE"
            FAIL_COUNT=$((FAIL_COUNT + 1))
            echo "[FAIL] $test_name ($error_info)"
        fi
        rm -f "$exe_name"
    else
        # Extract first compilation error line
        # grep "error:" gets lines with error, head -n 1 gets first, sed cleans it up
        compile_error=$(grep "error:" "$error_log" | head -n 1 | sed 's/|/\\|/g' | cut -c 1-100)
        if [ -z "$compile_error" ]; then
             compile_error="Unknown Compile Error"
        fi
        echo "| $test_name | FAIL | Compile: $compile_error... |" >> "$REPORT_FILE"
        FAIL_COUNT=$((FAIL_COUNT + 1))
        echo "[FAIL] $test_name (Compile Error)"
    fi
    rm -f "$error_log"
done

echo "" >> "$REPORT_FILE"
echo "**Summary:** Passed: $PASS_COUNT, Failed: $FAIL_COUNT, Total: $((PASS_COUNT + FAIL_COUNT))" >> "$REPORT_FILE"

echo "------------------------------------------------"
echo "Tests Passed: $PASS_COUNT"
echo "Tests Failed: $FAIL_COUNT"
