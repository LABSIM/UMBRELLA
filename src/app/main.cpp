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

#include <cstdlib>
#include <iostream>

using namespace Labsim::project::umbrella;

auto
main(
    int     argc,
    char ** pArgv
)
    -> int
{
    status_code lib_status{status_code::UMBRELLA_UNDEFINED_};

    //
    // easy peasy ==> init -> run -> close
    //
    lib_status = Umbrella_Initialize(argc, pArgv);
    if(lib_status != status_code::UMBRELLA_LIB_OK_)
    {
        if(lib_status != status_code::UMBRELLA_LIB_EXIT_)
        {
            std::cerr 
                << "[Umbrella-app] - lib failed with status { Umbrella_Initialize() = [" 
                << (int)lib_status 
                << "] }, exiting...\n"
                << std::flush;
            return EXIT_FAILURE;
        }
        // clean exit required
        return EXIT_SUCCESS;
    };
    lib_status = Umbrella_Run();
    if(lib_status != status_code::UMBRELLA_LIB_OK_)
    {
        if(lib_status != status_code::UMBRELLA_LIB_EXIT_)
        {
            std::cerr 
                << "[Umbrella-app] - lib failed with status { Umbrella_Run() = [" 
                << (int)lib_status 
                << "] }, exiting...\n"
                << std::flush;
            return EXIT_FAILURE;
        }
        // clean exit required
        return EXIT_SUCCESS;
    };
    lib_status = Umbrella_Close();
    if(lib_status != status_code::UMBRELLA_LIB_OK_)
    {
        if(lib_status != status_code::UMBRELLA_LIB_EXIT_)
        {
            std::cerr 
                << "[Umbrella-app] - lib failed with status { Umbrella_Close() = [" 
                << (int)lib_status 
                << "] }, exiting...\n"
                << std::flush;
            return EXIT_FAILURE;
        }
        // clean exit required
        return EXIT_SUCCESS;
    };

    // success !
    return EXIT_SUCCESS;

} /* main() */
