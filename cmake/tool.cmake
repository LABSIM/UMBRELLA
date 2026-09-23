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
#                                    COMPILATION OPTIMIZER                                  #
#############################################################################################

#[[

ccache - external tool
- [https://ccache.dev/]
- [https://cmake.org/cmake/help/latest/envvar/CMAKE_LANG_COMPILER_LAUNCHER.html]

#]]

find_program(LABSIM_CCACHE_PATH ccache)

if(LABSIM_CCACHE_PATH)

	option(LABSIM_ENABLE_CCACHE_OPTIMIZATION "Utiliser l'optimiseur de cache pour la compilation ccache" ON)

endif(LABSIM_CCACHE_PATH)

if(LABSIM_ENABLE_CCACHE_OPTIMIZATION)

    # generally... 
	set_property(
		GLOBAL 
		PROPERTY 
			RULE_LAUNCH_COMPILE ccache
	)

endif()

function(LabsimCMakeTool_ccache arg_target)

    if(LABSIM_ENABLE_CCACHE_OPTIMIZATION)
    
        set_target_properties(
            ${arg_target}
            PROPERTIES 
                CXX_COMPILER_LAUNCHER
                ${LABSIM_CCACHE_PATH}
        )
        
    endif()

endfunction()


#############################################################################################
#                                         FORMATTER                                         #
#############################################################################################

#[[

clang-format - external tool
- [https://clang.llvm.org/docs/ClangFormat.html]

#]]
find_program(CLANG_FORMAT_PATH clang-format)

if(CLANG_FORMAT_PATH)

	option(LABSIM_ENABLE_CLANG_FORMATTER "Utiliser le formatteur clang-format (see .clang-format configuration file)" ON)
    mark_as_advanced(LABSIM_ENABLE_CLANG_FORMATTER)

endif(CLANG_FORMAT_PATH)

function(LabsimCMakeTool_ClangFormat arg_target)

    if(LABSIM_ENABLE_CLANG_FORMATTER)

        # list
        set(ALL_SOURCES)

        # extract common source files
        get_target_property(
            TARGET_PRIVATE_SOURCES
            ${arg_target}
            SOURCES
        )

        # append if any
        if(TARGET_PRIVATE_SOURCES)
            list(APPEND ALL_SOURCES ${TARGET_PRIVATE_SOURCES})
        endif()

        # extract all interface_headers + header
        get_target_property(
            TARGET_INTERFACE_HEADER_SETS 
            ${arg_target} 
            INTERFACE_HEADER_SETS
        )
        
        foreach(TARGET_INTERFACE_HEADER_SET_NAME IN LISTS TARGET_INTERFACE_HEADER_SETS)

            get_target_property(
                TARGET_CURRENT_HEADER_SET  
                ${arg_target} 
                "HEADER_SET_${TARGET_INTERFACE_HEADER_SET_NAME}"
            )

            if(TARGET_CURRENT_HEADER_SET)

                list(APPEND ALL_SOURCES ${TARGET_CURRENT_HEADER_SET})  

            endif()

        endforeach()

        # check if there is any
        if(ALL_SOURCES)

            # remove duplicates 
            list(REMOVE_DUPLICATES ALL_SOURCES)

            # filter generated files under /build/ or any out of the source tree
            foreach(CURRENT_SOURCE ${ALL_SOURCES})

                # Check if the item matches the directory
                if (("${CURRENT_SOURCE}" MATCHES "(.*)/build/(.*)") 
                    OR NOT (
                        ("${CURRENT_SOURCE}" MATCHES "${CMAKE_SOURCE_DIR}/include/(.*)") 
                        OR ("${CURRENT_SOURCE}" MATCHES "${CMAKE_SOURCE_DIR}/src/(.*)")
                    )
                )

                    # Remove the item from the list
                    list(REMOVE_ITEM ALL_SOURCES ${CURRENT_SOURCE})
                
                endif()
            
            endforeach()

            # tie a pre-build custom command
            add_custom_command(
                TARGET 
                    ${arg_target}
                PRE_BUILD COMMAND
                    ${CLANG_FORMAT_PATH} -i --style=file --sort-includes ${ALL_SOURCES}
            )
            
        endif()

    endif()

endfunction()

#############################################################################################
#                                 STATIC CHECKER / LINTER                                   #
#############################################################################################

#[[

clang-tidy - external tool
- [https://clang.llvm.org/extra/clang-tidy/]

#]]
find_program(CLANG_TIDY_PATH clang-tidy)

if(CLANG_TIDY_PATH)

	option(LABSIM_ENABLE_CLANG_LINTER "Utiliser le linter clang-tidy (see .clang-tidy configuration file). !ANNULE L'UTILISATION DES PCHS!" OFF)
    mark_as_advanced(LABSIM_ENABLE_CLANG_LINTER)

endif(CLANG_TIDY_PATH)

function(LabsimCMakeTool_ClangTidy arg_target)

    if(LABSIM_ENABLE_CLANG_LINTER)

        # update target properties
        set_target_properties(
            ${arg_target} 
            PROPERTIES 
                CXX_CLANG_TIDY
            "${CLANG_TIDY_PATH};--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy"
        )

    endif()

endfunction()

#[[

Link What You Use - built-in cmake checker tool
- [https://cmake.org/cmake/help/latest/prop_tgt/LINK_WHAT_YOU_USE.html#prop_tgt:LINK_WHAT_YOU_USE]

#]]

option(LABSIM_ENABLE_LINKWHATYOUUSE "Utiliser le static checker de liens durant le linkage - LinkWhatYouUse" ON)
mark_as_advanced(LABSIM_ENABLE_LINKWHATYOUUSE)

function(LabsimCMakeTool_LinkWhatYouUse arg_target)

    if(LABSIM_ENABLE_LINKWHATYOUUSE)
        
        set_target_properties(
            ${arg_target}
            PROPERTIES 
                LINK_WHAT_YOU_USE 
            TRUE
        )
        
    endif()

endfunction()