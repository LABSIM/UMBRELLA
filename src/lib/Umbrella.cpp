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

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <dlfcn.h>
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

    // void
    // sleepMS(
    //     unsigned int ms
    // )
    // {
    //     usleep(ms * 1000);
    // }

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

        // the ForceSetDI motion platform handle
        auto fsdi_apis = std::vector< FSDI_Handle >{};

        // try ethernet first
        if(!connectNET(fsdi_apis, fsdi_platform_index, fsdi_platform_ip.data()))
        {
            // failsafe over USB ?
            if(!connectUSB(fsdi_apis, fsdi_platform_index, fsdi_platform_sn.data()))
            {
                // fail...
                return;

            } /* if() */

        } /* if() */

        // init idle
        std::this_thread::sleep_for(idle_init_period);

        // should always be 1 !
        if(fsdi_apis.size() != fsdi_platform_count)
        {
            // fail...
            return;

        } /* if() */

        // initialize FSDI structs

        static FSDI_TopTablePositionPhysical fsdi_requestedPosition;
        memset(&fsdi_requestedPosition, 0, sizeof(fsdi_requestedPosition));
        fsdi_requestedPosition.structSize          = sizeof(fsdi_requestedPosition);
        fsdi_requestedPosition.maxSpeed            = 65535;
        fsdi_requestedPosition.pause               = FSDI_False;
        fsdi_requestedPosition.strategy            = FSDI_Strategy_BestMatch;
        fsdi_requestedPosition.accelerationProfile = FSDI_AP_Rapid;

        static FSDI_ActualActuatorsPositionLogical fsdi_actualPosition;
        fsdi_actualPosition.structSize = sizeof(FSDI_ActualActuatorsPositionLogical);

        static FSDI_ActualTopTablePositionPhysical fsdi_actualTopTablePosition;
        fsdi_actualTopTablePosition.structSize = sizeof(FSDI_ActualTopTablePositionPhysical);

        static FSDI_PlatformInfo fsdi_actualPlatformInfo;
        fsdi_actualPlatformInfo.structSize = sizeof(FSDI_PlatformInfo);

        //
        // infinite stim loop
        //
        while(true)
        {
            // update ticking
            ticking_loop_start       = std::chrono::high_resolution_clock::now();
            ticking_platform_elapsed = (ticking_loop_start - ticking_platform_start);

            // reset requested
            fsdi_requestedPosition.yaw
                = fsdi_requestedPosition.pitch
                = fsdi_requestedPosition.roll
                = fsdi_requestedPosition.heave
                = fsdi_requestedPosition.sway
                = fsdi_requestedPosition.surge
                = 0.0f;

            // sway calculus
            if(stim_axis.AnyOf(UmbrellaStimAxisType::SWAY_))
            {
                //
                // x(t) = Ax * sin((2*pi*t) / T)
                //
                fsdi_requestedPosition.sway
                    = ((stim_amplitude_mm / mm_to_m_factor)
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
                    = ((stim_amplitude_mm / mm_to_m_factor)
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
                    = ((stim_amplitude_mm / mm_to_m_factor)
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
            if(stim_axis.AnyOf(BitmaskEnum{UmbrellaStimAxisType::YAW_ | UmbrellaStimAxisType::PITCH_ | UmbrellaStimAxisType::ROLL_}))
            {
                // nothing for the moment

            } /* if()*/

            // send requested datas !
            send(
                fsdi_apis,
                fsdi_platform_index,
                fsdi_requestedPosition
            );

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

            // // Prepare demo data
            // auto value = sinf(iterator * 3.1415f / 180) * 5 * 3.1415f / 180;
            // if(++iterator > 360)
            // {
            //     iterator = 0;
            // }

            //             pos.roll  = value;
            //             pos.pitch = 0;

            //             send(apis, platform_index, pos);

            //             // if(NumberOfPlatforms > 1)
            //             // {
            //             //     pos.roll  = -value;
            //             //     pos.pitch = 0;
            //             //     send(apis, 1, pos);
            //             // }

            //             // if(NumberOfPlatforms > 2)
            //             // {
            //             //     pos.roll  = 0;
            //             //     pos.pitch = value;
            //             //     send(apis, 2, pos);
            //             // }

            //             // if(NumberOfPlatforms > 3)
            //             // {
            //             //     pos.roll  = 0;
            //             //     pos.pitch = -value;
            //             //     send(apis, 3, pos);
            //             // }

            //             // Read current actuators position
            //             for(uint32_t i = 0; i < NumberOfPlatforms; ++i)
            //             {
            //                 if(ForceSeatDI_GetActuatorsPosLog(apis[i], &actualPositions[i]) != FSDI_True)
            //                 {
            //                     printf("Failed to get actuator pos from %u\n", (i + 1));
            //                 }
            //                 if(ForceSeatDI_GetTopTablePosPhy(apis[i], &topTablePos[i]) != FSDI_True)
            //                 {
            //                     printf("Failed to get top table pos from %u\n", (i + 1));
            //                 }
            //                 if(ForceSeatDI_GetPlatformInfo(apis[i], &platformInfos[i]) != FSDI_True)
            //                 {
            //                     printf("Failed to get platform info from %u\n", (i + 1));
            //                     printf("ErrorCode: %d", ForceSeatDI_GetRecentErrorCode(apis[i]));
            //                 }
            //             }

            // #if 1
            //             // NOTE: Displaying data is slow
            //             for(uint32_t i = 0; i < NumberOfPlatforms; ++i)
            //             {
            //                 printf("Platform: %u, Pos: %u, %u, %u, %u, %u, %u\n", (i + 1), actualPositions[i].actualMotorPosition[0], actualPositions[i].actualMotorPosition[1], actualPositions[i].actualMotorPosition[2], actualPositions[i].actualMotorPosition[3], actualPositions[i].actualMotorPosition[4], actualPositions[i].actualMotorPosition[5]);
            //                 printf("    Error code: %d.%d\n", static_cast< int >(platformInfos[i].moduleErrorCode), static_cast< int >(platformInfos[i].moduleErrorIndex));
            //                 printf("    Top Frame: roll %f, pitch %f, yaw %f, heave %f, surge %f, sway %f\n", topTablePos[i].roll, topTablePos[i].pitch, topTablePos[i].yaw, topTablePos[i].heave, topTablePos[i].surge, topTablePos[i].sway);
            //                 static_assert(FSDI_MotorsCount == 6, "");
            //             }
            // #endif
            

            //             sleepMS(5);

        } /* while() */
        
        printf("SIM ended...\n");

        // cleanup
        for(FSDI_Handle api: fsdi_apis)
        {
            ForceSeatDI_Park(api, FSDI_ParkMode_Normal);
            ForceSeatDI_Delete(api);

        } /* for() */

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
                | UmbrellaStimAxisType::SWAY_
                | UmbrellaStimAxisType::SURGE_
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
