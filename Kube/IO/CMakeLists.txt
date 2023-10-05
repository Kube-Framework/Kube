kube_add_library(IO
    SOURCES
        Base.hpp
        File.cpp
        File.hpp
        File.ipp
        ResourceManager.cpp
        ResourceManager.hpp
        StandardPaths.hpp
        StandardPaths.ipp

    LIBRARIES
        Core
        whereami
        platform_folders

    INCLUDES
        ${CMRC_INCLUDE_DIR}
)