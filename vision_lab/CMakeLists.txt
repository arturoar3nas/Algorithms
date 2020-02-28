cmake_minimum_required(VERSION 3.5)
project(Vison_Lab LANGUAGES CXX)


#
# Debugging Options
#
set (CMAKE_VERBOSE_MAKEFILE 1) # Use 1 for debugging, 0 for release


find_program(CMAKE_CXX_HALIDE NAMES halide)
if (CMAKE_CXX_HALIDE)
  message(STATUS "  HALIDE FOUND...")
endif()



# Add local project module path so CMake includes custom CMake modules.
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")

include(CTest)
enable_testing()


#
# Create executable that uses the hello lib to say Hello.
#
add_executable(vision_lab src/main.cpp)

#
# Create a library target for the Catch header-only test framework.
#
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE test/ )

#
# Create a target for executing the Catch unit tests.
#
add_executable(unit_tests test/main.cpp)
target_include_directories(unit_tests PUBLIC test/)
target_link_libraries(unit_tests Catch)

install(TARGETS unit_tests RUNTIME DESTINATION bin)

set(CPACK_PACKAGE_NAME "Hello")
set(CPACK_PACKAGE_VENDOR "erichschroeter.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Hello - CPack Component Installation Example")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Hello CPack Component Example")


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O0 -g -DEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lopencv_imgproc -lopencv_core -lopencv_imgcodecs -L/usr/local/bin/ -lHalide -ljpeg -lpng  -lopenvx ")
# -lvx_opencv
include_directories ("/usr/local/include/")
include_directories ("/usr/local/tools/")

# --- Halide ---
link_directories ("/usr/local/bin/")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/bin/ -lHalide")

target_link_libraries(vision_lab "/usr/local/bin/libHalide.so")
target_link_libraries(vision_lab "/usr/local/lib/libopenvx.so")

# This must always be last!
include(CPack)

