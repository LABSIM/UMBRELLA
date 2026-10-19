//
// UMBRELLA : Acronyme AH ?
// Copyright (C) 2026-present Nawfel KINANI
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
//

#ifndef INCLUDE_UMBRELLA_UMBRELLA_HPP_
#define INCLUDE_UMBRELLA_UMBRELLA_HPP_

#include <Umbrella/Umbrella_API.hpp>

// avoid namespace pollution
namespace Labsim::project::umbrella
{

    // simple entry points

    UMBRELLA_LIB_API auto
    Umbrella_Initialize(
        int     argc,
        char ** pArgv
    ) -> status_code_type;

    UMBRELLA_LIB_API auto
    Umbrella_Run()
        -> status_code_type;

    UMBRELLA_LIB_API auto
    Umbrella_Close()
        -> status_code_type;

} // namespace Labsim::project::umbrella

#endif /* INCLUDE_UMBRELLA_UMBRELLA_HPP_ */
