cmake_minimum_required(VERSION 3.25)

project(parquet_analyze)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_BUILD_TYPE Release)

add_compile_options(-Wall -Wextra -Wconversion -pedantic -Wfatal-errors -O3)

include(FetchContent)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog
        GIT_TAG        v1.17.0
)
FetchContent_MakeAvailable(spdlog)

find_package(absl CONFIG REQUIRED)

find_package(ArrowFlight REQUIRED)

find_package(Protobuf REQUIRED)

add_subdirectory(protos)

add_subdirectory(cli)
add_subdirectory(server)


option(COPY_DATA "Copy data to the build directory" ON)

if(COPY_DATA)
    file(GLOB_RECURSE DATASETS RELATIVE ${CMAKE_SOURCE_DIR} "${CMAKE_SOURCE_DIR}/data/*")

    foreach (DATASET IN LISTS DATASETS)
        message(STATUS "Copying ${DATASET} into build directory " "${CMAKE_BINARY_DIR}/server/${DATASET}")
        configure_file(${CMAKE_SOURCE_DIR}/${DATASET} ${CMAKE_BINARY_DIR}/server/${DATASET} COPYONLY)
    endforeach ()
endif()



