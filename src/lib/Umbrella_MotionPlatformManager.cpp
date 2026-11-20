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
#include <Umbrella/Umbrella_MotionPlatformManager.hpp>
#include <Umbrella/Umbrella_Stim0MotionPlatformProcessor.hpp>
#include <Umbrella/Umbrella_Stim1MotionPlatformProcessor.hpp>
#include <Umbrella/Umbrella_Stim2MotionPlatformProcessor.hpp>

#include <Ouranos/log/Ouranos_Logger_log.hpp>

#include <boost/exception/all.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <chrono>
#include <cstdint>
#include <cstring>
#include <dlfcn.h>
#include <format>
#include <fstream>
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

    } /* anonymous namespace */

    // NOLINTEND

    MotionPlatformManager::MotionPlatformManager(
        base_type::_token_type const & _token
    ) noexcept
        : base_type(_token)
        , m_pPlatform(std::make_shared< MotionPlatformInfo >())
        , m_pProcessor(std::make_shared< IdleMotionPlatformProcessor >())
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
        memset(&this->m_pPlatform->m_fsdi_requestedPosition, 0, sizeof(this->m_pPlatform->m_fsdi_requestedPosition));
        this->m_pPlatform->m_fsdi_requestedPosition.structSize          = sizeof(this->m_pPlatform->m_fsdi_requestedPosition);
        this->m_pPlatform->m_fsdi_requestedPosition.maxSpeed            = _s_default_maxSpeed;
        this->m_pPlatform->m_fsdi_requestedPosition.pause               = FSDI_False;
        this->m_pPlatform->m_fsdi_requestedPosition.strategy            = FSDI_Strategy_BestMatch;
        this->m_pPlatform->m_fsdi_requestedPosition.accelerationProfile = FSDI_AP_Rapid;

        // downstream
        // set sizes
        this->m_pPlatform->m_fsdi_actualPosition.structSize         = sizeof(FSDI_ActualActuatorsPositionLogical);
        this->m_pPlatform->m_fsdi_actualTopTablePosition.structSize = sizeof(FSDI_ActualTopTablePositionPhysical);
        this->m_pPlatform->m_fsdi_actualPlatformInfo.structSize     = sizeof(FSDI_PlatformInfo);

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
        static bool                      setting_is_debug;
        static uint32_t                  setting_index;
        static uint32_t                  setting_count;
        static std::string               setting_ip;
        static std::wstring              setting_sn;
        static std::chrono::milliseconds setting_idle_duration;

        { /* lock */

            auto handle = ApplicationManager::get();

            setting_is_debug      = handle->getOptions().isDebugSimulation();
            setting_index         = handle->getOptions().getFSDIPlatformIndex();
            setting_count         = handle->getOptions().getFSDIPlatformCount();
            setting_ip            = handle->getOptions().getFSDIPlatformIP();
            setting_sn            = handle->getOptions().getFSDIPlatformSN();
            setting_idle_duration = handle->getOptions().getIdleInitDuration();

        } /* release */

        // check if offline sim is true
        if(setting_is_debug)
        {
            // log
            LogInfo("[Umbrella]"sv)
                << "MotionPlatformManager::Connect(): offline simulation required, do not attempt to connect."sv;

            // if this point is reached, then its a skip !
            return true;

        } /* if() */

        // try ethernet first
        if(!connectNET(this->m_pPlatform->m_fsdi_apis, setting_index, setting_ip.data()))
        {
            // failsafe over USB ?
            if(!connectUSB(this->m_pPlatform->m_fsdi_apis, setting_index, setting_sn.data()))
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
        if(this->m_pPlatform->m_fsdi_apis.size() != setting_count)
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
        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::Process(): entry"sv;

        // call internal

        if(!this->m_pProcessor->Execute(*this->m_pPlatform, _elapsed_tick_count))
        {
            // log
            LogError("[Umbrella]"sv)
                << "MotionPlatformManager::Process(): failed to execute corresponding processor"sv;

            // fail...
            return false;

        } /* if() */

        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::Process(): exit"sv;

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
        for(FSDI_Handle api: this->m_pPlatform->m_fsdi_apis)
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

    void
    MotionPlatformManager::EnableStim0Processor()
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::EnableStim0Processor(): entry"sv;

        // allocate
        this->m_pProcessor = std::make_shared< Stim0MotionPlatformProcessor >();

        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::EnableStim0Processor(): exit"sv;

    } /* EnableStim0Processor() */

    void
    MotionPlatformManager::EnableStim1Processor()
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::EnableStim1Processor(): entry"sv;

        // allocate
        this->m_pProcessor = std::make_shared< Stim1MotionPlatformProcessor >();

        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::EnableStim1Processor(): exit"sv;

    } /* EnableStim1Processor() */

    void
    MotionPlatformManager::EnableStim2Processor()
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::EnableStim2Processor(): entry"sv;

        // allocate
        this->m_pProcessor = std::make_shared< Stim2MotionPlatformProcessor >();

        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::EnableStim2Processor(): exit"sv;

    } /* EnableStim2Processor() */

    void
    MotionPlatformManager::DisableAnyProcessor()
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::DisableAnyProcessor(): entry"sv;

        // allocate
        this->m_pProcessor = std::make_shared< IdleMotionPlatformProcessor >();

        // log
        LogDebug("[Umbrella]"sv)
            << "MotionPlatformManager::DisableAnyProcessor(): exit"sv;

    } /* EnableStim0Processor() */

} // namespace Labsim::project::umbrella
