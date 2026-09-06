// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFORBITALTARGETING_HPP
#define WSFORBITALTARGETING_HPP

#include "wsf_space_export.h"

#include <numeric>
#include <utility>
#include <vector>

#include "UtCalendar.hpp"
#include "UtCloneablePtr.hpp"
#include "UtLambertProblem.hpp"
class UtOrbitalPropagatorBase;
#include "UtVec3.hpp"

namespace wsf
{
namespace space
{
class OrbitalTargetPoint;
class OrbitalTargetingCost;
} // namespace space
} // namespace wsf

//! A class that can compute solutions to the targeting problem.
class WSF_SPACE_EXPORT WsfOrbitalTargeting
{
public:
   WsfOrbitalTargeting(const UtCalendar&                     aTime,
                       UtOrbitalPropagatorBase&              aInterceptorProp,
                       const wsf::space::OrbitalTargetPoint& aTarget);

   WsfOrbitalTargeting(const UtCalendar&                     aTime,
                       UtOrbitalPropagatorBase&              aInterceptorProp,
                       const wsf::space::OrbitalTargetPoint& aTarget,
                       bool                                  aAllowHyperbolic,
                       bool                                  aAllowEarthHit);

   WsfOrbitalTargeting(const WsfOrbitalTargeting& aOther) = delete;
   WsfOrbitalTargeting(WsfOrbitalTargeting&&)             = default;

   ~WsfOrbitalTargeting() = default;

   UtLambertProblem::Result
   MinimizeDeltaV(double aDtMax, double aDvMax, bool aRendezvous, double aTimeTolerance, double& aDt, double& aDv) const;

   UtLambertProblem::Result
   MinimizeDeltaT(double aDtMax, double aDvMax, bool aRendezvous, double aTimeTolerance, double& aDt, double& aDv) const;

   //! Solve the targeting problem to minimize the provided cost for the transfer.
   //! This solution will impose a number of conditions on the solution. They include those of the LambertUniversal
   //! function, as well as that the maximum allowed time is not exceeded, and that the solution can be accomplished
   //! without exceeding the provided @p aDvMax. Unless the user of this object explicitly allows it during
   //! construction, solutions may not have transfer orbits that are hyperbolic, or which intersect the Earth.
   //! @param aCost          The CostFunction used to define the optimum solution.
   //! @param aDtMax         The maximum allowable targeting solution time.
   //! @param aDvMax         The maximum allowable targeting delta v for the solution.
   //! @param aRendezvous    Indicates if this targeting is a rendezvous.
   //! @param aTimeTolerance The desired precision in seconds of the solution (i.e., valid to within aTimeTolerance
   //! seconds).
   //!                       A value of <=1E-4 s will generally provide an error of approximately one meter or less.
   //! @param [out] aDt      The transit time of the minimum delta v solution.
   //! @param [out] aDv      The minimum delta v.
   //! @return               true if the solution satisfies the imposed constraints; false otherwise.
   UtLambertProblem::Result MinimizeCost(const wsf::space::OrbitalTargetingCost& aCost,
                                         double                                  aDtMax,
                                         double                                  aDvMax,
                                         bool                                    aRendezvous,
                                         double                                  aTimeTolerance,
                                         double&                                 aDt,
                                         double&                                 aDv) const;

   static bool GoShortWay(const UtVec3d& aLocationInterceptor,
                          const UtVec3d& aVelocityInterceptor,
                          const UtVec3d& aLocationTarget);

   UtLambertProblem::Result Solve(double aDt, bool aRendezvous, double& aDeltaV) const;

   UtLambertProblem::Result Solve(double aDt, UtVec3d& aDeltaV_Intercept, UtVec3d& aDeltaV_Rendezvous) const;

   double GetTolerance() const { return mTolerance; }

   void SetTolerance(double aTolerance) { mTolerance = aTolerance; }

private:
   //! A simple structure collecting the scalar results of a solution attempt.
   struct Sample
   {
      Sample() = default;
      Sample(double aDt, double aDv, bool aHasSolution)
         : mDt{aDt}
         , mDv{aDv}
         , mHasSolution{aHasSolution}
      {
      }

      double mDt{std::numeric_limits<double>::max()};
      double mDv{std::numeric_limits<double>::max()};
      double mCost{std::numeric_limits<double>::max()};
      bool   mHasSolution{false};
   };

   Sample Solve(double aDt, bool aRendezvous) const;

   //! Find ranges of intercept time in which a minimum of the cost function may be found.
   //! This method steps through the given intercept time window, between @p aDtZero and @p aDtMax,
   //! and finds sub-intervals which may contain the minimum cost solution. If the
   //! delta-v for a transfer should be computed as for a rendezvous, set @p aRendezvous to true.
   //! The resulting set of intervals each contain a likely local minimum of the cost function.
   //! @param aCost       The cost to be minimized.
   //! @param aDtZero     The minimum intercept time to consider for a solution.
   //! @param aDtMax      The maximum intercept time to consider for a solution.
   //! @param aDvMax      The maximum allowable delta-v to consider as a solution.
   //! @param aRendezvous true if the computation should be done for a rendezvous; false for an intercept.
   //! @return            A vector of delta-t intervals in which cost minima can be found.
   std::vector<std::pair<double, double>> FindAllRangesCost(const wsf::space::OrbitalTargetingCost& aCost,
                                                            double                                  aDtZero,
                                                            double                                  aDtMax,
                                                            double                                  aDvMax,
                                                            bool                                    aRendezvous) const;

   //! Search a range of interest for the delta-t for which the given cost is minimized.
   //! Search the given range of transfer times, @p aDtLow to @p aDtHigh, for the transfer
   //! time that minimizes the cost of the transfer, considered to be a rendezvous if
   //! @p aRendezvous is true. Only solutions that have delta-v less than or equal to
   //! @p aDvMax are considered. The resulting solution will be within @p aTimeTolerance
   //! of the minimum.
   //! @param aCost          The cost used to define the optimal solution.
   //! @param aDtLow         The smallest transfer time over which to search.
   //! @param aDtHigh        The largest transfer time over which to search.
   //! @param aRendezvous    true if the computation should be for a rendezvous; false for an intercept.
   //! @param aDvMax         The maximum allowed delta-v.
   //! @param aTimeTolerance The tolerance in delta-t for finding a solution.
   //! @return               The cost optimal solution.
   UtLambertProblem::Result SearchCost(const wsf::space::OrbitalTargetingCost& aCost,
                                       double                                  aDtLow,
                                       double                                  aDtHigh,
                                       bool                                    aRendezvous,
                                       double                                  aDvMax,
                                       double                                  aTimeTolerance,
                                       double&                                 aDeltaV) const;

   const UtCalendar                               mTime;
   UtOrbitalPropagatorBase&                       mInterceptorProp;
   UtCloneablePtr<wsf::space::OrbitalTargetPoint> mTargetPtr;
   double                                         mTolerance{1.0e-9};
   bool                                           mAllowEarthHit{false};
};

#endif
