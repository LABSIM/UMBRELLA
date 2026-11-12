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

#include <Ouranos/Ouranos_Common.hpp>
#include <Ouranos/log/Ouranos_Logger_log.hpp>

#include <boost/exception/all.hpp>

#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/SocketAddress.h>

#include <charconv>
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

        // instantiate the socket
        this->m_pUDPSocket = std::make_shared< Poco::Net::DatagramSocket >();

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

        // constants
        static constexpr auto _s_default_addr = "127.0.0.1"s;
        static constexpr auto _s_default_port = 12345_UI16;

        // connect to the endpoint
        this->m_pUDPSocket->bind(Poco::Net::SocketAddress(_s_default_addr, _s_default_port));
        // this->m_pUDPSocket->setReceiveTimeout(Poco::Timespan(std::chrono::microseconds(100)));
        this->m_pUDPSocket->setBlocking(false);
        this->m_pUDPSocket->setKeepAlive(true);

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

        // constants
        static constexpr auto _s_default_capacity = std::size_t{1024};

        // read
        auto buffer  = Poco::Buffer< char >(_s_default_capacity);
        auto recv_sz = this->m_pUDPSocket->receiveBytes(buffer, 0, 0);

        // check
        if(recv_sz != 0)
        {
            // log
            LogDebug("[Umbrella]"sv)
                << "NetworkManager::Process(): Has fresh data ! ["sv
                << recv_sz
                << "] bytes to buffer with size/cap "sv
                << buffer.size()
                << "/"sv
                << buffer.capacity();

            // extract
            std::uint8_t value{0};
            std::from_chars(buffer.begin(), buffer.end(), value);
            this->m_networkMsg.push(titans::ouranos::to_enum< UmbrellaNetworkMessageType >(value));

            // mark
            this->m_bPendingData = true;

        } /* if() */

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

        this->m_pUDPSocket->close();
        this->m_pUDPSocket.reset();

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

        this->m_bPendingData = false;

        // log
        LogDebug("[Umbrella]"sv)
            << "NetworkManager::FlushPendingData(): exit"sv;

        return {this->m_networkMsg};

    } /* FlushPendingData() */

} // namespace Labsim::project::umbrella
