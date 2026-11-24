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
#include <Umbrella/Umbrella_IdleMotionPlatformProcessor.hpp>
#include <Umbrella/Umbrella_MotionPlatformInfo.hpp>

#include <Ouranos/log/Ouranos_Logger_log.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <algorithm>
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
        // log
        LogDebug("[Umbrella]"sv)
            << "IdleMotionPlatformProcessor::doInitFirstPass(): entry"sv;

        // constant
        static constexpr auto const _s_one_second = 1.0s;

        // get a handle
        auto handle = ApplicationManager::get();

        // extract settings
        this->m_setting_is_debug       = handle->getOptions().isDebugSimulation();
        this->m_setting_index          = handle->getOptions().getFSDIPlatformIndex();
        this->m_setting_reset_duration = boost::numeric_cast< double >(handle->getOptions().getIdleResetDuration() / _s_one_second);

        // log
        LogDebug("[Umbrella]"sv)
            << "IdleMotionPlatformProcessor::doInitFirstPass(): exit"sv;

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

        // constant
        static constexpr auto const _s_default_maxSpeed = 10000_UI16;

        // reset all
        platform.m_fsdi_requestedPosition.maxSpeed            = _s_default_maxSpeed;
        platform.m_fsdi_requestedPosition.pause               = FSDI_False;
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
        // log
        LogDebug("[Umbrella]"sv)
            << "IdleMotionPlatformProcessor::doBuildUpstream(): entry"sv;

        // classical linear interpolation lambda
        constexpr auto lambda_LERP
            = [=](
                  double const & _start,
                  double const & _end,
                  double const & _ratio
              ) -> double
        {
            return (_start + (_ratio * (_end - _start)));
        };

        if(!this->HasInitialized())
        {
            // get the ref point
            this->m_current_ref = _elapsed_tick_count;
        }
        else
        {
            // updating ratio
            auto ratio
                = 1.0 - std::clamp((this->m_setting_reset_duration - (_elapsed_tick_count - this->m_current_ref)) / this->m_setting_reset_duration, 0.0, 1.0);

            // simple LERP
            platform.m_fsdi_requestedPosition.heave = boost::numeric_cast< float >(lambda_LERP(this->m_startPos[0], 0.0, ratio));
            platform.m_fsdi_requestedPosition.sway  = boost::numeric_cast< float >(lambda_LERP(this->m_startPos[1], 0.0, ratio));
            platform.m_fsdi_requestedPosition.surge = boost::numeric_cast< float >(lambda_LERP(this->m_startPos[2], 0.0, ratio));

        } /* if()*/

        // log
        LogDebug("[Umbrella]"sv)
            << "IdleMotionPlatformProcessor::doBuildUpstream(): exit"sv;

        // if this point is reached, then success
        return true;

    } /* BuildUpstream() */

    auto
    IdleMotionPlatformProcessor::doSendUpstream(
        [[maybe_unused]] MotionPlatformInfo & platform
    )
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "IdleMotionPlatformProcessor::doSendUpstream(): entry"sv;

        // check
        if(!this->m_setting_is_debug && !this->HasInitialized())
        {
            // send requested datas !
            if(ForceSeatDI_SendTopTablePosPhy(
                   platform.m_fsdi_apis[this->m_setting_index],
                   &platform.m_fsdi_requestedPosition
               )
               != FSDI_True)
            {
                // failed...
                LogError("[Umbrella]"sv)
                    << "IdleMotionPlatformProcessor::doSendUpstream(): failed to send request to platform [id:"sv
                    << (this->m_setting_index + 1)
                    << "]"sv;

            } /* if() */
        }
        else
        {
            // log
            LogDebug("[Umbrella]"sv)
                << "IdleMotionPlatformProcessor::doSendUpstream(): API send("sv
                << "heave:"sv << platform.m_fsdi_requestedPosition.heave
                << "/sway:"sv << platform.m_fsdi_requestedPosition.sway
                << "/surge:"sv << platform.m_fsdi_requestedPosition.surge
                << ")";

        } /* if() */

        // log
        LogDebug("[Umbrella]"sv)
            << "IdleMotionPlatformProcessor::doSendUpstream(): exit"sv;

        // if this point is reached, then success
        return true;

    } /* SendUpstream() */

    auto
    IdleMotionPlatformProcessor::doReadDownstream(
        [[maybe_unused]] MotionPlatformInfo & platform
    )
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "IdleMotionPlatformProcessor::doReadDownstream(): entry"sv;

        // check
        if(!this->m_setting_is_debug && !this->HasInitialized())
        {
            // Read current actuators position only once

            if(ForceSeatDI_GetActuatorsPosLog(
                   platform.m_fsdi_apis.at(this->m_setting_index),
                   &platform.m_fsdi_actualPosition
               )
               != FSDI_True)
            {
                // failed...
                LogError("[Umbrella]"sv)
                    << "IdleMotionPlatformProcessor::doReadDownstream(): Failed to get actuator pos"sv;

            } /* if() */

            if(ForceSeatDI_GetTopTablePosPhy(
                   platform.m_fsdi_apis.at(this->m_setting_index),
                   &platform.m_fsdi_actualTopTablePosition
               )
               != FSDI_True)
            {
                // failed...
                LogError("[Umbrella]"sv)
                    << "IdleMotionPlatformProcessor::doReadDownstream(): Failed to get top table pos"sv;

            } /* if() */

            if(ForceSeatDI_GetPlatformInfo(
                   platform.m_fsdi_apis.at(this->m_setting_index),
                   &platform.m_fsdi_actualPlatformInfo
               )
               != FSDI_True)
            {
                // failed...
                LogError("[Umbrella]"sv)
                    << "IdleMotionPlatformProcessor::doReadDownstream(): Failed to get platform info with [ErrorCode:"sv
                    << ForceSeatDI_GetRecentErrorCode(platform.m_fsdi_apis.at(this->m_setting_index))
                    << "]"sv;

            } /* if() */

            // save init pos
            this->m_startPos[0] = static_cast< double >(platform.m_fsdi_actualTopTablePosition.heave);
            this->m_startPos[1] = static_cast< double >(platform.m_fsdi_actualTopTablePosition.sway);
            this->m_startPos[2] = static_cast< double >(platform.m_fsdi_actualTopTablePosition.surge);

        } /* if() */

        // log
        LogDebug("[Umbrella]"sv)
            << "IdleMotionPlatformProcessor::doReadDownstream(): exit"sv;

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
