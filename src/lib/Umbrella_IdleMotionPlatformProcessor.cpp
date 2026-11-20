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

#include <Umbrella/Umbrella_IdleMotionPlatformProcessor.hpp>
#include <Umbrella/Umbrella_MotionPlatformInfo.hpp>

#include <Ouranos/log/Ouranos_Logger_log.hpp>

#include <string_view>

#include "ForceSeatDI.h"

using namespace std::literals::string_view_literals;
using namespace std::literals::string_literals;
using namespace std::literals::chrono_literals;

// avoid namespace pollution
namespace Labsim::project::umbrella
{

    auto
    IdleMotionPlatformProcessor::doInitFirstPass()
        -> bool
    {
        
        // if this point is reached, then success
        return true;

    } /* doInitFirstPass() */

    auto
    IdleMotionPlatformProcessor::doResetUpstream(
        MotionPlatformInfo & platform
    )
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "IdleMotionPlatformProcessor::doResetUpstream(): entry"sv;

        // center position requested only once
        if(!this->HasInitialized())
        {
            
            // constant
            static constexpr auto const _s_default_maxSpeed = 65535_UI16;

            // reset all
            platform.m_fsdi_requestedPosition.maxSpeed            = _s_default_maxSpeed;
            platform.m_fsdi_requestedPosition.pause               = FSDI_True;
            platform.m_fsdi_requestedPosition.strategy            = FSDI_Strategy_BestMatch;
            platform.m_fsdi_requestedPosition.accelerationProfile = FSDI_AP_Smoothest;

            // also zero requested computed value
            platform.m_fsdi_requestedPosition.yaw
                = platform.m_fsdi_requestedPosition.pitch
                = platform.m_fsdi_requestedPosition.roll
                = platform.m_fsdi_requestedPosition.heave
                = platform.m_fsdi_requestedPosition.sway
                = platform.m_fsdi_requestedPosition.surge
                = 0.0F;
        
            // loop
            for(FSDI_Handle api: platform.m_fsdi_apis)
            {

                // mark as paused
                if(ForceSeatDI_SendTopTablePosPhy(
                    api,
                    &platform.m_fsdi_requestedPosition
                )
                != FSDI_True)
                {
                    // failed...
                    LogError("[Umbrella]"sv)
                        << "IdleMotionPlatformProcessor::doSendUpstream(): failed to send request to platform..."sv;

                } /* if() */

                // log
                LogInfo("[Umbrella]"sv)
                    << "IdleMotionPlatformProcessor::doResetUpstream(): will park to center, request sent"sv;
                
                // call
                ForceSeatDI_Park(api, FSDI_ParkMode_ToCenter);

            } /* for() */

        } /* if() */

        // log
        LogDebug("[Umbrella]"sv)
            << "IdleMotionPlatformProcessor::doResetUpstream(): exit"sv;

        // if this point is reached, then success
        return true;

    } /* ResetUpstream() */

    auto
    IdleMotionPlatformProcessor::doBuildUpstream(
        [[maybe_unused]] MotionPlatformInfo & platform,
        [[maybe_unused]] double const &       _elapsed_tick_count
    )
        -> bool
    {
        // if this point is reached, then success
        return true;

    } /* BuildUpstream() */

    auto
    IdleMotionPlatformProcessor::doSendUpstream(
        [[maybe_unused]] MotionPlatformInfo & platform
    )
        -> bool
    {
        // loop
        for(FSDI_Handle api: platform.m_fsdi_apis)
        {
            // call
            ForceSeatDI_Park(api, FSDI_ParkMode_ToCenter);

        } /* for() */

        // if this point is reached, then success
        return true;

    } /* SendUpstream() */

    auto
    IdleMotionPlatformProcessor::doReadDownstream(
        [[maybe_unused]] MotionPlatformInfo & platform
    )
        -> bool
    {
        // if this point is reached, then success
        return true;

    } /* ReadDownstream() */

    auto
    IdleMotionPlatformProcessor::doProcessDownstream(
        [[maybe_unused]] MotionPlatformInfo & platform
    )
        -> bool
    {
        // if this point is reached, then success
        return true;

    } /* ReadDownstream() */

} // namespace Labsim::project::umbrella
