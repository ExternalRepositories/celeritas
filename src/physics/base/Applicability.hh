//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file Applicability.hh
//---------------------------------------------------------------------------//
#pragma once

#include <functional>
#include <tuple>

#include "base/Assert.hh"
#include "base/OpaqueId.hh"
#include "base/Types.hh"
#include "physics/material/Types.hh"
#include "Types.hh"
#include "Units.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Range where a model and/or process is valid.
 *
 * This class is used during setup for specifying the ranges of applicability
 * for a physics model or process. The interval is *open* on the lower energy
 * range and *closed* on the upper energy. So a threshold reaction should have
 * the lower energy set to the threshold. Models valid to zero energy but have
 * special "at rest" models should set upper to zero.
 *
 * An unset value for "material" means it applies to all materials; however,
 * the particle ID should always be set.
 */
struct Applicability
{
    MaterialDefId    material{};
    ParticleDefId    particle{};
    units::MevEnergy lower = zero_quantity();
    units::MevEnergy upper = max_quantity();

    //! Range for a particle at rest
    static inline Applicability at_rest(ParticleDefId id)
    {
        CELER_EXPECT(id);
        Applicability result;
        result.particle = id;
        result.lower    = neg_max_quantity();
        result.upper    = zero_quantity();
        return result;
    }
};

//!@{
//! Comparators
inline bool operator==(const Applicability& lhs, const Applicability& rhs)
{
    return std::make_tuple(lhs.material, lhs.particle, lhs.lower, lhs.upper)
           == std::make_tuple(rhs.material, rhs.particle, rhs.lower, rhs.upper);
}

inline bool operator<(const Applicability& lhs, const Applicability& rhs)
{
    return std::make_tuple(lhs.material, lhs.particle, lhs.lower, lhs.upper)
           < std::make_tuple(rhs.material, rhs.particle, rhs.lower, rhs.upper);
}
//!@}

//---------------------------------------------------------------------------//
} // namespace celeritas
