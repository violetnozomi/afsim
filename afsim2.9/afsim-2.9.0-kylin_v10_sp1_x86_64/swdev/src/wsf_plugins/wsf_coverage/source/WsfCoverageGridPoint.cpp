// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCoverageGridPoint.hpp"

#include "UtEarth.hpp"
#include "WsfCoverageGrid.hpp"

namespace wsf
{
namespace coverage
{

Point::Point(PointId aPointId, double aLatitudeDeg, double aLongitudeDeg, double aAltitude)
   : mId{aPointId}
   , mLocationLLA{aLatitudeDeg, aLongitudeDeg, aAltitude}
{
}

//! Set the point's LLA position.
//!
//! \param aLatitudeDeg  - The latitude of the point in degrees.
//! \param aLongitudeDeg - The longitude of the point in degrees.
//! \param aAltitude     - Tha altitude of the point in meters.
void Point::SetLLA(double aLatitudeDeg, double aLongitudeDeg, double aAltitude)
{
   mLocationLLA.Set(aLatitudeDeg, aLongitudeDeg, aAltitude);
}

//! Set the point's LLA position.
//!
//! \param aLocationLLA - The LLA position of the point.
void Point::SetLLA(const UtLLAPos& aLocationLLA)
{
   mLocationLLA = aLocationLLA;
}

} // namespace coverage
} // namespace wsf
