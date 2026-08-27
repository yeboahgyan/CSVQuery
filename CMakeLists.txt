# CMakeList.txt : CMake project for CSVQuery, include source and define
# project specific logic here.
#

cmake_minimum_required(VERSION 3.20)

project(CQLInterpreter LANGUAGES CXX)

include(FetchContent)

#Tabulate library for pretty printing tables in the console
FetchContent_Declare(
    tabulate
    GIT_REPOSITORY https://github.com/p-ranav/tabulate.git
    GIT_TAG v1.5
)


# replxx library for command line editing and history
FetchContent_Declare(
    replxx
    GIT_REPOSITORY https://github.com/AmokHuginnsson/replxx.git
    GIT_TAG release-0.0.4
)


# Fetch csv-parser and make it available to my-executable
# Vince's CSV Parser
FetchContent_Declare(
    csv_parser
    GIT_REPOSITORY https://github.com/vincentlaucsb/csv-parser.git
    GIT_TAG 5.3.0
)

FetchContent_MakeAvailable(
    tabulate
    replxx
    csv_parser
)

# add support for Qt5
list(APPEND CMAKE_PREFIX_PATH
    "D:/Qt/5.15.2/msvc2019_64"
)
find_package(Qt5 REQUIRED COMPONENTS Core)


set(SOURCES
    "include/about_license.h"
    "include/assignstatement.h"
    
    "include/conditionalexpression.h"
    "include/csvfile.h"
    "include/csvfile2.h"
    "include/expression.h"
    "include/functions.h"
    "include/importstatement.h"
    "include/parser.h"
    "include/selectstatement.h"
    "include/term.h"
    "include/test.h"
    "include/tokenizer.h"
    "include/types.h"
    "include/updatestatement.h"

    "src/about_license.cpp"
    "src/assignstatement.cpp"
    "src/conditionalexpression.cpp"
    "src/csvfile.cpp"
    "src/csvfile2.cpp"
    "src/expression.cpp"
    "src/functions.cpp"
    "src/importstatement.cpp"
    "src/parser.cpp"
    "src/selectstatement.cpp"
    "src/term.cpp"
    "src/test.cpp"
    "src/tokenizer.cpp"
    "src/types.cpp"
    "src/updatestatement.cpp"
    "src/main.cpp"
)

if(WIN32)
    list(APPEND SOURCES
        resources/CQLInterpreter.rc
    )
endif()

add_executable(CQLInterpreter ${SOURCES})

# Add include directory for the Boost libraries & project's own include directory
target_include_directories(CQLInterpreter PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "D:/libraries/boost_1_89_0"
)

target_compile_features(CQLInterpreter PRIVATE cxx_std_20)

target_link_libraries(CQLInterpreter PRIVATE tabulate::tabulate replxx csv Qt5::Core)

# TODO: Add tests and install targets if needed.
