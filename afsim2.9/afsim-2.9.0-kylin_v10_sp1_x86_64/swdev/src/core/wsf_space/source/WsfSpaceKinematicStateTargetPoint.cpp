// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSpaceKinematicStateTargetPoint.hpp"

#include <cmath>

#include "UtEarth.hpp"
#include "UtMath.hpp"

namespace wsf
{
namespace space
{

double KinematicStateTargetPoint::GetCharacteristicTime() const
{
   // A fixed point has no intrinsic timescale, so this estimates one for the point by
   // using the distance from the Earth, and the gravitational parameter of the Earth.
   double r = mInertialState.GetLocation().Magnitude();
   return UtMath::cTWO_PI * std::sqrt(r * r * r / ut::EarthEllipsoidWGS84().GetGravitationalParameter());
}

} // namespace space
} // namespace wsf
