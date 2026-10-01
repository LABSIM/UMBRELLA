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

#ifndef INCLUDE_UMBRELLA_UMBRELLA_COMMON_HPP_
#define INCLUDE_UMBRELLA_UMBRELLA_COMMON_HPP_

#include <Umbrella/Umbrella_API.hpp>

#include <Ouranos/Ouranos_Common.hpp>

#include <map>
#include <random>
#include <ranges>
#include <string>
#include <string_view>

// avoid namespace pollution
namespace Labsim::project::umbrella
{

    // all credit goes to [ https://codereview.stackexchange.com/a/109266 ]

    template<
        typename T    = std::mt19937,
        std::size_t N = T::state_size * sizeof(typename T::result_type) >
    static constexpr auto
    ProperlySeededRandomEngine()
        -> T
    {
        std::random_device source;

        auto random_data
            = std::views::iota(std::size_t(), ((N - 1) / sizeof(source())) + 1)
              | std::views::transform([&](auto)
                                      {
                                          return source();
                                      });

        std::seed_seq seeds(std::begin(random_data), std::end(random_data));

        return T(seeds);

    }; /* ProperlySeededRandomEngine() */

    // enums class

    class UMBRELLA_LIB_API Common
    {
    public:

        // using

        using self_type           = Common;
        using uuid_generator_type = decltype(ProperlySeededRandomEngine());
        using uuid_type           = decltype(ProperlySeededRandomEngine()());

        // methods

        static constexpr auto
        GenerateUUID()
            -> uuid_type;

    private:

        // static const members

        static uuid_generator_type s_uuid_generator;

    }; /* struct Common */

    // impl

    constexpr auto
    Common::GenerateUUID()
        -> uuid_type
    {
        return self_type::s_uuid_generator();
    }

    // enums class

    enum class UmbrellaStimAxisType : std::uint8_t
    {
        UNDEFINED_ = 0_UI8,
        X_         = 1_UI8 << 0_UI8,
        Y_         = 1_UI8 << 1_UI8,
        Z_         = 1_UI8 << 2_UI8,
        HEAVE_     = Y_,
        SWAY_      = X_,
        SURGE_     = Z_
    };

    // global scope opt-in for bitfields-ops

    consteval void ouranos_enable_bitmask_ops(UmbrellaStimAxisType);

} // namespace Labsim::project::umbrella

#endif /* INCLUDE_UMBRELLA_UMBRELLA_HPP_ */
