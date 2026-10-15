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

#include <Umbrella/Umbrella_ApplicationManager.hpp>
#include <Umbrella/Umbrella_MotionPlatformManager.hpp>

#include <Ouranos/log/Ouranos_Logger_log.hpp>

#include <boost/exception/diagnostic_information.hpp>

#include <exception>
#include <random>
#include <ranges>
#include <span>
#include <string_view>

using namespace std::literals::string_literals;
using namespace std::literals::string_view_literals;

// avoid namespace pollution
namespace Labsim::project::umbrella
{

    ApplicationManager::ApplicationManager(
        base_type::_token_type const & _token
    ) noexcept
        : base_type(_token)
    {
        // debug
        Labsim::LogDebug("[Umbrella]"sv)
            << "ApplicationManager::ApplicationManager() : token constructor call"sv;
    };

    auto
    ApplicationManager::Initialize(
        int const & _argc,
        char *      pArgv[]
    )
        -> status_code_type
    {
        // debug
        Labsim::LogInfo("[Umbrella]"sv)
            << "ApplicationManager::Initialize() : call"sv;

        // set default to error
        auto status{status_code::UMBRELLA_LIB_ERROR_};

        // encapsulate
        try
        {
            // debug
            Labsim::LogDebug("[Umbrella]"sv)
                << "ApplicationManager::Initialize() : Extracting program options"sv;

            // stock & parse command line options
            this->m_argc = std::size_t(_argc);

            // post c++23
            // this->m_argv.assign_range(pArgv);

            // pre c++23
            this->m_argv.assign(
                pArgv,
                pArgv + this->m_argc
            );

            auto cli_status = BitmaskEnum{this->m_options.Parse()};
            if(cli_status != CommandLineOptions::CLI_OPTIONS_OK_)
            {
                // debug
                Labsim::LogDebug("[Umbrella]"sv)
                    << "ApplicationManager::Initialize() : command line parsing ended with exit required ["sv
                    << cli_status
                    << "]"sv;

                // switch status
                status = status_code::UMBRELLA_LIB_EXIT_;

                // exit
                return status;

            } /* if() */

            // configure APIs structs
            if(!MotionPlatformManager::get()->Configure())
            {
                // debug
                Labsim::LogDebug("[Umbrella]"sv)
                    << "ApplicationManager::Initialize() : failed to configure the motion platform APIs"sv;

                // switch status
                status = status_code::UMBRELLA_LIB_EXIT_;

                // exit
                return status;

            } /* if() */

            // connect to platform
            if(!MotionPlatformManager::get()->Connect())
            {
                // debug
                Labsim::LogDebug("[Umbrella]"sv)
                    << "ApplicationManager::Initialize() : failed to connect to the motion platform"sv;

                // switch status
                status = status_code::UMBRELLA_LIB_EXIT_;

                // exit
                return status;

            } /* if() */

            // debug
            Labsim::LogDebug("[Umbrella]"sv)
                << "ApplicationManager::Initialize() : OK !"sv;

            // switch status
            status = status_code::UMBRELLA_LIB_OK_;
        }
        catch(boost::exception & ex)
        {
            // debug
            Labsim::LogError("[Umbrella]"sv)
                << "ApplicationManager::Initialize() : caught boost::exception exception "sv
                << boost::diagnostic_information(ex);

            // switch status
            status = status_code::UMBRELLA_LIB_ERROR_EXCEPTION_RAISED_;
        }
        catch(std::exception & ex)
        {
            // debug
            Labsim::LogError("[Umbrella]"sv)
                << "ApplicationManager::Initialize() : caught std::exception exception "sv
                << ex.what();

            // switch status
            status = status_code::UMBRELLA_LIB_ERROR_EXCEPTION_RAISED_;
        }
        catch(...)
        {
            // debug
            Labsim::LogError("[Umbrella]"sv)
                << "ApplicationManager::Initialize() : caught unhandled exception "sv
                << boost::current_exception_diagnostic_information();

            // switch status
            status = status_code::UMBRELLA_LIB_ERROR_EXCEPTION_RAISED_;

        } /* try() */

        // status
        return status;

    } /* Initialize() */

    auto
    ApplicationManager::Run() const
        -> status_code_type
    {
        // debug
        Labsim::LogInfo("[Umbrella]"sv)
            << "ApplicationManager::Run() : call"sv;

        // set default to error
        auto status{status_code::UMBRELLA_LIB_ERROR_};

        // ticking mechanism
        static auto const ticking_platform_start   = std::chrono::high_resolution_clock::now();
        static auto       ticking_platform_elapsed = std::chrono::duration< double >{std::chrono::high_resolution_clock::now() - ticking_platform_start};
        static auto       ticking_loop_start       = std::chrono::high_resolution_clock::now();
        static auto       ticking_loop_end         = std::chrono::high_resolution_clock::now();
        static auto       ticking_loop_elapsed     = std::chrono::duration< double, std::milli >{ticking_loop_end - ticking_loop_start};

        // encapsulate
        try
        {
            // infinite stim loop
            while(true)
            {
                // update ticking
                ticking_loop_start       = std::chrono::high_resolution_clock::now();
                ticking_platform_elapsed = (ticking_loop_start - ticking_platform_start);

                // process motion
                if(!MotionPlatformManager::get()->Process(ticking_platform_elapsed.count()))
                {
                    // debug
                    Labsim::LogDebug("[Umbrella]"sv)
                        << "ApplicationManager::Run() : motion platform failed to process..."sv;

                    // switch status
                    status = status_code::UMBRELLA_LIB_EXIT_;

                    // exit
                    return status;

                } /* if() */

                // unpdate end timings
                ticking_loop_end     = std::chrono::high_resolution_clock::now();
                ticking_loop_elapsed = (ticking_loop_end - ticking_loop_start);

                // check objectives
                if(ticking_loop_elapsed > this->getOptions().getTargetLoopPeriod())
                {
                    // simply log overflow
                    LogWarn("[Umbrella]"sv)
                        << " => main loop has overshoot target period : current["sv
                        << ticking_loop_elapsed
                        << "] > target ["sv
                        << this->getOptions().getTargetLoopPeriod()
                        << "]"sv;

                } /* if() */

            } /* while() */

            // debug
            Labsim::LogDebug("[Umbrella]"sv)
                << "ApplicationManager::Run() : end"sv;

            // switch status
            status = status_code::UMBRELLA_LIB_OK_;
        }
        catch(boost::exception & ex)
        {
            // debug
            Labsim::LogError("[Umbrella]"sv)
                << "ApplicationManager::Run() : caught boost::exception exception "sv
                << boost::diagnostic_information(ex);

            // switch status
            status = status_code::UMBRELLA_LIB_ERROR_EXCEPTION_RAISED_;
        }
        catch(std::exception & ex)
        {
            // debug
            Labsim::LogError("[Umbrella]"sv)
                << "ApplicationManager::Run() : caught std::exception exception "sv
                << ex.what();

            // switch status
            status = status_code::UMBRELLA_LIB_ERROR_EXCEPTION_RAISED_;
        }
        catch(...)
        {
            // debug
            Labsim::LogError("[Umbrella]"sv)
                << "ApplicationManager::Run() : caught unhandled exception "sv
                << boost::current_exception_diagnostic_information();

            // switch status
            status = status_code::UMBRELLA_LIB_ERROR_EXCEPTION_RAISED_;

        } /* try() */

        // status
        return status;

    } /* Run() */

    auto
    ApplicationManager::Close()
        -> status_code_type
    {
        // debug
        Labsim::LogInfo("[Umbrella]"sv)
            << "ApplicationManager::Close() : call"sv;

        // set default to error
        auto status{status_code::UMBRELLA_LIB_ERROR_};

        // encapsulate
        try
        {
            // cleanup
            if(!MotionPlatformManager::get()->Cleanup())
            {
                // debug
                Labsim::LogDebug("[Umbrella]"sv)
                    << "ApplicationManager::Close() : motion platform failed to cleanup..."sv;

                // switch status
                status = status_code::UMBRELLA_LIB_EXIT_;

                // exit
                return status;

            } /* if() */

            // switch status
            status = status_code::UMBRELLA_LIB_OK_;
        }
        catch(boost::exception & ex)
        {
            // debug
            Labsim::LogError("[Umbrella]"sv)
                << "ApplicationManager::Close() : caught boost::exception exception "sv
                << boost::diagnostic_information(ex);

            // switch status
            status = status_code::UMBRELLA_LIB_ERROR_EXCEPTION_RAISED_;
        }
        catch(std::exception & ex)
        {
            // debug
            Labsim::LogError("[Umbrella]"sv)
                << "ApplicationManager::Close() : caught std::exception exception "sv
                << ex.what();

            // switch status
            status = status_code::UMBRELLA_LIB_ERROR_EXCEPTION_RAISED_;
        }
        catch(...)
        {
            // debug
            Labsim::LogError("[Umbrella]"sv)
                << "ApplicationManager::Close() : caught unhandled exception "sv
                << boost::current_exception_diagnostic_information();

            // switch status
            status = status_code::UMBRELLA_LIB_ERROR_EXCEPTION_RAISED_;

        } /* try() */

        // status
        return status;

    } /* Close() */

} // namespace Labsim::project::umbrella
