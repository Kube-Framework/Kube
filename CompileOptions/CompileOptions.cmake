# Using C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Handle Werror flag
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    if (KF_WERROR)
        set(WerrorFlag "-Werror")
    endif()
else()
    set(WerrorFlag "")
endif()

# Add compiler specific flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Weffc++$<SEMICOLON>-Wcast-qual$<SEMICOLON>-Wold-style-cast$<SEMICOLON>-Wno-non-virtual-dtor>$<SEMICOLON>>)
    add_compile_options(
        -fPIC
        -Wall
        -fdiagnostics-color=always
        -Wextra
        -Wpedantic
        -Wconversion
        -Wunused
        -Wpointer-arith
        -Wno-missing-field-initializers
        ${WerrorFlag}
    )
    # No exceptions !
    if(KF_NOEXCEPT)
        add_compile_options(-fno-exceptions)
    endif()
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(
            -Wno-dtor-name
            -Wgnu-alignof-expression
            -fdeclspec
            -Wno-bitwise-instead-of-logical
            -Wno-nullability-extension
            -Wno-nullability-completeness
            -Wno-float-conversion
            -Wno-missing-braces
        )
        # add_link_options(
        #     -stdlib=stdc++
        #     -lc++abi
        # )
    endif()
    # Add debug flags
    if(KF_DEEP_DEBUG)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
        set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    # Enable __VA_OPT__
    add_compile_options(/Zc:preprocessor)
    # No exceptions !
    if(KF_NOEXCEPT)
        add_compile_options(/D_HAS_EXCEPTIONS=0)
    endif()
    add_compile_definitions(
        NOMINMAX # MSVC min / max issue
    )
endif()