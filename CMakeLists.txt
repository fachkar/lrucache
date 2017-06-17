project(cpptestbench)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE Release)
## Compiler flags
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-O2 -std=c++0x -Wall -Wextra")        ## Optimize
    set(CMAKE_EXE_LINKER_FLAGS "-s")  ## Strip binary
endif()
FIND_PACKAGE( Boost 1.54 COMPONENTS thread system REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
add_executable(cpptestbench main.cpp)
TARGET_LINK_LIBRARIES( cpptestbench ${Boost_LIBRARIES} pthread )
