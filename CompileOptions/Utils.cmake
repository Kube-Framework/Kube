# Compile every shaders and link them to target
# This function is private to this file
function(_utils_target_compile_shaders_impl TargetName)
    # Compile all shader sources
    get_filename_component(Source ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    foreach(Source IN LISTS TMP_TARGET_SHADERS)
        get_filename_component(Filename ${Source} NAME)
        get_filename_component(AbsolutePath ${Source} ABSOLUTE)
        file(RELATIVE_PATH RelativePath "${CMAKE_CURRENT_SOURCE_DIR}" "${AbsolutePath}")
        if(RelativePath MATCHES "^\\.\\.")
            message(SEND_ERROR "/!\ Cannot add file '${Source}': File must be in a subdirectory of ${CMAKE_CURRENT_SOURCE_DIR}")
            continue()
        endif()
        set(OutputPath ${CMAKE_CURRENT_BINARY_DIR}/${RelativePath})
        add_custom_command(
            COMMAND
                # glslang
                # --depfile ${Filename}.d
                # -V
                glslc
                -MD -MF ${Filename}.d
                -mfmt=bin
                -o ${OutputPath}.spv
                ${Source}
            OUTPUT ${OutputPath}.spv
            DEPENDS ${Source}
            DEPFILE ${Filename}.d
            COMMENT "[${TargetName}] Compiling shader ${Filename}"
        )
        list(APPEND ${TargetName}ShaderObjects ${OutputPath}.spv)
    endforeach()
    set(${TargetName}ShaderObjects ${${TargetName}ShaderObjects} PARENT_SCOPE)

    # Add shader custom target
    # add_custom_target(${TargetName}Shaders ALL DEPENDS ${${TargetName}ShaderObjects})
endfunction()

# Compile every resources and link them to target
# This function is private to this file
function(_utils_target_compile_resources_impl TargetName)
    # Compile shaders
    _utils_target_compile_shaders_impl(${TargetName})

    # Create a resource library with all resources & shaders
    set(Files ${TMP_TARGET_RESOURCES} ${${TargetName}ShaderObjects})
    list(LENGTH Files FileCount)
    if(${FileCount})
        cmrc_add_resource_library(${TargetName}Resources NAMESPACE ${TargetName} ${TMP_TARGET_RESOURCES} ${${TargetName}ShaderObjects})
        target_link_libraries(${TargetName} PUBLIC ${TargetName}Resources)
    endif()
endfunction()

# Create a library
# Usage:
# Tag               Args                Description
# ----------------------------------------------------
# OUTPUT            name                Output name
# LIBTYPE           STATIC|SHARED|...   Type of library
# SOURCES           path0 path1 ...     Sources files
# INCLUDES          dir0 dir1 ...       Include directories
# LIBRARIES         lib0 lib1 ...       Dependent libraries
# RESOURCES         path0 path1 ...     Resource files embedded to the target
# SHADERS           path0 path1 ...     Shader files compiled and embedded to the target
# PCH               path0 path1 ...     Precompiled headers
# TESTS             ON|OFF              Enable unit testing ('Tests' directory)
# BENCHMARKS        ON|OFF              Enable benchmarking ('Benchmarks' directory)
function(utils_add_library LibraryName)
    set(options)
    set(oneValueArgs OUTPUT TESTS BENCHMARKS LIBTYPE)
    set(multipleValueArgs SOURCES INCLUDES LIBRARIES RESOURCES SHADERS PCH)
    cmake_parse_arguments(TMP_TARGET "${options}" "${oneValueArgs}" "${multipleValueArgs}" ${ARGN})

    # If no libtype is specified, set it to static
    if(NOT DEFINED TMP_TARGET_LIBTYPE)
        set(TMP_TARGET_LIBTYPE STATIC)
    endif()

    # Set visibility
    if(TMP_TARGET_LIBTYPE STREQUAL "INTERFACE")
        set(TARGET_VISIBLITY INTERFACE)
    else()
        set(TARGET_VISIBLITY PUBLIC)
    endif()

    # Build library target
    add_library(${LibraryName} ${TMP_TARGET_LIBTYPE} ${TMP_TARGET_SOURCES})
    target_include_directories(${LibraryName} ${TARGET_VISIBLITY} ${TMP_TARGET_INCLUDES})
    target_link_libraries(${LibraryName} ${TARGET_VISIBLITY} ${TMP_TARGET_LIBRARIES})

    # Precompiled header
    if (NOT ("${TMP_TARGET_PCH}" STREQUAL ""))
        target_precompile_headers(${LibraryName} ${TARGET_VISIBLITY} ${TMP_TARGET_PCH})
    endif()

    # Set output name
    if (NOT ("${TMP_TARGET_OUTPUT}" STREQUAL ""))
        set_target_properties(${LibraryName} PROPERTIES OUTPUT_NAME ${TMP_TARGET_OUTPUT})
    endif()

    # Unit tests
    if(${TMP_TARGET_TESTS})
        enable_testing()
        add_subdirectory(Tests)
    endif()

    # Benchmarks
    if(${TMP_TARGET_BENCHMARKS})
        add_subdirectory(Benchmarks)
    endif()

    # Create a resource library with all resources & shaders
    _utils_target_compile_resources_impl(${LibraryName})
endfunction()

# Create an executable
# Usage:
# Tag           Args                Description
# ----------------------------------------------------
# OUTPUT        name                Output name
# SOURCES       path0 path1 ...     Sources files
# INCLUDES      dir0 dir1 ...       Include directories
# LIBRARIES     lib0 lib1 ...       Dependent libraries
# RESOURCES     path0 path1 ...     Resource files embedded to the target
# SHADERS       path0 path1 ...     Shader files compiled and embedded to the target
# PCH           path0 path1 ...     Precompiled headers
# TESTS         ON|OFF              Enable testing ('Tests' directory)
# BENCHMARKS    ON|OFF              Enable ('Benchmarks' directory)
# WINRC         path                Set windows application resource file
function(utils_add_executable ExecutableName)
    set(options)
    set(oneValueArgs OUTPUT TESTS BENCHMARKS WINRC)
    set(multipleValueArgs SOURCES INCLUDES LIBRARIES RESOURCES SHADERS PCH)
    cmake_parse_arguments(TMP_TARGET "${options}" "${oneValueArgs}" "${multipleValueArgs}" ${ARGN})

    # Build executable target
    if(WIN32)
        add_executable(${ExecutableName} ${TMP_TARGET_SOURCES} ${TMP_TARGET_SHADERS} ${TMP_TARGET_WINRC})
        # set_target_properties(${ExecutableName} PROPERTIES WIN32_EXECUTABLE TRUE)
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        add_executable(${ExecutableName} MACOSX_BUNDLE ${TMP_TARGET_SOURCES} ${TMP_TARGET_SHADERS})
    else()
        add_executable(${ExecutableName} ${TMP_TARGET_SOURCES} ${TMP_TARGET_SHADERS})
    endif()
    target_include_directories(${ExecutableName} PRIVATE ${TMP_TARGET_INCLUDES})
    target_link_libraries(${ExecutableName} PRIVATE ${TMP_TARGET_LIBRARIES})

    # Precompiled header
    if (NOT ("${TMP_TARGET_PCH}" STREQUAL ""))
        target_precompile_headers(${ExecutableName} PRIVATE ${TMP_TARGET_PCH})
    endif()

    # Set output name
    if (NOT ("${TMP_TARGET_OUTPUT}" STREQUAL ""))
        set_target_properties(${ExecutableName} PROPERTIES OUTPUT_NAME ${TMP_TARGET_OUTPUT})
    endif()

    # Unit tests
    if(${TMP_TARGET_TESTS})
        enable_testing()
        add_subdirectory(Tests)
    endif()

    # Benchmarks
    if(${TMP_TARGET_BENCHMARKS})
        add_subdirectory(Benchmarks)
    endif()

    # Create a resource library with all resources & shaders
    _utils_target_compile_resources_impl(${ExecutableName})
endfunction()

# Create a unit tests executable
# Usage:
# Tag           Args                Description
# ----------------------------------------------------
# OUTPUT        name                Output name
# SOURCES       path0 path1 ...     Sources files
# INCLUDES      dir0 dir1 ...       Include directories
# LIBRARIES     lib0 lib1 ...       Dependent libraries
# RESOURCES     path0 path1 ...     Resource files embedded to the target
# SHADERS       path0 path1 ...     Shader files compiled and embedded to the target
# PCH           path0 path1 ...     Precompiled headers
# COVERAGE      ON|OFF              Code coverage support
function(utils_add_unit_tests ExecutableName)
    set(options)
    set(oneValueArgs OUTPUT COVERAGE)
    set(multipleValueArgs SOURCES INCLUDES LIBRARIES RESOURCES SHADERS PCH)
    cmake_parse_arguments(TMP_TARGET "${options}" "${oneValueArgs}" "${multipleValueArgs}" ${ARGN})

    # Build unit tests executable target
    add_executable(${ExecutableName} ${TMP_TARGET_SOURCES})
    add_test(NAME ${ExecutableName} COMMAND ${ExecutableName})
    target_include_directories(${ExecutableName} PRIVATE ${TMP_TARGET_INCLUDES})
    target_link_libraries(${ExecutableName} PRIVATE ${TMP_TARGET_LIBRARIES} GTest::GTest GTest::Main)

    # Precompiled header
    if (NOT ("${TMP_TARGET_PCH}" STREQUAL ""))
        target_precompile_headers(${ExecutableName} PRIVATE ${TMP_TARGET_PCH})
    endif()

    # Set output name
    if (NOT ("${TMP_TARGET_OUTPUT}" STREQUAL ""))
        set_target_properties(${ExecutableName} PROPERTIES OUTPUT_NAME ${TMP_TARGET_OUTPUT})
    endif()

    # Coverage
    if(${TMP_TARGET_COVERAGE})
        target_compile_options(${ExecutableName} PUBLIC --coverage)
        target_link_options(${ExecutableName} PUBLIC --coverage)
    endif()

    # Create a resource library with all resources & shaders
    _utils_target_compile_resources_impl(${ExecutableName})
endfunction()

# Create a benchmark executable
# Usage:
# Tag           Args                Description
# ----------------------------------------------------
# OUTPUT        name                Output name
# SOURCES       path0 path1 ...     Sources files
# INCLUDES      dir0 dir1 ...       Include directories
# LIBRARIES     lib0 lib1 ...       Dependent libraries
# RESOURCES     path0 path1 ...     Resource files embedded to the target
# SHADERS       path0 path1 ...     Shader files compiled and embedded to the target
# PCH           path0 path1 ...     Precompiled headers
function(utils_add_benchmarks ExecutableName)
    set(options)
    set(oneValueArgs OUTPUT)
    set(multipleValueArgs SOURCES INCLUDES LIBRARIES RESOURCES SHADERS PCH)
    cmake_parse_arguments(TMP_TARGET "${options}" "${oneValueArgs}" "${multipleValueArgs}" ${ARGN})

    # Build unit benchmark target
    add_executable(${ExecutableName} ${TMP_TARGET_SOURCES})
    target_include_directories(${ExecutableName} PRIVATE ${TMP_TARGET_INCLUDES})
    target_link_libraries(${ExecutableName} PRIVATE ${TMP_TARGET_LIBRARIES} benchmark::benchmark benchmark::benchmark_main)

    # Precompiled header
    if (NOT ("${TMP_TARGET_PCH}" STREQUAL ""))
        target_precompile_headers(${ExecutableName} PRIVATE ${TMP_TARGET_PCH})
    endif()

    # Set output name
    if (NOT ("${TMP_TARGET_OUTPUT}" STREQUAL ""))
        set_target_properties(${ExecutableName} PROPERTIES OUTPUT_NAME ${TMP_TARGET_OUTPUT})
    endif()

    # Create a resource library with all resources & shaders
    _utils_target_compile_resources_impl(${ExecutableName})
endfunction()