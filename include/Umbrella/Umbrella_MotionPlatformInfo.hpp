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

#ifndef INCLUDE_UMBRELLA_UMBRELLA_MOTIONPLATFORMINFO_HPP_
#define INCLUDE_UMBRELLA_UMBRELLA_MOTIONPLATFORMINFO_HPP_

#include <Umbrella/Umbrella_API.hpp>
#include <Umbrella/Umbrella_Common.hpp>

#include <vector>

#include "ForceSeatDI.h"

// avoid namespace pollution
namespace Labsim::project::umbrella
{

    struct UMBRELLA_LIB_API MotionPlatformInfo final
    {
    public:

        // using

        using self_type = MotionPlatformInfo;

        // Coplien form - rule of 5

        constexpr MotionPlatformInfo() noexcept
            = default;

        constexpr MotionPlatformInfo(self_type const & _rhs) noexcept
            = default;

        constexpr MotionPlatformInfo(self_type && _rhs)
            = default;

        ~MotionPlatformInfo() noexcept
            = default;

        constexpr auto
        operator=(self_type const & _rhs) noexcept
            -> self_type & = default;

        constexpr auto
        operator=(self_type && _rhs) noexcept
            -> self_type & = default;

        // all public members

        std::vector< FSDI_Handle >          m_fsdi_apis{};
        FSDI_TopTablePositionPhysical       m_fsdi_requestedPosition{};
        FSDI_ActualActuatorsPositionLogical m_fsdi_actualPosition{};
        FSDI_ActualTopTablePositionPhysical m_fsdi_actualTopTablePosition{};
        FSDI_PlatformInfo                   m_fsdi_actualPlatformInfo{};

    }; /* class MotionPlatformInfo */

} // namespace Labsim::project::umbrella

#endif /* INCLUDE_UMBRELLA_UMBRELLA_MOTIONPLATFORMINFO_HPP_ */
