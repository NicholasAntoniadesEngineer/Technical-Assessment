
# Overview

This repository contains example code for patch position calculations and unit tests for verifying the correctness of the algorithms implemented. 

Initially, there is a Python prototype script (`matrix_translation_rotation_prototype.py`) that demonstrates the translation 
and rotation of patches in a matrix.

The C file is implemented (`patch_position_calculation.c`) implementing functions for calculating patch positions within an array and updating patch positions 
based on array rotation. 

Finally the C implementation is unit tested with the gtest framework (`patch_position_calculations_test.cpp`)

# Files

matrix_translation_rotation_prototype.py: 
This Python script is a prototype for matrix translation and rotation. It demonstrates the translation of patches within a matrix and their rotation by 90,
180, or 270 degrees.

patch_position_calculation.c: 
This C file contains functions for calculating patch positions within an array and updating patch positions based on array rotation. 

patch_position_calculations_test.cpp: 
Unit tests for the patch position calculation functions implemented in patch_position_calculation.c. 
These tests ensure the correctness of the algorithms by comparing the calculated patch positions with expected values.

## Python Prototype
1. Run the `matrix_translation_rotation_prototype.py` script.
2. Adjust the test cases in the `main()` function to test different array sizes and rotations.

## C Code
1. Include the `patch_position_calculation.c` file in your project.
2. Include the `config.h` header file if configuration constants are required.
3. Call the functions `project_algorithms_init_patches`, `project_algorithms_calc_patch_pose`, and `project_algorithms_rot_pos_update` as needed to calculate patch positions and update them based on array rotation.

## Cpp Unit Tests
1. Include the `patch_position_calculations_test.cpp` file in your testing framework.
2. Ensure that the necessary dependencies, such as Google Test (`gtest`), are set up in your project.
3. Run the unit tests to verify the correctness of the patch position calculation functions.


# Dependencies

Google Test (`gtest`): Required for running the unit tests in `patch_position_calculations_test.cpp`.
