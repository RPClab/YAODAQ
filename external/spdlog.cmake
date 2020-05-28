if(NOT TARGET spdlog)
  include(ExternalProject)
  # misc tweakme options
  if(WIN32)
    option(SPDLOG_WCHAR_SUPPORT "Support wchar api" ON)
    option(SPDLOG_WCHAR_FILENAMES "Support wchar filenames" ON)
  endif()
  if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    option(SPDLOG_CLOCK_COARSE "Use the much faster (but much less accurate) CLOCK_REALTIME_COARSE instead of the regular clock," OFF)
  endif()
  # ----- spdlog package -----
  ExternalProject_Add(
                      spdlog_project
                      GIT_REPOSITORY ${SPDLOG_REPOSITORY}
                      GIT_TAG ${SPDLOG_VERSION}
                      GIT_PROGRESS TRUE
                      GIT_SHALLOW TRUE
                      CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD} -DCMAKE_CXX_STANDARD_REQUIRED=${CMAKE_CXX_STANDARD_REQUIRED} -DCMAKE_CXX_EXTENSIONS=${CMAKE_CXX_EXTENSIONS} -DCMAKE_POSITION_INDEPENDENT_CODE=${CMAKE_POSITION_INDEPENDENT_CODE} -SPDLOG_MASTER_PROJECT=OFF -DSPDLOG_BUILD_ALL=OFF -DSPDLOG_ENABLE_PCH=ON -DSPDLOG_BUILD_EXAMPLE=OFF -DSPDLOG_BUILD_EXAMPLE_HO=OFF -DSPDLOG_BUILD_TESTS=OFF -DSPDLOG_BUILD_TESTS_HO=OFF -DSPDLOG_BUILD_BENCH=OFF -DSPDLOG_SANITIZE_ADDRESS=OFF -DSPDLOG_BUILD_WARNINGS=OFF -DSPDLOG_INSTALL=ON -DSPDLOG_FMT_EXTERNAL=ON -DSPDLOG_FMT_EXTERNAL_HO=OFF -DSPDLOG_NO_EXCEPTIONS=OFF -DSPDLOG_WCHAR_SUPPORT=${SPDLOG_WCHAR_SUPPORT} -DSPDLOG_WCHAR_FILENAMES=${SPDLOG_WCHAR_FILENAMES} -DSPDLOG_CLOCK_COARSE=OFF -DSPDLOG_PREVENT_CHILD_FD=OFF -DSPDLOG_NO_THREAD_ID=OFF -DSPDLOG_NO_TLS=OFF -DSPDLOG_NO_ATOMIC_LEVELS=OFF
                      PREFIX ${CMAKE_BINARY_DIR}/spdlog_project
                      INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
                      LOG_DOWNLOAD ON
                    )
  add_library(spdlog INTERFACE)
  add_dependencies(spdlog spdlog_project)
  target_link_libraries(spdlog INTERFACE spdlog INTERFACE fmt::fmt)
  target_include_directories(spdlog INTERFACE "${INCLUDE_OUTPUT_DIR}/spdlog")
  target_compile_definitions(spdlog INTERFACE "SPDLOG_FMT_EXTERNAL")
endif()
