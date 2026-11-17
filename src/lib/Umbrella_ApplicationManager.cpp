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
#include <Umbrella/Umbrella_NetworkManager.hpp>

#include <Ouranos/Ouranos_Common.hpp>
#include <Ouranos/log/Ouranos_Logger_log.hpp>

#include <boost/exception/diagnostic_information.hpp>

#include <csignal>
#include <exception>
#include <span>
#include <string_view>
#include <thread>

using namespace std::literals::string_literals;
using namespace std::literals::string_view_literals;

// avoid namespace pollution
namespace Labsim::project::umbrella
{

    ApplicationManager::ApplicationManager(
        base_type::_token_type const & _token
    ) noexcept
        : base_type(_token)
        // init the session timestamp
        , m_sessionTimestamp(titans::ouranos::Common::GenerateTimestamp())
    {
        // debug
        Labsim::LogDebug("[Umbrella]"sv)
            << "ApplicationManager::ApplicationManager() : token constructor call"sv;
    
    }; /* ApplicationManager() */

    auto
    ApplicationManager::Initialize(
        int const & _argc,
        char **     pArgv
    )
        -> status_code_type
    {
        // debug
        Labsim::LogDebug("[Umbrella]"sv)
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

            // tail SIGINT
            (void)std::signal(
                SIGINT,
                []([[maybe_unused]] int signum)
                {
                    ApplicationManager::get()->MarkGeneralClosureAsRequested();
                }
            );

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

            // configure the network manager
            if(!NetworkManager::get()->Configure())
            {
                // debug
                Labsim::LogDebug("[Umbrella]"sv)
                    << "ApplicationManager::Initialize() : failed to configure the network manager"sv;

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

            // connect
            if(!NetworkManager::get()->Connect())
            {
                // debug
                Labsim::LogDebug("[Umbrella]"sv)
                    << "ApplicationManager::Initialize() : failed to connect the network manager"sv;

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
    ApplicationManager::Run()
        -> status_code_type
    {
        // debug
        Labsim::LogDebug("[Umbrella]"sv)
            << "ApplicationManager::Run() : call"sv;

        // set default to error
        auto status{status_code::UMBRELLA_LIB_ERROR_};

        // settings
        static auto const setting_loop_period = this->getOptions().getTargetLoopPeriod();
        static auto const setting_stim_mode   = this->getOptions().getStimMode();

        // ticking mechanism
        static auto const ticking_platform_start   = std::chrono::high_resolution_clock::now();
        static auto       ticking_platform_elapsed = std::chrono::duration< double, std::ratio< 1 > >{std::chrono::high_resolution_clock::now() - ticking_platform_start};
        static auto       ticking_loop_start       = std::chrono::high_resolution_clock::now();
        static auto       ticking_loop_end         = std::chrono::high_resolution_clock::now();
        static auto       ticking_loop_elapsed     = std::chrono::duration< double, std::milli >{ticking_loop_end - ticking_loop_start};

        // encapsulate
        try
        {
            // prepare processor
            switch(setting_stim_mode)
            {
                case CommandLineOptions::S0_:
                {
                    // log
                    Labsim::LogInfo("[Umbrella]"sv)
                        << "ApplicationManager::Run() : will force offline Stim0 processing"sv;

                    // S0 processing
                    MotionPlatformManager::get()->EnableStim0Processor();

                    break;
                } /* case CommandLineOptions::S0_ */

                case CommandLineOptions::S1_:
                {
                    // log
                    Labsim::LogInfo("[Umbrella]"sv)
                        << "ApplicationManager::Run() : will force offline Stim1 processing"sv;

                    // S1 processing
                    MotionPlatformManager::get()->EnableStim1Processor();

                    break;
                } /* case CommandLineOptions::S1_ */

                case CommandLineOptions::S2_:
                {
                    // log
                    Labsim::LogInfo("[Umbrella]"sv)
                        << "ApplicationManager::Run() : will force offline Stim2 processing"sv;

                    // S2 processing
                    MotionPlatformManager::get()->EnableStim2Processor();

                    break;
                } /* case CommandLineOptions::S2_ */

                case CommandLineOptions::ONLINE_:
                default:
                {
                    // debug
                    Labsim::LogInfo("[Umbrella]"sv)
                        << "ApplicationManager::Run() : will listen to online command before processing, going idle"sv;

                    // online processing
                    MotionPlatformManager::get()->DisableAnyProcessor();

                    break;

                } /* case CommandLineOptions::ONLINE_ | default */

            } /* switch() */

            // infinite stim loop until ctr-c signal
            while(!this->HasGeneralClosureBeenRequested())
            {
                // update ticking
                ticking_loop_start       = std::chrono::high_resolution_clock::now();
                ticking_platform_elapsed = (ticking_loop_start - ticking_platform_start);

                // process network connectivity
                if(!NetworkManager::get()->Process())
                {
                    // debug
                    Labsim::LogDebug("[Umbrella]"sv)
                        << "ApplicationManager::Run() : network failed to process..."sv;

                    // switch status
                    status = status_code::UMBRELLA_LIB_ERROR_UNKNOWN_;

                    // exit
                    return status;

                } /* if() */

                // has any pending msg
                if(NetworkManager::get()->HasPendingData())
                {
                    // log
                    Labsim::LogInfo("[Umbrella]"sv)
                        << "ApplicationManager::Run() : network has pending data, will proceed."sv;

                    // process only the last(freshiest) one, discard the rest...
                    switch(NetworkManager::get()->FlushPendingData().top())
                    {
                        case UmbrellaNetworkMessageType::BEGIN_STIM_0_:
                        {
                            // log
                            Labsim::LogInfo("[Umbrella]"sv)
                                << "ApplicationManager::Run() : requesting Stim0 processing"sv;

                            // S0 processing
                            MotionPlatformManager::get()->EnableStim0Processor();
                            break;
                        }

                        case UmbrellaNetworkMessageType::BEGIN_STIM_1_:
                        {
                            // log
                            Labsim::LogInfo("[Umbrella]"sv)
                                << "ApplicationManager::Run() : requesting Stim1 processing"sv;

                            // S1 processing
                            MotionPlatformManager::get()->EnableStim1Processor();
                            break;
                        }

                        case UmbrellaNetworkMessageType::BEGIN_STIM_2_:
                        {
                            // log
                            Labsim::LogInfo("[Umbrella]"sv)
                                << "ApplicationManager::Run() : requesting Stim2 processing"sv;

                            // S2 processing
                            MotionPlatformManager::get()->EnableStim2Processor();
                            break;
                        }

                        case UmbrellaNetworkMessageType::STOP_ALL_STIM_:
                        {
                            // log
                            Labsim::LogInfo("[Umbrella]"sv)
                                << "ApplicationManager::Run() : requesting to stop, going idle"sv;

                            // idle processing
                            MotionPlatformManager::get()->DisableAnyProcessor();
                            break;
                        }

                        case UmbrellaNetworkMessageType::STOP_APP_:
                        {
                            // log
                            Labsim::LogInfo("[Umbrella]"sv)
                                << "ApplicationManager::Run() : requesting to stop the app !"sv;

                            // idle processing & mark as end
                            MotionPlatformManager::get()->DisableAnyProcessor();
                            this->MarkGeneralClosureAsRequested();
                            break;

                            break;
                        }

                        case UmbrellaNetworkMessageType::UNDEFINED_:
                        default:
                        {
                            // Error
                            Labsim::LogError("[Umbrella]"sv)
                                << "ApplicationManager::Run() : UNDEFINED message... stop the app !"sv;

                            // idle processing & mark as end
                            MotionPlatformManager::get()->DisableAnyProcessor();
                            this->MarkGeneralClosureAsRequested();
                            break;
                        }

                    } /* switch()  */

                } /* if() */

                // process motion
                if(!MotionPlatformManager::get()->Process(ticking_platform_elapsed.count()))
                {
                    // debug
                    Labsim::LogDebug("[Umbrella]"sv)
                        << "ApplicationManager::Run() : motion platform failed to process..."sv;

                    // switch status
                    status = status_code::UMBRELLA_LIB_ERROR_UNKNOWN_;

                    // exit
                    return status;

                } /* if() */

                // update end timings
                ticking_loop_end     = std::chrono::high_resolution_clock::now();
                ticking_loop_elapsed = (ticking_loop_end - ticking_loop_start);

                // check objectives
                if(ticking_loop_elapsed > setting_loop_period)
                {
                    // simply log overflow
                    LogWarn("[Umbrella]"sv)
                        << " => main loop has overshoot target period : current["sv
                        << ticking_loop_elapsed
                        << "] > target ["sv
                        << setting_loop_period
                        << "]"sv;
                }
                else
                {
                    // wait the remaining
                    std::this_thread::sleep_for(setting_loop_period - ticking_loop_elapsed);

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
        Labsim::LogDebug("[Umbrella]"sv)
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
