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

#ifndef INCLUDE_UMBRELLA_UMBRELLA_COMMON_HPP_
#define INCLUDE_UMBRELLA_UMBRELLA_COMMON_HPP_

#include <Umbrella/Umbrella_API.hpp>

#include <Ouranos/Ouranos_Common.hpp>

// avoid namespace pollution
namespace Labsim::project::umbrella
{

    // enums class

    enum class UmbrellaStimAxisType : std::uint8_t
    {
        UNDEFINED_ = 0_UI8,
        X_         = 1_UI8 << 0_UI8,
        Y_         = 1_UI8 << 1_UI8,
        Z_         = 1_UI8 << 2_UI8,
        RX_        = 1_UI8 << 3_UI8,
        RY_        = 1_UI8 << 4_UI8,
        RZ_        = 1_UI8 << 5_UI8,
        HEAVE_     = Y_,
        SWAY_      = X_,
        SURGE_     = Z_,
        PITCH_     = RX_,
        ROLL_      = RY_,
        YAW_       = RZ_
    };

    // global scope opt-in for bitfields-ops

    consteval void ouranos_enable_bitmask_ops(UmbrellaStimAxisType);

} // namespace Labsim::project::umbrella

#endif /* INCLUDE_UMBRELLA_UMBRELLA_HPP_ */
