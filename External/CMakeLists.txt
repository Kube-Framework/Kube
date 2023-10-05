# PCG Random generator
add_library(pcg-cpp INTERFACE)
target_include_directories(pcg-cpp INTERFACE pcg-cpp/include)

# Vulkan
if(APPLE)
    find_package(Vulkan REQUIRED COMPONENTS MoltenVK)
else()
    find_package(Vulkan REQUIRED)
endif()

# Vulkan memory allocator
add_library(VulkanMemoryAllocator INTERFACE VulkanMemoryAllocator/include/vk_mem_alloc.h)
target_include_directories(VulkanMemoryAllocator INTERFACE VulkanMemoryAllocator/include/)

# Vulkan Collection
add_library(VulkanCollection INTERFACE)
target_link_libraries(VulkanCollection
    INTERFACE
        Vulkan::Vulkan
        VulkanMemoryAllocator
)
if(APPLE)
    target_link_libraries(VulkanCollection INTERFACE Vulkan::MoltenVK)
endif()

# CMakeRC
include(cmrc/CMakeRC.cmake)

# stb
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)

# FreeType
if(MSVC)
    find_package(freetype REQUIRED)
else()
    find_package(Freetype MODULE REQUIRED)
endif()
add_library(FreetypeStatic INTERFACE)
target_link_libraries(FreetypeStatic INTERFACE ${FREETYPE_LIBRARIES})
target_include_directories(FreetypeStatic INTERFACE ${FREETYPE_INCLUDE_DIRS})

# whereami
add_library(whereami STATIC whereami/src/whereami.c whereami/src/whereami.h)
target_include_directories(whereami PUBLIC whereami/src/)

# sago
add_subdirectory(PlatformFolders SYSTEM)

# tinyfiledialogs
add_subdirectory(tinyfiledialogs SYSTEM)