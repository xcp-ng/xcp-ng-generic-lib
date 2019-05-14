# ==============================================================================
# CMakeLists.txt
#
# Copyright (C) 2019  vtbx
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
# Config & flags.
# ------------------------------------------------------------------------------

project(vtbx VERSION 1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 11)

set(VTBX_LIB vtbx)
set(VTBX_CONFIG_NAME Vtbx)

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

# ------------------------------------------------------------------------------
# Sources & binary.
# ------------------------------------------------------------------------------

set(SOURCES
  src/io.c
  src/network.c
  src/string.c
)
add_library(${VTBX_LIB} SHARED ${SOURCES})

target_include_directories(${VTBX_LIB}
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_compile_options(${CUSTOM_C_FLAGS})

set_target_properties(${VTBX_LIB} PROPERTIES
  LINKER_LANGUAGE C
  VERSION ${PROJECT_VERSION}
  EXPORT_NAME ${VTBX_CONFIG_NAME}
)

# ------------------------------------------------------------------------------
# Install.
# ------------------------------------------------------------------------------

include(GNUInstallDirs)

set(VTBX_INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${VTBX_CONFIG_NAME})
set(VTBX_TARGETS ${VTBX_LIB}-targets)

set(VTBX_TARGETS_FILE ${VTBX_CONFIG_NAME}Targets.cmake)
set(VTBX_CONFIG_FILE ${VTBX_CONFIG_NAME}Config.cmake)
set(VTBX_CONFIG_VERSION_FILE ${VTBX_CONFIG_NAME}ConfigVersion.cmake)

install(TARGETS ${VTBX_LIB}
  EXPORT ${VTBX_TARGETS}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT ${VTBX_TARGETS}
  FILE ${VTBX_TARGETS_FILE}
  NAMESPACE ${VTBX_CONFIG_NAME}::
  DESTINATION ${VTBX_INSTALL_CONFIGDIR}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/${VTBX_CONFIG_VERSION_FILE}
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/config/${VTBX_CONFIG_FILE}.in
  ${CMAKE_CURRENT_BINARY_DIR}/${VTBX_CONFIG_FILE}
  INSTALL_DESTINATION ${VTBX_INSTALL_CONFIGDIR}
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${VTBX_CONFIG_FILE}
  ${CMAKE_CURRENT_BINARY_DIR}/${VTBX_CONFIG_VERSION_FILE}
  DESTINATION ${VTBX_INSTALL_CONFIGDIR}
)

export(EXPORT ${VTBX_TARGETS} FILE ${CMAKE_CURRENT_BINARY_DIR}/${VTBX_TARGETS_FILE} NAMESPACE ${VTBX_CONFIG_NAME}::)

export(PACKAGE ${VTBX_CONFIG_NAME})
