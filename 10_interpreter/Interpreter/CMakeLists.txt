set(TARGET_NAME Interpreter)
log(INFO "CMake generation for target ${TARGET_NAME}.")

log(INFO "Creating and configuration target ${TARGET_NAME}.")

add_library(${TARGET_NAME}_object OBJECT
        Grammar.h
        Grammar.cpp
        Lexer.cpp
        Lexer.h
        Syntaxer.cpp
        Syntaxer.h
        Lexeme.cpp
        Lexeme.h
        Convolution.h
        Convolution.cpp)

# properties
set_target_properties(${TARGET_NAME}_object PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

add_executable(${TARGET_NAME} $<TARGET_OBJECTS:${TARGET_NAME}_object> main.cpp)