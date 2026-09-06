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

#ifndef WSFSIXDOFFORCEANDROTATIONOBJECT_HPP
#define WSFSIXDOFFORCEANDROTATIONOBJECT_HPP

#include "wsf_six_dof_export.h"

#include "UtMath.hpp"
#include "UtVec2.hpp"
#include "UtVec3dX.hpp"

namespace wsf
{
namespace six_dof
{
// The ForceAndRotationObject class provides a means to "build-up" forces
// and rotation effects for more simplified SixDOF Movers.
// It should be noted that the class assumes that forces and other data are in
// body-coordinates.
class WSF_SIX_DOF_EXPORT ForceAndRotationObject
{
public:
   ForceAndRotationObject() = default;

   ForceAndRotationObject* Clone() const;

   // This adds aSrc F&M to the current F&M object, transforming the aSrc F&M
   // as required for the current reference point
   ForceAndRotationObject& operator+=(const ForceAndRotationObject& aSrc);

   // This clears the force and moment but leaves the reference point unchanged
   void ClearForcesAndRotation();

   // This points the force in the correct direction for a parent vehicle, based on attitude relative to that parent.
   void ReorientForce_ft(const UtVec3dX& aAngRelToParentYPR_rad);

   // This adds the specified force.
   void AddForce(const UtVec3dX& aForceVec_lbs);

   // This returns the magnitude of the force in lbs
   double GetForceMagnitude_lbs() const;

   // This returns the vector of the force in lbs
   UtVec3dX GetForce_lbs() const;

   // This limits the magnitude of the force in lbs, reducing it, if needed
   void LimitMaxForceMagnitude_lbs(double aMaxForce_lbs);

   // This returns the vector of the maximum angular acceleration in radians/second^2
   void SetMaximumAngularAcceleration_rps2(const UtVec3dX& aOmegaDot_rps2) { mMaximumOmegaDot_rps2 = aOmegaDot_rps2; };

   // This accumulates the vector of the maximum angular acceleration in radians/second^2
   void AddMaximumAngularAcceleration_rps2(const UtVec3dX& aOmegaDot_rps2) { mMaximumOmegaDot_rps2 += aOmegaDot_rps2; };

   // This returns the vector of the maximum angular acceleration in radians/second^2
   UtVec3dX GetMaximumAngularAcceleration_rps2() const { return mMaximumOmegaDot_rps2; };

   // This returns the vector of the maximum angular acceleration in degrees/second^2
   UtVec3dX GetMaximumAngularAcceleration_dps2() const { return mMaximumOmegaDot_rps2 * UtMath::cDEG_PER_RAD; };

   // This returns the vector of the maximum angular acceleration in radians/second^2
   void SetStabilizingFrequency_rps(const UtVec3dX& aOmega_rps) { mStabilizingFrequency_rps = aOmega_rps; };

   // This accumulates the vector of the maximum angular acceleration in radians/second^2
   void AddStabilizingFrequency_rps(const UtVec3dX& aOmega_rps) { mStabilizingFrequency_rps += aOmega_rps; };

   // This returns the vector of the maximum angular acceleration in radians/second^2
   UtVec3dX GetStabilizingFrequency_rps() const { return mStabilizingFrequency_rps; };

   // This returns a ForceAndRotationObject that is the "average" of
   // the "this" F&M object and the specified F&M object (argument object)
   ForceAndRotationObject CalcAverageWith(const ForceAndRotationObject& aOtherFM) const;

protected:
   UtVec3dX mForceVec_lbs;
   UtVec3dX mMaximumOmegaDot_rps2;
   UtVec3dX mStabilizingFrequency_rps;
};
} // namespace six_dof
} // namespace wsf

#endif
