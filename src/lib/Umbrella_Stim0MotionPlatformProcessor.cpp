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
#include <Umbrella/Umbrella_MotionPlatformInfo.hpp>
#include <Umbrella/Umbrella_Stim0MotionPlatformProcessor.hpp>

#include <Ouranos/log/Ouranos_Logger_log.hpp>

#include <boost/exception/all.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <chrono>
#include <filesystem>
#include <format>
#include <regex>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include "ForceSeatDI.h"

using namespace std::literals::string_view_literals;
using namespace std::literals::string_literals;
using namespace std::literals::chrono_literals;

// avoid namespace pollution
namespace Labsim::project::umbrella
{

    auto
    Stim0MotionPlatformProcessor::doInitFirstPass()
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "Stim0MotionPlatformProcessor::doInitFirstPass(): entry"sv;

        // get a handle
        auto handle = ApplicationManager::get();

        // extract settings
        this->m_setting_is_debug = handle->getOptions().isDebugSimulation();
        this->m_setting_index    = handle->getOptions().getFSDIPlatformIndex();
        auto output_filename
            = "umbrella_"s
              + std::regex_replace(
                  titans::ouranos::Common::GenerateTimestamp(),
                  std::regex{":"},
                  "-"
              )
              + "_stim0.csv"s;
        auto output_session_path
            = std::regex_replace(
                handle->getSessionTimestamp(),
                std::regex{":"},
                "-"
            );
        auto output_path
            = std::filesystem::path{handle->getOptions().getOutputDataDirectory()};

        // check emptyness
        if(output_path.empty())
        {
            // iff. concat session path
            output_path += output_session_path;
        }
        else
        {
            // check existence
            if(!std::filesystem::directory_entry{output_path}.exists())
            {
                // log
                LogError("[Umbrella]"sv)
                    << "Stim0MotionPlatformProcessor::doInitFirstPass(): output directory does not exist..."sv;

                // failure
                return false;

            } /* if() */

            // append with OS sep.
            output_path /= output_session_path;

        } /* if() */

        // create dir
        std::filesystem::create_directories(output_path);

        // configure outputs
        this->m_output_file
            = std::ofstream{
                  output_path / output_filename,
                  std::ios_base::out | std::ios_base::app
              }
              << "timestamp,requestedPosition.sway,requestedPosition.heave,requestedPosition.surge,actualPosition.sway,actualPosition.heave,actualPosition.surge\n"sv;

        // log
        LogDebug("[Umbrella]"sv)
            << "Stim0MotionPlatformProcessor::doInitFirstPass(): exit"sv;

        // if this point is reached, then success
        return true;

    } /* doInitFirstPass() */

    auto
    Stim0MotionPlatformProcessor::doResetUpstream(
        MotionPlatformInfo & platform
    )
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "Stim0MotionPlatformProcessor::doResetUpstream(): entry"sv;

        // constant
        static constexpr auto const _s_default_maxSpeed = 65535_UI16;

        // reset all
        platform.m_fsdi_requestedPosition.maxSpeed            = _s_default_maxSpeed;
        platform.m_fsdi_requestedPosition.pause               = FSDI_False;
        platform.m_fsdi_requestedPosition.strategy            = FSDI_Strategy_BestMatch;
        platform.m_fsdi_requestedPosition.accelerationProfile = FSDI_AP_Rapid;

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
            << "Stim0MotionPlatformProcessor::doResetUpstream(): exit"sv;

        // if this point is reached, then success
        return true;

    } /* ResetUpstream() */

    auto
    Stim0MotionPlatformProcessor::doBuildUpstream(
        [[maybe_unused]] MotionPlatformInfo & platform,
        [[maybe_unused]] double const &       _elapsed_tick_count
    )
        -> bool
    {
        // if this point is reached, then success
        return true;

    } /* BuildUpstream() */

    auto
    Stim0MotionPlatformProcessor::doSendUpstream(
        MotionPlatformInfo & platform
    )
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "Stim0MotionPlatformProcessor::doSendUpstream(): entry"sv;

        // check
        if(!this->m_setting_is_debug)
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
                    << "Stim0MotionPlatformProcessor::doSendUpstream(): failed to send request to platform [id:"sv
                    << (this->m_setting_index + 1)
                    << "]"sv;

            } /* if() */
        }
        else
        {
            // log
            LogDebug("[Umbrella]"sv)
                << "Stim0MotionPlatformProcessor::doSendUpstream(): API send("sv
                << "heave:"sv << platform.m_fsdi_requestedPosition.heave
                << "/sway:"sv << platform.m_fsdi_requestedPosition.sway
                << "/surge:"sv << platform.m_fsdi_requestedPosition.surge
                << ")";

        } /* if() */

        // log
        LogDebug("[Umbrella]"sv)
            << "Stim0MotionPlatformProcessor::doSendUpstream(): exit"sv;

        // if this point is reached, then success
        return true;

    } /* SendUpstream() */

    auto
    Stim0MotionPlatformProcessor::doReadDownstream(
        MotionPlatformInfo & platform
    )
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "Stim0MotionPlatformProcessor::doReadDownstream(): entry"sv;

        // check
        if(!this->m_setting_is_debug)
        {
            // Read current actuators position

            if(ForceSeatDI_GetActuatorsPosLog(
                   platform.m_fsdi_apis.at(this->m_setting_index),
                   &platform.m_fsdi_actualPosition
               )
               != FSDI_True)
            {
                // failed...
                LogError("[Umbrella]"sv)
                    << "Stim0MotionPlatformProcessor::doReadDownstream(): Failed to get actuator pos"sv;

            } /* if() */

            if(ForceSeatDI_GetTopTablePosPhy(
                   platform.m_fsdi_apis.at(this->m_setting_index),
                   &platform.m_fsdi_actualTopTablePosition
               )
               != FSDI_True)
            {
                // failed...
                LogError("[Umbrella]"sv)
                    << "Stim0MotionPlatformProcessor::doReadDownstream(): Failed to get top table pos"sv;

            } /* if() */

            if(ForceSeatDI_GetPlatformInfo(
                   platform.m_fsdi_apis.at(this->m_setting_index),
                   &platform.m_fsdi_actualPlatformInfo
               )
               != FSDI_True)
            {
                // failed...
                LogError("[Umbrella]"sv)
                    << "Stim0MotionPlatformProcessor::doReadDownstream(): Failed to get platform info with [ErrorCode:"sv
                    << ForceSeatDI_GetRecentErrorCode(platform.m_fsdi_apis.at(this->m_setting_index))
                    << "]"sv;

            } /* if() */

        } /* if() */

        // log
        LogDebug("[Umbrella]"sv)
            << "Stim0MotionPlatformProcessor::doReadDownstream(): exit"sv;

        // if this point is reached, then success
        return true;

    } /* ReadDownstream() */

    auto
    Stim0MotionPlatformProcessor::doProcessDownstream(
        MotionPlatformInfo & platform
    )
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "Stim0MotionPlatformProcessor::doProcessDownstream(): entry"sv;

        // log to file
        this->m_output_file
            << titans::ouranos::Common::GenerateTimestamp()
            << ","sv
            << platform.m_fsdi_requestedPosition.sway
            << ","sv
            << platform.m_fsdi_requestedPosition.heave
            << ","sv
            << platform.m_fsdi_requestedPosition.surge
            << ","sv
            << platform.m_fsdi_actualTopTablePosition.sway
            << ","sv
            << platform.m_fsdi_actualTopTablePosition.heave
            << ","sv
            << platform.m_fsdi_actualTopTablePosition.surge
            << "\n";

        // log
        LogDebug("[Umbrella]"sv)
            << "Stim0MotionPlatformProcessor::doProcessDownstream(): exit"sv;

        // if this point is reached, then success
        return true;

    } /* ReadDownstream() */

} // namespace Labsim::project::umbrella
