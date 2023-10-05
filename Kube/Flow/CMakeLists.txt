kube_add_library(Flow
    SOURCES
        Base.hpp
        Graph.cpp
        Graph.hpp
        Graph.ipp
        Scheduler.cpp
        Scheduler.hpp
        Task.cpp
        Task.hpp
        Task.ipp

    LIBRARIES
        Core
)