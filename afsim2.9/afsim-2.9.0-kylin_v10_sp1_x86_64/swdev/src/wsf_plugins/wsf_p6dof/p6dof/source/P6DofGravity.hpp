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

#ifndef P6DOFGRAVITY_H
#define P6DOFGRAVITY_H

#include "p6dof_export.h"

class UtVec3dX;

//! P6DofAtmosphere provides atmospheric data for Pseudo 6DOF objects.

class P6DOF_EXPORT P6DofGravity
{
public:
   P6DofGravity()        = default;
   ~P6DofGravity()       = default;
   P6DofGravity& operator=(const P6DofGravity& other) = delete;

   P6DofGravity* Clone() const;

   // This returns a gravity vector in the specified coordinate frame
   static UtVec3dX NormalizedGravitationalAccel(const double aLat,
                                                const double aLon,
                                                const double aAlt_m,
                                                const bool   aUseSphericalEarth);

   // This returns a gravity magnitude at the specified altitude
   static double NormalizedGravitationalAccel(const double aAlt_m);

   // This returns a gravity vector in the specified coordinate frame
   static UtVec3dX NormalizedGravitationalAccel(const UtVec3dX& aLocation_m, const bool aUseSphericalEarth);

protected:
   P6DofGravity(const P6DofGravity& aSrc) = default;
};

#endif
