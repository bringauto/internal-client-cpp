SET(CMAKE_FIND_USE_CMAKE_SYSTEM_PATH FALSE)

BA_PACKAGE_LIBRARY(fleet-protocol-interface v2.0.0 PLATFORM_STRING_MODE any_machine NO_DEBUG ON)
BA_PACKAGE_LIBRARY(protobuf                 v4.21.12)
BA_PACKAGE_LIBRARY(zlib                     v1.2.11)
