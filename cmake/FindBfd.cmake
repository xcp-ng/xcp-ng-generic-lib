# ==============================================================================
# FindBfd.cmake
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

# Find the json-c library.
#
# This will define the following variables:
#   Bfd_FOUND
#   Bfd_INCLUDE_DIRS
#   Bfd_LIBRARIES
#
# and the following imported targets:
#   Bfd::Bfd

find_path(Bfd_INCLUDE_DIR
  NAMES bfd.h
  PATHS /usr/include
)

find_library(Bfd_LIBRARY
  NAMES bfd
  PATHS /usr/lib /usr/lib64
)

mark_as_advanced(Bfd_FOUND Bfd_INCLUDE_DIR Bfd_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Bfd
  REQUIRED_VARS Bfd_INCLUDE_DIR Bfd_LIBRARY
)

if (Bfd_FOUND)
  get_filename_component(Bfd_INCLUDE_DIRS ${Bfd_INCLUDE_DIR} DIRECTORY)
endif ()
set(Bfd_LIBRARIES ${Bfd_LIBRARY})

if (Bfd_FOUND AND NOT TARGET Bfd::Bfd)
  add_library(Bfd::Bfd INTERFACE IMPORTED)
  set_target_properties(Bfd::Bfd PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${Bfd_INCLUDE_DIRS}"
    INTERFACE_LINK_LIBRARIES "${Bfd_LIBRARIES}"
  )
endif ()
