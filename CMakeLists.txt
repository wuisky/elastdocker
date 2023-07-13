# CMakeLists.txt

cmake_minimum_required(VERSION 3.0)
project(MyProject)

# Set the output directory for the compiled binaries
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

##gptsample
# Find required libraries (cURL)
find_package(CURL REQUIRED)
# Add the executable
add_executable(main cpp_sample/main.cpp)
# Link against cURL
target_link_libraries(main PRIVATE ${CURL_LIBRARIES})
# Add include directories for cURL
target_include_directories(main PRIVATE ${CURL_INCLUDE_DIRS})
