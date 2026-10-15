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
#                            PREPROCESSING                         #
####################################################################


####################################################################
#                               TARGET                             #
####################################################################

project(
    Umbrella
    LANGUAGES 
        CXX C
    DESCRIPTION
        "UMBRELLA"
    HOMEPAGE_URL
        "https://github.com/LABSIM/UMBRELLA"
)

####################################################################
#                            PREPROCESSING                         #
####################################################################

###################################################################
#                          DEPENDENCIES                            #
####################################################################

####################################################################
#                             USER INPUT                           #
####################################################################

# ForceSeatDI 
set(
	FORCESEATDI_VERSION
	"2.191" 
	CACHE 
        STRING "La version de la librairie ForceSeatDI"
)
set(
	FORCESEATDI_DIR
	"~/third_party/forceseat-di-${FORCESEATDI_VERSION}" 
	CACHE 
		PATH "Le path d'installation de la librairie ForceSeatDI"
)
set(
	FORCESEATDI_LIBRARIES
	"${FORCESEATDI_DIR}/ForceSeatDI64.LinuxPC.so" 
	CACHE 
		FILEPATH "La librairie ForceSeatDI a utiliser"
)
set(
	FORCESEATDI_INCLUDEDIR
	"${FORCESEATDI_DIR}/code" 
	CACHE 
		PATH "La librairie ForceSeatDI a utiliser"
)

# then 
configure_file(
    ${UMBRELLA_ROOT}/src/lib/ForceSeatDI_Loader.c.in 
    ${CMAKE_CURRENT_BINARY_DIR}/ForceSeatDI_Loader.c
    @ONLY
)

####################################################################
#                             PROCESSING                           #
####################################################################

# generate
add_library(
    Umbrella
    ${LABSIM_LIBRARY_BUILD_TYPE}
)
target_include_directories(
    Umbrella
    PUBLIC
        $<BUILD_INTERFACE:${UMBRELLA_ROOT}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${FORCESEATDI_INCLUDEDIR}
)
target_sources(
    Umbrella
    INTERFACE
        FILE_SET 
            interface_header
        TYPE 
            HEADERS
        BASE_DIRS
            $<BUILD_INTERFACE:${UMBRELLA_ROOT}/include>
            $<INSTALL_INTERFACE:include>
        FILES
            ${UMBRELLA_ROOT}/include/Umbrella/Umbrella_API.hpp
    PUBLIC
        FILE_SET
            public_header
        TYPE 
            HEADERS
        BASE_DIRS
            $<BUILD_INTERFACE:${UMBRELLA_ROOT}/include>
            $<INSTALL_INTERFACE:include>
        FILES
            ${UMBRELLA_ROOT}/include/Umbrella/Umbrella.hpp
            ${UMBRELLA_ROOT}/include/Umbrella/Umbrella_Common.hpp
            ${UMBRELLA_ROOT}/include/Umbrella/Umbrella_ApplicationManager.hpp
            ${UMBRELLA_ROOT}/include/Umbrella/Umbrella_MotionPlatformManager.hpp
            ${UMBRELLA_ROOT}/include/Umbrella/Umbrella_CommandLineOptions.hpp
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/ForceSeatDI_Loader.c
        ${UMBRELLA_ROOT}/src/lib/Umbrella.cpp
        ${UMBRELLA_ROOT}/src/lib/Umbrella_ApplicationManager.cpp
        ${UMBRELLA_ROOT}/src/lib/Umbrella_MotionPlatformManager.cpp
        ${UMBRELLA_ROOT}/src/lib/Umbrella_CommandLineOptions.cpp
)
target_compile_definitions(
    Umbrella
	PRIVATE 
        BUILDING_UMBRELLA_API
        UMBRELLA_LIB_SOURCE
)
target_link_libraries(
    Umbrella
    PRIVATE
        Boost::headers
        Boost::thread
        Boost::filesystem
        Boost::program_options
        dl
        pthread
        Ouranos::log
        Ouranos::pattern
)

####################################################################
#                           OPTIMIZATION                           #
####################################################################

LabsimCMakeBuild_Unity(Umbrella)

####################################################################
#                             TOOCHAIN                             #
####################################################################

LabsimCMakeToolchain_Compiler(Umbrella)
LabsimCMakeToolchain_Debugger(Umbrella)
LabsimCMakeToolchain_Linker(Umbrella)
LabsimCMakeToolchain_Feature(Umbrella)

####################################################################
#                           POSTPROCESSING                         #
####################################################################

if(LABSIM_ENABLE_PRECOMPILEDHEADER_OPTIMIZATION AND NOT LABSIM_ENABLE_CLANG_LINTER)

	include(${UMBRELLA_ROOT}/cmake/umbrella-pch.cmake)

endif()

####################################################################
#                              TOOLING                             #
####################################################################

LabsimCMakeTool_ccache(Umbrella)
LabsimCMakeTool_LinkWhatYouUse(Umbrella)
LabsimCMakeTool_ClangFormat(Umbrella)
LabsimCMakeTool_ClangTidy(Umbrella)

####################################################################
#                              INSTALL                             #
####################################################################

# install instructions 
set_target_properties(
    Umbrella
    PROPERTIES 
        EXPORT_NAME umbrella
) 

install(
    TARGETS 
        Umbrella
    EXPORT 
        UMBRELLA_APP_EXPORT
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/$<CONFIG>
    LIBRARY
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/$<CONFIG>
    ARCHIVE 
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/static/$<CONFIG>
    FILE_SET interface_header
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/
    FILE_SET public_header
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/
)

install(
    EXPORT 
        UMBRELLA_APP_EXPORT
    FILE 
        umbrella.cmake
    NAMESPACE 
        Umbrella::
    DESTINATION 
        ${UMBRELLA_CMAKECONFIG_INSTALL_DIR}
    COMPONENT 
        umbrella
    EXPORT_LINK_INTERFACE_LIBRARIES
)
