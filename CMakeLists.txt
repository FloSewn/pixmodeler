#***********************************************************
# This is a simple cmake file for creating a
# single-file application, that depends on
# system libraries
#
# Reference: https://stackoverflow.com/questions/39598323/how-to-properly-link-libraries-with-cmake
#***********************************************************
cmake_minimum_required(VERSION 3.10)
project(cad_tool)

# Set C++ flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

# Define the main module, which also holds the executable
add_executable(cad_tool
               Shape.cpp
               Grid.cpp
               Cursor.cpp
               Menu.cpp
               ModelSpace.cpp
               main.cpp)

# Link system libaries
target_link_libraries(cad_tool -lX11)
target_link_libraries(cad_tool -lGL)
target_link_libraries(cad_tool -lpthread)
target_link_libraries(cad_tool -lpng)
target_link_libraries(cad_tool -lstdc++fs)

