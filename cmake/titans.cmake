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

# Labsim Titans : Ouranos
# - normally, it should be installed under current ${LABSIM_INSTALL_PREFIX}/titans/ouranos
#
set(
	Ouranos_ROOT 
	"${LABSIM_INSTALL_PREFIX}/titans/ouranos" 
	CACHE 
		PATH "Le path d'installation de la librairie LABSIM : Ouranos"
)
mark_as_advanced(
	Ouranos_ROOT
)
list(
	APPEND 
	CMAKE_PREFIX_PATH 
	"${Ouranos_ROOT}"
)
find_package(
	Ouranos
	CONFIG
	REQUIRED COMPONENTS
		log
		datamodel
		unitmodel
		pattern
		proptree
)
