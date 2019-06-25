# ==============================================================================
# FindValgrind.cmake
#
# Copyright (C) 2019  xcp-ng-generic-lib
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

# Find the valgrind header.
#
# This will define the following variables:
#   Valgrind_FOUND
#   Valgrind_INCLUDE_DIRS
#   Valgrind_LIBRARIES
#
# and the following imported targets:
#   Valgrind::Valgrind

find_path(Valgrind_INCLUDE_DIR
  NAMES valgrind/valgrind.h
  PATHS /usr/include
)

mark_as_advanced(Valgrind_FOUND Valgrind_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Valgrind
  REQUIRED_VARS Valgrind_INCLUDE_DIR
)

if (Valgrind_FOUND)
  get_filename_component(Valgrind_INCLUDE_DIRS ${Valgrind_INCLUDE_DIR} DIRECTORY)
endif ()

if (Valgrind_FOUND AND NOT TARGET Valgrind::Valgrind)
  add_library(Valgrind::Valgrind INTERFACE IMPORTED)
  set_target_properties(Valgrind::Valgrind PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${Valgrind_INCLUDE_DIRS}"
  )
endif ()
