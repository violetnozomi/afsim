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

#ifndef P6DOFWIND_H
#define P6DOFWIND_H

#include "p6dof_export.h"

class UtVec3dX;

//! P6DofWind provides wind data for Pseudo 6DOF objects. This base class
// produces no wind. Specific wind models should derive from this class.

class P6DOF_EXPORT P6DofWind
{
public:
   P6DofWind()        = default;
   ~P6DofWind()       = default;
   P6DofWind& operator=(const P6DofWind& other) = delete;

   P6DofWind* Clone() const;

   // WindAtLocationLLA returns a UtVec3dX (meters/sec) Mach for a vehicle at the specified
   // location [lat/lon (deg) and alt (meters above sea level)]
   UtVec3dX WindAtLocationLLA(double aLatitude, double aLongitude, double aAltitude_m);

   // WindAtLocationWGS returns a UtVec3dX (meters/sec) Mach for a vehicle at the specified
   // WGS location
   UtVec3dX WindAtLocationWGS(const UtVec3dX& aWGS_m);

protected:
   P6DofWind(const P6DofWind& aSrc) = default;
};

#endif
