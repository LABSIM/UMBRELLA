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

#include <cstdint>
#include <string>
#include <vector>

// avoid namespace pollution
namespace Labsim::project::umbrella
{

    // entry point

    UMBRELLA_LIB_API
    static void
    main(
        std::size_t const &                _argc,
        std::vector< std::string > const & _argv
    );

} // namespace Labsim::project::umbrella

#endif /* INCLUDE_UMBRELLA_UMBRELLA_HPP_ */
