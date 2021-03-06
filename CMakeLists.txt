###################################
#   CMakeLists.txt to build DAQ   #
#   Author LAGARDE Francois       #
###################################
cmake_minimum_required(VERSION 3.15...3.19.2 FATAL_ERROR)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake" "${CMAKE_CURRENT_SOURCE_DIR}/external" "${CMAKE_CURRENT_SOURCE_DIR}")
include(GetCMakeMM)
cmmm(VERSION "1.0" REPOSITORY "flagarde/CMakeMM" VERBOSITY VERBOSE DESTINATION "CMakeMM")

project(YAODAQ VERSION "0.0.1.0" DESCRIPTION "YAODAQ for RPClab" HOMEPAGE_URL "https://github.com/RPClab/YAODAQ" LANGUAGES CXX)

cmmm_modules_list(GIT_REPOSITORY "flagarde/CMakeCM" DESTINATION "Modules")

#PreventInSourceBuilds is in  common but try to avoid downloading many files
include(PreventInSourceBuilds)
prevent_in_source_builds()

include(DefaultInstallPrefix)
default_install_prefix("${CMAKE_SOURCE_DIR}/bin")

include(Standards)
cxx_17()
c_11()

#include(DefaultConfigurations)
#include(ConfigurationCheck)

#set_default_configuration("Debug")

#include(Formatters)

include(Settings)

include(Doctest)

# Ask CMake to output a compile_commands.json file for use with things like Vim YCM.
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set(CMAKE_LINK_DEPENDS_NO_SHARED TRUE)

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
  include(GNUInstallDirs)
  set(LIBRARY_OUTPUT_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
  set(RUNTIME_OUTPUT_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
  set(ARCHIVE_OUTPUT_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
  set(INCLUDE_OUTPUT_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")
  set(CMAKE_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/cmake")
else()
  set(LIBRARY_OUTPUT_DIR "${CMAKE_INSTALL_PREFIX}/lib")
  set(RUNTIME_OUTPUT_DIR "${CMAKE_INSTALL_PREFIX}")
  set(ARCHIVE_OUTPUT_DIR "${CMAKE_INSTALL_PREFIX}/lib")
  set(INCLUDE_OUTPUT_DIR "${CMAKE_INSTALL_PREFIX}/include")
  set(CMAKE_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib/cmake")
endif()

set(CMAKE_MACOSX_RPATH TRUE)
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "${LIBRARY_OUTPUT_DIR}")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
link_directories("${LIBRARY_OUTPUT_DIR}")

# FIXME
set(CMAKE_PREFIX_PATH "${CMAKE_OUTPUT_DIRECTORY}" "${CMAKE_INSTALL_PREFIX}/lib/cmake" "${CMAKE_INSTALL_PREFIX}/lib64/cmake")

# Install doctest to allow testing exectuables and test inside executables
include(CTest)

# If we want to have test in the executables
if(ENABLE_DOCTESTS)
  add_dependencies(YAODAQ::doctest)
  add_definitions(-DENABLE_DOCTEST_IN_LIBRARY)
endif()

include(ProcessorCount)
processorcount(N)
if(NOT N EQUAL 0)
  set(CTEST_BUILD_FLAGS -j${N})
endif()

include(Architectures)

set(CMAKE_MESSAGE_CONTEXT_SHOW TRUE)


if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif()

include(PositionIndependentExecutable)

# Optional IPO. Do not use IPO if it's not supported by compiler.
include(CheckIPOSupported)
check_ipo_supported(RESULT aResult OUTPUT aOutput)
if(aResult)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(WARNING "IPO is not supported:# ${aOutput}")
endif()

include(Sanitizers)

include(DependencyGraph)
gen_dep_graph(OUTPUT_DIR ${CMAKE_INSTALL_PREFIX} TARGET_NAME dependency-graph OUTPUT_TYPE pdf)

include(Tools)
clang_tidy(-format-style=file -checks=* -header-filter='${CMAKE_SOURCE_DIR}/*')
cppcheck(--enable=warning,performance,portability,missingInclude --template="[{severity}][{id}] {message} {callstack} \(On {file}:{line}\)" --suppress=missingIncludeSystem --quiet --verbose --force)
include_what_you_use(-Xiwyu)

add_subdirectory(src)

if(ENABLE_EXTRAS)
  add_subdirectory(extras)
endif()

add_subdirectory(apps)

if(ENABLE_TESTS)
  add_subdirectory(tests)
endif()

if(ENABLE_DOCS)
  add_subdirectory(docs)
endif()
