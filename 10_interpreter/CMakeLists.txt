cmake_minimum_required (VERSION 3.13)

include(Logger.cmake)
include(AdditionalFunctions.cmake)

# Global configuration
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
project ("Interpreter" VERSION 1.0.0.0 LANGUAGES C CXX)

identify_target_platform()
identify_library_postfix()

log(INFO "CMake generation for ${PROJECT_NAME} ${${PROJECT_NAME}_VERSION}")
log(INFO "Platform: ${TARGET_PLATFORM}")
log(INFO "Building type: ${CMAKE_BUILD_TYPE}")
log(DEBUG "Library postfix: ${LIBRARY_POSTFIX}")
log(DEBUG "Project directory: ${PROJECT_SOURCE_DIR}")

# Include external projects.
set(GTEST_GIT "https://github.com/google/googletest.git")
message("")
add_gtest(${GTEST_GIT})

# Include sub-projects.
message("")
add_subdirectory (Interpreter)
message("")
add_subdirectory (Interpreter.Tests)

