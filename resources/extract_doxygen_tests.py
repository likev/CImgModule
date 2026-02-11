import os
import re

SOURCE_DIR = "module"
TEST_DIR = "tests"

def fix_snippet(code):
    # Fix 1: Comment out ellipses "..."
    code = code.replace("...", "// ...")
    
    # Fix 2: Remove invalid macro defines like #define "CImg.h"
    code = re.sub(r'#define\s+"[^"]+"', '// #define ... (invalid)', code)

    # Fix 3: Add declarations for common variables if they are used but not declared
    # We use a crude check: if "word" is in code but not "Type word" or "Type *word" or "Type& word"
    # This is hard to do perfectly with regex, so we just pre-pend common mocks if the variable name appears.
    
    mocks = []
    
    # Mock 'disp' (CImgDisplay)
    if re.search(r'\bdisp\.', code) and not re.search(r'CImgDisplay\s+disp', code):
        mocks.append('CImgDisplay disp(100,100,"Test Display",0);')

    # Mock 'img' (CImg<float>)
    # Check for img, img1, img2...
    if re.search(r'\bimg\.', code) and not re.search(r'CImg<[^>]+>\s+img', code):
        mocks.append('CImg<float> img(100,100,1,3);')
    if re.search(r'\bimg1\.', code) and not re.search(r'CImg<[^>]+>\s+img1', code):
        mocks.append('CImg<float> img1(100,100,1,3);')
    if re.search(r'\bimg2\.', code) and not re.search(r'CImg<[^>]+>\s+img2', code):
        mocks.append('CImg<float> img2(100,100,1,3);')
        
    # Mock 'list' (CImgList<float>)
    if re.search(r'\blist\.', code) or re.search(r'cimglist_for\(list', code) and not re.search(r'CImgList<[^>]+>\s+list', code):
        mocks.append('CImgList<float> list(4, 100, 100, 1, 3);')

    # Mock 'points', 'primitives', 'colors', 'opacities' (for 3D)
    if 'primitives' in code and 'colors' in code and 'opacities' in code:
        if not re.search(r'CImgList<[^>]+>\s+primitives', code):
             mocks.append('CImgList<unsigned int> primitives;')
        if not re.search(r'CImgList<[^>]+>\s+colors', code):
             mocks.append('CImgList<unsigned char> colors;')
        if not re.search(r'CImg<[^>]+>\s+opacities', code) and not re.search(r'CImgList<[^>]+>\s+opacities', code): # Opacities can be list or img
             mocks.append('CImg<float> opacities;')
        if not re.search(r'CImgList<[^>]+>\s+points', code) and not re.search(r'CImgList<[^>]+>\s+points3d', code):
             mocks.append('CImgList<float> points3d;')
             mocks.append('CImgList<unsigned int> faces3d;') # Often used with points3d
             mocks.append('CImgList<unsigned char> colors3d;')

    # Mock 'window_title'
    if 'window_title' in code and 'char' not in code:
        mocks.append('const char* window_title = "Test Window";')
        
    # Mock 'warning_message'
    if 'warning_message' in code and 'char' not in code:
         mocks.append('const char* warning_message = "Test Warning";')

    if mocks:
        code = "\n".join(mocks) + "\n" + code

    return code

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

                    # Apply fixes
                    fixed_block = fix_snippet(code_block)

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
{fixed_block}
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
