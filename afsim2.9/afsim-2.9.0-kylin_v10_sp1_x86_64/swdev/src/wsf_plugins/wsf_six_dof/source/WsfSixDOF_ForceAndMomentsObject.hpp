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

#ifndef WSFSIXDOFFORCEANDMOMENTOBJECT_HPP
#define WSFSIXDOFFORCEANDMOMENTOBJECT_HPP

#include "wsf_six_dof_export.h"

#include "UtVec3dX.hpp"

namespace wsf
{
namespace six_dof
{
// The ForceAndMomentsObject class provides a means to "build-up" forces
// and moments (F&M) provides a collection of state data for SixDOF Movers.
// It should be noted that the class assumes that forces and moments are in
// body-coordinates.
class WSF_SIX_DOF_EXPORT ForceAndMomentsObject
{
public:
   ForceAndMomentsObject() = default;

   ForceAndMomentsObject* Clone() const;

   // This adds aSrc F&M to the current F&M object, transforming the aSrc F&M
   // as required for the current reference point
   ForceAndMomentsObject& operator+=(const ForceAndMomentsObject& aSrc);

   // This clears the force and moment but leaves the reference point unchanged
   void ClearForcesAndMoments();

   // This clears the force, moment, and reference point
   void ClearReferenceAndForceAndMoment();

   // This moves the reference point to the specified point, and modifies
   // the current F&M accordingly
   void MoveRefPoint_ft(const UtVec3dX& aSpecifiedPoint_ft);

   // This moves the reference point to the specified point, and modifies
   // the current F&M accordingly. This version uses attitude data.
   void RelocateForce_ft(const UtVec3dX& aSpecifiedPoint_ft, const UtVec3dX& aAngRelToParentYPR_rad);

   // This adds the specified force and moment applied at the specified point.
   // The function calculates the new F&M at the internal reference point.
   void AddForceAndMomentAtSpecifiedPoint(const UtVec3dX& aForceVec_lbs,
                                          const UtVec3dX& aMomentVec_ftlbs,
                                          const UtVec3dX& aSpecifiedPoint_ft);

   // This adds the specified force and moment applied at the reference point.
   void AddForceAndMomentAtReferencePoint(const UtVec3dX& aForceVec_lbs, const UtVec3dX& aMomentVec_ftlbs);

   // This adds the specified force at the specified point. The function calculates
   // the new F&M at the internal reference point.
   void AddForceAtSpecifiedPoint(const UtVec3dX& aForceVec_lbs, const UtVec3dX& aSpecifiedPoint_ft);

   // This adds the specified force at the reference point.
   void AddForceAtReferencePoint(const UtVec3dX& aForceVec_lbs);

   // This adds the specified moment. The function calculates the new F&M at
   // the internal reference point.
   void AddMoment(const UtVec3dX& aMomentVec_ftlbs);

   // This returns (via references) the current force and moment at the specified
   // point, using the current F&M at the internal reference point.
   void GetForceAndMomentAtSpecifiedPoint(UtVec3dX&       aForceVec_lbs,
                                          UtVec3dX&       aMomentVec_ftlbs,
                                          const UtVec3dX& aSpecifiedPoint_ft) const;

   // This returns (via references) the current force and moment at the current
   // internal reference point.
   void GetForceAndMomentAtCurrentRefPoint(UtVec3dX& aForceVec_lbs, UtVec3dX& aMomentVec_ftlbs) const;

   // This returns the magnitude of the force in lbs
   double GetForceMagnitude_lbs() const;

   // This returns the vector of the force in lbs
   UtVec3dX GetForceAtRefPoint_lbs() const;

   // This returns the vector of the moment in ft-lbs
   UtVec3dX GetMomentAtRefPoint_ftlbs() const;

   // This limits the magnitude of the force in lbs, reducing it, if needed
   void LimitMaxForceMagnitude_lbs(double aMaxForce_lbs);

   // This limits the magnitude of the moment in ft-lbs, reducing it, if needed
   void LimitMomentMagnitude_ftlbs(double aMaxMoment_ftlbs);

   // This returns a ForceAndMomentsObject that is the "average" of
   // the "this" F&M object and the specified F&M object (argument object)
   ForceAndMomentsObject CalcAverageWith(const ForceAndMomentsObject& aOtherFM) const;

   // This returns the vector of the force in lbs
   UtVec3dX GetReferencePoint_ft() const;

protected:
   UtVec3dX mForceVec_lbs;
   UtVec3dX mMomentVec_ftlbs;
   UtVec3dX mRefPoint_ft;
};
} // namespace six_dof
} // namespace wsf

#endif
