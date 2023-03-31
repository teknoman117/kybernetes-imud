# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

# Written by Nathaniel R. Lewis <github@nrlewis.dev>

#[=======================================================================[.rst:
Findasio
------------

Find asio headers.

Imported Targets
^^^^^^^^^^^^^^^^

``asio::asio``
  The asio library, if found.

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables in your project:

``asio_FOUND``
  true if asio is available.
``asio_INCLUDE_DIRS``
  where to find the asio headers.

#]=======================================================================]

# asio is a header only library and we just need to find the directory with
# the top-level header
set(_ASIO_PATHS "")
find_path(asio_INCLUDE_DIR NAMES asio.hpp HINTS ${_ASIO_PATHS} PATH_SUFFIXES include)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(asio
  FOUND_VAR
    asio_FOUND
  REQUIRED_VARS
    asio_INCLUDE_DIR
)

if(asio_FOUND AND NOT TARGET asio::asio)
  add_library(asio::asio INTERFACE IMPORTED)
  set_target_properties(asio::asio PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${asio_INCLUDE_DIR}"
  )
endif()

mark_as_advanced(asio_INCLUDE_DIR)

if(asio_FOUND)
  set(asio_INCLUDE_DIRS "${asio_INCLUDE_DIR}")
endif()
