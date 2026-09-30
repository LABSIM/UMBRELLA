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

#ifndef INCLUDE_UMBRELLA_UMBRELLA_API_HPP_
#define INCLUDE_UMBRELLA_UMBRELLA_API_HPP_

//
// general include
//
#include <boost/config.hpp>
#include <boost/preprocessor/control/iif.hpp>
#include <boost/preprocessor/facilities/expand.hpp>
#include <boost/vmd/is_empty.hpp>

//
// high level convention
//

#if defined(BUILDING_UMBRELLA_API)
#
#  define UMBRELLA_API                 BOOST_SYMBOL_EXPORT
#  define UMBRELLA_AS_DEPENDENCIES_API BOOST_SYMBOL_IMPORT
#
#else
#
#  define UMBRELLA_API                 BOOST_SYMBOL_IMPORT
#  define UMBRELLA_AS_DEPENDENCIES_API BOOST_SYMBOL_IMPORT
#
#endif /* if (defined(BUILDING_UMBRELLA_API) */

//
// module convention
//

#define UMBRELLA_LIB_API                               \
    BOOST_PP_IIF(                                      \
        BOOST_VMD_IS_EMPTY(UMBRELLA_LIB_SOURCE),       \
        BOOST_PP_EXPAND(UMBRELLA_AS_DEPENDENCIES_API), \
        BOOST_PP_EXPAND(UMBRELLA_API)                  \
    )

// status code
#include <cstdint>

// avoid namespace pollution
namespace Labsim
{

    // fixed width integer litteral trick

    constexpr auto
    operator""_I8(
        unsigned long long value
    ) noexcept
        -> std::int8_t
    {
        return static_cast< std::int8_t >(value);
    }

    constexpr auto
    operator""_UI8(
        unsigned long long value
    ) noexcept
        -> std::uint8_t
    {
        return static_cast< std::uint8_t >(value);
    }

    constexpr auto
    operator""_I16(
        unsigned long long value
    ) noexcept
        -> std::int16_t
    {
        return static_cast< std::int16_t >(value);
    }

    constexpr auto
    operator""_UI16(
        unsigned long long value
    ) noexcept
        -> std::uint16_t
    {
        return static_cast< std::uint16_t >(value);
    }

    constexpr auto
    operator""_I32(
        unsigned long long value
    ) noexcept
        -> std::int32_t
    {
        return static_cast< std::int32_t >(value);
    }

    constexpr auto
    operator""_UI32(
        unsigned long long value
    ) noexcept
        -> std::uint32_t
    {
        return static_cast< std::uint32_t >(value);
    }

    constexpr auto
    operator""_I64(
        unsigned long long value
    ) noexcept
        -> std::int64_t
    {
        return static_cast< std::int64_t >(value);
    }

    constexpr auto
    operator""_UI64(
        unsigned long long value
    ) noexcept
        -> std::uint64_t
    {
        return static_cast< std::uint64_t >(value);
    }

    constexpr auto
    operator""_IF8(
        unsigned long long value
    ) noexcept
        -> std::int_fast8_t
    {
        return static_cast< std::int_fast8_t >(value);
    }

    constexpr auto
    operator""_UIF8(
        unsigned long long value
    ) noexcept
        -> std::uint_fast8_t
    {
        return static_cast< std::uint_fast8_t >(value);
    }

    constexpr auto
    operator""_IF16(
        unsigned long long value
    ) noexcept
        -> std::int_fast16_t
    {
        return static_cast< std::int_fast16_t >(value);
    }

    constexpr auto
    operator""_UIF16(
        unsigned long long value
    ) noexcept
        -> std::uint_fast16_t
    {
        return static_cast< std::uint_fast16_t >(value);
    }

    constexpr auto
    operator""_IF32(
        unsigned long long value
    ) noexcept
        -> std::int_fast32_t
    {
        return static_cast< std::int_fast32_t >(value);
    }

    constexpr auto
    operator""_UIF32(
        unsigned long long value
    ) noexcept
        -> std::uint_fast32_t
    {
        return static_cast< std::uint_fast32_t >(value);
    }

    constexpr auto
    operator""_IF64(
        unsigned long long value
    ) noexcept
        -> std::int_fast64_t
    {
        return static_cast< std::int_fast64_t >(value);
    }

    constexpr auto
    operator""_UIF64(
        unsigned long long value
    ) noexcept
        -> std::uint_fast64_t
    {
        return static_cast< std::uint_fast64_t >(value);
    }

    constexpr auto
    operator""_IL8(
        unsigned long long value
    ) noexcept
        -> std::int_least8_t
    {
        return static_cast< std::int_least8_t >(value);
    }

    constexpr auto
    operator""_UIL8(
        unsigned long long value
    ) noexcept
        -> std::uint_least8_t
    {
        return static_cast< std::uint_least8_t >(value);
    }

    constexpr auto
    operator""_IL16(
        unsigned long long value
    ) noexcept
        -> std::int_least16_t
    {
        return static_cast< std::int_least16_t >(value);
    }

    constexpr auto
    operator""_UIL16(
        unsigned long long value
    ) noexcept
        -> std::uint_least16_t
    {
        return static_cast< std::uint_least16_t >(value);
    }

    constexpr auto
    operator""_IL32(
        unsigned long long value
    ) noexcept
        -> std::int_least32_t
    {
        return static_cast< std::int_least32_t >(value);
    }

    constexpr auto
    operator""_UIL32(
        unsigned long long value
    ) noexcept
        -> std::uint_least32_t
    {
        return static_cast< std::uint_least32_t >(value);
    }

    constexpr auto
    operator""_IL64(
        unsigned long long value
    ) noexcept
        -> std::int_least64_t
    {
        return static_cast< std::int_least64_t >(value);
    }

    constexpr auto
    operator""_UIL64(
        unsigned long long value
    ) noexcept
        -> std::uint_least64_t
    {
        return static_cast< std::uint_least64_t >(value);
    }

    namespace project::umbrella
    {

        // NOLINTNEXTLINE(performance-enum-size)
        using status_code_type
            = enum class status_code
            : std::uint16_t
        {
            // ----------------------------------- //
            // status
            // ----------------------------------- //
            UMBRELLA_UNDEFINED_ = 0_UI16,
            UMBRELLA_OK_        = 1_UI16 << 0_UI16,
            UMBRELLA_EXIT_      = 1_UI16 << 1_UI16,
            UMBRELLA_ERROR_     = 1_UI16 << 2_UI16,
            // ----------------------------------- //
            // errors
            // ----------------------------------- //
            UMBRELLA_ERROR_UNKNOWN_          = 1_UI16 << 3_UI16,
            UMBRELLA_ERROR_ALLOCATE_         = 1_UI16 << 4_UI16,
            UMBRELLA_ERROR_CLONE_            = 1_UI16 << 5_UI16,
            UMBRELLA_ERROR_DEALLOCATE_       = 1_UI16 << 6_UI16,
            UMBRELLA_ERROR_EXCEPTION_RAISED_ = 1_UI16 << 7_UI16,
            UMBRELLA_FATAL_ERROR_            = 1_UI16 << 8_UI16,
            // ----------------------------------- //
            // emiter
            // ----------------------------------- //
            UMBRELLA_APP_    = 1_UI16 << 9_UI16,
            UMBRELLA_LIB_    = 1_UI16 << 10_UI16,
            FORCESEATDI_LIB_ = 1_UI16 << 11_UI16,
            // ----------------------------------- //
            // UMBRELLA APP CODE
            // ----------------------------------- //
            UMBRELLA_APP_OK_                     = UMBRELLA_APP_ | UMBRELLA_OK_,
            UMBRELLA_APP_EXIT_                   = UMBRELLA_APP_ | UMBRELLA_EXIT_,
            UMBRELLA_APP_ERROR_                  = UMBRELLA_APP_ | UMBRELLA_ERROR_,
            UMBRELLA_APP_ERROR_UNKNOWN_          = UMBRELLA_APP_ERROR_ | UMBRELLA_ERROR_UNKNOWN_,
            UMBRELLA_APP_ERROR_ALLOCATE_         = UMBRELLA_APP_ERROR_ | UMBRELLA_ERROR_ALLOCATE_,
            UMBRELLA_APP_ERROR_CLONE_            = UMBRELLA_APP_ERROR_ | UMBRELLA_ERROR_CLONE_,
            UMBRELLA_APP_ERROR_DEALLOCATE_       = UMBRELLA_APP_ERROR_ | UMBRELLA_ERROR_DEALLOCATE_,
            UMBRELLA_APP_ERROR_EXCEPTION_RAISED_ = UMBRELLA_APP_ERROR_ | UMBRELLA_ERROR_EXCEPTION_RAISED_,
            UMBRELLA_APP_FATAL_ERROR_            = UMBRELLA_APP_ERROR_ | UMBRELLA_FATAL_ERROR_,
            // ----------------------------------- //
            // UMBRELLA LIB CODE
            // ----------------------------------- //
            UMBRELLA_LIB_OK_                     = UMBRELLA_LIB_ | UMBRELLA_OK_,
            UMBRELLA_LIB_EXIT_                   = UMBRELLA_LIB_ | UMBRELLA_EXIT_,
            UMBRELLA_LIB_ERROR_                  = UMBRELLA_LIB_ | UMBRELLA_ERROR_,
            UMBRELLA_LIB_ERROR_UNKNOWN_          = UMBRELLA_LIB_ERROR_ | UMBRELLA_ERROR_UNKNOWN_,
            UMBRELLA_LIB_ERROR_ALLOCATE_         = UMBRELLA_LIB_ERROR_ | UMBRELLA_ERROR_ALLOCATE_,
            UMBRELLA_LIB_ERROR_CLONE_            = UMBRELLA_LIB_ERROR_ | UMBRELLA_ERROR_CLONE_,
            UMBRELLA_LIB_ERROR_DEALLOCATE_       = UMBRELLA_LIB_ERROR_ | UMBRELLA_ERROR_DEALLOCATE_,
            UMBRELLA_LIB_ERROR_EXCEPTION_RAISED_ = UMBRELLA_LIB_ERROR_ | UMBRELLA_ERROR_EXCEPTION_RAISED_,
            UMBRELLA_LIB_FATAL_ERROR_            = UMBRELLA_LIB_ERROR_ | UMBRELLA_FATAL_ERROR_,
            // ----------------------------------- //
            // FORCESEATDI LIB CODE
            // ----------------------------------- //
            FORCESEATDI_LIB_OK_                     = FORCESEATDI_LIB_ | UMBRELLA_OK_,
            FORCESEATDI_LIB_EXIT_                   = FORCESEATDI_LIB_ | UMBRELLA_EXIT_,
            FORCESEATDI_LIB_ERROR_                  = FORCESEATDI_LIB_ | UMBRELLA_ERROR_,
            FORCESEATDI_LIB_ERROR_UNKNOWN_          = FORCESEATDI_LIB_ERROR_ | UMBRELLA_ERROR_UNKNOWN_,
            FORCESEATDI_LIB_ERROR_ALLOCATE_         = FORCESEATDI_LIB_ERROR_ | UMBRELLA_ERROR_ALLOCATE_,
            FORCESEATDI_LIB_ERROR_CLONE_            = FORCESEATDI_LIB_ERROR_ | UMBRELLA_ERROR_CLONE_,
            FORCESEATDI_LIB_ERROR_DEALLOCATE_       = FORCESEATDI_LIB_ERROR_ | UMBRELLA_ERROR_DEALLOCATE_,
            FORCESEATDI_LIB_ERROR_EXCEPTION_RAISED_ = FORCESEATDI_LIB_ERROR_ | UMBRELLA_ERROR_EXCEPTION_RAISED_,
            FORCESEATDI_LIB_FATAL_ERROR_            = FORCESEATDI_LIB_ERROR_ | UMBRELLA_FATAL_ERROR_
        };

        // custom trick to hack the clang-tidy check [hicpp-signed-bitwise]

    } // namespace project::umbrella

} // namespace Labsim

#endif /* INCLUDE_UMBRELLA_UMBRELLA_API_HPP_ */
