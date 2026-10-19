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
    Umbrella-app
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

####################################################################
#                          DEPENDENCIES                            #
####################################################################

####################################################################
#                             USER INPUT                           #
####################################################################

####################################################################
#                             PROCESSING                           #
####################################################################

# generate
add_executable(
    Umbrella-app
)
target_include_directories(
    Umbrella-app
    PUBLIC
        $<BUILD_INTERFACE:${UMBRELLA_ROOT}/include>
        $<INSTALL_INTERFACE:include>
)
target_sources(
    Umbrella-app
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
    PRIVATE
        ${UMBRELLA_ROOT}/src/app/main.cpp
)
# target_compile_definitions(
#     Umbrella-app
# 	PRIVATE 
#         PROTOBUF_ENABLE_DEBUG_LOGGING_MAY_LEAK_PII
# )
target_link_libraries(
    Umbrella-app
    PRIVATE
        Umbrella
        Boost::headers
)

####################################################################
#                           OPTIMIZATION                           #
####################################################################

LabsimCMakeBuild_Unity(Umbrella-app)

####################################################################
#                             TOOCHAIN                             #
####################################################################

LabsimCMakeToolchain_Compiler(Umbrella-app)
LabsimCMakeToolchain_Debugger(Umbrella-app)
LabsimCMakeToolchain_Linker(Umbrella-app)
LabsimCMakeToolchain_Feature(Umbrella-app)

####################################################################
#                              TOOLING                             #
####################################################################

LabsimCMakeTool_ccache(Umbrella-app)
LabsimCMakeTool_LinkWhatYouUse(Umbrella-app)
LabsimCMakeTool_ClangFormat(Umbrella-app)
LabsimCMakeTool_ClangTidy(Umbrella-app)

####################################################################
#                              INSTALL                             #
####################################################################

# install instructions 
set_target_properties(
    Umbrella-app
    PROPERTIES 
        EXPORT_NAME broker-app
) 

install(
    TARGETS 
        Umbrella-app
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
)

install(
    EXPORT 
        UMBRELLA_APP_EXPORT
    FILE 
        umbrella-app.cmake
    NAMESPACE 
        Umbrella::
    DESTINATION 
        ${UMBRELLA_CMAKECONFIG_INSTALL_DIR}
    COMPONENT 
        umbrella-app
    EXPORT_LINK_INTERFACE_LIBRARIES
)