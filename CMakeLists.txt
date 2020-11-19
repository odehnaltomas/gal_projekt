cmake_minimum_required(VERSION 3.14)
project(gal_projekt)

set(CMAKE_CXX_STANDARD 17)
set(OGDF_BUILD_PATH PATH ./build)
set(OGDF_DIR PATH ./ogdf)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

find_package(OGDF CONFIG PATHS ${OGDF_BUILD_PATH} ${OGDF_DIR})

include_directories(${OGDF_INCLUDE_DIRS} ./include ./src) # OGDF_INCLUDE_DIRS is from find_package
link_directories(./include ./src)
link_libraries(OGDF)

file(GLOB prog_src "main.cpp" "./src/*.cpp" "./src/**/*.cpp" "./include/*.h" "./include/**/*.h")

add_executable(gal_projekt ${prog_src})
target_include_directories(gal_projekt PRIVATE ${OGDF_INCLUDE_DIRS} ./include ./src)
target_link_libraries(gal_projekt)
