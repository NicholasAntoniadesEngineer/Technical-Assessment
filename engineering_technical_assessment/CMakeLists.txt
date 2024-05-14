cmake_minimum_required(VERSION 3.10)  

project(EmbeddedSoftwareHomework)     

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

<<<<<<< Updated upstream
=======
# Sets wether or nor this will be compiled to run on the laptop or the target processor
>>>>>>> Stashed changes
add_definitions(-DMOCK_TEST)

# Add executable target with source files
add_executable(AppMain ${CMAKE_CURRENT_SOURCE_DIR}/src/app_main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/ATSAMV71.cpp)

