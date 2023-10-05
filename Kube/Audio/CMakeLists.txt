find_package(SDL2 REQUIRED)

kube_add_library(Audio
    SOURCES
        Base.hpp
        Device.cpp
        Device.hpp
        Input.cpp
        Input.hpp
        Manager.cpp
        Manager.hpp
        Output.cpp
        Output.hpp

    LIBRARIES
        Core
        SDL2::SDL2

    INCLUDES
        ${SDL2_INCLUDE_DIR}/..
)