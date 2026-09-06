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

#ifndef WSFSIXDOFFORCESOBJECT_HPP
#define WSFSIXDOFFORCESOBJECT_HPP

#include "wsf_six_dof_export.h"

#include "UtVec3dX.hpp"

namespace wsf
{
namespace six_dof
{
// The ForcesObject class provides a means to "build-up" forces
// and moments (F&M) provides a collection of state data for SixDOF_Objects.
// It should be noted that the class assumes that forces and moments are in
// body-coordinates.
class WSF_SIX_DOF_EXPORT ForcesObject
{
public:
   ForcesObject() = default;

   ForcesObject* Clone() const;

   // This adds aSrc F&M to the current F&M object, transforming the aSrc F&M
   // as required for the current reference point
   ForcesObject& operator+=(const ForcesObject& aSrc);

   // This clears the force
   void ClearForces();

   // This moves the reference point to the specified point, and modifies
   // the current F&M accordingly. This version uses attitude data.
   void MoveRefPoint_ft(const UtVec3dX& aAngRelToParentYPR_rad);

   // This adds the specified force and moment applied at the specified point.
   // The function calculates the new F&M at the internal reference point.
   void AddForce(const UtVec3dX& aForceVec_lbs);

   // This returns (via references) the current force and moment at the specified
   // point, using the current F&M at the internal reference point.
   void GetForce(UtVec3dX& aForceVec_lbs) const;

   // This returns the magnitude of the force in lbs
   double GetForceMagnitude_lbs() const;

   // This returns the vector of the force in lbs
   UtVec3dX GetForce_lbs() const;

   // This limits the magnitude of the force in lbs, reducing it, if needed
   void LimitMaxForceMagnitude_lbs(double aMaxForce_lbs);

   // This returns a ForceAndMomentsObject that is the "average" of
   // the "this" F&M object and the specified F&M object (argument object)
   ForcesObject CalcAverageWith(const ForcesObject& aOtherFM) const;

protected:
   UtVec3dX mForceVec_lbs;
};
} // namespace six_dof
} // namespace wsf

#endif
