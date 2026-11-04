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

#ifndef INCLUDE_UMBRELLA_UMBRELLA_IDLEMOTIONPLATFORMPROCESSOR_HPP_
#define INCLUDE_UMBRELLA_UMBRELLA_IDLEMOTIONPLATFORMPROCESSOR_HPP_

#include <Umbrella/Umbrella_AbstractMotionPlatformProcessor.hpp>

// avoid namespace pollution
namespace Labsim::project::umbrella
{

    class UMBRELLA_LIB_API IdleMotionPlatformProcessor final
        : public AbstractMotionPlatformProcessor
    {
    public:

        // using

        using base_type = AbstractMotionPlatformProcessor;
        using self_type = IdleMotionPlatformProcessor;

        // Coplien form - rule of 5

        constexpr IdleMotionPlatformProcessor() noexcept
            = default;

        IdleMotionPlatformProcessor(self_type const & _rhs) noexcept
            = default;

        IdleMotionPlatformProcessor(self_type && _rhs) noexcept
            = default;

        ~IdleMotionPlatformProcessor() override
            = default;

        auto
        operator=(self_type const & _rhs) noexcept
            -> self_type & = default;

        auto
        operator=(self_type && _rhs) noexcept
            -> self_type & = default;

    protected:

        // pure virtual protected impl

        bool
        doResetUpstream(MotionPlatformInfo & platform) override;

        bool
        doBuildUpstream(MotionPlatformInfo & platform, double const & _elapsed_tick_count) override;

        bool
        doSendUpstream(MotionPlatformInfo & platform) override;

        bool
        doReadDownstream(MotionPlatformInfo & platform) override;

        bool
        doProcessDownstream(MotionPlatformInfo & platform) const override;

    }; /* class IdleMotionPlatformProcessor */

} // namespace Labsim::project::umbrella

#endif /* INCLUDE_UMBRELLA_UMBRELLA_IDLEMOTIONPLATFORMPROCESSOR_HPP_ */
