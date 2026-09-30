#
# UMBRELLA : acronyme AH ?
# Copyright (C) 2026-present Nawfel KINANI
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
# along with this program; see the file COPYING.
# If not, see <http://www.gnu.org/licenses/>.
#

####################################################################
#                         PREPROCESSING                            #
####################################################################

####################################################################
#                           USER INPUT                             #
####################################################################

# root
get_filename_component(DEFAULT_UMBRELLA_ROOT "${CMAKE_SOURCE_DIR}" ABSOLUTE)
set(UMBRELLA_ROOT "${DEFAULT_UMBRELLA_ROOT}" CACHE PATH "Le path racine du projet Umbrella")

# this way others can override this value form command line
set(CMAKE_TOOLCHAIN_FILE ~/third_party/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE FILEPATH "Path to vcpkg toolchain")

####################################################################
#                            PROJECT                               #
####################################################################

# Definiton standard CMake
cmake_minimum_required(VERSION 3.31 FATAL_ERROR)

# default project name
project(project-Umbrella CXX)

####################################################################
#                          THIRD PARTY                             #
####################################################################

include(${UMBRELLA_ROOT}/cmake/third_party.cmake)

####################################################################
#                    	      CONFIG                               #
####################################################################

# labsim project install path
set(LABSIM_INSTALL_PATH "project/umbrella")

# labsim build 
include(${UMBRELLA_ROOT}/cmake/build.cmake)

# CMake config tuning
# --> [ https://cmake.org/cmake/help/v3.18/command/find_package.html?highlight=find_package#search-procedure ]
if(WIN32)

	set(UMBRELLA_CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/cmake/umbrella")

else()

	set(UMBRELLA_CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib/cmake/umbrella")

endif()


####################################################################
#                               TARGET                             #
####################################################################

# other LABSIM SSE titans software : Ouranos
include(cmake/titans.cmake)

# linux libstdc++ toolchain
include(cmake/linux-libstdc++-toolchain.cmake)

# tooling
include(cmake/tool.cmake)

# lib
include(cmake/umbrella/umbrella.cmake)

# bin
include(cmake/umbrella/umbrella-app.cmake)

# labsim custom targets
include(cmake/target.cmake)

####################################################################
#                           POSTPROCESSING                         #
####################################################################

if(LABSIM_ENABLE_PRECOMPILEDHEADER_OPTIMIZATION AND NOT LABSIM_ENABLE_CLANG_LINTER)

	include(cmake/umbrella/umbrella-pch.cmake)

endif()

####################################################################
#                              INSTALL                             #
####################################################################

# pkgconfig

include(CMakePackageConfigHelpers)

configure_package_config_file(
	cmake/umbrella/umbrella-config.cmake.in 
	${CMAKE_CURRENT_BINARY_DIR}/umbrella-config.cmake
	INSTALL_DESTINATION 
		${UMBRELLA_CMAKECONFIG_INSTALL_DIR}
)

write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/umbrella-config-version.cmake 
	VERSION
		1.0.0
	COMPATIBILITY 
	  	SameMajorVersion 
)

install(
	FILES 
		${CMAKE_CURRENT_BINARY_DIR}/umbrella-config.cmake 
		${CMAKE_CURRENT_BINARY_DIR}/umbrella-config-version.cmake
	DESTINATION 
		${UMBRELLA_CMAKECONFIG_INSTALL_DIR}
)

# EOF