kube_add_unit_tests(ECSTests
    SOURCES
        tests_System.cpp
        tests_Executor.cpp
        tests_ComponentTable.cpp

    LIBRARIES
        ECS
)
