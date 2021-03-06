//---------------------------------*-CUDA-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file ParticleParams.hh
//---------------------------------------------------------------------------//
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "base/DeviceVector.hh"
#include "ParticleParamsPointers.hh"
#include "PDGNumber.hh"
#include "ParticleDef.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Data management for Standard Model particle classifications.
 *
 * This class represents "per-problem" shared data about standard
 * model particles being used.
 *
 * The ParticleParams is constructed on the host with a vector that
 * combines metadata (used for debugging output and interfacing with physics
 * setup) and data (used for on-device transport). Each entry in the
 * construction is assigned a unique \c ParticleDefId used for runtime access.
 *
 * The PDG Monte Carlo number is a unique "standard model" identifier for a
 * particle. See "Monte Carlo Particle Numbering Scheme" in the "Review of
 * Particle Physics":
 * https://pdg.lbl.gov/2020/reviews/rpp2020-rev-monte-carlo-numbering.pdf
 * It should be used to identify particle types during construction time.
 */
class ParticleParams
{
  public:
    //! Define a particle's input data
    struct ParticleInput
    {
        std::string             name;     //!< Particle name
        PDGNumber               pdg_code; //!< See "Review of Particle Physics"
        units::MevMass          mass;     //!< Rest mass [MeV / c^2]
        units::ElementaryCharge charge;   //!< Charge in units of [e]
        real_type               decay_constant; //!< Decay constant [1/s]
    };

    //! Input data to construct this class
    using Input = std::vector<ParticleInput>;

  public:
    // Construct with a vector of particle definitions
    explicit ParticleParams(const Input& defs);

    //// HOST ACCESSORS ////

    //! Number of particle definitions
    size_type size() const { return md_.size(); }

    // Get particle name
    inline const std::string& id_to_label(ParticleDefId id) const;

    // Get PDG code
    inline PDGNumber id_to_pdg(ParticleDefId id) const;

    // Find the ID from a name
    inline ParticleDefId find(const std::string& name) const;

    // Find the ID from a PDG code
    inline ParticleDefId find(PDGNumber pdg_code) const;

    // Access definition on host for construction
    inline const ParticleDef& get(ParticleDefId id) const;

    // TESTING ONLY: Get a view to the managed data
    ParticleParamsPointers host_pointers() const;

    //// DEVICE ACCESSORS ////

    // Get a view to the managed data
    ParticleParamsPointers device_pointers() const;

  private:
    // Saved copy of metadata
    std::vector<std::pair<std::string, PDGNumber>> md_;

    // Map particle names to registered IDs
    std::unordered_map<std::string, ParticleDefId> name_to_id_;

    // Map particle codes to registered IDs
    std::unordered_map<PDGNumber, ParticleDefId> pdg_to_id_;

    // Host copy of definitions for host construction of other classes
    std::vector<ParticleDef> host_defs_;

    // Particle definitions on device
    DeviceVector<ParticleDef> device_defs_;
};

//---------------------------------------------------------------------------//
} // namespace celeritas

#include "ParticleParams.i.hh"
