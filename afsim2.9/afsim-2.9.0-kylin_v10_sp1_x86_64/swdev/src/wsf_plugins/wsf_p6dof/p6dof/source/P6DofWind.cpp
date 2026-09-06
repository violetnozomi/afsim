// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "P6DofWind.hpp"

#include "UtVec3dX.hpp"

P6DofWind* P6DofWind::Clone() const
{
   return new P6DofWind(*this);
}

UtVec3dX P6DofWind::WindAtLocationLLA(double, // aLatitude,
                                      double, // aLongitude,
                                      double) // aAltitude_m )
{
   return UtVec3dX(0., 0., 0.);
}

UtVec3dX P6DofWind::WindAtLocationWGS(const UtVec3dX&) // aWGS_m )
{
   return UtVec3dX(0., 0., 0.);
}
