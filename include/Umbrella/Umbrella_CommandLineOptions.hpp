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

#ifndef INCLUDE_UMBRELLA_UMBRELLA_COMMANDLINEOPTIONS_HPP_
#define INCLUDE_UMBRELLA_UMBRELLA_COMMANDLINEOPTIONS_HPP_

#include <Umbrella/Umbrella_API.hpp>
#include <Umbrella/Umbrella_Common.hpp>

#include <Ouranos/Ouranos_Common.hpp>

#include <chrono>
#include <istream>
#include <memory>
#include <string>
#include <string_view>

using namespace std::literals::string_view_literals;
using namespace std::literals::chrono_literals;

// avoid namespace pollution
namespace Labsim::project::umbrella
{

    // enums class

    enum class UMBRELLA_LIB_API CLIOptionsType : std::uint8_t
    {
        CLI_OPTIONS_UNDEFINED_ = 0_UI8,
        CLI_OPTIONS_HELP_      = 1_UI8 << 0_UI8,
        CLI_OPTIONS_OK_        = 1_UI8 << 1_UI8,
        CLI_OPTIONS_ERROR_     = 1_UI8 << 2_UI8,
    };

    // global scope opt-in for bitfields-ops
    consteval void ouranos_enable_bitmask_ops(CLIOptionsType);

    class UMBRELLA_LIB_API CommandLineOptions final
    {
    public:

        // variable timings

        static constexpr auto const _s_default_idle_init_duration_ms = 500ms;
        static constexpr auto const _s_default_target_loop_period_ms = 10ms;

        // ForceSeatDI

        static constexpr auto const _s_default_fsdi_platform_index = 0_UI32;
        static constexpr auto const _s_default_fsdi_platform_count = 1_UI32;
        static constexpr auto const _s_default_fsdi_platform_ip    = "193.0.208.12"sv;
        static constexpr auto const _s_default_fsdi_platform_sn    = L"250031-000457-315839-323120"sv;

        // Stimulation default invariant

        enum stim_mode
        {
            S0_ = 0,
            S1_,
            S2_,
            IDLE_
        };

        friend auto
        operator>>(
            std::istream & input,
            stim_mode &    mode
        ) -> std::istream &;

        static constexpr auto const _s_default_stim_mode = stim_mode::S1_;

        //
        // case 1
        // ---
        // Vol en turbulences modérées 0.2 Hz 20.0 mm
        //
        static constexpr auto const _s_default_stim1_frequency_hz = 0.2;
        static constexpr auto const _s_default_stim1_amplitude_mm = 20.0;

        //
        // case 2
        // ---
        // Balourd rotor principal modéré 5.0 Hz 0.8 mm
        //
        static constexpr auto const _s_default_stim2_frequency_hz = 5.0;
        static constexpr auto const _s_default_stim2_amplitude_mm = 0.8;

        //
        // phase offset
        // ---
        // X axis : no offset / reference
        // Y axis : has offset
        // Z axis : has offset
        //
        static constexpr auto const _s_default_y_axis_phase_offset_s = 5.0;
        static constexpr auto const _s_default_z_axis_phase_offset_s = 7.0;

        // using enum

        using enum CLIOptionsType;

        // using

        using self_type = CommandLineOptions;

        // Coplien form - rule of 5

        constexpr CommandLineOptions()
            = default;

        constexpr CommandLineOptions(self_type const & _rhs) noexcept
            = default;

        constexpr CommandLineOptions(self_type && _rhs) noexcept
            = default;

        ~CommandLineOptions() noexcept
            = default;

        constexpr auto
        operator=(self_type const & _rhs) noexcept
            -> self_type & = default;

        constexpr auto
        operator=(self_type && _rhs) noexcept
            -> self_type & = default;

        // method

        auto
        Parse()
            -> CLIOptionsType;

        // getter

        [[nodiscard]] constexpr auto
        isDebugSimulation() const
            -> decltype(auto)
        {
            return this->m_bDebugSimulation;
        };

        [[nodiscard]] constexpr auto
        getFSDIPlatformIndex() const
            -> decltype(auto)
        {
            return this->m_FSDIPlatformIndex;
        };

        [[nodiscard]] constexpr auto
        getFSDIPlatformCount() const
            -> decltype(auto)
        {
            return this->m_FSDIPlatformCount;
        };

        [[nodiscard]] constexpr auto
        getFSDIPlatformIP() const
            -> decltype(auto)
        {
            return this->m_FSDIPlatformIP;
        };

        [[nodiscard]] constexpr auto
        getFSDIPlatformSN() const
            -> decltype(auto)
        {
            return this->m_FSDIPlatformSN;
        };

        [[nodiscard]] constexpr auto
        getIdleInitDuration() const
            -> decltype(auto)
        {
            return this->m_idleInitDuration;
        };

        [[nodiscard]] constexpr auto
        getTargetLoopPeriod() const
            -> decltype(auto)
        {
            return this->m_targetLoopPeriod;
        };

        [[nodiscard]] constexpr auto
        getStimMode() const
            -> decltype(auto)
        {
            return this->m_stimMode;
        };

        [[nodiscard]] constexpr auto
        getStim1Frequency() const
            -> decltype(auto)
        {
            return this->m_stim1Frequency;
        };

        [[nodiscard]] constexpr auto
        getStim1Amplitude() const
            -> decltype(auto)
        {
            return this->m_stim1Amplitude;
        };

        [[nodiscard]] constexpr auto
        getStim2Frequency() const
            -> decltype(auto)
        {
            return this->m_stim2Frequency;
        };

        [[nodiscard]] constexpr auto
        getStim2Amplitude() const
            -> decltype(auto)
        {
            return this->m_stim2Amplitude;
        };

        [[nodiscard]] constexpr auto
        getYAxisPhaseOffset() const
            -> decltype(auto)
        {
            return this->m_YAxisPhaseOffset;
        };

        [[nodiscard]] constexpr auto
        getZAxisPhaseOffset() const
            -> decltype(auto)
        {
            return this->m_ZAxisPhaseOffset;
        };

    private:

        // members

        bool                      m_bDebugSimulation{};
        uint32_t                  m_FSDIPlatformIndex{};
        uint32_t                  m_FSDIPlatformCount{};
        std::string               m_FSDIPlatformIP;
        std::wstring              m_FSDIPlatformSN;
        std::chrono::milliseconds m_idleInitDuration{};
        std::chrono::milliseconds m_targetLoopPeriod{};
        stim_mode                 m_stimMode{};
        double                    m_stim1Frequency{};
        double                    m_stim1Amplitude{};
        double                    m_stim2Frequency{};
        double                    m_stim2Amplitude{};
        double                    m_YAxisPhaseOffset{};
        double                    m_ZAxisPhaseOffset{};

    }; /* class CommandLineOptions */

} // namespace Labsim::project::umbrella

#endif /* INCLUDE_UMBRELLA_UMBRELLA_COMMANDLINEOPTIONS_HPP_ */
