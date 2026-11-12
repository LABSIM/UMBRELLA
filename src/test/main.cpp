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

#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/NetException.h>
#include <Poco/Net/SocketAddress.h>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <string>
#include <thread>

auto
main(
    [[maybe_unused]] int     argc,
    [[maybe_unused]] char ** pArgv
)
    -> int
{
    // la "valeur" des messsage ultra simple ;)
    enum class UmbrellaNetworkMessageType : std::uint8_t
    {
        UNDEFINED_     = 0,
        BEGIN_STIM_0_  = 1 << 0,
        BEGIN_STIM_1_  = 1 << 1,
        BEGIN_STIM_2_  = 1 << 2,
        STOP_ALL_STIM_ = 1 << 3,
        STOP_APP_      = 1 << 4
    };

    // the UDP Socket
    auto pUDPSocket = std::make_shared< Poco::Net::DatagramSocket >();
    pUDPSocket->bind(Poco::Net::SocketAddress("localhost", 12346));
    pUDPSocket->setBlocking(false);
    pUDPSocket->setKeepAlive(true);

    // the network message scenario
    auto const rg
        = {
            UmbrellaNetworkMessageType::BEGIN_STIM_0_,
            UmbrellaNetworkMessageType::STOP_ALL_STIM_,
            UmbrellaNetworkMessageType::BEGIN_STIM_1_,
            UmbrellaNetworkMessageType::STOP_ALL_STIM_,
            UmbrellaNetworkMessageType::BEGIN_STIM_2_,
            UmbrellaNetworkMessageType::STOP_ALL_STIM_,
            UmbrellaNetworkMessageType::BEGIN_STIM_0_,
            UmbrellaNetworkMessageType::STOP_ALL_STIM_,
            UmbrellaNetworkMessageType::BEGIN_STIM_1_,
            UmbrellaNetworkMessageType::STOP_ALL_STIM_,
            UmbrellaNetworkMessageType::BEGIN_STIM_2_,
            UmbrellaNetworkMessageType::STOP_ALL_STIM_,
            UmbrellaNetworkMessageType::STOP_APP_,
        };
    auto sequence = std::queue< UmbrellaNetworkMessageType >{rg.begin(), rg.end()};

    // loop !
    while(!sequence.empty())
    {
        // get the next element
        auto msg     = sequence.front();
        auto msg_str = std::to_string(static_cast< std::uint8_t >(msg));
        sequence.pop();

        // debug
        std::cout
            << "> will send:"
            << msg_str
            << " / remaining:"
            << sequence.size()
            << std::endl;

        // send
        try
        {
            pUDPSocket->sendTo(msg_str.data(), msg_str.size(), Poco::Net::SocketAddress("localhost", 12345));
        }
        catch(Poco::Net::NetException & ex)
        {
            // fcuk...
            std::cerr
                << "caught: "
                << ex.what()
                << " / "
                << ex.displayText()
                << " / "
                << ex.message()
                << std::endl;

        } /* try() */

        // petite pause
        std::this_thread::sleep_for(std::chrono::seconds(10));

    } /* while()*/

    // success !
    return EXIT_SUCCESS;

} /* main() */
