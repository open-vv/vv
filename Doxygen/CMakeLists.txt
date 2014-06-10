#
# Build the documentation
#
include(${CMAKE_ROOT}/Modules/Documentation.cmake)

if(BUILD_DOXYGEN)

  #
  # Configure the script and the doxyfile, then add target
  #
  configure_file(${CLITK_SOURCE_DIR}/Doxygen/doxygen.config.in
                 ${PROJECT_BINARY_DIR}/Doxygen/doxygen.config)

               configure_file(${CLITK_SOURCE_DIR}/Doxygen/itkdoxygen.pl.in
                 ${PROJECT_BINARY_DIR}/Doxygen/itkdoxygen.pl)

  add_custom_target(Documentation
    COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxygen/doxygen.config
    MAIN_DEPENDENCY ${PROJECT_BINARY_DIR}/Doxygen/doxygen.config
    DEPENDS         ${PROJECT_BINARY_DIR}/Doxygen/itkdoxygen.pl
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/Doxygen
    )

endif(BUILD_DOXYGEN)
