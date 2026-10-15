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
#include <Umbrella/Umbrella_Common.hpp>

#include <Ouranos/log/Ouranos_Logger_log.hpp>

#include <boost/exception/all.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <dlfcn.h>
#include <exception>
#include <numbers>
#include <string_view>
#include <thread>
#include <unistd.h>
#include <vector>

#include "ForceSeatDI.h"

using namespace std::literals::string_view_literals;
using namespace std::literals::chrono_literals;

namespace Labsim::project::umbrella
{

    bool
    prepareAPI(
        std::vector< FSDI_Handle > & apis,
        uint32_t                     index
    )
    {
        apis.resize(index + 1);
        if(apis[index] == nullptr)
        {
            apis[index] = ForceSeatDI_Create();
        }

        if(apis[index] == nullptr)
        {
            printf("ForceSeatDI .so was not loaded, dlopen result = %s\n", dlerror());
            return false;
        }

        return true;
    }

    bool
    checkPlatform(
        std::vector< FSDI_Handle > const & apis,
        uint32_t                           index
    )
    {
        FSDI_Char sn[FSDI_SerialNumberStringLength];
        if(ForceSeatDI_GetSerialNumber(apis[index], sn) != FSDI_True)
        {
            printf("Failed to get platform info from %d\n", (index + 1));
            return false;
        }

        printf("Platform %d S/N: %s\n", (index + 1), reinterpret_cast< char const * >(sn));

        FSDI_Bool isLicenseValid = FSDI_False;
        if(ForceSeatDI_GetLicenseStatus(apis[index], &isLicenseValid) != FSDI_True)
        {
            printf("Failed to get license status from %d\n", (index + 1));
            return false;
        }

        if(isLicenseValid != FSDI_True)
        {
            printf("License is not valid for %d\n", (index + 1));
            return false;
        }
        return true;
    }

    bool
    connectNET(
        std::vector< FSDI_Handle > & apis,
        uint32_t                     index,
        char const *                 ipAddress
    )
    {
        if(!prepareAPI(apis, index))
        {
            return false;
        }

        if(ForceSeatDI_ConnectToNetworkDevice(apis[index], ipAddress) != FSDI_True)
        {
            printf("Failed to connect to %s, error: %d\n", (ipAddress ? ipAddress : "NULL"), ForceSeatDI_GetRecentErrorCode(apis[index]));
            return false;
        }

        return checkPlatform(apis, index);
    }

    bool
    connectUSB(
        std::vector< FSDI_Handle > & apis,
        uint32_t                     index,
        wchar_t const *              sn
    )
    {
        if(!prepareAPI(apis, index))
        {
            return false;
        }

        if(ForceSeatDI_ConnectToUsbDevice(apis[index], nullptr, sn) != FSDI_True)
        {
            wprintf(L"Failed to connect to %ls, error: %d\n", (sn ? sn : L"NULL"), ForceSeatDI_GetRecentErrorCode(apis[index]));
            return false;
        }

        return checkPlatform(apis, index);
    }

    void
    send(
        std::vector< FSDI_Handle > const &    apis,
        uint32_t                              index,
        FSDI_TopTablePositionPhysical const & pos
    )
    {
        if(ForceSeatDI_SendTopTablePosPhy(apis[index], &pos) != FSDI_True)
        {
            printf("Failed to send request to platform %d\n", (index + 1));
        }
    }

    void
    doWork(
        BitmaskEnum< UmbrellaStimAxisType > stim_axis,
        double                              stim_frequency_hz,
        double                              stim_amplitude_mm,
        double                              stim_x_axis_phase_offset_s,
        double                              stim_y_axis_phase_offset_s,
        double                              stim_z_axis_phase_offset_s
    )
    {
        // ================================================================================ //
        //                                 DEFINING SECTION                                 //
        // ================================================================================ //

        // invariant
        static constexpr auto fsdi_platform_index = 0_UI32;
        static constexpr auto fsdi_platform_count = 1_UI32;
        static constexpr auto fsdi_platform_ip    = "10.1.1.75"sv;
        static constexpr auto fsdi_platform_sn    = L"250031-000457-315839-323120"sv;
        static constexpr auto mm_to_m_factor      = 1000.0;
        static constexpr auto two_constant        = 2.0;

        // various period
        static auto const idle_init_period   = 500ms;
        static auto const target_loop_period = 100ms;

        // ticking mechanism
        static auto const ticking_platform_start   = std::chrono::high_resolution_clock::now();
        static auto       ticking_platform_elapsed = std::chrono::duration< double >{std::chrono::high_resolution_clock::now() - ticking_platform_start};
        static auto       ticking_loop_start       = std::chrono::high_resolution_clock::now();
        static auto       ticking_loop_end         = std::chrono::high_resolution_clock::now();
        static auto       ticking_loop_elapsed     = std::chrono::duration< double, std::milli >{ticking_loop_end - ticking_loop_start};

        // the ForceSetDI motion platform API handle
        static auto fsdi_apis                   = std::vector< FSDI_Handle >{};
        static auto fsdi_requestedPosition      = FSDI_TopTablePositionPhysical{};
        static auto fsdi_actualPosition         = FSDI_ActualActuatorsPositionLogical{};
        static auto fsdi_actualTopTablePosition = FSDI_ActualTopTablePositionPhysical{};
        static auto fsdi_actualPlatformInfo     = FSDI_PlatformInfo{};

        // ================================================================================ //
        //                                  LAMBDA SECTION                                  //
        // ================================================================================ //

        static auto lambda_configure
            = [&]() -> void
        {
            // log
            LogInfo("[Umbrella]"sv)
                << "lambda_configure(): entry"sv;

            // upstream
            // allocate & configure FSDI resquested struct
            memset(&fsdi_requestedPosition, 0, sizeof(fsdi_requestedPosition));
            fsdi_requestedPosition.structSize          = sizeof(fsdi_requestedPosition);
            fsdi_requestedPosition.maxSpeed            = 65535;
            fsdi_requestedPosition.pause               = FSDI_False;
            fsdi_requestedPosition.strategy            = FSDI_Strategy_BestMatch;
            fsdi_requestedPosition.accelerationProfile = FSDI_AP_Rapid;

            // downstream
            // set sizes
            fsdi_actualPosition.structSize         = sizeof(FSDI_ActualActuatorsPositionLogical);
            fsdi_actualTopTablePosition.structSize = sizeof(FSDI_ActualTopTablePositionPhysical);
            fsdi_actualPlatformInfo.structSize     = sizeof(FSDI_PlatformInfo);

            // log
            LogInfo("[Umbrella]"sv)
                << "lambda_configure(): exit"sv;
        }; /* lambda_configure() */

        static auto lambda_connect
            = [&]() -> bool
        {
            // log
            LogInfo("[Umbrella]"sv)
                << "lambda_connect(): entry"sv;

            // try ethernet first
            if(!connectNET(fsdi_apis, fsdi_platform_index, fsdi_platform_ip.data()))
            {
                // failsafe over USB ?
                if(!connectUSB(fsdi_apis, fsdi_platform_index, fsdi_platform_sn.data()))
                {
                    // log
                    LogError("[Umbrella]"sv)
                        << "lambda_connect(): Failed to connect to platform"sv;

                    // fail...
                    return false;
                }
                else
                {
                    // log
                    LogInfo("[Umbrella]"sv)
                        << "lambda_connect(): successfuly connected to platform over USB."sv;

                } /* if() */
            }
            else
            {
                // log
                LogInfo("[Umbrella]"sv)
                    << "lambda_connect(): successfuly connected to platform over ethernet."sv;

            } /* if() */

            // init idle
            std::this_thread::sleep_for(idle_init_period);

            // should always be 1 !
            if(fsdi_apis.size() != fsdi_platform_count)
            {
                // log
                LogError("[Umbrella]"sv)
                    << "lambda_connect(): APIs does not match our..."sv;

                // fail...
                return false;

            } /* if() */

            // log
            LogInfo("[Umbrella]"sv)
                << "lambda_connect(): everything ok."sv;

            // if this point is reached, then its a success
            return true;

            // log
            LogInfo("[Umbrella]"sv)
                << "lambda_connect(): exit"sv;
        }; /* lambda_connect() */

        static auto lambda_resetUpstream
            = [&]() -> void
        {
            // log
            LogInfo("[Umbrella]"sv)
                << "lambda_resetUpstream(): entry"sv;

            // reset requested
            fsdi_requestedPosition.yaw
                = fsdi_requestedPosition.pitch
                = fsdi_requestedPosition.roll
                = fsdi_requestedPosition.heave
                = fsdi_requestedPosition.sway
                = fsdi_requestedPosition.surge
                = 0.0f;

            // log
            LogInfo("[Umbrella]"sv)
                << "lambda_resetUpstream(): exit"sv;
        }; /* lambda_resetUpstream() */

        static auto lambda_buildUpstream
            = [&]() -> void
        {
            // log
            LogInfo("[Umbrella]"sv)
                << "lambda_buildUpstream(): entry"sv;

            // encapsulate all calculus
            try
            {
                // sway calculus
                if(stim_axis.AnyOf(UmbrellaStimAxisType::SWAY_))
                {
                    //
                    // x(t) = Ax * sin((2*pi*t) / T)
                    //
                    fsdi_requestedPosition.sway
                        = boost::numeric_cast< FSDI_FLOAT >(
                            (stim_amplitude_mm / mm_to_m_factor)
                            * std::sin(
                                (two_constant * std::numbers::pi * ticking_platform_elapsed.count())
                                / stim_frequency_hz
                            )
                        );

                } /* if()*/

                // heave calculus
                if(stim_axis.AnyOf(UmbrellaStimAxisType::HEAVE_))
                {
                    //
                    // y(t) = Ay * sin( ((2*pi*t) / T) + ( (pi/2) * sin((2*pi*t) / T(y)) ) )
                    //
                    fsdi_requestedPosition.heave
                        = boost::numeric_cast< FSDI_FLOAT >(
                            (stim_amplitude_mm / mm_to_m_factor)
                            * std::sin(
                                (
                                    (two_constant * std::numbers::pi * ticking_platform_elapsed.count())
                                    / stim_frequency_hz
                                )
                                + ((std::numbers::pi / two_constant)
                                   * ((two_constant * std::numbers::pi * ticking_platform_elapsed.count())
                                      / stim_y_axis_phase_offset_s
                                   )
                                )
                            )
                        );

                } /* if()*/

                // surge calculus
                if(stim_axis.AnyOf(UmbrellaStimAxisType::SURGE_))
                {
                    //
                    // z(t) = Az * sin( ((2*pi*t) / T) + ( (pi/2) * sin((2*pi*t) / T(z)) ) )
                    //
                    fsdi_requestedPosition.surge
                        = boost::numeric_cast< FSDI_FLOAT >(
                            (stim_amplitude_mm / mm_to_m_factor)
                            * std::sin(
                                (
                                    (two_constant * std::numbers::pi * ticking_platform_elapsed.count())
                                    / stim_frequency_hz
                                )
                                + ((std::numbers::pi / two_constant)
                                   * ((two_constant * std::numbers::pi * ticking_platform_elapsed.count())
                                      / stim_z_axis_phase_offset_s
                                   )
                                )
                            )
                        );

                } /* if()*/

                // rotational calculus
                // if(stim_axis.AnyOf(UmbrellaStimAxisType::YAW_ | UmbrellaStimAxisType::PITCH_ | UmbrellaStimAxisType::ROLL_))
                // {
                //     // nothing for the moment

                // } /* if()*/
            }
            catch(boost::numeric::negative_overflow const & _exception)
            {
                // log
                LogWarn("[Umbrella]"sv)
                    << " => caught boost::numeric::negative_overflow : "sv
                    << _exception.what();
            }
            catch(boost::numeric::positive_overflow const & _exception)
            {
                // log
                LogWarn("[Umbrella]"sv)
                    << " => caught boost::numeric::positive_overflow : "sv
                    << _exception.what();
            }
            catch(boost::numeric::bad_numeric_cast const & _exception)
            {
                // log
                LogWarn("[Umbrella]"sv)
                    << " => caught boost::numeric::bad_numeric_cast : "sv
                    << _exception.what();
            }
            catch(boost::exception const & _exception)
            {
                // log
                LogWarn("[Umbrella]"sv)
                    << " => caught boost::exception : "sv
                    << boost::diagnostic_information(_exception);
            }
            catch(std::exception const & _exception)
            {
                // log
                LogWarn("[Umbrella]"sv)
                    << " => caught caught std::exception : "sv
                    << _exception.what();
            }
            catch(...)
            {
                // log
                LogWarn("[Umbrella]"sv)
                    << " => caught caught unhandled exception : "sv
                    << boost::current_exception_diagnostic_information();

            } /* try() */

            // log
            LogInfo("[Umbrella]"sv)
                << "lambda_buildUpstream(): exit"sv;
        }; /* lambda_buildUpstream */

        static auto lambda_sendUpstream
            = [&]() -> void
        {
            // send requested datas !
            send(
                fsdi_apis,
                fsdi_platform_index,
                fsdi_requestedPosition
            );
        }; /* lambda_sendRequest */

        static auto lambda_readDownstream
            = [&]() -> void
        {
            // Read current actuators position

            if(ForceSeatDI_GetActuatorsPosLog(
                   fsdi_apis.at(fsdi_platform_index),
                   &fsdi_actualPosition
               )
               != FSDI_True)
            {
                // failed...
                // printf("Failed to get actuator pos");

            } /* if() */

            if(ForceSeatDI_GetTopTablePosPhy(
                   fsdi_apis.at(fsdi_platform_index),
                   &fsdi_actualTopTablePosition
               )
               != FSDI_True)
            {
                // failed...
                // printf("Failed to get top table pos");

            } /* if() */

            if(ForceSeatDI_GetPlatformInfo(
                   fsdi_apis.at(fsdi_platform_index),
                   &fsdi_actualPlatformInfo
               )
               != FSDI_True)
            {
                // failed...
                // printf("Failed to get platform info");
                // printf("ErrorCode: %d", ForceSeatDI_GetRecentErrorCode(fsdi_apis.at(fsdi_platform_index)));

            } /* if() */
        }; /* lambda_readActuator */

        static auto lambda_processDownstream
            = [&]() -> void
        {
            // // NOTE: Displaying data is slow
            // for(uint32_t i = 0; i < NumberOfPlatforms; ++i)
            // {
            //     printf("Platform: %u, Pos: %u, %u, %u, %u, %u, %u\n", (i + 1), actualPositions[i].actualMotorPosition[0], actualPositions[i].actualMotorPosition[1], actualPositions[i].actualMotorPosition[2], actualPositions[i].actualMotorPosition[3], actualPositions[i].actualMotorPosition[4], actualPositions[i].actualMotorPosition[5]);
            //     printf("    Error code: %d.%d\n", static_cast< int >(platformInfos[i].moduleErrorCode), static_cast< int >(platformInfos[i].moduleErrorIndex));
            //     printf("    Top Frame: roll %f, pitch %f, yaw %f, heave %f, surge %f, sway %f\n", topTablePos[i].roll, topTablePos[i].pitch, topTablePos[i].yaw, topTablePos[i].heave, topTablePos[i].surge, topTablePos[i].sway);
            //     static_assert(FSDI_MotorsCount == 6, "");
            // }
        }; /* lambda_processDownstream */

        static auto lambda_cleanup
            = [&]() -> void
        {
            // cleanup
            for(FSDI_Handle api: fsdi_apis)
            {
                ForceSeatDI_Park(api, FSDI_ParkMode_Normal);
                ForceSeatDI_Delete(api);

            } /* for() */
        }; /* lambda_cleanup() */

        // ================================================================================ //
        //                                 PROCESSING LOOP                                  //
        // ================================================================================ //

        // configure APIs structs
        lambda_configure();

        // connect to platform
        if(!lambda_connect())
        {
            // failed, return
            return;

        } /* if()*/

        // infinite stim loop
        while(true)
        {
            // update ticking
            ticking_loop_start       = std::chrono::high_resolution_clock::now();
            ticking_platform_elapsed = (ticking_loop_start - ticking_platform_start);

            // call lambdas
            lambda_resetUpstream();
            lambda_buildUpstream();
            lambda_sendUpstream();
            lambda_readDownstream();
            lambda_processDownstream();

            // unpdate end timings
            ticking_loop_end     = std::chrono::high_resolution_clock::now();
            ticking_loop_elapsed = (ticking_loop_end - ticking_loop_start);

            // check objectives
            if(ticking_loop_elapsed > target_loop_period)
            {
                // simply log overflow
                LogWarn("[Umbrella]"sv)
                    << " => main loop has overshoot target period : current["sv
                    << ticking_loop_elapsed
                    << "] > target ["sv
                    << target_loop_period
                    << "]"sv;

            } /* if() */

        } /* while() */

        // clean before leaving
        lambda_cleanup();

    } /* doWork() */

    auto
    processMain(
        std::size_t const &                _argc,
        std::vector< std::string > const & _argv
    )
        -> int
    {
        //
        // case 1
        // ---
        // Vol en turbulences modérées 0.2 Hz 20.0 mm
        //
        [[maybe_unused]] static constexpr auto default_C1_frequency_hz = 0.2;
        [[maybe_unused]] static constexpr auto default_C1_amplitude_mm = 20.0;

        //
        // case 2
        // ---
        // Balourd rotor principal modéré 5.0 Hz 0.8 mm
        //
        [[maybe_unused]] static constexpr auto default_C2_frequency_hz = 5.0;
        [[maybe_unused]] static constexpr auto default_C2_amplitude_mm = 0.8;

        //
        // phase offset
        // ---
        // X axis : no offset / reference
        // Y axis : has offset
        // Z axis : has offset
        //
        static constexpr auto default_x_axis_phase_offset_s = 0.0;
        static constexpr auto default_y_axis_phase_offset_s = 5.0;
        static constexpr auto default_z_axis_phase_offset_s = 7.0;

        //
        // stim axis
        // ---
        // by default, 3D movement
        //
        static constexpr auto default_stim_axis
            = BitmaskEnum{
                UmbrellaStimAxisType::HEAVE_
                & UmbrellaStimAxisType::SWAY_
                & UmbrellaStimAxisType::SURGE_
            };

        //
        // do the WORK !
        //
        doWork(
            default_stim_axis,
            default_C1_frequency_hz,
            default_C1_amplitude_mm,
            default_x_axis_phase_offset_s,
            default_y_axis_phase_offset_s,
            default_z_axis_phase_offset_s
        );

        return titans::ouranos::to_underlying(status_code::UMBRELLA_OK_);

    } /* processMain() */

} // namespace Labsim::project::umbrella
