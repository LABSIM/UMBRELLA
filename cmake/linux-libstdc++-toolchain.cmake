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

#[[

on LINUX platform, we assume at least:

+ GNU Toolchain :
    > GCC
    > GDB 
    > Binutils : GNU ar/gold

+ LLVM Toolchain :
    > clang
    > lldb
    > lld
    > libc++ & libc++abi

#]] 

####################################################################
#                             COMPILER                             #
####################################################################

option(LABSIM_ENABLE_COMPILATION_OPTIMIZATION "Utiliser les options d'optimisations de compilation du projet (valeur par defaut)" ON)
option(LABSIM_ENABLE_COMPILATION_WARN "Utiliser les warning de compilation avancées sur le projet (valeur par defaut)" ON)

function(LabsimCMakeToolchain_Compiler arg_TARGET)

    set(status "[Labsim][configure] -- INFO : [${arg_TARGET}] Compiler optimization overriding")

    # process optionnal args
    set(
        options_args
        OVERRIDE
        COMPILATION_OPTIMIZATION
        COMPILATION_WARN
    )
    set(
        one_value_args
        ""
    )
    set(
        multi_value_args 
        ""
    )
    cmake_parse_arguments(PARSE_ARGV 1 arg "${options_args}" "${one_value_args}" "${multi_value_args}")

     #[[
        
    ==========================================================================================
    GNU GCC / LLVM Clang
    ------------------------------------------------------------------------------------------

    -stdlib=libstdc++

    -fvisibility=hidden 
        set default symbol visibility to hidden (very important to obtain small binaries)
    
    -fvisibility-inlines-hidden
        This switch declares that the user does not attempt to compare pointers to inline 
        functions or methods where the addresses of the two functions are taken in different 
        shared objects.

    ==========================================================================================

    #]]
    target_compile_options(
        ${arg_TARGET}
        PRIVATE
            $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-fvisibility=hidden>
            $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-fvisibility-inlines-hidden>
            $<$<CXX_COMPILER_ID:Clang>:-stdlib=libstdc++>
    )

    # ------------------------------------------- #
    #                 OPTIMIZATION                #
    # ------------------------------------------- #
    if((LABSIM_ENABLE_COMPILATION_OPTIMIZATION AND NOT arg_OVERRIDE) OR arg_COMPILATION_OPTIMIZATION)

        if(arg_OVERRIDE)
            set(status "${status}\n\t- COMPILATION_OPTIMIZATION = ON")
        endif()

        target_compile_options(
            ${arg_TARGET}
            PRIVATE
                $<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:Debug>>:-Og>
                $<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:Release>>:-O3>
                $<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:RelWithDebInfo>>:-O2>
                $<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:MinSizeRel>>:-Os>
        )
        target_compile_definitions(
            ${arg_TARGET}
            PRIVATE
                $<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>>:NDEBUG>
        )

    else()

        if(arg_OVERRIDE)
            set(status "${status}\n\t- COMPILATION_OPTIMIZATION = OFF")
        endif()

    endif()

    # -------------------------------- #
    #            ADVANCE WARN          #
    # -------------------------------- #
    if((LABSIM_ENABLE_COMPILATION_WARN AND NOT arg_OVERRIDE) OR arg_COMPILATION_WARN)

        if(arg_OVERRIDE)
            set(status "${status}\n\t- COMPILATION_WARN = ON")
        endif()

        #[[

        ==========================================================================================
        GNU GCC & LLVM Clang
        ------------------------------------------------------------------------------------------
        
        -Wall
            Activer tous les warnings
        -Wextra
            Activer tous les "extra" warnings
        -Wshadow
            Warn whenever a local variable or type declaration shadows another variable, parameter, type, class member (in C++), 
            or instance variable (in Objective-C) or whenever a built-in function is shadowed.
        -Wdouble-promotion
            Give a warning when a value of type float is implicitly promoted to double.
        -Wformat=2
            Enable -Wformat plus additional format checks.
        -Wformat-overflow=2
            Warn about calls to formatted input/output functions such as sprintf and vsprintf that might overflow 
            the destination buffer. Level 2 warns also about calls that might overflow the destination buffer given 
            an argument of sufficient length or magnitude. 
        -Wformat-truncation=2
            Warn about calls to formatted input/output functions such as snprintf and vsnprintf that might result 
            in output truncation. Level 2 warns also about calls to bounded functions whose return value is used and that 
            might result in truncation given an argument of sufficient length or magnitude. 
        -Wundef
            Warn if an undefined identifier is evaluated in an #if directive. Such identifiers are replaced with zero. 
        -Wconversion
            Warn for implicit conversions that may alter a value.
        -Warith-conversion
            Do warn about implicit conversions from arithmetic operations even when conversion of the operands to the same 
            type cannot change their values.
        -Wpadded
            Warn if padding is included in a structure, either to align an element of the structure or to align the whole 
            structure.
        -Wswitch-default
            Warn whenever a switch statement does not have a default case. 
        -Wswitch-enum
            Warn whenever a switch statement has an index of enumerated type and lacks a case for one or more of the named 
            codes of that enumeration.
        -Wredundant-decls
            Warn if anything is declared more than once in the same scope, even in cases where multiple declaration is valid 
            and changes nothing.
        -Wmissing-declarations
            Warn if a global function is defined without a previous declaration.
        -Wmissing-include-dirs
            Warn if a user-supplied include directory does not exist.
            
        ==========================================================================================

        #]]
        target_compile_options(
            ${arg_TARGET}
            PRIVATE
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wextra>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wshadow>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wdouble-promotion>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wformat=2>
                $<$<CXX_COMPILER_ID:GNU>:-Wformat-overflow=2>
                $<$<CXX_COMPILER_ID:GNU>:-Wformat-truncation=2>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wundef>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wconversion>
                $<$<CXX_COMPILER_ID:GNU>:-Warith-conversion>
                # $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wpadded>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wswitch-default>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wswitch-enum>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wredundant-decls>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wmissing-declarations>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wmissing-include-dirs>
        )

    else()
        
        #[[

        ==========================================================================================
        GNU GCC & LLVM Clang
        ------------------------------------------------------------------------------------------
        
        -Wno-deprecated-declarations
            eviter les boost::shared_ptr -> std::auto_ptr
        -Wno-unused-parameter
            supprimer les warnings sur les arguments non-utilisés
        -Wno-unused-variable
            suppression des warnings qui remontent de boost sur les variables unused
        -Wno-unused-local-typedefs
            supprimer les warnings sur les typedefs non-utilisés
        -Wno-sign-compare
            pour bug SimD comparaison dans methode take() (class dds::reader)
        -Wno-multichar
            supprimer les warning de constantes multicaractères
        -Wno-non-virtual-dtor
            supprimer les warnings concernant l'implémentation des destructeurs dans certaines 
            classes d'OpenSpliceDDS

        ==========================================================================================

        #]]
        target_compile_options(
            ${arg_TARGET}
            PRIVATE
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wno-deprecated-declarations>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wno-unused-parameter>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wno-unused-variable>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wno-unused-local-typedefs>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wno-sign-compare>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wno-multichar>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wno-non-virtual-dtor>
        )

        if(arg_OVERRIDE)
            set(status "${status}\n\t- COMPILATION_WARN = OFF")
        endif()

    endif()

    if(arg_OVERRIDE)
        message(STATUS "${status}")
    endif()

endfunction() # LabsimCMakeToolchain_Compiler()

####################################################################
#                             DEBUGGER                             #
####################################################################

option(LABSIM_ENABLE_DEBUG_OPTIMIZATION "Utiliser les options d'optimisations du debug du projet (valeur par defaut)" ON)

function(LabsimCMakeToolchain_Debugger arg_TARGET)

    set(status "[Labsim][configure] -- INFO : [${arg_TARGET}] Debug optimization overriding")

    # process optionnal args
    set(
        options_args
        OVERRIDE
        DEBUG_OPTIMIZATION
    )
    set(
        one_value_args
        ""
    )
    set(
        multi_value_args 
        ""
    )
    cmake_parse_arguments(PARSE_ARGV 1 arg "${options_args}" "${one_value_args}" "${multi_value_args}")

    # ------------------------------------------- #
    #                 OPTIMIZATION                #
    # ------------------------------------------- #
    if((LABSIM_ENABLE_DEBUG_OPTIMIZATION AND NOT arg_OVERRIDE) OR arg_DEBUG_OPTIMIZATION)
        
        if(arg_OVERRIDE)
            set(status "${status}\n\t- DEBUG_OPTIMIZATION = ON")
        endif()

        target_compile_options(
            ${arg_TARGET}
            PRIVATE
                $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>>:-ggdb3>
                $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>>:-glldb>
        )

    else()

        if(arg_OVERRIDE)
            set(status "${status}\n\t- DEBUG_OPTIMIZATION = OFF")
        endif()
        
    endif()

    if(arg_OVERRIDE)
        message(STATUS "${status}")
    endif()

endfunction() # LabsimCMakeToolchain_Debugger()

####################################################################
#                              LINKER                              #
####################################################################

option(LABSIM_ENABLE_LINK_OPTIMIZATION "Utiliser les options d'optimisations de linkage (visibility => size) du projet (valeur par defaut)" ON)
option(LABSIM_ENABLE_INTERPROCEDURAL_OPTIMIZATION "Utiliser les options d'optimisations LTO/IPO du projet (valeur par defaut)" ON)

function(LabsimCMakeToolchain_Linker arg_TARGET)

    set(status "[Labsim][configure] -- INFO : [${arg_TARGET}] Linker optimization overriding")

    # process optionnal args
    set(
        options_args
        OVERRIDE
        LINK_OPTIMIZATION
        INTERPROCEDURAL_OPTIMIZATION
    )
    set(
        one_value_args
        ""
    )
    set(
        multi_value_args 
        ""
    )
    cmake_parse_arguments(PARSE_ARGV 1 arg "${options_args}" "${one_value_args}" "${multi_value_args}")

    #[[
        
    ==========================================================================================
    LLVM Clang 
    ------------------------------------------------------------------------------------------

    -fuse-ld=lld 
        Use the lld linker instead of the default linker.
        
    ==========================================================================================
    GNU GCC 
    ------------------------------------------------------------------------------------------

    -fuse-ld=gold 
        Use the gold linker instead of the default linker.
        
    ==========================================================================================
    
    #]]    
    target_link_options(
        ${arg_TARGET}
        PRIVATE
            $<$<CXX_COMPILER_ID:Clang>:-fuse-ld=lld>
            $<$<CXX_COMPILER_ID:GNU>:-fuse-ld=gold>
    )

    # ------------------------------------------- #
    #                 OPTIMIZATION                #
    # ------------------------------------------- #
    if((LABSIM_ENABLE_LINK_OPTIMIZATION AND NOT arg_OVERRIDE) OR arg_LINK_OPTIMIZATION)

        if(arg_OVERRIDE)
            set(status "${status}\n\t- LINK_OPTIMIZATION = ON")
        endif()

        #[[
            
        ==========================================================================================
        GNU GCC & LLVM Clang
        ------------------------------------------------------------------------------------------

        -Wl,--no-whole-archive
            Include only needed archive contents
        -Wl,--exclude-libs,ALL
            Exclude ALL libraries from automatic export
        
        --------------------------------------------------------------------------------------
        CHECK
        Wait until GNU GCC Docker provide a least binutils 1.36...
        [ https://sourceware.org/bugzilla/show_bug.cgi?id=26200 ]
        --------------------------------------------------------------------------------------
        
        -Wl,--threads 
            Run the linker multi-threaded
        -Wl,--thread-count=${LABSIM_NUMBER_OF_BUILD_PROCESS}
            Number of threads to use
        
        ==========================================================================================
        
        #]]    
        target_link_options(
            ${arg_TARGET}
            PRIVATE
                # $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wl,--no-whole-archive>
                # $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wl,--exclude-libs,ALL>                
                $<$<CXX_COMPILER_ID:GNU>:-Wl,--threads>
                $<$<CXX_COMPILER_ID:GNU>:-Wl,--thread-count=${LABSIM_NUMBER_OF_BUILD_PROCESS}>
        )

    else()

        if(arg_OVERRIDE)
            set(status "${status}\n\t- LINK_OPTIMIZATION = OFF")
        endif()
        
    endif()

    # -------------------------------- #
    #              IPO/LTO             #
    # -------------------------------- #
    if((LABSIM_ENABLE_INTERPROCEDURAL_OPTIMIZATION AND NOT arg_OVERRIDE) OR arg_INTERPROCEDURAL_OPTIMIZATION)
        
        if(arg_OVERRIDE)
            set(status "${status}\n\t- INTERPROCEDURAL_OPTIMIZATION = ON")
        endif()

        # check
        include(CheckIPOSupported)
        check_ipo_supported(RESULT supported OUTPUT error)

        # evaluate 
        if(supported)

            # activate support
            set_target_properties(
                ${arg_TARGET} 
                PROPERTIES 
                    INTERPROCEDURAL_OPTIMIZATION TRUE
            )

        else()

            # print error
            message(STATUS "[Labsim][configure] -- WARNING : [${arg_TARGET}] requested IPO / LTO is not supported: <${error}>")

        endif()

    else()

        if(arg_OVERRIDE)
            set(status "${status}\n\t- INTERPROCEDURAL_OPTIMIZATION = OFF")
        endif()

    endif()

    if(arg_OVERRIDE)
        message(STATUS "${status}")
    endif()
        
endfunction() # LabsimCMakeToolchain_Linker()


####################################################################
#                             FEATURE                              #
####################################################################

# default project-wide values
option(LABSIM_ENABLE_CXX11_FEATURE "Utiliser les features C++11 sur le projet (valeur par defaut)" OFF)
option(LABSIM_ENABLE_CXX14_FEATURE "Utiliser les features C++14 sur le projet (valeur par defaut)" OFF)
option(LABSIM_ENABLE_CXX17_FEATURE "Utiliser les features C++17 sur le projet (valeur par defaut)" OFF)
option(LABSIM_ENABLE_CXX20_FEATURE "Utiliser les features C++20 sur le projet (valeur par defaut)" OFF)
option(LABSIM_ENABLE_CXX23_FEATURE "Utiliser les features C++23 sur le projet (valeur par defaut)" ON)
option(LABSIM_ENABLE_CXX26_FEATURE "Utiliser les features C++26 sur le projet (valeur par defaut)" OFF)
option(LABSIM_ENABLE_CXX_STRICT_STANDARD "Forcer l'utilisation stricte du standard C++ sans extensions (gnu,msvc,etc.) sur le projet (valeur par defaut)" ON)
option(LABSIM_ENABLE_C90_FEATURE "Utiliser les features C 90 sur le projet (valeur par defaut)" OFF)
option(LABSIM_ENABLE_C99_FEATURE "Utiliser les features C 99 sur le projet (valeur par defaut)" OFF)
option(LABSIM_ENABLE_C11_FEATURE "Utiliser les features C 11 sur le projet (valeur par defaut)" OFF)
option(LABSIM_ENABLE_C17_FEATURE "Utiliser les features C 17 sur le projet (valeur par defaut)" OFF)
option(LABSIM_ENABLE_C23_FEATURE "Utiliser les features C 23 sur le projet (valeur par defaut)" ON)
option(LABSIM_ENABLE_C_STRICT_STANDARD "Forcer l'utilisation stricte du standard C sans extensions (gnu,msvc,etc.) sur le projet (valeur par defaut)" ON)

function(LabsimCMakeToolchain_Feature arg_TARGET)    
        
    # process optionnal args
    set(
        options_args
        OVERRIDE
        CXX11_FEATURE
        CXX14_FEATURE
        CXX17_FEATURE
        CXX20_FEATURE
        CXX23_FEATURE
        CXX26_FEATURE
        CXX_STRICT_STANDARD
        C90_FEATURE
        C99_FEATURE
        C11_FEATURE
        C17_FEATURE
        C23_FEATURE
        C_STRICT_STANDARD
    )
    set(
        one_value_args
        ""
    )
    set(
        multi_value_args 
        ""
    )
    cmake_parse_arguments(PARSE_ARGV 1 arg "${options_args}" "${one_value_args}" "${multi_value_args}")

    set(status "[Labsim][configure] -- INFO : [${arg_TARGET}] Feature overriding")

    # -------------------------------- #
    #           CXX FEATURE            #
    # -------------------------------- #
    if((LABSIM_ENABLE_CXX26_FEATURE AND NOT arg_OVERRIDE) OR arg_CXX26_FEATURE)

        if(arg_OVERRIDE)
            set(status "${status}\n\t- CXX_STANDARD = 26")
        endif()

        set_target_properties(
            ${arg_TARGET}
            PROPERTIES
                CXX_STANDARD 26
        )

    elseif((LABSIM_ENABLE_CXX23_FEATURE AND NOT arg_OVERRIDE) OR arg_CXX23_FEATURE)

        if(arg_OVERRIDE)
            set(status "${status}\n\t- CXX_STANDARD 23")
        endif()

        set_target_properties(
            ${arg_TARGET}
            PROPERTIES
                CXX_STANDARD 23
        )

    elseif((LABSIM_ENABLE_CXX20_FEATURE AND NOT arg_OVERRIDE) OR arg_CXX20_FEATURE)

        if(arg_OVERRIDE)
            set(status "${status}\n\t- CXX_STANDARD 20")
        endif()

        set_target_properties(
            ${arg_TARGET}
            PROPERTIES
                CXX_STANDARD 20
        )

    elseif((LABSIM_ENABLE_CXX17_FEATURE AND NOT arg_OVERRIDE) OR arg_CXX17_FEATURE)
     
        if(arg_OVERRIDE)
            set(status "${status}\n\t- CXX_STANDARD 17")
        endif()
   
        set_target_properties(
            ${arg_TARGET}
            PROPERTIES
                CXX_STANDARD 17
        )
        
    elseif((LABSIM_ENABLE_CXX14_FEATURE AND NOT arg_OVERRIDE) OR arg_CXX14_FEATURE)
        
        if(arg_OVERRIDE)
            set(status "${status}\n\t- CXX_STANDARD 14")
        endif()

        set_target_properties(
            ${arg_TARGET}
            PROPERTIES
                CXX_STANDARD 14
        )

    elseif((LABSIM_ENABLE_CXX11_FEATURE AND NOT arg_OVERRIDE) OR arg_CXX11_FEATURE)
        
        if(arg_OVERRIDE)
            set(status "${status}\n\t- CXX_STANDARD 11")
        endif()

        set_target_properties(
            ${arg_TARGET}
            PROPERTIES
                CXX_STANDARD 11
        )

    else()

        message(FATAL_ERROR "[Labsim][configure] -- ERROR : [${arg_TARGET}] Feature overriding required but no CXX_FEATURE specified...")
    
    endif()

    # -------------------------------- #
    #       CXX STRICT STANDARD        #
    # -------------------------------- #
    if((LABSIM_ENABLE_CXX_STRICT_STANDARD AND NOT arg_OVERRIDE) OR arg_CXX_STRICT_STANDARD)

        if(arg_OVERRIDE)
            set(status "${status}\n\t- CXX_STRICT_STANDARD ON")
        endif()

        set_target_properties(
            ${arg_TARGET}
            PROPERTIES
                CXX_EXTENSIONS OFF
                CXX_STANDARD_REQUIRED ON
        )

    else()

        if(arg_OVERRIDE)
            set(status "${status}\n\t- CXX_STRICT_STANDARD OFF")
        endif()

        set_target_properties(
            ${arg_TARGET}
            PROPERTIES
                CXX_EXTENSIONS ON
                CXX_STANDARD_REQUIRED OFF
        )
        
    endif()

    # -------------------------------- #
    #            C FEATURE             #
    # -------------------------------- #

    if((LABSIM_ENABLE_C23_FEATURE AND NOT arg_OVERRIDE) OR arg_C23_FEATURE)

        if(arg_OVERRIDE)
            set(status "${status}\n\t- C_STANDARD 23")
        endif()

        set_target_properties(
            ${arg_TARGET}
            PROPERTIES
                C_STANDARD 23
        )

    elseif((LABSIM_ENABLE_FEATURE AND NOT arg_OVERRIDE) OR arg_C17_FEATURE)

        if(arg_OVERRIDE)
            set(status "${status}\n\t- C_STANDARD 17")
        endif()

        set_target_properties(
            ${arg_TARGET}
            PROPERTIES
                C_STANDARD 17
        )

    elseif((LABSIM_ENABLE_C11_FEATURE AND NOT arg_OVERRIDE) OR arg_C11_FEATURE)

        if(arg_OVERRIDE)
            set(status "${status}\n\t- C_STANDARD 11")
        endif()

        set_target_properties(
            ${arg_TARGET}
            PROPERTIES
                C_STANDARD 11
        )

    elseif((LABSIM_ENABLE_C99_FEATURE AND NOT arg_OVERRIDE) OR arg_C99_FEATURE)

        if(arg_OVERRIDE)
            set(status "${status}\n\t- C_STANDARD 99")
        endif()

        set_target_properties(
            ${arg_TARGET}
            PROPERTIES
                C_STANDARD 99
        )

    elseif((LABSIM_ENABLE_C90_FEATURE AND NOT arg_OVERRIDE) OR arg_C90_FEATURE)

        if(arg_OVERRIDE)
            set(status "${status}\n\t- C_STANDARD 90")
        endif()

        set_target_properties(
            ${arg_TARGET}
            PROPERTIES
                C_STANDARD 90
        )

    else()

        message(FATAL_ERROR "[Labsim][configure] -- ERROR : [${arg_TARGET}] Feature overriding required but no C_FEATURE specified...")
    
    endif()
    
    # -------------------------------- #
    #        C STRICT STANDARD         #
    # -------------------------------- #
    if((LABSIM_ENABLE_C_STRICT_STANDARD AND NOT arg_OVERRIDE) OR arg_C_STRICT_STANDARD)
        
        if(arg_OVERRIDE)
            set(status "${status}\n\t- C_STRICT_STANDARD ON")
        endif()

        set_target_properties(
            ${arg_TARGET}
            PROPERTIES
                C_EXTENSIONS OFF
                C_STANDARD_REQUIRED ON
        )

    else()
    
        if(arg_OVERRIDE)
            set(status "${status}\n\t- C_STRICT_STANDARD OFF")
        endif()

        set_target_properties(
            ${arg_TARGET}
            PROPERTIES
                C_EXTENSIONS ON
                C_STANDARD_REQUIRED OFF
        )
        
    endif()

    # -------------------------------- #
    #       STRICT STANDARD TWEAK      #
    # -------------------------------- #
    if(((LABSIM_ENABLE_CXX_STRICT_STANDARD OR LABSIM_ENABLE_C_STRICT_STANDARD) AND NOT arg_OVERRIDE) OR arg_CXX_STRICT_STANDARD OR arg_C_STRICT_STANDARD)

        #[[

        if any, add compile options once:	
            -pedantic
                Issue all the warnings demanded by strict ISO C and ISO C++; 
                reject all programs that use forbidden extensions, and some 
                other programs that do not follow ISO C and ISO C++
            -pedantic-errors
                Give an error whenever the base standard (see -Wpedantic) requires 
                a diagnostic, in some cases where there is undefined behavior at 
                compile-time and in some other cases that do not prevent compilation 
                of programs that are valid according to the standard.

        #]]
        target_compile_options(
            ${arg_TARGET}
            PRIVATE
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-pedantic>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-pedantic-errors>
        )

    endif()

    if(arg_OVERRIDE)
        message(STATUS "${status}")
    endif()

endfunction() # LabsimCMakeToolchain_Feature()
