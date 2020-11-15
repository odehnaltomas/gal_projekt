cmake_minimum_required(VERSION 3.14)
project(gal_projekt)

set(CMAKE_CXX_STANDARD 17)
set(OGDF_BUILD_PATH PATH ./build)
set(OGDF_DIR PATH ./ogdf)

find_package(OGDF CONFIG PATHS ${OGDF_BUILD_PATH})

include_directories(${OGDF_INCLUDE_DIRS} ./include ./src) # OGDF_INCLUDE_DIRS is from find_package
link_libraries(OGDF)

file(GLOB prog_src "main.cpp" "./src/*.cpp" "./include/*.h")

add_executable(gal_projekt ${prog_src})
target_include_directories(gal_projekt PRIVATE ${OGDF_INCLUDE_DIRS} ./include ./src)
target_link_libraries(gal_projekt)
