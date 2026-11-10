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
    Umbrella-test
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
    Umbrella-test
)
target_include_directories(
    Umbrella-test
    PUBLIC
        $<BUILD_INTERFACE:${UMBRELLA_ROOT}/include>
        $<INSTALL_INTERFACE:include>
)
target_sources(
    Umbrella-test
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
        ${UMBRELLA_ROOT}/src/test/main.cpp
)
# target_compile_definitions(
#     Umbrella-test
# 	PRIVATE 
#         PROTOBUF_ENABLE_DEBUG_LOGGING_MAY_LEAK_PII
# )
target_link_libraries(
    Umbrella-test
    PRIVATE
        Boost::headers
        Poco::Net
)

####################################################################
#                           OPTIMIZATION                           #
####################################################################

LabsimCMakeBuild_Unity(Umbrella-test)

####################################################################
#                             TOOCHAIN                             #
####################################################################

LabsimCMakeToolchain_Compiler(Umbrella-test)
LabsimCMakeToolchain_Debugger(Umbrella-test)
LabsimCMakeToolchain_Linker(Umbrella-test)
LabsimCMakeToolchain_Feature(Umbrella-test)

####################################################################
#                              TOOLING                             #
####################################################################

LabsimCMakeTool_ccache(Umbrella-test)
LabsimCMakeTool_LinkWhatYouUse(Umbrella-test)
LabsimCMakeTool_ClangFormat(Umbrella-test)
LabsimCMakeTool_ClangTidy(Umbrella-test)

####################################################################
#                              INSTALL                             #
####################################################################

# install instructions 
set_target_properties(
    Umbrella-test
    PROPERTIES 
        EXPORT_NAME umbrella-test
) 

install(
    TARGETS 
        Umbrella-test
    EXPORT 
        UMBRELLA_TEST_EXPORT
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
        UMBRELLA_TEST_EXPORT
    FILE 
        umbrella-test.cmake
    NAMESPACE 
        Umbrella::
    DESTINATION 
        ${UMBRELLA_CMAKECONFIG_INSTALL_DIR}
    COMPONENT 
        umbrella-test
    EXPORT_LINK_INTERFACE_LIBRARIES
)