# Using C++20
set(CMAKE_CXX_STANDARD 20)
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
    add_compile_options(
        -Wall
        -fdiagnostics-color=always
        -Wextra
        -Wpedantic
        -Wconversion
        -Wunused
        -Wold-style-cast
        -Wpointer-arith
        -Wcast-qual
        -Wno-missing-braces
        -fno-exceptions # No exceptions !
        ${WerrorFlag}
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_options(
        /Zc:preprocessor # Enable __VA_OPT__
        /D_HAS_EXCEPTIONS=0 # No exceptions !
    )
    add_compile_definitions(
        NOMINMAX # MSVC min / max issue
    )
endif()