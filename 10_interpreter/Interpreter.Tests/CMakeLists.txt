set(TARGET_NAME InterpreterTests)
log(INFO "CMake generation for target ${TARGET_NAME}.")

# Add source to this project's executable.
log(INFO "Creating and configuration target ${TARGET_NAME}.")
add_executable (${TARGET_NAME}
        test.cpp
        LexerCommonTests.cpp
        LexerBracketsAndFunctionsTests.cpp
        LexerLogicalTests.cpp
        LexerComparisonTests.cpp
        LexerArraysTests.cpp
        SyntaxerCommonTests.cpp
        SyntaxerPriorityTests.cpp
        SyntaxerLogicalTests.cpp
        SyntaxerComparisonTests.cpp
        SyntaxerFunctionsTests.cpp
        SyntaxerArraysTests.cpp
        )
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

# Include
add_include_directories(${TARGET_NAME} "${gtest_SOURCE_DIR}/include;${gmock_SOURCE_DIR}/include;${PROJECT_SOURCE_DIR}/")

# Link
add_link_libraries(${TARGET_NAME} "Interpreter_object;gtest;gmock")