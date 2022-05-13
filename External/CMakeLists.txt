# PCG Random generator
add_library(pcg-cpp INTERFACE)
target_include_directories(pcg-cpp INTERFACE pcg-cpp/include)

# Vulkan
find_package(Vulkan REQUIRED)

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

# CMakeRC
include(cmrc/CMakeRC.cmake)

# stb
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)

# FreeType
# find_package(freetype MODULE REQUIRED)
find_package(freetype REQUIRED)
add_library(FreeTypeStatic INTERFACE)
target_link_libraries(FreeTypeStatic INTERFACE ${FREETYPE_LIBRARIES})
target_include_directories(FreeTypeStatic INTERFACE ${FREETYPE_INCLUDE_DIRS})