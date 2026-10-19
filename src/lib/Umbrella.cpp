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
#include <Umbrella/Umbrella_ApplicationManager.hpp>

#include <Ouranos/log/Ouranos_Logger_log.hpp>

#include <string_view>

using namespace std::literals::string_view_literals;

// avoid namespace pollution
namespace Labsim::project::umbrella
{

    auto
    Umbrella_Initialize(
        int     argc,
        char ** pArgv
    )
        -> status_code_type
    {
        // encapsute all
        try
        {
            // debug
            Labsim::LogDebug("[Umbrella]"sv)
                << "Umbrella_Initialize() entry point"sv;

            // set default to error
            status_code_type status{status_code::UMBRELLA_LIB_ERROR_};

            //////////////////
            //  Initialize  //
            //////////////////

            Labsim::LogDebug("[Umbrella]"sv)
                << "begin ApplicationManager::Initialize() procedure"sv;

            status = ApplicationManager::get()->Initialize(argc, pArgv);
            if(status != status_code::UMBRELLA_LIB_OK_)
            {
                if(status != status_code_type::UMBRELLA_LIB_EXIT_)
                {
                    // log
                    Labsim::LogError("[Umbrella]"sv)
                        << "error detected during ApplicationManager::Initialize() procedure..."sv;
                }
                else
                {
                    // log
                    Labsim::LogDebug("[Umbrella]"sv)
                        << "clean exit required in ApplicationManager::Initialize() procedure, exiting..."sv;

                } /* if() */

                // failed, exit with status
                return status;

            } /* if() */

            Labsim::LogDebug("[Umbrella]"sv)
                << "end ApplicationManager::Initialize() procedure"sv;

            // exit with status
            return status;
        }
        catch(...)
        {
            // fcuk
            return status_code::UMBRELLA_FATAL_ERROR_;

        } /* try */

    } /* Umbrella_Initialize() */

    auto
    Umbrella_Run()
        -> status_code_type
    {
        // encapsute all
        try
        {
            // debug
            Labsim::LogDebug("[Umbrella]"sv)
                << "Umbrella_Run() entry point"sv;

            // set default to error
            status_code_type status{status_code::UMBRELLA_LIB_ERROR_};

            //////////////////
            //      Run     //
            //////////////////

            Labsim::LogDebug("[Umbrella]"sv)
                << "begin ApplicationManager::Run() procedure"sv;

            status = ApplicationManager::get()->Run();
            if(status != status_code::UMBRELLA_LIB_OK_)
            {
                if(status != status_code_type::UMBRELLA_LIB_EXIT_)
                {
                    // log
                    Labsim::LogError("[Umbrella]"sv)
                        << "error detected during ApplicationManager::Close() procedure..."sv;
                }
                else
                {
                    // log
                    Labsim::LogDebug("[Umbrella]"sv)
                        << "clean exit required in ApplicationManager::Close() procedure, exiting..."sv;

                } /* if() */

                // exit with status
                return status;

            } /* if() */

            Labsim::LogDebug("[Umbrella]"sv)
                << "end ApplicationManager::Run() procedure"sv;

            // success !
            return status;
        }
        catch(...)
        {
            // fcuk
            return status_code::UMBRELLA_FATAL_ERROR_;

        } /* try */

    } /* Umbrella_Run() */

    auto
    Umbrella_Close()
        -> status_code_type
    {
        // encapsute all
        try
        {
            // debug
            Labsim::LogDebug("[Umbrella]"sv)
                << "Umbrella_Close() entry point"sv;

            // set default to error
            status_code_type status{status_code::UMBRELLA_LIB_ERROR_};

            //////////////////
            //     Close    //
            //////////////////

            Labsim::LogDebug("[Umbrella]"sv)
                << "begin ApplicationManager::Close() procedure"sv;

            status = ApplicationManager::Close();
            if(status != status_code::UMBRELLA_LIB_OK_)
            {
                if(status != status_code_type::UMBRELLA_LIB_EXIT_)
                {
                    // log
                    Labsim::LogError("[Umbrella]"sv)
                        << "error detected during ApplicationManager::Close() procedure..."sv;
                }
                else
                {
                    // log
                    Labsim::LogDebug("[Umbrella]"sv)
                        << "clean exit required in ApplicationManager::Close() procedure, exiting..."sv;

                } /* if() */

                // exit with status
                return status;

            } /* if() */

            Labsim::LogDebug("[Umbrella]"sv)
                << "end ApplicationManager::Close() procedure"sv;

            // success !
            return status;
        }
        catch(...)
        {
            // fcuk
            return status_code::UMBRELLA_FATAL_ERROR_;

        } /* try */

    } /* Umbrella_Close() */

} // namespace Labsim::project::umbrella
