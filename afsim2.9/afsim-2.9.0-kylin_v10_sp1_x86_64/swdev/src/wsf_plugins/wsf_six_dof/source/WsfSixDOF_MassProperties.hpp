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

#ifndef WSFSIXDOFMASSPROPERTIES_HPP
#define WSFSIXDOFMASSPROPERTIES_HPP

#include "wsf_six_dof_export.h"

class UtInput;
#include "UtVec3dX.hpp"

namespace wsf
{
namespace six_dof
{
// MassProperties provides mass and rotational inertia properties for
// Mover objects. It also provides functions to "build-up" mass properties
// including those from subobjects (Movers) as well as contributors
// from consumables such as fuel.
class WSF_SIX_DOF_EXPORT MassProperties
{
public:
   bool ProcessInput(UtInput& aInput);

   // This is the "preferred" method of adding mass properties
   MassProperties& operator+=(const MassProperties& aSrc);

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
   void SetBaseMassProperties(double aBaseMass_lbm, const UtVec3dX& aBaseCmPosRelToRef_ft);

   // This sets the base properties and (internally) set the current properties
   void SetBaseMassProperties(double          aBaseMass_lbm,
                              double          aBaseIxx_slugft2,
                              double          aBaseIyy_slugft2,
                              double          aBaseIzz_slugft2,
                              const UtVec3dX& aBaseCmPosRelToRef_ft);

   // This adds a mass, with the specified rotational inertia,
   // at the specified location
   void AddMassAtLocation(double aMass_lbm, const UtVec3dX& aLocation_ft);

   // This adds a mass, with the specified rotational inertia,
   // at the specified location
   void AddMassAtLocation(double          aMass_lbm,
                          double          aIxx_slugft2,
                          double          aIyy_slugft2,
                          double          aIzz_slugft2,
                          const UtVec3dX& aLocation_ft);

   // This is a convenience function to apply rotational inertia
   // from an additional mass
   void AddInertiaFromParallelAxis(double          aAdditionalMass_lbm,
                                   const UtVec3dX& aPositionDelta_ft,
                                   double&         aIxx_slugft2,
                                   double&         aIyy_slugft2,
                                   double&         aIzz_slugft2);

   // This moves to a new reference point
   void MoveToLocation(const UtVec3dX& aLocation_ft);

protected:
   double   mBaseMass_lbm    = 0.0;
   double   mBaseIxx_slugft2 = 0.0;
   double   mBaseIyy_slugft2 = 0.0;
   double   mBaseIzz_slugft2 = 0.0;
   UtVec3dX mBaseCmPosRelToRef_ft; // CM location relative to reference point

   double   mCurrentMass_lbm    = 0.0;
   double   mCurrentIxx_slugft2 = 0.0;
   double   mCurrentIyy_slugft2 = 0.0;
   double   mCurrentIzz_slugft2 = 0.0;
   UtVec3dX mCurrentCmPosRelToRef_ft; // CM location relative to reference point
};
} // namespace six_dof
} // namespace wsf

#endif
