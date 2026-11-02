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
#include <format>
#include <fstream>
#include <numbers>
#include <string_view>
#include <thread>
#include <unistd.h>
#include <vector>

#include "ForceSeatDI.h"

using namespace std::literals::string_view_literals;
using namespace std::literals::string_literals;
using namespace std::literals::chrono_literals;

// avoid namespace pollution
namespace Labsim::project::umbrella
{
    // NOLINTBEGIN
    // anonymous namespace, direcly from ForceSeatDI API exemple...
    namespace
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

    } /* anonymous namespace */

    // NOLINTEND

    MotionPlatformManager::MotionPlatformManager(
        base_type::_token_type const & _token
    ) noexcept
        : base_type(_token)
    {
    }

    auto
    MotionPlatformManager::Configure()
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::Configure(): entry"sv;

        static constexpr auto const _s_default_maxSpeed = 65535_UI16;

        // upstream
        // allocate & configure FSDI resquested struct
        memset(&this->m_fsdi_requestedPosition, 0, sizeof(this->m_fsdi_requestedPosition));
        this->m_fsdi_requestedPosition.structSize          = sizeof(this->m_fsdi_requestedPosition);
        this->m_fsdi_requestedPosition.maxSpeed            = _s_default_maxSpeed;
        this->m_fsdi_requestedPosition.pause               = FSDI_False;
        this->m_fsdi_requestedPosition.strategy            = FSDI_Strategy_BestMatch;
        this->m_fsdi_requestedPosition.accelerationProfile = FSDI_AP_Rapid;

        // downstream
        // set sizes
        this->m_fsdi_actualPosition.structSize         = sizeof(FSDI_ActualActuatorsPositionLogical);
        this->m_fsdi_actualTopTablePosition.structSize = sizeof(FSDI_ActualTopTablePositionPhysical);
        this->m_fsdi_actualPlatformInfo.structSize     = sizeof(FSDI_PlatformInfo);

        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::Configure(): exit"sv;

        // success
        return true;

    } /* Configure() */

    auto
    MotionPlatformManager::Connect()
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::Connect(): entry"sv;

        // local vars
        static bool                      setting_is_offline;
        static uint32_t                  setting_index;
        static uint32_t                  setting_count;
        static std::string               setting_ip;
        static std::wstring              setting_sn;
        static std::chrono::milliseconds setting_idle_duration;

        { /* lock */

            auto handle = ApplicationManager::get();

            setting_is_offline    = handle->getOptions().isOfflineSimulation();
            setting_index         = handle->getOptions().getFSDIPlatformIndex();
            setting_count         = handle->getOptions().getFSDIPlatformCount();
            setting_ip            = handle->getOptions().getFSDIPlatformIP();
            setting_sn            = handle->getOptions().getFSDIPlatformSN();
            setting_idle_duration = handle->getOptions().getIdleInitDuration();

        } /* release */

        // check if offline sim is true
        if(setting_is_offline)
        {
            // log
            LogInfo("[Umbrella]"sv)
                << "MotionPlatformManager::Connect(): offline simulation required, do not attempt to connect."sv;

            // if this point is reached, then its a skip !
            return true;

        } /* if() */

        // try ethernet first
        if(!connectNET(this->m_fsdi_apis, setting_index, setting_ip.data()))
        {
            // failsafe over USB ?
            if(!connectUSB(this->m_fsdi_apis, setting_index, setting_sn.data()))
            {
                // log
                LogError("[Umbrella]"sv)
                    << "MotionPlatformManager::Connect(): Failed to connect to platform"sv;

                // fail...
                return false;

            } /* if() */

            // log
            LogInfo("[Umbrella]"sv)
                << "MotionPlatformManager::Connect(): successfuly connected to platform over USB."sv;
        }
        else
        {
            // log
            LogInfo("[Umbrella]"sv)
                << "MotionPlatformManager::Connect(): successfuly connected to platform over ethernet."sv;

        } /* if() */

        // init idle
        std::this_thread::sleep_for(setting_idle_duration);

        // should always be 1 !
        if(this->m_fsdi_apis.size() != setting_count)
        {
            // log
            LogError("[Umbrella]"sv)
                << "MotionPlatformManager::Connect(): APIs does not match our..."sv;

            // fail...
            return false;

        } /* if() */

        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::Connect(): exit"sv;

        // if this point is reached, then its a success
        return true;

    } /* Connect() */

    auto
    MotionPlatformManager::Process(
        double const & _elapsed_tick_count
    )
        -> bool
    {
        // call internal

        if(!this->ResetUpstream())
        {
            // log
            LogError("[Umbrella]"sv)
                << "MotionPlatformManager::Process(): failed to reset upstream"sv;

            // fail...
            return false;

        } /* if() */

        if(!this->BuildUpstream(_elapsed_tick_count))
        {
            // log
            LogError("[Umbrella]"sv)
                << "MotionPlatformManager::Process(): failed to build upstream"sv;

            // fail...
            return false;

        } /* if() */

        if(!this->SendUpstream())
        {
            // log
            LogError("[Umbrella]"sv)
                << "MotionPlatformManager::Process(): failed to send upstream"sv;

            // fail...
            return false;

        } /* if() */

        if(!this->ReadDownstream())
        {
            // log
            LogError("[Umbrella]"sv)
                << "MotionPlatformManager::Process(): failed to read downstream"sv;

            // fail...
            return false;

        } /* if() */

        if(!this->ProcessDownstream())
        {
            // log
            LogError("[Umbrella]"sv)
                << "MotionPlatformManager::Process(): failed to process downstream"sv;

            // fail...
            return false;

        } /* if() */

        // if reached then it's a success
        return true;

    } /* Process() */

    auto
    MotionPlatformManager::Cleanup()
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::Cleanup(): entry"sv;

        // cleanup
        for(FSDI_Handle api: this->m_fsdi_apis)
        {
            ForceSeatDI_Park(api, FSDI_ParkMode_Normal);
            ForceSeatDI_Delete(api);

        } /* for() */

        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::Cleanup(): exit"sv;

        // if reached then it's a success
        return true;

    } /* Cleanup() */

    auto
    MotionPlatformManager::ResetUpstream()
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::ResetUpstream(): entry"sv;

        // reset requested
        this->m_fsdi_requestedPosition.yaw
            = this->m_fsdi_requestedPosition.pitch
            = this->m_fsdi_requestedPosition.roll
            = this->m_fsdi_requestedPosition.heave
            = this->m_fsdi_requestedPosition.sway
            = this->m_fsdi_requestedPosition.surge
            = 0.0F;

        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::ResetUpstream(): exit"sv;

        // if this point is reached, then success
        return true;

    } /* ResetUpstream() */

    auto
    MotionPlatformManager::BuildUpstream(
        double const & _elapsed_tick_count
    )
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::BuildUpstream(): entry"sv;

        // invariant
        static constexpr auto const _mm_to_m_factor = 1000.0;
        static constexpr auto const _one_constant   = 1.0;
        static constexpr auto const _two_constant   = 2.0;

        // local vars
        static bool   bHasInit = false;
        static double setting_amplitude;
        static double setting_frequency;
        static double setting_y_axis_phase_offset;
        static double setting_z_axis_phase_offset;

        if(!bHasInit)
        {
            // get a handle
            auto handle = ApplicationManager::get();

            // extract settings
            if(handle->getOptions().getStimMode() == CommandLineOptions::stim_mode::S1_)
            {
                setting_amplitude = handle->getOptions().getStim1Amplitude();
                setting_frequency = handle->getOptions().getStim1Frequency();
            }
            else
            {
                setting_amplitude = handle->getOptions().getStim2Amplitude();
                setting_frequency = handle->getOptions().getStim2Frequency();
            }
            setting_y_axis_phase_offset = handle->getOptions().getYAxisPhaseOffset();
            setting_z_axis_phase_offset = handle->getOptions().getZAxisPhaseOffset();

            // set
            bHasInit = true;

        } /* if() */

        // encapsulate all calculus
        try
        {
            //
            // Sway
            // x(t) = Ax * sin((2*pi*t) / T)
            //
            this->m_fsdi_requestedPosition.sway
                = boost::numeric_cast< FSDI_FLOAT >(
                    setting_amplitude
                    * std::sin(
                        (_two_constant * std::numbers::pi * _elapsed_tick_count)
                        / (_one_constant / setting_frequency)
                    )
                );

            //
            // Heave
            // y(t) = Ay * sin( ((2*pi*t) / T) + ( (pi/2) * sin((2*pi*t) / T(y)) ) )
            //
            this->m_fsdi_requestedPosition.heave
                = boost::numeric_cast< FSDI_FLOAT >(
                    setting_amplitude
                    * std::sin(
                        (
                            (_two_constant * std::numbers::pi * _elapsed_tick_count)
                            / (_one_constant / setting_frequency)
                        )
                        + ((std::numbers::pi / _two_constant)
                           * ((_two_constant * std::numbers::pi * _elapsed_tick_count)
                              / ((_one_constant / setting_frequency)
                                 * setting_y_axis_phase_offset
                              )
                           )
                        )
                    )
                );

            //
            // Surge
            // z(t) = Az * sin( ((2*pi*t) / T) + ( (pi/2) * sin((2*pi*t) / T(z)) ) )
            //
            this->m_fsdi_requestedPosition.surge
                = boost::numeric_cast< FSDI_FLOAT >(
                    setting_amplitude
                    * std::sin(
                        (
                            (_two_constant * std::numbers::pi * _elapsed_tick_count)
                            / (_one_constant / setting_frequency)
                        )
                        + ((std::numbers::pi / _two_constant)
                           * ((_two_constant * std::numbers::pi * _elapsed_tick_count)
                              / ((_one_constant / setting_frequency)
                                 * setting_z_axis_phase_offset
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
                << "MotionPlatformManager::BuildUpstream() => caught boost::numeric::negative_overflow : "sv
                << _exception.what();
        }
        catch(boost::numeric::positive_overflow const & _exception)
        {
            // log
            LogWarn("[Umbrella]"sv)
                << "MotionPlatformManager::BuildUpstream() => caught boost::numeric::positive_overflow : "sv
                << _exception.what();
        }
        catch(boost::numeric::bad_numeric_cast const & _exception)
        {
            // log
            LogWarn("[Umbrella]"sv)
                << "MotionPlatformManager::BuildUpstream() => caught boost::numeric::bad_numeric_cast : "sv
                << _exception.what();
        }
        catch(boost::exception const & _exception)
        {
            // log
            LogWarn("[Umbrella]"sv)
                << "MotionPlatformManager::BuildUpstream() => caught boost::exception : "sv
                << boost::diagnostic_information(_exception);
        }
        catch(std::exception const & _exception)
        {
            // log
            LogWarn("[Umbrella]"sv)
                << "MotionPlatformManager::BuildUpstream() => caught caught std::exception : "sv
                << _exception.what();
        }
        catch(...)
        {
            // log
            LogWarn("[Umbrella]"sv)
                << "MotionPlatformManager::BuildUpstream() => caught caught unhandled exception : "sv
                << boost::current_exception_diagnostic_information();

        } /* try() */

        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::BuildUpstream(): exit"sv;

        // if this point is reached, then success
        return true;

    } /* BuildUpstream() */

    auto
    MotionPlatformManager::SendUpstream()
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::SendUpstream(): entry"sv;

        // setting
        static auto const _setting_is_offline
            = ApplicationManager::get()->getOptions().isOfflineSimulation();
        static auto const _setting_index
            = ApplicationManager::get()->getOptions().getFSDIPlatformIndex();

        // check
        if(!_setting_is_offline)
        {
            // send requested datas !
            send(
                this->m_fsdi_apis,
                _setting_index,
                this->m_fsdi_requestedPosition
            );
        }
        else
        {
            // log
            LogInfo("[Umbrella]"sv)
                << "MotionPlatformManager::SendUpstream(): API send("sv
                << "heave:"sv << this->m_fsdi_requestedPosition.heave
                << "/sway:"sv << this->m_fsdi_requestedPosition.sway
                << "/surge:"sv << this->m_fsdi_requestedPosition.surge
                << ")";

        } /* if() */

        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::SendUpstream(): exit"sv;

        // if this point is reached, then success
        return true;

    } /* SendUpstream() */

    auto
    MotionPlatformManager::ReadDownstream()
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::ReadDownstream(): entry"sv;

        // setting
        static auto const _setting_is_offline
            = ApplicationManager::get()->getOptions().isOfflineSimulation();
        static auto const _setting_index
            = ApplicationManager::get()->getOptions().getFSDIPlatformIndex();

        // check
        if(!_setting_is_offline)
        {
            // Read current actuators position

            if(ForceSeatDI_GetActuatorsPosLog(
                   this->m_fsdi_apis.at(_setting_index),
                   &this->m_fsdi_actualPosition
               )
               != FSDI_True)
            {
                // failed...
                LogError("[Umbrella]"sv)
                    << "MotionPlatformManager::ReadDownstream(): Failed to get actuator pos"sv;

            } /* if() */

            if(ForceSeatDI_GetTopTablePosPhy(
                   this->m_fsdi_apis.at(_setting_index),
                   &this->m_fsdi_actualTopTablePosition
               )
               != FSDI_True)
            {
                // failed...
                LogError("[Umbrella]"sv)
                    << "MotionPlatformManager::ReadDownstream(): Failed to get top table pos"sv;

            } /* if() */

            if(ForceSeatDI_GetPlatformInfo(
                   this->m_fsdi_apis.at(_setting_index),
                   &this->m_fsdi_actualPlatformInfo
               )
               != FSDI_True)
            {
                // failed...
                LogError("[Umbrella]"sv)
                    << "MotionPlatformManager::ReadDownstream(): Failed to get platform info with [ErrorCode:"sv
                    << ForceSeatDI_GetRecentErrorCode(this->m_fsdi_apis.at(_setting_index))
                    << "]"sv;

            } /* if() */

        } /* if() */

        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::ReadDownstream(): exit"sv;

        // if this point is reached, then success
        return true;

    } /* ReadDownstream() */

    auto
    MotionPlatformManager::ProcessDownstream() const
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::ProcessDownstream(): entry"sv;

        // log to file
        static auto const path
            = "umbrella_"s + std::format("{0:%T}", std::chrono::high_resolution_clock::now()) + ".csv"s;
        static auto output_file
            = std::ofstream{path.data(), std::ios_base::out | std::ios_base::app}
              << "timestamp,requestedPosition.sway,requestedPosition.heave,requestedPosition.surge,actualPosition.sway,actualPosition.heave,actualPosition.surge\n"sv;

        output_file
            << std::chrono::high_resolution_clock::now()
            << ","sv
            << this->m_fsdi_requestedPosition.sway
            << ","sv
            << this->m_fsdi_requestedPosition.heave
            << ","sv
            << this->m_fsdi_requestedPosition.surge
            << ","sv
            << this->m_fsdi_actualTopTablePosition.sway
            << ","sv
            << this->m_fsdi_actualTopTablePosition.heave
            << ","sv
            << this->m_fsdi_actualTopTablePosition.surge
            << "\n";

        // // NOTE: Displaying data is slow
        // for(uint32_t i = 0; i < NumberOfPlatforms; ++i)
        // {
        //     printf("Platform: %u, Pos: %u, %u, %u, %u, %u, %u\n", (i + 1), actualPositions[i].actualMotorPosition[0], actualPositions[i].actualMotorPosition[1], actualPositions[i].actualMotorPosition[2], actualPositions[i].actualMotorPosition[3], actualPositions[i].actualMotorPosition[4], actualPositions[i].actualMotorPosition[5]);
        //     printf("    Error code: %d.%d\n", static_cast< int >(platformInfos[i].moduleErrorCode), static_cast< int >(platformInfos[i].moduleErrorIndex));
        //     printf("    Top Frame: roll %f, pitch %f, yaw %f, heave %f, surge %f, sway %f\n", topTablePos[i].roll, topTablePos[i].pitch, topTablePos[i].yaw, topTablePos[i].heave, topTablePos[i].surge, topTablePos[i].sway);
        //     static_assert(FSDI_MotorsCount == 6, "");
        // }

        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::ProcessDownstream(): exit"sv;

        // if this point is reached, then success
        return true;

    } /* ReadDownstream() */

} // namespace Labsim::project::umbrella
