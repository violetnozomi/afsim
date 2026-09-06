// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTAZEL_TYPES_HPP
#define UTAZEL_TYPES_HPP

//! Define data types used by the Azimuth Elevation Tables.

namespace ut
{
namespace azel
{
//! Type of interpolation to be performed
enum class InterpolationType
{
   cLinear,
   cLogarithmic
};
} // namespace azel
} // namespace ut

#endif
