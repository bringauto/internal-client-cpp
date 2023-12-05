CMAKE_MINIMUM_REQUIRED(VERSION 3.25 FATAL_ERROR)
PROJECT(internal_client CXX)

SET(CMAKE_BUILD_RPATH_USE_ORIGIN ON)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_EXTENSIONS OFF)   # For increased portability
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(INTERNAL_CLIENT_VERSION 1.0.0)

OPTION(BRINGAUTO_SAMPLES OFF)
OPTION(BRINGAUTO_TESTS "Enable tests" OFF)
OPTION(BRINGAUTO_INSTALL "Enable install" OFF)
OPTION(BRINGAUTO_PACKAGE "Enable package generation" OFF)
OPTION(BRINGAUTO_SYSTEM_DEP "Enable system dependencies" OFF)

FIND_PACKAGE(CMLIB
            COMPONENTS CMDEF CMUTIL STORAGE
            REQUIRED
            )

IF (NOT BRINGAUTO_SYSTEM_DEP)
    INCLUDE("cmake/Dependencies.cmake")
ENDIF ()

IF (BRINGAUTO_PACKAGE AND NOT BRINGAUTO_INSTALL)
    SET(BRINGAUTO_INSTALL ON
            CACHE BOOL
            "Installation forced by BRINGAUTO_PACKAGE creation"
            FORCE
            )
ENDIF ()

ADD_SUBDIRECTORY(${CMAKE_CURRENT_LIST_DIR}/lib/fleet-protocol)

ADD_LIBRARY(internal_client_api INTERFACE)
TARGET_INCLUDE_DIRECTORIES(internal_client_api INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/lib/fleet-protocol/lib/internal_client/include> $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/lib/fleet-protocol/lib/common_headers/include> $<INSTALL_INTERFACE:include>
)

FILE(GLOB_RECURSE source_files "${CMAKE_CURRENT_LIST_DIR}/source/*")

ADD_LIBRARY(internal_client SHARED ${source_files})
TARGET_INCLUDE_DIRECTORIES(internal_client PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include")
TARGET_LINK_LIBRARIES(internal_client PUBLIC internal_client_api)
TARGET_LINK_LIBRARIES(internal_client PRIVATE protobuf_cpp)


IF (BRINGAUTO_INSTALL)
    INCLUDE(GNUInstallDirs)

    INSTALL(TARGETS internal_client EXPORT internal_clientTargets
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
            RUNTIME DESTINATION bin
            INCLUDES DESTINATION include
    )

    INSTALL(TARGETS internal_client_api EXPORT internal_clientTargets
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
            RUNTIME DESTINATION bin
            INCLUDES DESTINATION include
    )

    INSTALL(
            DIRECTORY
            ${CMAKE_CURRENT_SOURCE_DIR}/lib/fleet-protocol/lib/common_headers/include/
            DESTINATION include
            COMPONENT Devel
    )

    INSTALL(
            DIRECTORY
            ${CMAKE_CURRENT_SOURCE_DIR}/lib/fleet-protocol/lib/internal_client/include/
            DESTINATION include
            COMPONENT Devel
    )

    INCLUDE(CMakePackageConfigHelpers)
    CONFIGURE_PACKAGE_CONFIG_FILE(
            cmake/internal_clientConfig.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/internal_client/internal_clientConfig.cmake
            INSTALL_DESTINATION ${LIB_INSTALL_DIR}/internal_client/cmake
    )

    WRITE_BASIC_PACKAGE_VERSION_FILE(
            "${CMAKE_CURRENT_BINARY_DIR}/internal_client/internal_clientConfigVersion.cmake"
            VERSION ${INTERNAL_CLIENT_VERSION}
            COMPATIBILITY SameMajorVersion
    )

    EXPORT(EXPORT internal_clientTargets
            FILE "${CMAKE_CURRENT_BINARY_DIR}/internal_client/internal_clientTargets.cmake"
    )

    SET(ConfigPackageLocation lib/cmake/internal_client)
    INSTALL(EXPORT internal_clientTargets
            FILE
            internal_clientTargets.cmake
            DESTINATION
            ${ConfigPackageLocation}
    )

    INSTALL(
            FILES
            cmake/internal_clientConfig.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/internal_client/internal_clientConfigVersion.cmake
            DESTINATION
            ${ConfigPackageLocation}
            COMPONENT
            Devel
    )
ENDIF ()

IF (BRINGAUTO_PACKAGE)
    SET(CPACK_GENERATOR ZIP)
    SET(CPACK_PACKAGE_CONTACT "Jakub Trubka <jakub.trubka@bringauto.com>")
    SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jakub Trubka <jakub.trubka@bringauto.com>")
    SET(CPACK_PACKAGE_VERSION ${libbringauto_logger_VERSION})
    INCLUDE(CPack)
ENDIF ()

IF (BRINGAUTO_SAMPLES)
    ADD_SUBDIRECTORY(${CMAKE_CURRENT_LIST_DIR}/examples/)
ENDIF ()