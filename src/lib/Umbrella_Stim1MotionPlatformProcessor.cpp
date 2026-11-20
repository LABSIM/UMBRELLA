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
#include <Umbrella/Umbrella_Stim1MotionPlatformProcessor.hpp>

#include <Ouranos/log/Ouranos_Logger_log.hpp>

#include <boost/exception/all.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <exception>
#include <filesystem>
#include <format>
#include <numbers>
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
    Stim1MotionPlatformProcessor::doInitFirstPass()
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "Stim1MotionPlatformProcessor::doInitFirstPass(): entry"sv;

        // get a handle
        auto handle = ApplicationManager::get();

        // extract settings
        this->m_setting_is_debug            = handle->getOptions().isDebugSimulation();
        this->m_setting_index               = handle->getOptions().getFSDIPlatformIndex();
        this->m_setting_amplitude           = handle->getOptions().getStim1Amplitude();
        this->m_setting_frequency           = handle->getOptions().getStim1Frequency();
        this->m_setting_y_axis_phase_offset = handle->getOptions().getYAxisPhaseOffset();
        this->m_setting_z_axis_phase_offset = handle->getOptions().getZAxisPhaseOffset();

        // configure outputs
        auto output_filename
            = "umbrella_"s
              + std::regex_replace(
                  titans::ouranos::Common::GenerateTimestamp(),
                  std::regex{":"},
                  "-"
              )
              + "_stim1.csv"s;
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
                    << "Stim1MotionPlatformProcessor::doInitFirstPass(): output directory does not exist..."sv;

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
            << "Stim1MotionPlatformProcessor::doInitFirstPass(): exit"sv;

        // if this point is reached, then success
        return true;

    } /* doInitFirstPass() */

    auto
    Stim1MotionPlatformProcessor::doResetUpstream(
        MotionPlatformInfo & platform
    )
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "Stim1MotionPlatformProcessor::doResetUpstream(): entry"sv;

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
            << "Stim1MotionPlatformProcessor::doResetUpstream(): exit"sv;

        // if this point is reached, then success
        return true;

    } /* ResetUpstream() */

    auto
    Stim1MotionPlatformProcessor::doBuildUpstream(
        MotionPlatformInfo & platform,
        double const &       _elapsed_tick_count
    )
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "Stim1MotionPlatformProcessor::doBuildUpstream(): entry"sv;

        // invariant
        static constexpr auto const _one_constant = 1.0;
        static constexpr auto const _two_constant = 2.0;

        // encapsulate all calculus
        try
        {
            //
            // Sway
            // x(t) = Ax * sin((2*pi*t) / T)
            //
            platform.m_fsdi_requestedPosition.sway
                = boost::numeric_cast< FSDI_FLOAT >(
                    this->m_setting_amplitude
                    * std::sin(
                        (_two_constant * std::numbers::pi * _elapsed_tick_count)
                        / (_one_constant / this->m_setting_frequency)
                    )
                );

            //
            // Heave
            // y(t) = Ay * sin( ((2*pi*t) / T) + ( (pi/2) * sin((2*pi*t) / T(y)) ) )
            //
            platform.m_fsdi_requestedPosition.heave
                = boost::numeric_cast< FSDI_FLOAT >(
                    this->m_setting_amplitude
                    * std::sin(
                        (
                            (_two_constant * std::numbers::pi * _elapsed_tick_count)
                            / (_one_constant / this->m_setting_frequency)
                        )
                        + ((std::numbers::pi / _two_constant)
                           * ((_two_constant * std::numbers::pi * _elapsed_tick_count)
                              / ((_one_constant / this->m_setting_frequency)
                                 * this->m_setting_y_axis_phase_offset
                              )
                           )
                        )
                    )
                );

            //
            // Surge
            // z(t) = Az * sin( ((2*pi*t) / T) + ( (pi/2) * sin((2*pi*t) / T(z)) ) )
            //
            platform.m_fsdi_requestedPosition.surge
                = boost::numeric_cast< FSDI_FLOAT >(
                    this->m_setting_amplitude
                    * std::sin(
                        (
                            (_two_constant * std::numbers::pi * _elapsed_tick_count)
                            / (_one_constant / this->m_setting_frequency)
                        )
                        + ((std::numbers::pi / _two_constant)
                           * ((_two_constant * std::numbers::pi * _elapsed_tick_count)
                              / ((_one_constant / this->m_setting_frequency)
                                 * this->m_setting_z_axis_phase_offset
                              )
                           )
                        )
                    )
                );
        }
        catch(boost::numeric::negative_overflow const & _exception)
        {
            // log
            LogWarn("[Umbrella]"sv)
                << "Stim1MotionPlatformProcessor::doBuildUpstream() => caught boost::numeric::negative_overflow : "sv
                << _exception.what();
        }
        catch(boost::numeric::positive_overflow const & _exception)
        {
            // log
            LogWarn("[Umbrella]"sv)
                << "Stim1MotionPlatformProcessor::doBuildUpstream() => caught boost::numeric::positive_overflow : "sv
                << _exception.what();
        }
        catch(boost::numeric::bad_numeric_cast const & _exception)
        {
            // log
            LogWarn("[Umbrella]"sv)
                << "Stim1MotionPlatformProcessor::doBuildUpstream() => caught boost::numeric::bad_numeric_cast : "sv
                << _exception.what();
        }
        catch(boost::exception const & _exception)
        {
            // log
            LogWarn("[Umbrella]"sv)
                << "Stim1MotionPlatformProcessor::doBuildUpstream() => caught boost::exception : "sv
                << boost::diagnostic_information(_exception);
        }
        catch(std::exception const & _exception)
        {
            // log
            LogWarn("[Umbrella]"sv)
                << "Stim1MotionPlatformProcessor::doBuildUpstream() => caught caught std::exception : "sv
                << _exception.what();
        }
        catch(...)
        {
            // log
            LogWarn("[Umbrella]"sv)
                << "Stim1MotionPlatformProcessor::doBuildUpstream() => caught caught unhandled exception : "sv
                << boost::current_exception_diagnostic_information();

        } /* try() */

        // log
        LogDebug("[Umbrella]"sv)
            << "Stim1MotionPlatformProcessor::doBuildUpstream(): exit"sv;

        // if this point is reached, then success
        return true;

    } /* BuildUpstream() */

    auto
    Stim1MotionPlatformProcessor::doSendUpstream(
        MotionPlatformInfo & platform
    )
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "Stim1MotionPlatformProcessor::doSendUpstream(): entry"sv;

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
                    << "Stim1MotionPlatformProcessor::doSendUpstream(): failed to send request to platform [id:"sv
                    << (this->m_setting_index + 1)
                    << "]"sv;

            } /* if() */
        }
        else
        {
            // log
            LogDebug("[Umbrella]"sv)
                << "Stim1MotionPlatformProcessor::doSendUpstream(): API send("sv
                << "heave:"sv << platform.m_fsdi_requestedPosition.heave
                << "/sway:"sv << platform.m_fsdi_requestedPosition.sway
                << "/surge:"sv << platform.m_fsdi_requestedPosition.surge
                << ")";

        } /* if() */

        // log
        LogDebug("[Umbrella]"sv)
            << "Stim1MotionPlatformProcessor::doSendUpstream(): exit"sv;

        // if this point is reached, then success
        return true;

    } /* SendUpstream() */

    auto
    Stim1MotionPlatformProcessor::doReadDownstream(
        MotionPlatformInfo & platform
    )
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "Stim1MotionPlatformProcessor::doReadDownstream(): entry"sv;

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
                    << "Stim1MotionPlatformProcessor::doReadDownstream(): Failed to get actuator pos"sv;

            } /* if() */

            if(ForceSeatDI_GetTopTablePosPhy(
                   platform.m_fsdi_apis.at(this->m_setting_index),
                   &platform.m_fsdi_actualTopTablePosition
               )
               != FSDI_True)
            {
                // failed...
                LogError("[Umbrella]"sv)
                    << "Stim1MotionPlatformProcessor::doReadDownstream(): Failed to get top table pos"sv;

            } /* if() */

            if(ForceSeatDI_GetPlatformInfo(
                   platform.m_fsdi_apis.at(this->m_setting_index),
                   &platform.m_fsdi_actualPlatformInfo
               )
               != FSDI_True)
            {
                // failed...
                LogError("[Umbrella]"sv)
                    << "Stim1MotionPlatformProcessor::doReadDownstream(): Failed to get platform info with [ErrorCode:"sv
                    << ForceSeatDI_GetRecentErrorCode(platform.m_fsdi_apis.at(this->m_setting_index))
                    << "]"sv;

            } /* if() */

        } /* if() */

        // log
        LogDebug("[Umbrella]"sv)
            << "Stim1MotionPlatformProcessor::doReadDownstream(): exit"sv;

        // if this point is reached, then success
        return true;

    } /* ReadDownstream() */

    auto
    Stim1MotionPlatformProcessor::doProcessDownstream(
        MotionPlatformInfo & platform
    )
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "Stim1MotionPlatformProcessor::doProcessDownstream(): entry"sv;

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
            << "Stim1MotionPlatformProcessor::doProcessDownstream(): exit"sv;

        // if this point is reached, then success
        return true;

    } /* ReadDownstream() */

} // namespace Labsim::project::umbrella
