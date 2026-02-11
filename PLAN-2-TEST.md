# Phase 8 Test Report

| Test Filename | Status | Error Info |
|---|---|---|
| test_cimg_namespace_runtime_h_0 | PASS | - |
| test_display_base_h_0 | FAIL | Compile: tests/test_display_base_h_0.cpp:12:21: error: ‘img’ was not declared in this scope... |
| test_display_base_h_1 | PASS | - |
| test_display_base_h_2 | FAIL | Compile: tests/test_display_base_h_2.cpp:15:7: error: expected primary-expression before ‘catch’... |
| test_display_base_h_3 | FAIL | Compile: tests/test_display_base_h_3.cpp:16:7: error: expected primary-expression before ‘catch’... |
| test_display_base_h_4 | FAIL | Compile: tests/test_display_base_h_4.cpp:16:7: error: expected primary-expression before ‘catch’... |
| test_display_base_h_5 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,[untitled])] CImgDisplay::assign(): Failed to open X11 display. |
| test_display_base_h_6 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,[untitled])] CImgDisplay::assign(): Failed to open X11 display. |
| test_display_base_h_7 | PASS | - |
| test_display_base_h_8 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,[untitled])] CImgDisplay::assign(): Failed to open X11 display. |
| test_display_base_h_9 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,[untitled])] CImgDisplay::assign(): Failed to open X11 display. |
| test_exceptions_h_0 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(100,100,1,3,0x55cafec1a340,non-shared)] CImg<float32>::mirror(): Invalid specified axis 'e'. |
| test_exceptions_h_1 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,0,(nil),non-shared)] CImg<float32>::at(): Empty instance. |
| test_exceptions_h_2 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,0,(nil),non-shared)] CImg<float32>::load(): Failed to open file 'missing_file.jpg'. |
| test_exceptions_h_3 | FAIL | Compile: tests/test_exceptions_h_3.cpp:12:18: error: a function-definition is not allowed here before ‘{’... |
| test_image_body_analysis_h_0 | FAIL | Compile: tests/test_image_body_analysis_h_0.cpp:10:1: error: ‘permute_axes’ was not declared in this scop... |
| test_image_body_analysis_h_1 | FAIL | Compile: tests/test_image_body_analysis_h_1.cpp:12:20: error: expected ‘;’ before ‘}’ token... |
| test_image_body_arithmetic_h_0 | PASS | - |
| test_image_body_arithmetic_h_1 | PASS | - |
| test_image_body_arithmetic_h_10 | PASS | - |
| test_image_body_arithmetic_h_11 | FAIL | Compile: tests/test_image_body_arithmetic_h_11.cpp:13:11: error: conflicting declaration ‘const cimg_librar... |
| test_image_body_arithmetic_h_12 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,0,(nil),non-shared)] CImg<uint8>::load(): Failed to open file 'reference.jpg'. |
| test_image_body_arithmetic_h_13 | FAIL | Compile: tests/test_image_body_arithmetic_h_13.cpp:12:20: error: invalid use of non-static member function �... |
| test_image_body_arithmetic_h_14 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,0,(nil),non-shared)] CImg<float32>::load(): Failed to open file 'reference.jpg'. |
| test_image_body_arithmetic_h_2 | PASS | - |
| test_image_body_arithmetic_h_3 | FAIL | Compile: tests/test_image_body_arithmetic_h_3.cpp:11:14: error: ambiguous overload for ‘operator=’ (opera... |
| test_image_body_arithmetic_h_4 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(100,100,1,1,0x5ab999e13be0,non-shared)] CImg<float32>::load(): Failed to open file 'reference.jpg'. |
| test_image_body_arithmetic_h_5 | PASS | - |
| test_image_body_arithmetic_h_6 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,0,(nil),non-shared)] CImg<uint8>::load(): Failed to open file 'reference.jpg'. |
| test_image_body_arithmetic_h_7 | FAIL | Compile: tests/test_image_body_arithmetic_h_7.cpp:12:44: error: ‘img’ was not declared in this scope; did... |
| test_image_body_arithmetic_h_8 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,0,(nil),non-shared)] CImg<uint8>::load(): Failed to open file 'reference.jpg'. |
| test_image_body_arithmetic_h_9 | PASS | - |
| test_image_body_draw_1_h_0 | PASS | - |
| test_image_body_draw_1_h_1 | PASS | - |
| test_image_body_draw_1_h_2 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,0,(nil),non-shared)] CImg<uint8>::load(): Failed to open file 'texture256x256.ppm'. |
| test_image_body_draw_1_h_3 | PASS | - |
| test_image_body_object3d_h_0 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,0,(nil),non-shared)] CImg<float32>::load(): Failed to open file 'reference.jpg'. |
| test_image_body_object3d_h_1 | FAIL | Compile: tests/test_image_body_object3d_h_1.cpp:13:76: error: ‘colors3d’ was not declared in this scope... |
| test_image_body_object3d_h_2 | FAIL | Compile: tests/test_image_body_object3d_h_2.cpp:13:79: error: ‘colors3d’ was not declared in this scope... |
| test_image_body_object3d_h_3 | FAIL | Runtime Error (Exit: 1): CImg Exception: CImgDisplay::screen_height(): Failed to open X11 display. |
| test_image_body_object3d_h_4 | FAIL | Runtime Error (Exit: 1): CImg Exception: CImgDisplay::screen_height(): Failed to open X11 display. |
| test_image_body_object3d_h_5 | FAIL | Runtime Error (Exit: 1): CImg Exception: CImgDisplay::screen_height(): Failed to open X11 display. |
| test_image_body_object3d_h_6 | FAIL | Runtime Error (Exit: 1): CImg Exception: CImgDisplay::screen_height(): Failed to open X11 display. |
| test_image_body_object3d_h_7 | FAIL | Runtime Error (Exit: 1): CImg Exception: CImgDisplay::screen_height(): Failed to open X11 display. |
| test_image_body_object3d_h_8 | FAIL | Runtime Error (Exit: 1): CImg Exception: CImgDisplay::screen_height(): Failed to open X11 display. |
| test_image_body_object3d_h_9 | FAIL | Runtime Error (Exit: 1): CImg Exception: CImgDisplay::screen_height(): Failed to open X11 display. |
| test_image_body_pointwise_h_0 | FAIL | Compile: tests/test_image_body_pointwise_h_0.cpp:11:26: error: passing ‘const cimg_library::CImg<float>’ ... |
| test_image_body_pointwise_h_1 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,0,(nil),non-shared)] CImg<float32>::load(): Failed to open file 'reference.jpg'. |
| test_image_body_pointwise_h_10 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,0,(nil),non-shared)] CImg<float32>::load(): Failed to open file 'reference.jpg'. |
| test_image_body_pointwise_h_2 | FAIL | Compile: tests/test_image_body_pointwise_h_2.cpp:11:26: error: passing ‘const cimg_library::CImg<float>’ ... |
| test_image_body_pointwise_h_3 | FAIL | Compile: tests/test_image_body_pointwise_h_3.cpp:11:26: error: passing ‘const cimg_library::CImg<float>’ ... |
| test_image_body_pointwise_h_4 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,0,(nil),non-shared)] CImg<float32>::load(): Failed to open file 'reference.jpg'. |
| test_image_body_pointwise_h_5 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,0,(nil),non-shared)] CImg<float32>::load(): Failed to open file 'reference.jpg'. |
| test_image_body_pointwise_h_6 | FAIL | Compile: tests/test_image_body_pointwise_h_6.cpp:11:26: error: passing ‘const cimg_library::CImg<float>’ ... |
| test_image_body_pointwise_h_7 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,0,(nil),non-shared)] CImg<float32>::load(): Failed to open file 'reference.jpg'. |
| test_image_body_pointwise_h_8 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,0,(nil),non-shared)] CImg<float32>::load(): Failed to open file 'reference.jpg'. |
| test_image_body_pointwise_h_9 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,0,(nil),non-shared)] CImg<float32>::load(): Failed to open file 'reference.jpg'. |
| test_image_checks_h_0 | PASS | - |
| test_image_checks_h_1 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,0,(nil),non-shared)] CImg<float32>::load(): Failed to open file 'reference.jpg'. |
| test_image_class_decl_h_0 | FAIL | Compile: tests/test_image_class_decl_h_0.cpp:12:55: error: expected ‘;’ before ‘<’ token... |
| test_image_class_decl_h_1 | FAIL | Compile: tests/test_image_class_decl_h_1.cpp:13:49: error: invalid conversion from ‘cimg_library::CImg<floa... |
| test_image_iterators_h_0 | PASS | - |
| test_image_ops_basic_h_0 | FAIL | Compile: tests/test_image_ops_basic_h_0.cpp:11:19: error: redeclaration of ‘cimg_library::CImg<float> img2�... |
| test_image_ops_basic_h_1 | PASS | - |
| test_image_ops_basic_h_10 | PASS | - |
| test_image_ops_basic_h_11 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,0,(nil),non-shared)] CImg<float32>::load(): Failed to open file 'reference.jpg'. |
| test_image_ops_basic_h_12 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,0,(nil),non-shared)] CImg<float32>::load(): Failed to open file 'lena.jpg'. |
| test_image_ops_basic_h_13 | PASS | - |
| test_image_ops_basic_h_2 | FAIL | Compile: tests/test_image_ops_basic_h_2.cpp:14:18: error: no matching function for call to ‘cimg_library::C... |
| test_image_ops_basic_h_3 | FAIL | Compile: tests/test_image_ops_basic_h_3.cpp:14:18: error: no matching function for call to ‘cimg_library::C... |
| test_image_ops_basic_h_4 | FAIL | Compile: tests/test_image_ops_basic_h_4.cpp:11:18: error: no matching function for call to ‘cimg_library::C... |
| test_image_ops_basic_h_5 | PASS | - |
| test_image_ops_basic_h_6 | FAIL | Runtime Error (Exit: 1): CImg Exception: CImgDisplay::screen_width(): Failed to open X11 display. |
| test_image_ops_basic_h_7 | PASS | - |
| test_image_ops_basic_h_8 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,0,(nil),non-shared)] CImg<float32>::load(): Failed to open file 'reference.jpg'. |
| test_image_ops_basic_h_9 | FAIL | Compile: tests/test_image_ops_basic_h_9.cpp:14:5: error: expected unqualified-id before ‘}’ token... |
| test_image_pixels_h_0 | PASS | - |
| test_image_pixels_h_1 | PASS | - |
| test_image_pointwise_h_0 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,0,(nil),non-shared)] CImg<float32>::load(): Failed to open file 'reference.jpg'. |
| test_image_pointwise_h_1 | FAIL | Runtime Error (Exit: 1): CImg Exception: [instance(0,0,0,0,(nil),non-shared)] CImg<float32>::load(): Failed to open file 'reference.jpg'. |
| test_image_pointwise_h_2 | FAIL | Runtime Error (Exit: 1): CImg Exception: CImgDisplay::screen_width(): Failed to open X11 display. |
| test_list_base_h_0 | FAIL | Compile: tests/test_list_base_h_0.cpp:11:12: error: redeclaration of ‘cimg_library::CImgList<float> list’... |
| test_list_base_h_1 | PASS | - |
| test_list_base_h_2 | PASS | - |
| test_list_base_h_3 | FAIL | Compile: tests/test_list_base_h_3.cpp:10:2: error: ‘img1’ was not declared in this scope... |
| test_math_scalar_ops_h_0 | PASS | - |
| test_math_scalar_ops_h_1 | PASS | - |
| test_utils_h_0 | PASS | - |

**Summary:** Passed: 26, Failed: 62, Total: 88
