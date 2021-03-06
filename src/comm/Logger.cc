//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file Logger.cc
//---------------------------------------------------------------------------//
#include "Logger.hh"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include "base/Assert.hh"
#include "base/ColorUtils.hh"
#include "base/Range.hh"
#include "Communicator.hh"
#include "ScopedMpiInit.hh"

namespace
{
using namespace celeritas;
//---------------------------------------------------------------------------//
// HELPER CLASSES
//---------------------------------------------------------------------------//
//! Default global logger prints the error message with basic colors
void default_global_handler(Provenance prov, LogLevel lev, std::string msg)
{
    if (lev == LogLevel::debug || lev >= LogLevel::warning)
    {
        // Output problem line/file for debugging or high level
        std::cerr << color_code('x') << prov.file << ':' << prov.line
                  << color_code(' ') << ": ";
    }

    // clang-format off
    char c = ' ';
    switch (lev)
    {
        case LogLevel::debug:      c = 'x'; break;
        case LogLevel::diagnostic: c = 'x'; break;
        case LogLevel::status:     c = 'b'; break;
        case LogLevel::info:       c = 'g'; break;
        case LogLevel::warning:    c = 'y'; break;
        case LogLevel::error:      c = 'r'; break;
        case LogLevel::critical:   c = 'R'; break;
        default: CELER_ASSERT_UNREACHABLE();
    };
    // clang-format on
    std::cerr << color_code(c) << to_cstring(lev) << ": " << color_code(' ')
              << msg << std::endl;
}

//---------------------------------------------------------------------------//
//! Log the local node number as well as the message
class LocalHandler
{
  public:
    explicit LocalHandler(const Communicator& comm) : rank_(comm.rank()) {}

    void operator()(Provenance prov, LogLevel lev, std::string msg)
    {
        // To avoid multiple process output stepping on each other, write into
        // a buffer and then print with a single call.
        std::ostringstream os;
        os << color_code('x') << prov.file << ':' << prov.line
           << color_code(' ') << ": " << color_code('W') << "rank " << rank_
           << ": " << color_code('x') << to_cstring(lev) << ": "
           << color_code(' ') << msg << '\n';
        std::cerr << os.str();
    }

  private:
    int rank_;
};

//---------------------------------------------------------------------------//
} // namespace

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Construct with communicator (only rank zero is active) and handler.
 */
Logger::Logger(const Communicator& comm,
               LogHandler          handle,
               const char*         level_env)
{
    if (comm.rank() == 0)
    {
        // Accept handler, otherwise it is a "null" function pointer.
        handle_ = std::move(handle);
    }
    if (level_env)
    {
        // Search for the provided environment variable to set the default
        // logging level using the `to_cstring` function in LoggerTypes.
        if (const char* env_value = std::getenv(level_env))
        {
            auto levels = range(LogLevel::size_);
            auto iter   = std::find_if(
                levels.begin(), levels.end(), [env_value](LogLevel lev) {
                    return std::strcmp(env_value, to_cstring(lev)) == 0;
                });
            if (iter != levels.end())
            {
                min_level_ = *iter;
            }
            else if (comm.rank() == 0)
            {
                std::cerr << "Log level environment variable '" << level_env
                          << "' has an invalid value '" << env_value
                          << "': ignoring\n";
            }
        }
    }
}

//---------------------------------------------------------------------------//
// FREE FUNCTIONS
//---------------------------------------------------------------------------//
/*!
 * Parallel-enabled logger: print only on "main" process.
 *
 * Setting the "CELER_LOG" environment variable to "debug", "info", "error",
 * etc. will change the default log level.
 */
Logger& world_logger()
{
    // Use the null communicator if MPI isn't enabled, otherwise comm_world
    static Logger logger(
        ScopedMpiInit::status() != ScopedMpiInit::Status::disabled
            ? Communicator::comm_world()
            : Communicator{},
        &default_global_handler,
        "CELER_LOG");
    return logger;
}

//---------------------------------------------------------------------------//
/*!
 * Serial logger: print on *every* process that calls it.
 *
 * Setting the "CELER_LOG_LOCAL" environment variable to "debug", "info",
 * "error", etc. will change the default log level.
 */
Logger& self_logger()
{
    // Use the null communicator if MPI isn't enabled, otherwise comm_local.
    // If only
    static Logger logger(
        ScopedMpiInit::status() != ScopedMpiInit::Status::disabled
            ? Communicator::comm_world()
            : Communicator{},
        ScopedMpiInit::status() != ScopedMpiInit::Status::disabled
            ? LocalHandler{Communicator::comm_world()}
            : LogHandler{&default_global_handler},
        "CELER_LOG_LOCAL");
    return logger;
}

//---------------------------------------------------------------------------//
} // namespace celeritas
