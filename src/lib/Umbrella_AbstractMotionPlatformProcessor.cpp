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

#include <Umbrella/Umbrella_AbstractMotionPlatformProcessor.hpp>
#include <Umbrella/Umbrella_Common.hpp>

#include <Ouranos/log/Ouranos_Logger_log.hpp>

// avoid namespace pollution
namespace Labsim::project::umbrella
{

    // explicit
    AbstractMotionPlatformProcessor::~AbstractMotionPlatformProcessor()
        = default;

    auto
    AbstractMotionPlatformProcessor::Execute(
        MotionPlatformInfo & platform,
        double const &       _elapsed_tick_count
    )
        -> bool
    {
        // init iff. first pass
        if(!this->m_bInit)
        {
            if(!this->doInitFirstPass())
            {
                // log
                LogError("[Umbrella]"sv)
                    << "AbstractMotionPlatformProcessor::Execute(): failed to init processor"sv;

                // fail...
                return false;

            } /* if() */

        } /* if() */

        // call internal

        if(!this->doResetUpstream(platform))
        {
            // log
            LogError("[Umbrella]"sv)
                << "AbstractMotionPlatformProcessor::Execute(): failed to reset upstream"sv;

            // fail...
            return false;

        } /* if() */

        if(!this->doBuildUpstream(platform, _elapsed_tick_count))
        {
            // log
            LogError("[Umbrella]"sv)
                << "AbstractMotionPlatformProcessor::Execute(): failed to build upstream"sv;

            // fail...
            return false;

        } /* if() */

        if(!this->doSendUpstream(platform))
        {
            // log
            LogError("[Umbrella]"sv)
                << "AbstractMotionPlatformProcessor::Execute(): failed to send upstream"sv;

            // fail...
            return false;

        } /* if() */

        if(!this->doReadDownstream(platform))
        {
            // log
            LogError("[Umbrella]"sv)
                << "AbstractMotionPlatformProcessor::Process(): failed to read downstream"sv;

            // fail...
            return false;

        } /* if() */

        if(!this->doProcessDownstream(platform))
        {
            // log
            LogError("[Umbrella]"sv)
                << "AbstractMotionPlatformProcessor::Execute(): failed to process downstream"sv;

            // fail...
            return false;

        } /* if() */

        // mark as init == (first pass ok)
        this->m_bInit = true;

        // finally, if reached then it's a success
        return true;

    } /* Execute() */

    auto
    AbstractMotionPlatformProcessor::HasInitialized() const
        -> bool
    {
        // simply
        return this->m_bInit;

    } /* HasInitialized() */

} // namespace Labsim::project::umbrella
