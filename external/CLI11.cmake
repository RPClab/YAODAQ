if (NOT TARGET CLI11)
include(ExternalProject)
# ----- CLI11 package -----
ExternalProject_Add(
                    CLI11
                    GIT_REPOSITORY ${CLI11_repository}
                    GIT_TAG ${CLI11_version}
                    GIT_PROGRESS TRUE
                    GIT_SHALLOW TRUE
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD} -DCMAKE_CXX_STANDARD_REQUIRED=${CMAKE_CXX_STANDARD_REQUIRED} -DCMAKE_CXX_EXTENSIONS=${CMAKE_CXX_EXTENSIONS} -DCLI11_BUILD_DOCS=FALSE -DCLI11_BUILD_TESTS=FALSE -DCLI11_BUILD_EXAMPLES=FALSE -DCLI11_BUILD_EXAMPLES_JSON=FALSE -DCMAKE_POSITION_INDEPENDENT_CODE=${CMAKE_POSITION_INDEPENDENT_CODE}
                    PREFIX ${CMAKE_BINARY_DIR}/CLI11-prefix 
                    SOURCE_DIR ${CMAKE_BINARY_DIR}/CLI11
                    INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
                    LOG_DOWNLOAD ON
                    )
endif()
