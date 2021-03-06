//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file UniformGrid.i.hh
//---------------------------------------------------------------------------//
#include <algorithm>
#include "base/Assert.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Construct with data.
 */
CELER_FUNCTION
UniformGrid::UniformGrid(const UniformGridPointers& data) : data_(data)
{
    CELER_EXPECT(data_);
}

//---------------------------------------------------------------------------//
/*!
 * Access the number of grid points.
 */
CELER_FUNCTION size_type UniformGrid::size() const
{
    return data_.size;
}

//---------------------------------------------------------------------------//
/*!
 * Access the highest value.
 */
CELER_FUNCTION auto UniformGrid::back() const -> value_type
{
    return data_.back;
}

//---------------------------------------------------------------------------//
/*!
 * Get the value value at the given grid point.
 */
CELER_FUNCTION auto UniformGrid::operator[](size_type i) const -> value_type
{
    CELER_EXPECT(i < data_.size);
    return data_.front + data_.delta * i;
}

//---------------------------------------------------------------------------//
/*!
 * Find the value bin such that data[result] <= value < data[result + 1].
 *
 * The given value *must* be in range, because out-of-bounds values usually
 * require different treatment (e.g. clipping to the boundary values rather
 * than interpolating). It's easier to test the exceptional cases (final grid
 * point) outside of the grid view.
 */
CELER_FUNCTION size_type UniformGrid::find(value_type value) const
{
    CELER_EXPECT(value >= this->front() && value < this->back());
    auto bin = static_cast<size_type>((value - data_.front) / data_.delta);
    CELER_ENSURE(bin + 1 < this->size());
    return bin;
}

//---------------------------------------------------------------------------//
} // namespace celeritas
