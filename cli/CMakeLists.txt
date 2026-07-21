set(EXECUTABLE_NAME ${PROJECT_NAME}_cli)

add_executable(${EXECUTABLE_NAME} main.cpp)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE proto-objects ArrowFlight::arrow_flight_shared spdlog absl::flags absl::flags_parse)
target_include_directories(${EXECUTABLE_NAME} PRIVATE include)


add_subdirectory(include)
add_subdirectory(src)