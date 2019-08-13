# ==============================================================================
# CMakeLists.txt
#
# Copyright (C) 2019  xcp-ng-generic
# Copyright (C) 2019  Vates SAS
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
# ==============================================================================

cmake_minimum_required(VERSION 3.13.1)

# ------------------------------------------------------------------------------
# Options.
# ------------------------------------------------------------------------------

option(BUILD_SHARED_LIBS ON)

# ------------------------------------------------------------------------------
# Config & flags.
# ------------------------------------------------------------------------------

project(xcp-ng-generic VERSION 1.2.0 LANGUAGES C)
set(CMAKE_C_STANDARD 11)

set(XCP_LIB xcp-ng-generic)
set(XCP_NAMESPACE XcpNg)
set(XCP_MODULE Generic)

set(CUSTOM_C_FLAGS
  -Wall
  -Wcast-align
  -Wconversion
  -Werror
  -Wextra
  -Wfloat-equal
  -Wformat-nonliteral
  -Wpointer-arith
  -Wsign-conversion
)

if (CMAKE_C_COMPILER_ID MATCHES "GNU")
  list(APPEND CUSTOM_C_FLAGS
    -Wlogical-op
  )
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_THREAD_PREFER_PTHREAD YES)

# Required libs.
set(LIBS)

find_package(Bfd REQUIRED)
find_package(Threads REQUIRED)

list(APPEND LIBS Bfd::Bfd Threads::Threads m)

# Optional libs.
find_package(Valgrind)
if (Valgrind_FOUND)
  set(HAVE_VALGRIND 1)
  list(APPEND LIBS Valgrind::Valgrind)
endif ()

# ------------------------------------------------------------------------------
# Sources & binary.
# ------------------------------------------------------------------------------

add_compile_options(${CUSTOM_C_FLAGS})

set(SOURCES
  src/coroutine.c
  src/file.c
  src/io.c
  src/network.c
  src/path.c
  src/stacktrace.c
  src/string.c
)

if (BUILD_SHARED_LIBS)
  add_library(${XCP_LIB} SHARED ${SOURCES})
else ()
  add_library(${XCP_LIB} STATIC ${SOURCES})
endif ()

target_include_directories(${XCP_LIB}
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    ${CMAKE_BINARY_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${XCP_LIB} PRIVATE ${LIBS})

set_target_properties(${XCP_LIB} PROPERTIES
  LINKER_LANGUAGE C
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  EXPORT_NAME ${XCP_MODULE}
)

configure_file(src/config.h.in src/config.h @ONLY)

# ------------------------------------------------------------------------------
# Install.
# ------------------------------------------------------------------------------

include(GNUInstallDirs)

set(XCP_CONFIG_NAME ${XCP_NAMESPACE}${XCP_MODULE})
set(XCP_INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${XCP_CONFIG_NAME})
set(XCP_TARGETS ${XCP_LIB}-targets)

set(XCP_TARGETS_FILE ${XCP_CONFIG_NAME}Targets.cmake)
set(XCP_CONFIG_FILE ${XCP_CONFIG_NAME}Config.cmake)
set(XCP_CONFIG_VERSION_FILE ${XCP_CONFIG_NAME}ConfigVersion.cmake)

install(TARGETS ${XCP_LIB}
  EXPORT ${XCP_TARGETS}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT ${XCP_TARGETS}
  FILE ${XCP_TARGETS_FILE}
  NAMESPACE ${XCP_NAMESPACE}::
  DESTINATION ${XCP_INSTALL_CONFIGDIR}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/${XCP_CONFIG_VERSION_FILE}
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/config/${XCP_CONFIG_FILE}.in
  ${CMAKE_CURRENT_BINARY_DIR}/${XCP_CONFIG_FILE}
  INSTALL_DESTINATION ${XCP_INSTALL_CONFIGDIR}
)

configure_file(cmake/FindBfd.cmake FindBfd.cmake COPYONLY)
configure_file(cmake/FindValgrind.cmake FindValgrind.cmake COPYONLY)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${XCP_CONFIG_FILE}
  ${CMAKE_CURRENT_BINARY_DIR}/${XCP_CONFIG_VERSION_FILE}
  ${CMAKE_CURRENT_BINARY_DIR}/FindBfd.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/FindValgrind.cmake
  DESTINATION ${XCP_INSTALL_CONFIGDIR}
)

export(EXPORT ${XCP_TARGETS} FILE ${CMAKE_CURRENT_BINARY_DIR}/${XCP_TARGETS_FILE} NAMESPACE ${XCP_NAMESPACE}::)

export(PACKAGE ${XCP_MODULE})
