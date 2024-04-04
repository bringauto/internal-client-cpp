CMAKE_MINIMUM_REQUIRED(VERSION 3.25 FATAL_ERROR)
PROJECT(internal_client CXX)

SET(CMAKE_BUILD_RPATH_USE_ORIGIN ON)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_EXTENSIONS OFF)   # For increased portability
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(INTERNAL_CLIENT_VERSION 1.1.0)

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

FIND_PACKAGE(fleet-protocol-interface 2.0.0 REQUIRED)
FIND_PACKAGE(protobuf 3.21.12 REQUIRED)

FILE(GLOB_RECURSE source_files "${CMAKE_CURRENT_LIST_DIR}/source/*")
CMDEF_ADD_LIBRARY(
        LIBRARY_GROUP internal-client
        TYPE SHARED
        SOURCES ${source_files}
        INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include/"
        VERSION ${INTERNAL_CLIENT_VERSION}
)

TARGET_LINK_LIBRARIES(internal-client-shared
        PUBLIC
        fleet-protocol-interface::common-headers-interface
        fleet-protocol-interface::internal-client-interface
        PRIVATE
        protobuf::libprotobuf
        fleet-protocol-interface::protobuf-cpp-interface
)


IF (BRINGAUTO_INSTALL)
    CMDEF_INSTALL(
            TARGET internal-client-shared
            NAMESPACE internal-client-shared::
    )
ENDIF ()

IF (BRINGAUTO_PACKAGE)
    CMDEF_PACKAGE(
            MAIN_TARGET internal-client-shared
            VERSION ${INTERNAL_CLIENT_VERSION}
    )
    SET(CPACK_GENERATOR ZIP)
    SET(CPACK_PACKAGE_CONTACT "BringAuto s.r.o. <maintainers@bringauto.com>")
    INCLUDE(CPack)
ENDIF ()

IF (BRINGAUTO_SAMPLES)
    ADD_SUBDIRECTORY(${CMAKE_CURRENT_LIST_DIR}/examples/)
ENDIF ()