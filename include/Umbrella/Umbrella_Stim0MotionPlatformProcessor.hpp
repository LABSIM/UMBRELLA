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

#ifndef INCLUDE_UMBRELLA_UMBRELLA_STIM0MOTIONPLATFORMPROCESSOR_HPP_
#define INCLUDE_UMBRELLA_UMBRELLA_STIM0MOTIONPLATFORMPROCESSOR_HPP_

#include <Umbrella/Umbrella_AbstractMotionPlatformProcessor.hpp>

#include <fstream>

// avoid namespace pollution
namespace Labsim::project::umbrella
{

    class UMBRELLA_LIB_API Stim0MotionPlatformProcessor final
        : public AbstractMotionPlatformProcessor
    {
    public:

        // using

        using base_type = AbstractMotionPlatformProcessor;
        using self_type = Stim0MotionPlatformProcessor;

        // Coplien form - rule of 5

        Stim0MotionPlatformProcessor() noexcept
            = default;

        Stim0MotionPlatformProcessor(self_type const & _rhs) noexcept
            = default;

        Stim0MotionPlatformProcessor(self_type && _rhs) noexcept
            = default;

        ~Stim0MotionPlatformProcessor() override
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
        doInitFirstPass() override;

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

    private:

        // local members

        bool     m_setting_is_debug{false};
        uint32_t m_setting_index{0};
        std::ofstream m_output_file;

    }; /* class Stim0MotionPlatformProcessor */

} // namespace Labsim::project::umbrella

#endif /* INCLUDE_UMBRELLA_UMBRELLA_STIM0MOTIONPLATFORMPROCESSOR_HPP_ */
