SET(CMAKE_FIND_USE_CMAKE_SYSTEM_PATH FALSE)

BA_PACKAGE_LIBRARY(fleet-protocol-interface v2.0.0 NO_DEBUG ON)
BA_PACKAGE_LIBRARY(protobuf                 v4.21.12)
BA_PACKAGE_LIBRARY(zlib                     v1.2.11)
IF (BRINGAUTO_SAMPLES)
    BA_PACKAGE_LIBRARY(nlohmann-json v3.10.5 NO_DEBUG ON)
endif()
