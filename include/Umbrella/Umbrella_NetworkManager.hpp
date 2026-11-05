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

#ifndef INCLUDE_UMBRELLA_UMBRELLA_NETWORKMANAGER_HPP_
#define INCLUDE_UMBRELLA_UMBRELLA_NETWORKMANAGER_HPP_

#include <Umbrella/Umbrella_API.hpp>

#include <Ouranos/pattern/Ouranos_Singleton_pattern.hpp>

#include <atomic>
#include <memory>
#include <stack>

// avoid namespace pollution
namespace Labsim::project::umbrella
{

    // enums class

    enum class UmbrellaNetworkMessageType : std::uint8_t
    {
        UNDEFINED_     = 0_UI8,
        BEGIN_STIM_0_  = 1_UI8 << 0_UI8,
        BEGIN_STIM_1_  = 1_UI8 << 1_UI8,
        BEGIN_STIM_2_  = 1_UI8 << 2_UI8,
        STOP_ALL_STIM_ = 1_UI8 << 3_UI8,
        STOP_APP_      = 1_UI8 << 4_UI8
    };

    // global scope opt-in for bitfields-ops

    consteval void ouranos_enable_bitmask_ops(UmbrellaNetworkMessageType);

    class UMBRELLA_LIB_API NetworkManager final
        : public titans::ouranos::pattern::Singleton< NetworkManager >
    {
    public:

        // using

        using base_type = titans::ouranos::pattern::Singleton< NetworkManager >;
        using self_type = NetworkManager;

        // Singleton pattern validator

        explicit NetworkManager(base_type::_token_type const & _token) noexcept;

        // Coplien form - rule of 5

        constexpr NetworkManager() noexcept
            = delete;

        constexpr NetworkManager(self_type const & _rhs) noexcept
            = delete;

        constexpr NetworkManager(self_type && _rhs) noexcept
            = delete;

        ~NetworkManager() noexcept override
            = default;

        constexpr auto
        operator=(self_type const & _rhs) noexcept
            -> self_type & = delete;

        constexpr auto
        operator=(self_type && _rhs) noexcept
            -> self_type & = delete; // methods

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

        auto
        FlushPendingData()
            -> std::stack< UmbrellaNetworkMessageType >;

        constexpr auto
        HasPendingData() const
            -> bool
        {
            return this->m_bPendingData;
        };

    private:

        // members

        std::atomic< bool > m_bPendingData{false};

    }; /* class NetworkManager */

} // namespace Labsim::project::umbrella

#endif /* INCLUDE_UMBRELLA_UMBRELLA_NETWORKMANAGER_HPP_ */
