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

#include <Umbrella/Umbrella.hpp>

auto
main(
    int    argc,
    char * pArgv[]
)
    -> int
{
    // auto safe_argc
    //     = std::size_t(argc);

    // auto safe_argv
    //     = std::vector< std::string >{
    //         pArgv,
    //         pArgv + argc
    //     };

    return Labsim::project::umbrella::processMain(argc, pArgv);
}
