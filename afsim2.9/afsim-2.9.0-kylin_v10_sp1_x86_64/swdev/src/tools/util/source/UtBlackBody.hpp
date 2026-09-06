// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTBLACKBODY_HPP
#define UTBLACKBODY_HPP

#include "ut_export.h"

#include <cstddef> // For 'size_t'.
#include <iosfwd>

//! Implement the concept of a black body in thermal equilibrium.
//!
//! From Wikipedia (abbreviated):
//!
//! A black body is an idealized physical body that absorbs all incident electromagnetic radiation, regardless of
//! frequency or angle of incidence. A black body in thermal equilibrium (that is, at a constant temperature) emits
//! electromagnetic radiation called black-body radiation. The radiation is emitted according to Planck's law, meaning
//! that it has a spectrum that is determined by the temperature alone, not by the body's shape or composition.
//!
//! As implemented, this class is focused on the optical spectrum (100nm-1000um) and temperatures less than about 5800
//! K.

namespace ut
{
namespace BlackBody
{
UT_EXPORT double BandRadiance(double aTemperature, double aLowerWavelength, double aUpperWavelength);

UT_EXPORT double BandRadiance_Direct(double aTemperature, double aLowerWavelength, double aUpperWavelength, double aStepSize);

UT_EXPORT double SpectralRadiance(double aTemperature, double aWavelength);

UT_EXPORT double Temperature(double aRadiance);

UT_EXPORT double TotalRadiance(double aTemperature);

UT_EXPORT void TestRadiance(std::ostream& aOut);
} // namespace BlackBody
} // namespace ut

#endif
