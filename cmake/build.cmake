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

#############################################################################################
#                                          GENERAL                                          #
#############################################################################################

# mandatory - install prefix
if(NOT LABSIM_INSTALL_PREFIX)

	message(FATAL_ERROR "[Labsim][build] -- FATAL ERROR : LABSIM_INSTALL_PREFIX is mandatory")

endif()

# set installation path
set(
    CMAKE_INSTALL_PREFIX 
    "${LABSIM_INSTALL_PREFIX}/${LABSIM_INSTALL_PATH}"
    CACHE PATH
        "Le path d'installation du projet"
        FORCE
)

# export compile command for IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#############################################################################################
#                                          BUILD TYPE                                       #
#############################################################################################

# Set a default build type if none was specified
set(LABSIM_DEFAULT_BUILD_TYPE "Release")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")

	# Then it should be a dev env
	set(LABSIM_DEFAULT_BUILD_TYPE "Debug")
	  
endif()

# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their definition
# and dump it in the cache along with proper documentation, otherwise set CMAKE_BUILD_TYPE
# to LABSIM_DEFAULT_BUILD_TYPE prior to calling PROJECT()
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)

	message(STATUS "[Labsim][build] -- INFO : Setting build type to '${LABSIM_DEFAULT_BUILD_TYPE}' as none was specified.")
  	set(
		CMAKE_BUILD_TYPE 
	  	"${LABSIM_DEFAULT_BUILD_TYPE}" 
	  	CACHE STRING
		  "Choose the type of build." 
		  FORCE
	)

  	# Set the possible values of build type for cmake-gui
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
	  
endif()

#############################################################################################
#                                      LIBRARY BUILD TYPE                                   #
#############################################################################################

# library build type
set(
	LABSIM_DEFAULT_LIBRARY_BUILD_TYPE
	STATIC
)
set(
	LABSIM_LIBRARY_BUILD_TYPE 
	"${LABSIM_DEFAULT_LIBRARY_BUILD_TYPE}" 
	CACHE STRING
	 "Choose the library type of build." 
)
set_property(
	CACHE 
	LABSIM_LIBRARY_BUILD_TYPE 
	PROPERTY 
		STRINGS "SHARED" "STATIC" "MODULE"
)

#############################################################################################
#                                        VERBOSE BUILD                                      #
#############################################################################################

# verbosity
option(LABSIM_ENABLE_VERBOSE_MAKEFILE "Le niveau de verbosite des Makefiles ON/OFF" OFF)

# - extra cache force ==> [https://github.com/ninja-build/ninja/issues/900#issuecomment-609007990]
if(LABSIM_ENABLE_VERBOSE_MAKEFILE)

	set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON" FORCE)

else()

	set(CMAKE_VERBOSE_MAKEFILE OFF CACHE BOOL "OFF" FORCE)

endif()

#############################################################################################
#                                        PARRALEL BUILD                                     #
#############################################################################################

option(LABSIM_ENABLE_PARALLEL_COMPILATION_OPTIMIZATION "Utiliser la compilation parallelisée sur le projet" ON)

# use cmake host system info command
cmake_host_system_information(
    RESULT LABSIM_PROCESSOR_COUNT 
    QUERY  NUMBER_OF_LOGICAL_CORES
)

# be cool :)
math(EXPR LABSIM_PROCESSOR_COUNT "${LABSIM_PROCESSOR_COUNT}-1")

# cache
set(
	LABSIM_PARALLEL_COMPILATION_PROCESSOR_COUNT
	${LABSIM_PROCESSOR_COUNT}
	CACHE 
		STRING "le nombre de process de parallelisation desiree lors de la compilation du projet"
)

# expert usage...
mark_as_advanced(
	LABSIM_PARALLEL_COMPILATION_PROCESSOR_COUNT
)

if(LABSIM_ENABLE_PARALLEL_COMPILATION_OPTIMIZATION)
	
	set(
		LABSIM_NUMBER_OF_PARALLEL_BUILD_PROCESS
		${LABSIM_PARALLEL_COMPILATION_PROCESSOR_COUNT}
		CACHE INTERNAL
			"le nombre de process de parallelisation actuel lors de la compilation du projet"
	)

else()

	set(
		LABSIM_NUMBER_OF_PARALLEL_BUILD_PROCESS
		1
		CACHE INTERNAL
			"le nombre de process de parallelisation actuel lors de la compilation du projet"
	)

endif()

#############################################################################################
#                                         UNITY BUILD                                       #
#############################################################################################

option(LABSIM_ENABLE_UNITYBUILD_OPTIMIZATION "Utiliser l'optimisation de compilation par la technique du batch aka. unity build" ON)

# unity build trigger factor
set(
    LABSIM_UNITYBUILD_TRIGGER_FACTOR
    "2"
    CACHE
        STRING
        "Le facteur multiplicateur du nombre de processeur pour le calcul de la valeur trigger du unity build" 
)

# expert usage...
mark_as_advanced(
	LABSIM_UNITYBUILD_TRIGGER_FACTOR
)

# unity build trigger
if(LABSIM_ENABLE_UNITYBUILD_OPTIMIZATION)

	# arbitrary -> N*nproc
	math(
		EXPR 
		LABSIM_UNITYBUILD_TRIGGER_VALUE 
		${LABSIM_UNITYBUILD_TRIGGER_FACTOR}*${LABSIM_PROCESSOR_COUNT}
	)

endif()

function(LabsimCMakeBuild_Unity arg_target)

	if(LABSIM_ENABLE_UNITYBUILD_OPTIMIZATION)

		get_target_property(
			LABSIM_SOURCES
			${arg_target}
			SOURCES
		)
		list(
			FILTER
			LABSIM_SOURCES
			INCLUDE 
				REGEX "\.(cpp|cxx|c|cc)"
		)
		list(
			LENGTH 
			LABSIM_SOURCES 
			LABSIM_SOURCES_COUNT
		)

		if("${LABSIM_SOURCES_COUNT}" GREATER "${LABSIM_UNITYBUILD_TRIGGER_VALUE}")

			math(
				EXPR 
				LABSIM_UNITY_BATCH_SIZE 
				${LABSIM_SOURCES_COUNT}/${LABSIM_UNITYBUILD_TRIGGER_VALUE}
			)

			set_target_properties(
				${arg_target}
				PROPERTIES 
					UNITY_BUILD ON
					UNITY_BUILD_MODE BATCH
					UNITY_BUILD_BATCH_SIZE ${LABSIM_UNITY_BATCH_SIZE}
			)

			message(
				STATUS
				"[Labsim][configure] -- INFO : [${arg_target}] sources(${LABSIM_SOURCES_COUNT}) > 2*nproc(${LABSIM_PROCESSOR_COUNT}) => ++ unity build ++ [batch_size(${LABSIM_UNITY_BATCH_SIZE})]"
			)

		else()
		
			set_target_properties(
				${arg_target}
				PROPERTIES 
					UNITY_BUILD OFF
			)
		
			message(
				STATUS
				"[Labsim][configure] -- INFO : [${arg_target}] sources(${LABSIM_SOURCES_COUNT}) <= 2*nproc(${LABSIM_PROCESSOR_COUNT}) => -- no unity build --"
			)

		endif()

	endif()

endfunction()

#############################################################################################
#                                     PRECOMPILED HEADER                                    #
#############################################################################################

option(LABSIM_ENABLE_PRECOMPILEDHEADER_OPTIMIZATION "Utiliser l'optimisation de compilation par l'utilisation des header precompiles" OFF)

# TODO

# EOF