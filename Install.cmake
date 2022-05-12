# Create version config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${PROJECT_NAME}ConfigVersion.cmake
    VERSION
        ${PROJECT_VERSION}
    COMPATIBILITY
        AnyNewerVersion
)

# Install targets as config file
install(
    EXPORT      ${PROJECT_NAME}Targets
    FILE        ${PROJECT_NAME}Targets.cmake
    NAMESPACE   ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
)

# Install config files
configure_file(${PROJECT_NAME}Config.cmake.in ${PROJECT_NAME}Config.cmake @ONLY)
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake
)