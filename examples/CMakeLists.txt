CMAKE_MINIMUM_REQUIRED(VERSION 3.25 FATAL_ERROR)

ADD_EXECUTABLE(example main.cpp)

IF (INTERNAL_CLIENT_INSTALLED)
    FIND_PACKAGE(internal-client REQUIRED)
ENDIF ()

    TARGET_LINK_LIBRARIES(example PUBLIC internal_client_library-shared)

    TARGET_LINK_LIBRARIES(example PRIVATE protobuf_cpp) # Linking protobuf files only for example usage
