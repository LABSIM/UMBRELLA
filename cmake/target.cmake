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
#                           DOCUMENTATION                          #
####################################################################

# # package
# find_package(Doxygen)

# # Doxygen option
# if(DOXYGEN_FOUND)

# 	option(SCHEME_GENERATE_DOCUMENTATION "Generer la documentation API pour le projet via Doxygen" OFF)

# endif()

# if(SCHEME_GENERATE_DOCUMENTATION)

# 	# configure Doxyfile

# 	configure_file(
# 		${SCHEME_GATEWAY_DOC_ROOT}/scheme-gateway.doxy.in
# 		${CMAKE_CURRENT_BINARY_DIR}/scheme-gateway.doxy
# 		@ONLY
# 	)

# 	# variables

# 	set(DOXYGEN_INPUT ${CMAKE_CURRENT_BINARY_DIR}/scheme-gateway.doxy)
# 	set(DOXYGEN_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/html/index.html)

# 	# add a custom command

# 	add_custom_command(
# 		OUTPUT ${DOXYGEN_OUTPUT}
# 		COMMAND ${CMAKE_COMMAND} -E copy_directory ${SCHEME_GATEWAY_DOC_ROOT}/static ${CMAKE_CURRENT_BINARY_DIR}/static
# 		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/share/doc
# 		COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
# 		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
# 		DEPENDS ${DOXYGEN_INPUT}
# 	)

# 	add_custom_target(
# 		labsim-documentation
# 		DEPENDS ${DOXYGEN_OUTPUT}
# 		COMMENT "[Labsim][build] -- INFO : LABSIM documentation routine"
# 		VERBATIM
# 	)

# else()

# 	add_custom_target(
# 		labsim-documentation
# 		COMMENT "[Labsim][build] -- INFO : No LABSIM documentation required, continue..."
# 	)

# endif()

####################################################################
#                               CLEAN                              #
####################################################################

add_custom_target(
	labsim-clean
	COMMAND 
		${CMAKE_COMMAND} --build . --target clean
	WORKING_DIRECTORY 
		${CMAKE_BINARY_DIR}
	COMMENT 
		"[Labsim][build] -- INFO : LABSIM clean routine"
	VERBATIM
	USES_TERMINAL
)

####################################################################
#                            DEVELOPMENT                           #
####################################################################

add_custom_target(
	labsim-dev
	COMMAND ${CMAKE_COMMAND} --build . --target labsim-clean
	COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=Debug $<$<BOOL:CLANG_FORMAT_PATH>:-DLABSIM_ENABLE_CLANG_FORMATTER:BOOL=ON> $<$<BOOL:CLANG_TIDY_PATH>:-DLABSIM_ENABLE_CLANG_LINTER:BOOL=ON> -DLABSIM_ENABLE_LINKWHATYOUUSE:BOOL=ON ..
	COMMAND ${CMAKE_COMMAND} --build . --config Debug --target all -- -j${LABSIM_NUMBER_OF_PARALLEL_BUILD_PROCESS}
	WORKING_DIRECTORY 
		${CMAKE_BINARY_DIR}
	COMMENT 
		"[Labsim][build] -- INFO : LABSIM development routine : clang-format / clang-linter / LinkWhatYouUse built-in"
	VERBATIM
	USES_TERMINAL
)

####################################################################
#                         RELEASE [FAST] TARGET                    #
####################################################################

add_custom_target(
	labsim-release
	COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=Release $<$<BOOL:CLANG_FORMAT_PATH>:-DLABSIM_ENABLE_CLANG_FORMATTER:BOOL=OFF> $<$<BOOL:CLANG_TIDY_PATH>:-DLABSIM_ENABLE_CLANG_LINTER:BOOL=OFF> -DLABSIM_ENABLE_LINKWHATYOUUSE:BOOL=OFF ..
	COMMAND ${CMAKE_COMMAND} --build . --config Release --target all -- -j${LABSIM_NUMBER_OF_PARALLEL_BUILD_PROCESS}
	# COMMAND ${CMAKE_COMMAND} --build . --config Release --target all -- -j15
	WORKING_DIRECTORY 
		${CMAKE_BINARY_DIR}
	COMMENT 
		"[Labsim][build] -- INFO : LABSIM release routine"
	VERBATIM
	USES_TERMINAL
)

####################################################################
#                       RELEASE [CLEAN] TARGET                     #
####################################################################

add_custom_target(
	labsim-clean-release
	COMMAND ${CMAKE_COMMAND} --build . --target labsim-clean
	COMMAND ${CMAKE_COMMAND} --build . --target labsim-release
	WORKING_DIRECTORY 
		${CMAKE_BINARY_DIR}
	COMMENT 
		"[Labsim][build] -- INFO : LABSIM clean-release routine"
	VERBATIM
	USES_TERMINAL
)

####################################################################
#                          DEBUG [FAST] TARGET                     #
####################################################################

add_custom_target(
	labsim-debug
	COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug $<$<BOOL:CLANG_FORMAT_PATH>:-DLABSIM_ENABLE_CLANG_FORMATTER=OFF> $<$<BOOL:CLANG_TIDY_PATH>:-DLABSIM_ENABLE_CLANG_LINTER=OFF> -DLABSIM_ENABLE_LINKWHATYOUUSE=OFF ..  
	COMMAND ${CMAKE_COMMAND} --build . --config Debug --target all -- -j${LABSIM_NUMBER_OF_PARALLEL_BUILD_PROCESS}
	# COMMAND ${CMAKE_COMMAND} --build . --config Debug --target all -- -j15
	WORKING_DIRECTORY
		${CMAKE_BINARY_DIR}
	COMMENT 
		"[Labsim][build] -- INFO : LABSIM debug routine"
	VERBATIM
	USES_TERMINAL
)

####################################################################
#                        DEBUG [CLEAN] TARGET                      #
####################################################################

add_custom_target(
	labsim-clean-debug
	COMMAND ${CMAKE_COMMAND} --build . --target labsim-clean
	COMMAND ${CMAKE_COMMAND} --build . --target labsim-debug
	WORKING_DIRECTORY 
		${CMAKE_BINARY_DIR}
	COMMENT 
		"[Labsim][build] -- INFO : LABSIM clean-debug routine"
	VERBATIM
	USES_TERMINAL
)

####################################################################
#                         DEPLOY [FAST] TARGET                     #
####################################################################

add_custom_target(
	labsim-deploy
	COMMAND ${CMAKE_COMMAND} --build . --target labsim-release
	COMMAND ${CMAKE_COMMAND} --install . --config Release
	COMMAND ${CMAKE_COMMAND} --build . --target labsim-debug
	COMMAND ${CMAKE_COMMAND} --install . --config Debug
	WORKING_DIRECTORY 
		${CMAKE_BINARY_DIR}
	COMMENT 
		"[Labsim][build] -- INFO : LABSIM deploy routine"
	VERBATIM
	USES_TERMINAL
)

####################################################################
#                       DEPLOY [CLEAN] TARGET                      #
####################################################################

add_custom_target(
	labsim-clean-deploy
	COMMAND ${CMAKE_COMMAND} --build . --target labsim-clean-release
	COMMAND ${CMAKE_COMMAND} --install . --config Release
	COMMAND ${CMAKE_COMMAND} --build . --target labsim-clean-debug
	COMMAND ${CMAKE_COMMAND} --install . --config Debug
	COMMAND ${CMAKE_COMMAND} --build . --target labsim-clean
	WORKING_DIRECTORY 
		${CMAKE_BINARY_DIR}
	COMMENT 
		"[Labsim][build] -- INFO : LABSIM clean-deploy routine"
	VERBATIM
	USES_TERMINAL
)
