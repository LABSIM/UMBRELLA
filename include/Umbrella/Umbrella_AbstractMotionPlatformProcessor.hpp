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

#ifndef INCLUDE_UMBRELLA_UMBRELLA_ABSTRACTMOTIONPLATFORMPROCESSOR_HPP_
#define INCLUDE_UMBRELLA_UMBRELLA_ABSTRACTMOTIONPLATFORMPROCESSOR_HPP_

#include <Umbrella/Umbrella_API.hpp>

#include <Ouranos/pattern/Ouranos_Abstract_pattern.hpp>

// avoid namespace pollution
namespace Labsim::project::umbrella
{

    // forward decl.
    struct MotionPlatformInfo;

    class UMBRELLA_LIB_API AbstractMotionPlatformProcessor
        : public titans::ouranos::pattern::Abstract
    {
    public:

        // using

        using base_type = titans::ouranos::pattern::Abstract;
        using self_type = AbstractMotionPlatformProcessor;

        // Coplien form - rule of 5

        constexpr AbstractMotionPlatformProcessor() noexcept
            = default;

        AbstractMotionPlatformProcessor(self_type const & _rhs) noexcept
            = default;

        AbstractMotionPlatformProcessor(self_type && _rhs) noexcept
            = default;

        ~AbstractMotionPlatformProcessor() override
            = 0;

        auto
        operator=(self_type const & _rhs) noexcept
            -> self_type & = default;

        auto
        operator=(self_type && _rhs) noexcept
            -> self_type & = default;

        //  public facade

        auto
        Execute(MotionPlatformInfo & platform, double const & _elapsed_tick_count)
            -> bool;

        auto
        HasInitialized() const
            -> bool;

    protected:

        // pure virtual protected impl

        virtual auto
        doInitFirstPass()
            -> bool
            = 0;

        virtual auto
        doResetUpstream(MotionPlatformInfo & platform)
            -> bool
            = 0;

        virtual auto
        doBuildUpstream(MotionPlatformInfo & platform, double const & _elapsed_tick_count)
            -> bool
            = 0;

        virtual auto
        doSendUpstream(MotionPlatformInfo & platform)
            -> bool
            = 0;

        virtual auto
        doReadDownstream(MotionPlatformInfo & platform)
            -> bool
            = 0;

        virtual auto
        doProcessDownstream(MotionPlatformInfo & platform) const
            -> bool
            = 0;

    private:

        // members

        bool m_bInit{false};

    }; /* class AbstractMotionPlatformProcessor */

} // namespace Labsim::project::umbrella

#endif /* INCLUDE_UMBRELLA_UMBRELLA_ABSTRACTMOTIONPLATFORMPROCESSOR_HPP_ */
