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

#ifndef P6DOFMASSPROPERTIES_H
#define P6DOFMASSPROPERTIES_H

#include "p6dof_export.h"

#include "UtVec3dX.hpp"

class UtInput;

// P6DofMassProperties provides mass and rotational inertia properties for
// P6DofObjects. It also provides functions to "build-up" mass properties
// including those from subobjects (P6DofObjects) as well as contributors
// from consumables such as fuel.
class P6DOF_EXPORT P6DofMassProperties
{
public:
   P6DofMassProperties();

   bool ProcessInput(UtInput& aInput);

   // This is the "preferred" method of adding mass properties
   P6DofMassProperties& operator+=(const P6DofMassProperties& aSrc);

   // These functions provide the "current" values
   double   GetMass_lbs() const;
   double   GetIxx_slugft2() const;
   double   GetIyy_slugft2() const;
   double   GetIzz_slugft2() const;
   UtVec3dX GetCmPosRelToRef_ft() const;

   // These functions provide the "base" values
   double   GetBaseMass_lbs() const;
   double   GetBaseIxx_slugft2() const;
   double   GetBaseIyy_slugft2() const;
   double   GetBaseIzz_slugft2() const;
   UtVec3dX GetBaseCmPosRelToRef_ft() const;

   // This sets all mass properties data to zero
   void ClearData();

   // This sets the current properties to the base properties
   void SetCurrentDataToBaseData();

   // This sets the base properties and (internally) set the current properties
   void SetBaseMassProperties(double          aBaseMass_lbm,
                              double          aBaseIxx_slugft2,
                              double          aBaseIyy_slugft2,
                              double          aBaseIzz_slugft2,
                              const UtVec3dX& aBaseCmPosRelToRef_ft);

   // This adds a mass, with the specified rotational inertia,
   // at the specified location
   void AddMassAtLocation(double          aMass_lbm,
                          double          aIxx_slugft2,
                          double          aIyy_slugft2,
                          double          aIzz_slugft2,
                          const UtVec3dX& aLocation_ft);

   // This adds a point mass at the specified location
   void AddMassAtLocation(double aMass_lbm, const UtVec3dX& aLocation_ft);

   // This moves to a new reference point
   void MoveToLocation(const UtVec3dX& aLocation_ft);

protected:
   double   mBaseMass_lbm;
   double   mBaseIxx_slugft2;
   double   mBaseIyy_slugft2;
   double   mBaseIzz_slugft2;
   UtVec3dX mBaseCmPosRelToRef_ft; // CM location relative to reference point

   double   mCurrentMass_lbm;
   double   mCurrentIxx_slugft2;
   double   mCurrentIyy_slugft2;
   double   mCurrentIzz_slugft2;
   UtVec3dX mCurrentCmPosRelToRef_ft; // CM location relative to reference point
};

#endif
