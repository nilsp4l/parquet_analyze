add_library(proto-objects OBJECT query.proto)

# adapted from https://medium.com/3yourmind/generate-grpc-source-files-using-cmake-ec3acd246b45

target_link_libraries(proto-objects PUBLIC protobuf::libprotobuf)
set(PROTO_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${PROTO_BINARY_DIR})
set(PROTO_IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}")


protobuf_generate(
        TARGET proto-objects
        OUT_VAR PROTO_GENERATED_FILES
        IMPORT_DIRS ${PROTO_IMPORT_DIRS}
        PROTOC_OUT_DIR "${PROTO_BINARY_DIR}")
set_source_files_properties(${PROTO_GENERATED_FILES} PROPERTIES SKIP_UNITY_BUILD_INCLUSION on)

target_include_directories(proto-objects PUBLIC "$<BUILD_INTERFACE:${PROTO_BINARY_DIR}>")
