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
#include <cmath>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <dlfcn.h>
#include <unistd.h>
#include <vector>

#include "ForceSeatDI.h"

namespace Labsim::project::umbrella
{

    void
    sleepMS(
        unsigned int ms
    )
    {
        usleep(ms * 1000);
    }

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
    work(
        std::vector< FSDI_Handle > & apis
    )
    {
        bool ok = true;

        (void)&connectNET;
        (void)&connectUSB;

#if 0
		// Network 4 devices
		ok = ok &&  connectNET(apis, 0, "10.1.1.75");
		ok = ok &&  connectNET(apis, 1, "10.1.1.75");
		ok = ok &&  connectNET(apis, 2, "10.1.1.75");
		ok = ok &&  connectNET(apis, 3, "10.1.1.75");
#endif
#if 0
		// USB 4 devices
		ok = ok &&  connectUSB(apis, 0, L"250031-000457-315839-323120");
		ok = ok &&  connectUSB(apis, 1, L"250031-000457-315839-323120");
		ok = ok &&  connectUSB(apis, 2, L"250031-000457-315839-323120");
		ok = ok &&  connectUSB(apis, 3, L"250031-000457-315839-323120");
#endif
#if 1
        // USB 1 any device
        ok = ok && connectUSB(apis, 0, nullptr);
#endif

        if(!ok)
        {
            return;
        }

        sleepMS(500);
        auto const NumberOfPlatforms = static_cast< uint32_t >(apis.size());

        FSDI_TopTablePositionPhysical pos;
        memset(&pos, 0, sizeof(pos));
        pos.structSize          = sizeof(pos);
        pos.maxSpeed            = 65535;
        pos.pause               = FSDI_False;
        pos.strategy            = FSDI_Strategy_BestMatch;
        pos.accelerationProfile = FSDI_AP_Auto;

        std::vector< FSDI_ActualActuatorsPositionLogical > actualPositions;
        std::vector< FSDI_PlatformInfo >                   platformInfos;
        std::vector< FSDI_ActualTopTablePositionPhysical > topTablePos;
        actualPositions.resize(NumberOfPlatforms);
        platformInfos.resize(NumberOfPlatforms);
        topTablePos.resize(NumberOfPlatforms);

        for(uint32_t i = 0; i < NumberOfPlatforms; ++i)
        {
            actualPositions[i].structSize = sizeof(FSDI_ActualActuatorsPositionLogical);
            platformInfos[i].structSize   = sizeof(FSDI_PlatformInfo);
            topTablePos[i].structSize     = sizeof(FSDI_ActualTopTablePositionPhysical);
        }

        unsigned int iterator = 0;

        printf("SIM started...\n");
        while(true)
        {
            // Prepare demo data
            auto value = sinf(iterator * 3.1415f / 180) * 5 * 3.1415f / 180;
            if(++iterator > 360)
            {
                iterator = 0;
            }

            pos.roll  = value;
            pos.pitch = 0;
            send(apis, 0, pos);

            if(NumberOfPlatforms > 1)
            {
                pos.roll  = -value;
                pos.pitch = 0;
                send(apis, 1, pos);
            }

            if(NumberOfPlatforms > 2)
            {
                pos.roll  = 0;
                pos.pitch = value;
                send(apis, 2, pos);
            }

            if(NumberOfPlatforms > 3)
            {
                pos.roll  = 0;
                pos.pitch = -value;
                send(apis, 3, pos);
            }

            // Read current actuators position
            for(uint32_t i = 0; i < NumberOfPlatforms; ++i)
            {
                if(ForceSeatDI_GetActuatorsPosLog(apis[i], &actualPositions[i]) != FSDI_True)
                {
                    printf("Failed to get actuator pos from %u\n", (i + 1));
                }
                if(ForceSeatDI_GetTopTablePosPhy(apis[i], &topTablePos[i]) != FSDI_True)
                {
                    printf("Failed to get top table pos from %u\n", (i + 1));
                }
                if(ForceSeatDI_GetPlatformInfo(apis[i], &platformInfos[i]) != FSDI_True)
                {
                    printf("Failed to get platform info from %u\n", (i + 1));
                    printf("ErrorCode: %d", ForceSeatDI_GetRecentErrorCode(apis[i]));
                }
            }

#if 1
            // NOTE: Displaying data is slow
            for(uint32_t i = 0; i < NumberOfPlatforms; ++i)
            {
                printf("Platform: %u, Pos: %u, %u, %u, %u, %u, %u\n", (i + 1), actualPositions[i].actualMotorPosition[0], actualPositions[i].actualMotorPosition[1], actualPositions[i].actualMotorPosition[2], actualPositions[i].actualMotorPosition[3], actualPositions[i].actualMotorPosition[4], actualPositions[i].actualMotorPosition[5]);
                printf("    Error code: %d.%d\n", static_cast< int >(platformInfos[i].moduleErrorCode), static_cast< int >(platformInfos[i].moduleErrorIndex));
                printf("    Top Frame: roll %f, pitch %f, yaw %f, heave %f, surge %f, sway %f\n", topTablePos[i].roll, topTablePos[i].pitch, topTablePos[i].yaw, topTablePos[i].heave, topTablePos[i].surge, topTablePos[i].sway);
                static_assert(FSDI_MotorsCount == 6, "");
            }
#endif

            sleepMS(5);
        }
        printf("SIM ended...\n");
    }

    void
    do_run_stim(
        BitmaskEnum< UmbrellaStimAxisType > stim_axis,
        double                              stim_frequency_hz,
        double                              stim_amplitude_mm
    )
    {
        std::vector< FSDI_Handle > apis;
        work(apis);

        for(FSDI_Handle api: apis)
        {
            ForceSeatDI_Park(api, FSDI_ParkMode_Normal);
            ForceSeatDI_Delete(api);
        }
    }

    auto
    main(
        std::size_t const &                _argc,
        std::vector< std::string > const & _argv
    )
		-> 
    {
        // default arg
        do_run_stim(
            BitmaskEnum{UmbrellaStimAxisType::X_ | UmbrellaStimAxisType::Y_},
            30.0,
            0.2
        );
    }

} // namespace Labsim::project::umbrella
