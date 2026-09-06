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

#ifndef WSFLIBRATIONPOINT_HPP
#define WSFLIBRATIONPOINT_HPP

#include "wsf_space_export.h"

#include <memory>
#include <string>
#include <vector>

#include "UtCalendar.hpp"
#include "UtCentralBody.hpp"
#include "UtCloneablePtr.hpp"
#include "UtMat3.hpp"
#include "UtVec3.hpp"

namespace wsf
{
namespace space
{

//! This class provides computation for the libration points of a system.
//!
//! This class uses two reference frames: the AFSIM standard ECI frame, and the
//! rotating frame (RF). The rotating frame has its origin at the primary body,
//! an x-axis pointing toward the secondary body, a z-axis normal to the plane
//! of the orbit in the direction of the orbital angular momentum, and a
//! y-axis that completes the right-handed triple.
class WSF_SPACE_EXPORT LibrationPoint
{
public:
   //! The supported primary-secondary body pairs.
   enum class System
   {
      cSUN_EARTH,
      cEARTH_MOON,
      cSUN_JUPITER,
      cUNKNOWN
   };

   //! Identifiers for the five libration points.
   enum class Point
   {
      cL1,
      cL2,
      cL3,
      cL4,
      cL5,
      cUNKNOWN
   };

   explicit LibrationPoint(System aSystem);
   ~LibrationPoint() = default;

   //! Return the system for which this object was created.
   System GetSystem() const { return mSystem; }

   UtVec3d GetPositionECI(Point aPoint, const UtCalendar& aEpoch) const;
   UtVec3d GetVelocityECI(Point aPoint, const UtCalendar& aEpoch) const;

   UtVec3d TransformPositionECItoRF(const UtCalendar& aEpoch, const UtVec3d& aPositionECI) const;
   UtVec3d TransformPositionRFtoECI(const UtCalendar& aEpoch, const UtVec3d& aPositionRF) const;
   UtVec3d TransformVelocityECItoRF(const UtCalendar& aEpoch, const UtVec3d& aPositionECI, const UtVec3d& aVelocityECI) const;
   UtVec3d TransformVelocityRFtoECI(const UtCalendar& aEpoch, const UtVec3d& aPositionRF, const UtVec3d& aVelocityRF) const;

   double GetSystemPeriod() const;

   static System                   GetSystemFromIdentifier(const std::string& aIdentifier);
   static std::string              GetIdentifierFromSystem(System aSystem);
   static std::vector<std::string> GetSystemIdentifiers();
   static Point                    GetPointFromIdentifier(const std::string& aIdentifier);
   static std::string              GetIdentifierFromPoint(Point aPoint);
   static std::vector<std::string> GetPointIdentifiers();
   static bool                     ValidLibrationPoint(System aSystem, Point aPoint);

private:
   static std::unique_ptr<ut::CentralBody> GetPrimaryBody(System aSystem);
   static std::unique_ptr<ut::CentralBody> GetSecondaryBody(System aSystem);
   void                                    ComputeGamma();
   double                                  F_Gamma1(double aGamma) const;
   double                                  F_PrimeGamma1(double aGamma) const;
   double                                  F_Gamma2(double aGamma) const;
   double                                  F_PrimeGamma2(double aGamma) const;
   double                                  F_Gamma3(double aGamma) const;
   double                                  F_PrimeGamma3(double aGamma) const;
   UtVec3d                                 GetPointLocation(Point aPoint) const;
   void                                    UpdateTransform(const UtCalendar& aEpoch) const;
   static void SetMatrixFromVectors(UtMat3d& aMatrix, const UtVec3d& aColumn0, const UtVec3d& aColumn1, const UtVec3d& aColumn2);

   System                            mSystem;
   ut::CloneablePtr<ut::CentralBody> mPrimaryBody;
   ut::CloneablePtr<ut::CentralBody> mSecondaryBody;
   double                            mMuStar; // Related to the mass ratio
   double                            mGamma1; // non-dimensional position of L1
   double                            mGamma2; // non-dimensional position of L2
   double                            mGamma3; // non-dimensional position of L3

   // Reusable data updated each time the calculations are done for a new epoch.
   mutable UtVec3d    mPosDiff;
   mutable UtVec3d    mVelDiff;
   mutable UtVec3d    mPosOriginECI;
   mutable UtVec3d    mVelOriginECI;
   mutable UtMat3d    mTransform;    // transformation from rotating, primary-centered frame
   mutable UtMat3d    mTransformDot; // the time derivative of the transform
   mutable UtCalendar mCurrentEpoch; // Current time of the transform
};

} // namespace space
} // namespace wsf

#endif // WSFLIBRATIONPOINT_HPP
