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

#include <Ouranos/pattern/Ouranos_Singleton_pattern.hpp>

#include <memory>

using namespace std::literals::string_view_literals;

// avoid namespace pollution
namespace Labsim::project::umbrella
{

    // forward decl.
    class AbstractMotionPlatformProcessor;
    struct MotionPlatformInfo;

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
        Process(double const & _elapsed_tick_count)
            -> bool;

        auto
        Cleanup()
            -> bool;

        void
        EnableStim0Processor();

        void
        EnableStim1Processor();

        void
        EnableStim2Processor();

        void
        DisableAnyProcessor();

    private:

        // the motion platform info

        std::shared_ptr< MotionPlatformInfo > m_pPlatform{nullptr};

        // the current platform processor

        std::shared_ptr< AbstractMotionPlatformProcessor > m_pProcessor{nullptr};

    }; /* class MotionPlatformManager */

} // namespace Labsim::project::umbrella

#endif /* INCLUDE_UMBRELLA_UMBRELLA_MOTIONPLATFORMMANAGER_HPP_ */
