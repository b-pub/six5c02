cmake_minimum_required(VERSION 3.10)

project(six5c02 CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -Wall -Werror")

include_directories(include)

# Define the six5c02 static library

add_library(six5c02 STATIC
    src/insns_6502.cpp
    src/insns_65c02.cpp
    src/instructions.cpp
    src/modes.cpp
    src/six5c02.cpp
    src/clientutils.cpp
)

#----------------
# Add one test executable as a demo
add_executable(runtests
    tests/runtests.cpp
)

target_link_libraries(runtests six5c02)


#----------------
# Add a more complete example of an emulated
# machine with memory-mapped I/O.
add_executable(bbmachine
    bbmachine/main.cpp
)

target_link_libraries(bbmachine six5c02)
