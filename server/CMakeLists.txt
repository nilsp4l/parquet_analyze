set(EXECUTABLE_NAME "${PROJECT_NAME}_server")

add_executable(${EXECUTABLE_NAME} main.cpp)

target_include_directories(${EXECUTABLE_NAME} PRIVATE include)

find_package(Parquet REQUIRED)
find_package(ArrowDataset REQUIRED)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE proto-objects ArrowFlight::arrow_flight_shared Parquet::parquet_shared ArrowDataset::arrow_dataset_shared absl::flags_parse spdlog)

add_subdirectory(include)
add_subdirectory(src)