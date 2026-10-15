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

#ifndef INCLUDE_UMBRELLA_UMBRELLA_MOTIONPLATFORMMANAGER_HPP_
#define INCLUDE_UMBRELLA_UMBRELLA_MOTIONPLATFORMMANAGER_HPP_

#include <Umbrella/Umbrella_API.hpp>
#include <Umbrella/Umbrella_Common.hpp>

#include <Ouranos/pattern/Ouranos_Singleton_pattern.hpp>

#include <string_view>
#include <vector>

#include "ForceSeatDI.h"

using namespace std::literals::string_view_literals;

// avoid namespace pollution
namespace Labsim::project::umbrella
{

    class UMBRELLA_LIB_API MotionPlatformManager final
        : public titans::ouranos::pattern::Singleton< MotionPlatformManager >
    {
    public:

        // using

        using base_type = titans::ouranos::pattern::Singleton< MotionPlatformManager >;
        using self_type = MotionPlatformManager;

        // Singleton pattern validator

        explicit MotionPlatformManager(base_type::_token_type const & _token) noexcept;

        // Coplien form - rule of 5

        constexpr MotionPlatformManager() noexcept
            = delete;

        constexpr MotionPlatformManager(self_type const & _rhs) noexcept
            = delete;

        constexpr MotionPlatformManager(self_type && _rhs) noexcept
            = delete;

        ~MotionPlatformManager() noexcept override
            = default;

        constexpr auto
        operator=(self_type const & _rhs) noexcept
            -> self_type & = delete;

        constexpr auto
        operator=(self_type && _rhs) noexcept
            -> self_type & = delete;

        // public method

        auto
        Configure()
            -> bool;

        auto
        Connect()
            -> bool;

        auto
        Process()
            -> bool;

        auto
        Cleanup()
            -> bool;

    private:

        // the ForceSetDI motion platform API members

        std::vector< FSDI_Handle >          m_fsdi_apis{};
        FSDI_TopTablePositionPhysical       m_fsdi_requestedPosition{};
        FSDI_ActualActuatorsPositionLogical m_fsdi_actualPosition{};
        FSDI_ActualTopTablePositionPhysical m_fsdi_actualTopTablePosition{};
        FSDI_PlatformInfo                   m_fsdi_actualPlatformInfo{};

    }; /* class MotionPlatformManager */

} // namespace Labsim::project::umbrella

#endif /* INCLUDE_UMBRELLA_UMBRELLA_MOTIONPLATFORMMANAGER_HPP_ */
