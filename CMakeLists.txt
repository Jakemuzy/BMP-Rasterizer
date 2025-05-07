cmake_minimum_required(VERSION 3.18)

PROJECT (BMP)

set (CMAKE_CXX_STANDARD 17)

file( GLOB SOURCES "src/*.cpp" "src/*.c")
link_directories(${PROJECT_SOURCE_DIR}/dependencies/libs)

#Create exe
add_executable( BMP ${SOURCES} )

target_include_directories(
    BMP
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/dependencies/include
)
target_link_libraries(
    BMP
    PRIVATE 
)






