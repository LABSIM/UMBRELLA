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

#include <Umbrella/Umbrella_NetworkManager.hpp>

#include <Ouranos/log/Ouranos_Logger_log.hpp>

#include <boost/exception/all.hpp>

#include <exception>
#include <string_view>

using namespace std::literals::string_literals;
using namespace std::literals::string_view_literals;

// avoid namespace pollution
namespace Labsim::project::umbrella
{

    NetworkManager::NetworkManager(
        base_type::_token_type const & _token
    ) noexcept
        : base_type(_token)
    {
    }

    auto
    NetworkManager::Configure()
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "NetworkManager::Configure(): entry"sv;

        // log
        LogDebug("[Umbrella]"sv)
            << "NetworkManager::Configure(): exit"sv;

        // success
        return true;

    } /* Configure() */

    auto
    NetworkManager::Connect()
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "NetworkManager::Connect(): entry"sv;

        // log
        LogDebug("[Umbrella]"sv)
            << "NetworkManager::Connect(): exit"sv;

        // if this point is reached, then its a success
        return true;

    } /* Connect() */

    auto
    NetworkManager::Process()
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "NetworkManager::Process(): entry"sv;

        // log
        LogDebug("[Umbrella]"sv)
            << "NetworkManager::Process(): exit"sv;

        // if reached then it's a success
        return true;

    } /* Process() */

    auto
    NetworkManager::Cleanup()
        -> bool
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "NetworkManager::Cleanup(): entry"sv;

        // log
        LogDebug("[Umbrella]"sv)
            << "NetworkManager::Cleanup(): exit"sv;

        // if reached then it's a success
        return true;

    } /* Cleanup() */

    auto
    NetworkManager::FlushPendingData()
        -> std::stack< UmbrellaNetworkMessageType >
    {
        // log
        LogDebug("[Umbrella]"sv)
            << "NetworkManager::FlushPendingData(): entry"sv;

        // log
        LogDebug("[Umbrella]"sv)
            << "NetworkManager::FlushPendingData(): exit"sv;

        return std::stack< UmbrellaNetworkMessageType >{};

    } /* FlushPendingData() */

} // namespace Labsim::project::umbrella
