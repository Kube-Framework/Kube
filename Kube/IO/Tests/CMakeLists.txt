kube_add_unit_tests(IOTests
    SOURCES
        tests_File.cpp
        tests_StandardPaths.cpp

    RESOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/FileTest01.txt

    LIBRARIES
        IO
)
