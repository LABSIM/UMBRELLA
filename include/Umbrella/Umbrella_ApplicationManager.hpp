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

#ifndef INCLUDE_UMBRELLA_UMBRELLA_APPLICATIONMANAGER_HPP_
#define INCLUDE_UMBRELLA_UMBRELLA_APPLICATIONMANAGER_HPP_

#include <Umbrella/Umbrella_API.hpp>
#include <Umbrella/Umbrella_CommandLineOptions.hpp>
#include <Umbrella/Umbrella_Common.hpp>

#include <Ouranos/pattern/Ouranos_Singleton_pattern.hpp>

#include <atomic>
#include <vector>

// avoid namespace pollution
namespace Labsim::project::umbrella
{

    class UMBRELLA_LIB_API ApplicationManager final
        : public titans::ouranos::pattern::Singleton< ApplicationManager >
    {
    public:

        // using

        using base_type = titans::ouranos::pattern::Singleton< ApplicationManager >;
        using self_type = ApplicationManager;

        // Singleton pattern validator

        explicit ApplicationManager(base_type::_token_type const & _token) noexcept;

        // Coplien form - rule of 5

        constexpr ApplicationManager() noexcept
            = delete;

        constexpr ApplicationManager(self_type const & _rhs) noexcept
            = delete;

        constexpr ApplicationManager(self_type && _rhs) noexcept
            = delete;

        ~ApplicationManager() noexcept override
            = default;

        constexpr auto
        operator=(self_type const & _rhs) noexcept
            -> self_type & = delete;

        constexpr auto
        operator=(self_type && _rhs) noexcept
            -> self_type & = delete; // methods

        auto
        Initialize(int const & _argc, char ** pArgv)
            -> status_code_type;

        auto
        Run() const
            -> status_code_type;

        static auto
        Close()
            -> status_code_type;

        // public getter

        [[nodiscard]] constexpr auto
        getOptions() const
            -> CommandLineOptions const &
        {
            return this->m_options;
        };

    private:

        // accessibility

        friend class CommandLineOptions;

        // private getter

        [[nodiscard]] constexpr auto
        getArgc() const
            -> std::size_t
        {
            return this->m_argc;
        };

        [[nodiscard]] constexpr auto
        getArgv() const
            -> std::vector< std::string > const &
        {
            return this->m_argv;
        };

        // closure

        [[nodiscard]] constexpr auto
        HasGeneralClosureBeenRequested() const
            -> bool
        {
            return this->m_bEnd;
        }

        constexpr void
        MarkGeneralClosureAsRequested()
        {
            this->m_bEnd = true;
        }

        // members

        std::atomic< bool >        m_bEnd{false};
        std::size_t                m_argc{0UZ};
        std::vector< std::string > m_argv;
        CommandLineOptions         m_options;

    }; /* class ApplicationManager */

} // namespace Labsim::project::umbrella

#endif /* INCLUDE_UMBRELLA_UMBRELLA_APPLICATIONMANAGER_HPP_ */
