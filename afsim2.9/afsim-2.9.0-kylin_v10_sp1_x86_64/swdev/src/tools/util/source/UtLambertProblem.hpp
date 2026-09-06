// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTLAMBERTPROBLEM_HPP
#define UTLAMBERTPROBLEM_HPP

#include "ut_export.h"


namespace ut
{
class CentralBodyEllipsoid;
class OrbitalState;
} // namespace ut
#include "UtCloneablePtr.hpp"
#include "UtVec3.hpp"

namespace UtLambertProblem
{

class UT_EXPORT Result
{
public:
   Result() = default;

   Result(double                          aTransferTime,
          const UtVec3d&                  aInitialPosition,
          const UtVec3d&                  aFinalPosition,
          const ut::CentralBodyEllipsoid& aEllipsoid)
      : mTransferTime{aTransferTime}
      , mInitialPosition{aInitialPosition}
      , mFinalPosition{aFinalPosition}
      , mEllipsoidPtr{ut::clone(aEllipsoid)}
   {
   }

   double         GetTransferTime() const { return mTransferTime; }
   const UtVec3d& GetInitialPosition() const { return mInitialPosition; }
   const UtVec3d& GetFinalPosition() const { return mFinalPosition; }
   const UtVec3d& GetInitialVelocity() const { return mInitialVelocity; }
   const UtVec3d& GetFinalVelocity() const { return mFinalVelocity; }

   //! Return if the Lambert solver found a solution.
   //! This is not the same as saying the solution is acceptable to the client. The
   //! results from IsHyperbolic and HitsCentralBody should be examined to see if
   //! the other requirements on the solution are met.
   bool IsSolution() const { return mIsSolution; }

   //! Return if the solution was an unbound orbit.
   //! This value is only guaranteed to be accurate if IsSolution returns true.
   bool IsHyperbolic() const { return mIsHyperbolic; }

   //! Return if the solution intersects the central body.
   //! This value is only guaranteed to be accurate if IsSoltion returns true.
   bool HitsCentralBody() const { return mHitsCentralBody; }

   bool Assess(bool aAllowHyperbolic, bool aAllowHitsCentralBody) const;

   void SetSolution(bool aIsSolution, const UtVec3d& aInitialVelocity, const UtVec3d& aFinalVelocity);

private:
   double                                     mTransferTime{};
   UtVec3d                                    mInitialPosition{};
   UtVec3d                                    mFinalPosition{};
   UtVec3d                                    mInitialVelocity{};
   UtVec3d                                    mFinalVelocity{};
   ut::CloneablePtr<ut::CentralBodyEllipsoid> mEllipsoidPtr{};
   bool                                       mIsSolution{false};
   bool                                       mIsHyperbolic{false};
   bool                                       mHitsCentralBody{false};
};

bool UT_EXPORT Gauss(const UtVec3d& aLocationInertial_1,
                     const UtVec3d& aLocationInertial_2,
                     double         aDt,
                     UtVec3d&       aVelocityInertial_1,
                     UtVec3d&       aVelocityInertial_2,
                     double         aGravitationalParameter,
                     bool           aIsShortWay,
                     double         aConvergenceTolerance = 1.0e-12);

Result UT_EXPORT Universal(const UtVec3d&                  aLocationInertial_1,
                           const UtVec3d&                  aLocationInertial_2,
                           double                          aDt,
                           const ut::CentralBodyEllipsoid& aEllipsoid,
                           bool                            aShortWay,
                           double                          aConvergenceTolerance = 1.0e-12);

bool UT_EXPORT Hits(const ut::CentralBodyEllipsoid& aEllipsoid,
                    const UtVec3d&                  aLocationInertial_1,
                    const UtVec3d&                  aLocationInertial_2,
                    const UtVec3d&                  aVelocityInertial_1,
                    const UtVec3d&                  aVelocityInertial_2);

bool UT_EXPORT Hyperbolic(double aGravitationalPararmeter, const UtVec3d& aLocationInertial, const UtVec3d& aVelocityInertial);

} // namespace UtLambertProblem

#endif // UTLAMBERTPROBLEM_HPP
