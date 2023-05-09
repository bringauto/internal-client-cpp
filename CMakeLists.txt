CMAKE_MINIMUM_REQUIRED(VERSION 3.25 FATAL_ERROR)
PROJECT(internal-client CXX)

SET(CMAKE_BUILD_RPATH_USE_ORIGIN ON)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_EXTENSIONS OFF)   # For increased portability

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

FILE(GLOB_RECURSE source_files "source/*")
CMDEF_ADD_LIBRARY(
        LIBRARY_GROUP internal_client_library
        TYPE SHARED
        SOURCES ${source_files}
        INCLUDE_DIRECTORIES "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/>" "$<INSTALL_INTERFACE:include/>"

        VERSION ${INTERNAL_CLIENT_VERSION}
)

ADD_SUBDIRECTORY(${CMAKE_CURRENT_LIST_DIR}/lib/fleet-protocol)

TARGET_LINK_LIBRARIES(internal_client_library-object PUBLIC fleet-protocol::internal_client_interface fleet-protocol::protobuf_cpp)

IF (BRINGAUTO_INSTALL)
    INCLUDE(GNUInstallDirs)
    CMDEF_INSTALL(TARGET internal_client_library-shared)
    CMDEF_INSTALL(TARGET internal_client_interface)
    CMDEF_INSTALL(TARGET common_headers)
ENDIF ()

IF (BRINGAUTO_PACKAGE)
    CMDEF_PACKAGE(
            MAIN_TARGET internal_client_library-shared
            VERSION ${INTERNAL_CLIENT_VERSION}
    )
    SET(CPACK_GENERATOR ZIP)
    SET(CPACK_PACKAGE_CONTACT "BringAuto s.r.o. <maintainers@bringauto.com>")
    SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "BringAuto s.r.o <maintainers@bringauto.com>")
    SET(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
    SET(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    INCLUDE(CPack)
ENDIF ()

IF (BRINGAUTO_SAMPLES)
    ADD_SUBDIRECTORY(${CMAKE_CURRENT_LIST_DIR}/examples/)
ENDIF ()