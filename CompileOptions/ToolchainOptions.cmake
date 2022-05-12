# Search for a triplet in environment if not explicitly set
if(NOT DEFINED VCPKG_TARGET_TRIPLET AND DEFINED ENV{VCPKG_TARGET_TRIPLET})
    message("VCPKG_TARGET_TRIPLET found in environment: " $ENV{VCPKG_TARGET_TRIPLET})
    set(VCPKG_TARGET_TRIPLET $ENV{VCPKG_TARGET_TRIPLET})
endif()

# Search for a toolchain in environment if not explicitly set
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{CMAKE_TOOLCHAIN_FILE})
    message("CMAKE_TOOLCHAIN_FILE found in environment: " $ENV{CMAKE_TOOLCHAIN_FILE})
    set(CMAKE_TOOLCHAIN_FILE $ENV{CMAKE_TOOLCHAIN_FILE})
endif()
