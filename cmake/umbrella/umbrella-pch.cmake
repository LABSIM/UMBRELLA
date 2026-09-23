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
#                               TARGET                             #
####################################################################

project(
    Umbrella-pch
    LANGUAGES 
        CXX
    DESCRIPTION
        "UMBRELLA"
    HOMEPAGE_URL
        "https://github.com/LABSIM/UMBRELLA"
)

####################################################################
#                            PREPROCESSING                         #
####################################################################

# collect all interface_headers + headers

list(
    APPEND 
    UMBRELLA_TARGETS 
    Umbrella
)

foreach(CURRENT_TARGET IN LISTS UMBRELLA_TARGETS)

    get_target_property(UMBRELLA_ALL_INTERFACE_HEADER_SETS ${CURRENT_TARGET} INTERFACE_HEADER_SETS)
    foreach(UMBRELLA_HEADER_SET_NAME IN LISTS UMBRELLA_ALL_INTERFACE_HEADER_SETS)

        get_target_property(UMBRELLA_CURRENT_HEADER_SET ${CURRENT_TARGET} "HEADER_SET_${UMBRELLA_HEADER_SET_NAME}")
        if(UMBRELLA_CURRENT_HEADER_SET)

            list(APPEND UMBRELLA_ALL_SOURCES_FILELIST ${UMBRELLA_CURRENT_HEADER_SET})  

        endif()

    endforeach()

    get_target_property(UMBRELLA_ALL_SOURCES ${CURRENT_TARGET} SOURCES)
    if(UMBRELLA_ALL_SOURCES)
        list(APPEND UMBRELLA_ALL_SOURCES_FILELIST ${UMBRELLA_ALL_SOURCES})
    endif()

endforeach()

# remove duplicates
list(REMOVE_DUPLICATES UMBRELLA_ALL_SOURCES_FILELIST)

####################################################################
#                          DEPENDENCIES                            #
####################################################################

####################################################################
#                             USER INPUT                           #
####################################################################

####################################################################
#                             PROCESSING                           #
####################################################################

add_library(
	Umbrella-pch-gen
    OBJECT
    EXCLUDE_FROM_ALL
    ${UMBRELLA_ALL_SOURCES_FILELIST}
)

target_include_directories(
    Umbrella-pch-gen
    PRIVATE
        $<BUILD_INTERFACE:${UMBRELLA_ROOT}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_options(
    Umbrella-pch-gen
    PRIVATE 
        $<$<CXX_COMPILER_ID:GNU>:-H>
        $<$<CXX_COMPILER_ID:GNU>:-E>
        $<$<CXX_COMPILER_ID:Clang>:--trace-includes>
        $<$<CXX_COMPILER_ID:Clang>:--preprocess>
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-fdirectives-only>
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-xc++-header>
)

# target_link_libraries(
#     Umbrella-pch-gen
#     PRIVATE
#         Boost::headers
#         Boost::program_options
#         Boost::thread
#         Boost::filesystem
#         Boost::container
#         XercesC::XercesC
# )

#[[

0. crawling
1. extracting
2. internal filtering
3. deepness filtering 
4. path formatting 
5. duplicate removing 
6. exclusion formatting 
7. finalizing 

redirect to file with dd :
 - [https://superuser.com/questions/504343/alternative-to-the-tee-command-without-stdout]

#]]
add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-0.log
    COMMAND
        ${CMAKE_COMMAND} --build . --target Umbrella-pch-gen 2>&1 | dd status=none of=${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-0.log
    COMMENT 
        "[Umbrella-pch][build] -- INFO : Generating temporary prefix file by crawling project include (compilation pre-processing only)"
    VERBATIM
    CODEGEN
)
add_custom_target(
    Umbrella-pch-0
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-0.log
)
add_custom_command(
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-0.log
        Umbrella-pch-0
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-1.log
    COMMAND
        awk "$1 ~ /^^\\./" ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-0.log 2>&1 | dd status=none of=${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-1.log
    COMMENT 
        "[Umbrella-pch][build] -- INFO : Extracting include"
    VERBATIM
    CODEGEN
)
add_custom_command(
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-0.log
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-1.log
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-2.log
    COMMAND
        grep -v "UMBRELLA/include"  ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-1.log 2>&1 | dd status=none of=${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-2.log
    COMMENT 
        "[Umbrella-pch][build] -- INFO : Filtering project internal include"
    VERBATIM
    CODEGEN
)
add_custom_command(
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-0.log
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-1.log
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-2.log
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-3.log
    COMMAND
        awk "/^\\.{1,2}[^\\.]/ { print; }" ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-2.log 2>&1 | dd status=none of=${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-3.log
    COMMENT 
        "[Umbrella-pch][build] -- INFO : Filtering from build tree with [2] level of deepness max"
    VERBATIM
    CODEGEN
)
add_custom_command(
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-0.log
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-1.log
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-2.log
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-3.log
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-4.log
    COMMAND
        sed --regexp-extended "s/\\.+\\ //g" ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-3.log 2>&1 | dd status=none of=${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-4.log
    COMMENT 
        "[Umbrella-pch][build] -- INFO : Extracting absolute include path"
    VERBATIM
    CODEGEN
)
add_custom_command(
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-0.log
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-1.log
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-2.log
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-3.log
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-4.log
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-5.log
    COMMAND
        awk "!seen[$0]++" ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-4.log 2>&1 | dd status=none of=${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-5.log
    COMMENT 
        "[Umbrella-pch][build] -- INFO : Removing duplicated entries"
    VERBATIM
    CODEGEN
)

add_custom_command(
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-0.log
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-1.log
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-2.log
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-3.log
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-4.log
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-5.log
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-6.log
    COMMAND
        sed "s/.*/\#include\ \"&\"/" ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-5.log 2>&1 | dd status=none of=${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-6.log
    COMMENT 
        "[Umbrella-pch][build] -- INFO : Formatting inclusion instructions"
    VERBATIM
    CODEGEN
)
add_custom_command(
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-0.log
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-1.log
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-2.log
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-3.log
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-4.log
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-5.log
        ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-6.log
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/Umbrella_prefix.hpp
    COMMAND
        touch ${CMAKE_CURRENT_BINARY_DIR}/Umbrella_prefix.hpp
    COMMAND
        echo "#pragma GCC system_header" >> Umbrella_prefix.hpp
    COMMAND
        cat ${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-6.log  >> Umbrella_prefix.hpp
    COMMENT 
        "[Umbrella-pch][build] -- INFO : Finalizing C++ prefix header"
    VERBATIM
    CODEGEN
)

# main PCH target
add_custom_target(
    Umbrella-pch
    DEPENDS 
        ${CMAKE_CURRENT_BINARY_DIR}/Umbrella_prefix.hpp
        Umbrella-pch-gen
    COMMENT 
        "[Umbrella-pch][build] -- INFO : C++ prefix header generated"
    VERBATIM
)

# mark as additionnal clean
set_target_properties(
    Umbrella-pch
    PROPERTIES 
        ADDITIONAL_CLEAN_FILES
    "${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-0.log;${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-1.log;${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-2.log;${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-3.log;${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-4.log;${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-5.log;${CMAKE_CURRENT_BINARY_DIR}/build_pch-ouranos-6.log;${CMAKE_CURRENT_BINARY_DIR}/Umbrella_prefix.hpp"
)

####################################################################
#                           OPTIMIZATION                           #
####################################################################

####################################################################
#                           POSTPROCESSING                         #
####################################################################

# tail dependancies

foreach(CURRENT_TARGET IN LISTS UMBRELLA_TARGETS)

    add_dependencies(
        ${CURRENT_TARGET} 
        Umbrella-pch
    )

    # enable pch
    target_precompile_headers(
        ${CURRENT_TARGET} 
        PRIVATE
            ${CMAKE_CURRENT_BINARY_DIR}/Umbrella_prefix.hpp
    )

endforeach()

# EOF