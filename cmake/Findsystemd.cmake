# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

# Written by Nathaniel R. Lewis <github@nrlewis.dev>

#[=======================================================================[.rst:
Findsystemd
------------

Find systemd library and include files.

Imported Targets
^^^^^^^^^^^^^^^^

This module defines the :prop_tgt:`IMPORTED` targets:

``systemd::systemd``
  Defined if the system has systemd.

Result Variables
^^^^^^^^^^^^^^^^

This module defines the following variables:

``systemd_FOUND``
  True if ``systemd`` was found.

``systemd_INCLUDE_DIRS``
  Where to find the systemd headers.

``systemd_LIBRARIES``
  List of libraries for using ``systemd``.

#]=======================================================================]

include(FindPackageHandleStandardArgs)

find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(PC_systemd QUIET libsystemd)
endif()

find_path(systemd_INCLUDE_DIR 
  NAMES systemd/sd-daemon.h
  HINTS ${PC_systemd_INCLUDE_DIRS}
  PATH_SUFFIXES include
)
mark_as_advanced(systemd_INCLUDE_DIR)

find_library(systemd_LIBRARY
  NAMES systemd
  HINTS ${PC_systemd_LIBRARIES}
)
mark_as_advanced(systemd_LIBRARY)

find_package_handle_standard_args(systemd
  FOUND_VAR
    systemd_FOUND
  REQUIRED_VARS
    systemd_INCLUDE_DIR
    systemd_LIBRARY
)

if(systemd_FOUND AND NOT TARGET systemd::systemd)
  add_library(systemd::systemd UNKNOWN IMPORTED)
  set_target_properties(systemd::systemd PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${systemd_INCLUDE_DIR}"
    IMPORTED_LOCATION "${systemd_LIBRARY}"
  )
endif()

if(systemd_FOUND)
  set(systemd_INCLUDE_DIRS "${systemd_INCLUDE_DIR}")
endif()