//
// SCHEME-GATEWAY : the SCHEME's simulator real-time interaction network gateway
// Copyright (C) 2012-present Nawfel KINANI
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

#include <Umbrella/Umbrella_CommandLineOptions.hpp>
#include <Umbrella/Umbrella_ApplicationManager.hpp>

#include <Ouranos/log/Ouranos_Logger_log.hpp>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>

#include <ranges>
#include <codecvt>

using namespace std::literals::string_literals;

// avoid namespace pollution
namespace Labsim::project::umbrella
{

    auto
    CommandLineOptions::Parse()
        -> CLIOptionsType
    {
        // variables
        bool bVersionFlag = false;
        bool bHelpFlag    = false;
        auto status       = CLI_OPTIONS_UNDEFINED_;

        // encapsulate
        try
        {
            Labsim::LogDebug("[Umbrella]"sv)
                << "CommandLineOptions::Parse() : Parsing command line"sv;

            // shortcut
            namespace po = boost::program_options;

            // vars
            using ms_type = decltype(self_type::_s_default_idle_init_duration_ms.count());
            ms_type idle_init_duration_buf{};
            ms_type target_loop_period_buf{};

            // Declare groups of options that will be used
            auto cmdline_options
                = po::options_description(
                    "Available Command-Line Interface options for <Scheme-gateway-app> are"s
                );

            // clang-format off
            cmdline_options.add_options()
                (
                    "version,v",
                    po::bool_switch(&bVersionFlag),
                    "print version string"
                )
                (
                    "help,h",
                    po::bool_switch(&bHelpFlag),
                    "produce help message"
                )
                (
                    "init-duration",
                    po::value< ms_type >(&idle_init_duration_buf)->default_value(self_type::_s_default_idle_init_duration_ms.count()),
                    "The idle init duration in ms"
                )
                (
                    "loop-period",
                    po::value< ms_type >(&target_loop_period_buf)->default_value(self_type::_s_default_target_loop_period_ms.count()),
                    "The main loop target period in ms"
                )
                (
                    "platform-index",
                    po::value< std::uint32_t >(&this->m_FSDIPlatformIndex)->default_value(self_type::_s_default_fsdi_platform_index),
                    "The MotionSystems platform requested ID"
                )
                (
                    "platform-count",
                    po::value< std::uint32_t >(&this->m_FSDIPlatformCount)->default_value(self_type::_s_default_fsdi_platform_count),
                    "The connected MotionSystems platform count"
                )
                (
                    "platform-ip",
                    po::value< std::string >(&this->m_FSDIPlatformIP)->default_value(std::string{self_type::_s_default_fsdi_platform_ip}),
                    "The MotionSystems platform ipv4 for primary ethernet connection"
                )
                (
                    "patform-sn",
                    po::value< std::wstring >(&this->m_FSDIPlatformSN)->default_value(std::wstring{self_type::_s_default_fsdi_platform_sn}),
                    "the MotionsSystems platform Serial Number for secondary USB failsafe"
                )
                (
                    "s1-frequency",
                    po::value< double >(&this->m_S1Frequency)->default_value(self_type::_s_default_S1_frequency_hz),
                    "The #1 simulation frequency in Hertz"
                )
                (
                    "s1-amplitude",
                    po::value< double >(&this->m_S1Amplitude)->default_value(self_type::_s_default_S1_amplitude_mm),
                    "The #1 simulation amplitude in Millimeter"
                )
                (
                    "s2-frequency",
                    po::value< double >(&this->m_S2Frequency)->default_value(self_type::_s_default_S2_frequency_hz),
                    "The #2 simulation frequency in Hertz"
                )
                (
                    "s2-amplitude",
                    po::value< double >(&this->m_S2Amplitude)->default_value(self_type::_s_default_S2_amplitude_mm),
                    "The #2 simulation amplitude in Millimeter"
                )
                (
                    "x-offset",
                    po::value< double >(&this->m_XAxisPhaseOffset)->default_value(self_type::_s_default_x_axis_phase_offset_s),
                    "The platform X axis simulation phase offset in Second"
                )
                (
                    "y-offset",
                    po::value< double >(&this->m_YAxisPhaseOffset)->default_value(self_type::_s_default_y_axis_phase_offset_s),
                    "The platform Y axis simulation phase offset in Second"
                )
                (
                    "z-offset",
                    po::value< double >(&this->m_ZAxisPhaseOffset)->default_value(self_type::_s_default_z_axis_phase_offset_s),
                    "The platform Z axis simulation phase offset in Second"
                );
            // clang-format on

            // parse command line argument
            po::variables_map vm_options;

            // parse on raw argv by using internal pointer. Do not try to delete...
            auto                  argv = ApplicationManager::get()->getArgv();
            std::vector< char * > raw_argv;

            // remember the nullptr terminator
            raw_argv.reserve(argv.size() + 1);
            std::ranges::transform(
                argv,
                std::back_inserter(raw_argv),
                [](auto & arg_str)
                    -> auto
                {
                    return arg_str.data();
                }
            );
            // raw_argv.push_back(nullptr);

            // then parse command line, prioritary so it should override values
            po::store(
                po::parse_command_line(
                    static_cast< int >(ApplicationManager::get()->getArgc()),
                    raw_argv.data(),
                    cmdline_options
                ),
                vm_options
            );

            // call every callback
            po::notify(vm_options);

            // check help argument
            if(bHelpFlag)
            {
                // print
                Labsim::LogInfo("[Umbrella]"sv)
                    << "\n"
                    << cmdline_options;

                status = CLI_OPTIONS_HELP_;

                // exit
                return status;

            } /* if() */

            // inject tmp vars into member
            this->m_IdleInitDuration = std::chrono::milliseconds(idle_init_duration_buf);
            this->m_TargetLoopPeriod = std::chrono::milliseconds(target_loop_period_buf);

            // log result
            Labsim::LogDebug("[Umbrella]"sv)
                << "CommandLineOptions::Parse() : Command line parsing OK ! found"sv
                << "\n[FSDIPlatformIndex] = "sv << this->getFSDIPlatformIndex()
                << "\n[FSDIPlatformCount] = "sv << this->getFSDIPlatformCount()
                << "\n[FSDIPlatformIP] = "sv << this->getFSDIPlatformIP()
                << "\n[FSDIPlatformSN] = "sv << std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>{}.to_bytes(this->getFSDIPlatformSN())
                << "\n[IdleInitDuration] = "sv << this->getIdleInitDuration()
                << "\n[TargetLoopPeriod] = "sv << this->getTargetLoopPeriod()
                << "\n[S1Frequency] = "sv << this->getS1Frequency()
                << "\n[S1Amplitude] = "sv << this->getS1Amplitude()
                << "\n[S2Frequency] = "sv << this->getS2Frequency()
                << "\n[S2Amplitude] = "sv << this->getS2Amplitude()
                << "\n[XAxisPhaseOffset] = "sv << this->getXAxisPhaseOffset()
                << "\n[YAxisPhaseOffset] = "sv << this->getYAxisPhaseOffset()
                << "\n[ZAxisPhaseOffset] = "sv << this->getZAxisPhaseOffset();

            // update status
            status = CLI_OPTIONS_OK_;

            // success
            return status;
        }
        catch(boost::exception & ex)
        {
            Labsim::LogError("[Umbrella]"sv)
                << "CommandLineOptions::Parse() : caught boost::exception exception : "sv
                << boost::diagnostic_information(ex);

            // fail status
            return CLI_OPTIONS_ERROR_;
        }
        catch(std::exception & ex)
        {
            Labsim::LogError("[Umbrella]"sv)
                << "CommandLineOptions::Parse() : caught std::bad_cast exception : "sv
                << ex.what();

            // fail status
            return CLI_OPTIONS_ERROR_;
        }
        catch(...)
        {
            Labsim::LogError("[Umbrella]"sv)
                << "CommandLineOptions::Parse() : caught unhandled exception : "sv
                << boost::current_exception_diagnostic_information();

            // fail status
            return CLI_OPTIONS_ERROR_;

        } /* catch(...) */

        // status
        return status;

    } /* Parse() */

} // namespace Labsim::project::umbrella
