cmake_minimum_required(VERSION 2.6)
project(monitor)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

file(GLOB SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
# TODO: Run -Werror in CI.
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
