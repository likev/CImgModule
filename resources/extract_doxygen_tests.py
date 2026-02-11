import os
import re

SOURCE_DIR = "module"
TEST_DIR = "tests"

def extract_tests():
    if not os.path.exists(TEST_DIR):
        os.makedirs(TEST_DIR)

    file_count = 0
    test_count = 0

    for root, dirs, files in os.walk(SOURCE_DIR):
        for file in files:
            if file.endswith(".h"):
                file_path = os.path.join(root, file)
                with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
                    content = f.read()

                # Find all \code ... \endcode blocks
                matches = re.finditer(r"\\code(.*?)\\endcode", content, re.DOTALL)
                
                for i, match in enumerate(matches):
                    code_block = match.group(1).strip()
                    if not code_block:
                        continue

                    # Generate a unique filename
                    safe_name = file.replace(".", "_")
                    test_filename = f"test_{safe_name}_{i}.cpp"
                    test_path = os.path.join(TEST_DIR, test_filename)

                    # Wrap in boilerplate
                    test_content = f"""
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {{
    // Extracted from {file_path}
    try {{
{code_block}
    }} catch (const CImgException& e) {{
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    }} catch (const std::exception& e) {{
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }}
    return 0;
}}
"""
                    with open(test_path, "w") as f_out:
                        f_out.write(test_content)
                    
                    test_count += 1
                
                if file_count % 10 == 0:
                    print(f"Processed {file_count} files...")
                file_count += 1

    print(f"Extraction complete. Extracted {test_count} tests from {file_count} files.")

if __name__ == "__main__":
    extract_tests()
