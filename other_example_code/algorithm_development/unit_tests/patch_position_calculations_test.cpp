/**
 * @file patch_position_calculations_test.cpp
 * @brief Unit tests for the patch orientation step of the pipeline
 * 
 * @author Nicholas Antoniades
 * @date 27 April 2024
 *
 */

#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>


extern "C" 
{
    #include "project_algorithms-algorithms_board_metadata_provider.h"
    #include "project_algorithms_algorithm_tile.h"
    #include "config.h"
}


#define SAVE_TO_CSV false
#define ENABLE_DEBUG_PRINT false  // Set this to true for detailed debug outputs during tests
const std::string base_directory = "lib/project_algorithms_beamforming_applet/api/project_algorithms_algorithm_tile/project_algorithms_algorithm_tile_tests/";

int NUMBER_OF_PATCHES = TILE_PATCHES_X*TILE_PATCHES_Y;


/**
 * @brief Reads patch poses from a CSV file and stores them in a vector of Poses.
 * 
 * @param filename Name of the CSV file to read, relative to a predefined base directory.
 * @return std::vector<Pose> A vector containing all the poses read from the file.
 */
std::vector<Pose> _patch_pose_read_in_csv(const std::string& filename) 
{
    std::vector<Pose> poses;
    std::string line;

    const std::string csv_file = base_directory + filename;
    std::ifstream file(csv_file);

    if (!file.is_open()) 
    {
        std::cerr << "Failed to open " << csv_file << std::endl;
        return {};   
    }
    if (ENABLE_DEBUG_PRINT)  std::cout << "Opened CSV file: " << csv_file << std::endl;

    std::getline(file, line); // Skip the header

    while (getline(file, line))
    {
        if (ENABLE_DEBUG_PRINT) std::cout << "Reading line: " << line << std::endl;
        std::stringstream ss(line);
        std::string cell;
        std::getline(ss, cell, ','); // Skip index part

        Pose pose;      
        int element_counter = 0;
        while (std::getline(ss, cell, ',')) 
        {
            size_t start = cell.find('(');
            size_t end = cell.find(')');
            std::string numbers = cell.substr(start + 1, end - start - 1);
            std::istringstream numStream(numbers);
            double value;
            numStream >> value;

            if (element_counter % 2 == 0) 
            {
                pose.x = value;
            } else {
                pose.y = value;
                poses.push_back(pose);
            }
            element_counter++;
        }
    }
    file.close();
    if (ENABLE_DEBUG_PRINT) std::cout << "Total poses read: " << poses.size() << std::endl;
    return poses;
}


/**
 * @brief Compares two vectors of poses to check if they match.
 * 
 * @param expected Vector of expected poses.
 * @param actual Vector of actual poses that are being tested.
 * @param tolerance Maximum allowed difference between corresponding poses to still consider them equal.
 * @return bool True if all corresponding poses are within the tolerance, otherwise false.
 */
bool _patch_pose_compare(const std::vector<Pose>& expected, const std::vector<Pose>& actual, double tolerance = 0.001) 
{
    if (expected.size() != actual.size()) 
    {
        std::cerr << "Array size mismatch: expected " << expected.size() << ", got " << actual.size() << std::endl;
        return false;
    }
    for (size_t i = 0; i < expected.size(); ++i) 
    {
        if (std::abs(expected[i].x - actual[i].x) > tolerance || std::abs(expected[i].y - actual[i].y) > tolerance) 
        {
            std::cerr << "Mismatch at index " << i << ": expected (" << expected[i].x << ", " << expected[i].y 
                      << "), got (" << actual[i].x << ", " << actual[i].y << ")" << std::endl;
            return false;
        }
    }
    return true;
}

/**
 * @brief Prints and optionally saves the poses of an array of patches to a CSV file.
 * 
 * @param message A header or message to print before listing the poses.
 * @param patches Array of patch structures containing the pose data.
 * @param size Total number of patches in the array.
 * @param row_width Number of patches per row for formatting the output.
 * @param save_csv Boolean flag to determine whether to save the output to a CSV file.
 * @param filename Optional filename for saving the CSV output, used only if save_csv is true.
 */
void _patch_pose_print_and_save(const char* message, struct project_algorithms_algorithm_EW_patch_t patches[], int size, int row_width, bool save_csv, const std::string& filename = "") 
{
    std::ofstream file;
    if (save_csv) {
        std::string fullPath = base_directory + filename;
        file.open(fullPath, std::ios::app);
        if (!file.is_open()) 
        {
            std::cerr << "Failed to open file: " << fullPath << std::endl;
            return;
        }
        file << message << "\n";
    }

    if (ENABLE_DEBUG_PRINT) std::cout << message << std::endl;

    for (int i = 0; i < size; i += row_width) {
        if (ENABLE_DEBUG_PRINT) std::cout << std::setw(4) << i << "-" << std::setw(3) << (i + row_width - 1) << ": ";
        if (save_csv) file << i << "-" << (i + row_width - 1) << ",";

        for (int j = i; j < i + row_width && j < size; ++j) 
        {
            if (ENABLE_DEBUG_PRINT) 
            {
                std::cout << "(" << std::fixed << std::setprecision(4) << std::setw(4) << patches[j].pose.t_x
                          << "," << std::fixed << std::setprecision(4) << std::setw(4) << patches[j].pose.t_y << ")";
            }
            if (save_csv) file << "(" << patches[j].pose.t_x << "," << patches[j].pose.t_y << ")";

            if (j < i + row_width - 1 && j < size - 1) 
            {
                if (ENABLE_DEBUG_PRINT) std::cout << ",";
                if (save_csv) file << ",";
            }
        }
        if (ENABLE_DEBUG_PRINT) std::cout << std::endl;
        if (save_csv) file << "\n";
    }
    if (save_csv) file.close();
}

/**
 * @brief Test pose calculation for the first column and row of the tile array.
 * 
 * Validates that the pose calculation for the top-left corner of the tile array matches
 * expected values from a CSV file, ensuring the algorithm's initial accuracy.
 */
TEST(project_algorithms_algorithm_tile, pose_calculation_col_0_row_0) 
{
    struct project_algorithms_algorithm_EW_patch_t array_patches[NUMBER_OF_PATCHES] = {0};
    const std::string file_path = "patch_pose_col_0_row_0.csv";

    project_algorithms_algorithms_tile_calc_patch_pose(COL_0, ROW_0, TILE_PATCHES_X, TILE_PATCHES_Y, PATCH_SPACING, array_patches);
    _patch_pose_print_and_save("Patch pose for position (col 0, row 0):", array_patches, NUMBER_OF_PATCHES, TILE_PATCHES_X, SAVE_TO_CSV, file_path);

    std::vector<Pose> expected = _patch_pose_read_in_csv(file_path);
    std::vector<Pose> actual;

    for (int i = 0; i < NUMBER_OF_PATCHES; ++i) 
    {
        actual.push_back(Pose{array_patches[i].pose.t_x, array_patches[i].pose.t_y});
    }
    ASSERT_TRUE(_patch_pose_compare(expected, actual));
}

/**
 * @brief Test pose calculation for the second column and first row of the tile array.
 * 
 * Checks the accuracy of pose calculations for the second column at the top of the tile array
 * against expected values from a CSV, verifying algorithm correctness for subsequent columns.
 */
TEST(project_algorithms_algorithm_tile, pose_calculation_col_1_row_0) 
{
    struct project_algorithms_algorithm_EW_patch_t array_patches[NUMBER_OF_PATCHES] = {0};
    const std::string file_path = "patch_pose_col_1_row_0.csv";
    
    project_algorithms_algorithms_tile_calc_patch_pose(COL_1, ROW_0, TILE_PATCHES_X, TILE_PATCHES_Y, PATCH_SPACING, array_patches);
    _patch_pose_print_and_save("Patch pose for position (col 1, row 0):", array_patches, NUMBER_OF_PATCHES, TILE_PATCHES_X, SAVE_TO_CSV, file_path);

    std::vector<Pose> expected = _patch_pose_read_in_csv(file_path);
    std::vector<Pose> actual;
    for (int i = 0; i < NUMBER_OF_PATCHES; ++i) 
    {
        actual.push_back(Pose{array_patches[i].pose.t_x, array_patches[i].pose.t_y});
    }
    ASSERT_TRUE(_patch_pose_compare(expected, actual));
}

/**
 * @brief Test 90-degree rotation pose updates for the tile array.
 * 
 * Tests the algorithm's ability to correctly update poses when the tile array is rotated
 * 90 degrees clockwise, comparing the result to expected values.
 */
TEST(project_algorithms_algorithm_tile, pose_rotation_90) 
{
    struct project_algorithms_algorithm_EW_patch_t array_patches[NUMBER_OF_PATCHES] = {0};
    const std::string file_path = "patch_rotation_90.csv";
    uint16_t tile_rotation = 90;

    project_algorithms_algorithms_tile_calc_patch_pose(COL_0, ROW_0, TILE_PATCHES_X, TILE_PATCHES_Y, PATCH_SPACING, array_patches);
    project_algorithms_algorithms_tile_rot_pos_update(tile_rotation, TILE_PATCHES_X, TILE_PATCHES_Y, array_patches);
    _patch_pose_print_and_save("After 90' clockwise rotation:", array_patches, NUMBER_OF_PATCHES, TILE_PATCHES_X, SAVE_TO_CSV, file_path);

    std::vector<Pose> expected = _patch_pose_read_in_csv(file_path);
    std::vector<Pose> actual;
    for (int i = 0; i < NUMBER_OF_PATCHES; ++i) 
    {
        actual.push_back(Pose{array_patches[i].pose.t_x, array_patches[i].pose.t_y});
    }
    ASSERT_TRUE(_patch_pose_compare(expected, actual));
}

/**
 * @brief Test 90-degree rotation pose updates for the tile array.
 * 
 * Tests the algorithm's ability to correctly update poses when the tile array is rotated
 * 90 degrees clockwise, comparing the result to expected values.
 */
TEST(project_algorithms_algorithm_tile, pose_rotation_180) 
{
    struct project_algorithms_algorithm_EW_patch_t array_patches[NUMBER_OF_PATCHES] = {0};
    const std::string file_path = "patch_rotation_180.csv";
    uint16_t tile_rotation = 180;

    project_algorithms_algorithms_tile_calc_patch_pose(COL_0, ROW_0, TILE_PATCHES_X, TILE_PATCHES_Y, PATCH_SPACING, array_patches);
    project_algorithms_algorithms_tile_rot_pos_update(tile_rotation, TILE_PATCHES_X, TILE_PATCHES_Y, array_patches);
    _patch_pose_print_and_save("After 180' clockwise rotation:", array_patches, NUMBER_OF_PATCHES, TILE_PATCHES_X, SAVE_TO_CSV, file_path);

    std::vector<Pose> expected = _patch_pose_read_in_csv(file_path);
    std::vector<Pose> actual;
    for (int i = 0; i < NUMBER_OF_PATCHES; ++i) 
    {
        actual.push_back(Pose{array_patches[i].pose.t_x, array_patches[i].pose.t_y});
    }
    ASSERT_TRUE(_patch_pose_compare(expected, actual));
}

/**
 * @brief Test 270-degree rotation pose updates for the tile array.
 * 
 * Confirms that poses are accurately updated when the tile array undergoes a 270-degree
 * clockwise rotation, according to predefined expected outcomes.
 */
TEST(project_algorithms_algorithm_tile, pose_rotation_270) 
{
    struct project_algorithms_algorithm_EW_patch_t array_patches[NUMBER_OF_PATCHES] = {0};
    const std::string file_path = "patch_rotation_270.csv";
    uint16_t tile_rotation = 270;

    project_algorithms_algorithms_tile_calc_patch_pose(COL_0, ROW_0, TILE_PATCHES_X, TILE_PATCHES_Y, PATCH_SPACING, array_patches);
    project_algorithms_algorithms_tile_rot_pos_update(tile_rotation, TILE_PATCHES_X, TILE_PATCHES_Y, array_patches);
    _patch_pose_print_and_save("After 270' clockwise rotation:", array_patches, NUMBER_OF_PATCHES, TILE_PATCHES_X, SAVE_TO_CSV, file_path);

    std::vector<Pose> expected = _patch_pose_read_in_csv(file_path);
    std::vector<Pose> actual;
    for (int i = 0; i < NUMBER_OF_PATCHES; ++i) 
    {
        actual.push_back(Pose{array_patches[i].pose.t_x, array_patches[i].pose.t_y});
    }
    ASSERT_TRUE(_patch_pose_compare(expected, actual));
}
