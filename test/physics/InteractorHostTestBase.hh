//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file InteractorHostTestBase.hh
//---------------------------------------------------------------------------//
#pragma once

#include <memory>
#include <random>
#include <vector>
#include "base/Array.hh"
#include "base/ArrayIO.hh"
#include "base/Span.hh"
#include "base/StackAllocatorPointers.hh"
#include "base/Types.hh"
#include "physics/base/ModelIdGenerator.hh"
#include "physics/base/ParticleParams.hh"
#include "physics/base/ParticleStatePointers.hh"
#include "physics/base/Secondary.hh"
#include "physics/base/Units.hh"
#include "physics/material/MaterialParams.hh"
#include "physics/material/MaterialStatePointers.hh"

// Test helpers
#include "base/HostStackAllocatorStore.hh"
#include "gtest/Test.hh"
#include "random/DiagnosticRngEngine.hh"

namespace celeritas
{
template<class T>
class StackAllocatorView;
class ParticleTrackView;
class MaterialTrackView;
struct Interaction;
struct Secondary;
} // namespace celeritas

namespace celeritas_test
{
//---------------------------------------------------------------------------//
/*!
 * Test harness base class for a host-side Interactor.
 *
 * This class initializes host versions of some of the common inputs to an
 * Interactor. It \b cannot be used for testing device instantiations.
 */
class InteractorHostTestBase : public celeritas::Test
{
  public:
    //!@{
    //! Type aliases
    using RandomEngine = DiagnosticRngEngine<std::mt19937>;

    using real_type = celeritas::real_type;
    using PDGNumber = celeritas::PDGNumber;
    using MevEnergy = celeritas::units::MevEnergy;

    using MaterialDefId     = celeritas::MaterialDefId;
    using MaterialParams    = celeritas::MaterialParams;
    using MaterialTrackView = celeritas::MaterialTrackView;

    using Interaction            = celeritas::Interaction;
    using ModelIdGenerator       = celeritas::ModelIdGenerator;
    using ModelId                = celeritas::ModelId;
    using ParticleDefId          = celeritas::ParticleDefId;
    using ParticleParams         = celeritas::ParticleParams;
    using ParticleTrackView      = celeritas::ParticleTrackView;
    using Real3                  = celeritas::Real3;
    using Secondary              = celeritas::Secondary;
    using SecondaryAllocatorView = celeritas::StackAllocatorView<Secondary>;
    using constSpanSecondaries   = celeritas::Span<const Secondary>;

    using HostSecondaryStore = HostStackAllocatorStore<Secondary>;
    //!@}

  public:
    //!@{
    //! Initialize and destroy
    InteractorHostTestBase();
    ~InteractorHostTestBase();
    //!@}

    //!@{
    //! Set and get material properties
    void                  set_material_params(MaterialParams::Input inp);
    const MaterialParams& material_params() const;
    //!@}

    //!@{
    //! Set and get particle params
    void                  set_particle_params(ParticleParams::Input inp);
    const ParticleParams& particle_params() const;
    std::shared_ptr<const ParticleParams> get_particle_params() const
    {
        CELER_EXPECT(particle_params_);
        return particle_params_;
    }
    //!@}

    //!@{
    //! Material properties
    void               set_material(const std::string& name);
    MaterialTrackView& material_track()
    {
        CELER_EXPECT(mt_view_);
        return *mt_view_;
    }
    //!@}

    //!@{
    //! Incident particle properties and access
    void                     set_inc_particle(PDGNumber n, MevEnergy energy);
    void                     set_inc_direction(const Real3& dir);
    const Real3&             direction() const { return inc_direction_; }
    const ParticleTrackView& particle_track() const
    {
        CELER_EXPECT(pt_view_);
        return *pt_view_;
    }
    //!@}

    //!@{
    //! Secondary stack storage and access
    void                      resize_secondaries(int count);
    const HostSecondaryStore& secondaries() const { return secondaries_; }
    SecondaryAllocatorView&   secondary_allocator()
    {
        CELER_EXPECT(sa_view_);
        return *sa_view_;
    }
    //!@}

    //!@{
    //! Random number generator
    RandomEngine& rng() { return rng_; }
    //!@}

    // Check for energy and momentum conservation
    void check_conservation(const Interaction& interaction) const;

    // Check for energy conservation
    void check_energy_conservation(const Interaction& interaction) const;

    // Check for momentum conservation
    void check_momentum_conservation(const Interaction& interaction) const;

  private:
    std::shared_ptr<MaterialParams> material_params_;
    std::shared_ptr<ParticleParams> particle_params_;
    RandomEngine                    rng_;

    celeritas::MaterialTrackState     mat_state_;
    std::vector<real_type>            mat_element_scratch_;
    celeritas::MaterialParamsPointers mp_pointers_;
    celeritas::MaterialStatePointers  ms_pointers_;

    celeritas::ParticleTrackState     particle_state_;
    celeritas::ParticleParamsPointers pp_pointers_;
    celeritas::ParticleStatePointers  ps_pointers_;
    Real3                             inc_direction_ = {0, 0, 1};
    HostSecondaryStore                secondaries_;

    // Views
    std::shared_ptr<MaterialTrackView>      mt_view_;
    std::shared_ptr<ParticleTrackView>      pt_view_;
    std::shared_ptr<SecondaryAllocatorView> sa_view_;
};

//---------------------------------------------------------------------------//
} // namespace celeritas_test
