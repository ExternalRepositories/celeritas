//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file ArrayUtils.test.cc
//---------------------------------------------------------------------------//
#include "base/ArrayUtils.hh"

#include "celeritas_test.hh"
#include "base/Constants.hh"
#include "base/ArrayIO.hh"

using celeritas::Array;
using Real3 = Array<double, 3>;

enum
{
    X = 0,
    Y = 1,
    Z = 2
};

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

TEST(ArrayUtilsTest, io)
{
    Array<int, 3> x{1, 3, 2};
    EXPECT_EQ("{1,3,2}", to_string(x));
}

TEST(ArrayUtilsTest, axpy)
{
    Array<int, 3> x{1, 3, 2};
    Array<int, 3> y{20, 30, 40};

    celeritas::axpy(4, x, &y);
    EXPECT_EQ(4 * 1 + 20, y[X]);
    EXPECT_EQ(4 * 3 + 30, y[Y]);
    EXPECT_EQ(4 * 2 + 40, y[Z]);
}

TEST(ArrayUtilsTest, dot_product)
{
    Array<int, 2> x{1, 3};
    Array<int, 2> y{2, 4};

    EXPECT_EQ(1 * 2 + 3 * 4, celeritas::dot_product(x, y));
}

TEST(ArrayUtilsTest, norm)
{
    EXPECT_SOFT_EQ(std::sqrt(4.0 + 9.0 + 16.0),
                   celeritas::norm(Array<double, 3>{2, 3, 4}));
}

TEST(ArrayUtilsTest, normalize_direction)
{
    Real3            direction{1, 2, 3};
    double           norm = 1 / std::sqrt(1 + 4 + 9);
    celeritas::normalize_direction(&direction);

    static const double expected[] = {1 * norm, 2 * norm, 3 * norm};
    EXPECT_VEC_SOFT_EQ(expected, direction);
}

TEST(ArrayUtilsTest, rotate)
{
    Real3 vec = {-1.1, 2.3, 0.9};
    celeritas::normalize_direction(&vec);

    // transform through some directions
    double costheta = std::cos(2.0 / 3.0);
    double sintheta = std::sqrt(1.0 - costheta * costheta);
    double phi      = 2 * celeritas::constants::pi / 3.0;

    double           a = 1.0 / sqrt(1.0 - vec[Z] * vec[Z]);
    Real3            expected
        = {vec[X] * costheta + vec[Z] * vec[X] * sintheta * cos(phi) * a
               - vec[Y] * sintheta * sin(phi) * a,
           vec[Y] * costheta + vec[Z] * vec[Y] * sintheta * cos(phi) * a
               + vec[X] * sintheta * sin(phi) * a,
           vec[Z] * costheta - sintheta * cos(phi) / a};

    auto scatter = celeritas::from_spherical(costheta, phi);
    EXPECT_VEC_SOFT_EQ(expected, celeritas::rotate(scatter, vec));

    // Transform degenerate vector along y
    expected = {-sintheta * cos(phi), sintheta * sin(phi), -costheta};
    EXPECT_VEC_SOFT_EQ(expected, celeritas::rotate(scatter, {0.0, 0.0, -1.0}));

    expected = {sintheta * cos(phi), sintheta * sin(phi), costheta};
    EXPECT_VEC_SOFT_EQ(expected, celeritas::rotate(scatter, {0.0, 0.0, 1.0}));

    // Transform almost degenerate vector
    vec = {3e-8, 4e-8, 1};
    celeritas::normalize_direction(&vec);
    EXPECT_VEC_SOFT_EQ(
        (Real3{-0.613930085414816, 0.0739664834328671, 0.785887275346237}),
        celeritas::rotate(scatter, vec));

    // Switch scattered z direction
    costheta *= -1;
    scatter = celeritas::from_spherical(costheta, phi);

    expected = {-sintheta * cos(phi), sintheta * sin(phi), -costheta};
    EXPECT_VEC_SOFT_EQ(expected, celeritas::rotate(scatter, {0.0, 0.0, -1.0}));

    expected = {sintheta * cos(phi), sintheta * sin(phi), costheta};
    vec      = celeritas::rotate(scatter, {0.0, 0.0, 1.0});
    EXPECT_VEC_SOFT_EQ(expected, vec);
}
