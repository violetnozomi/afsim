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

#include "WsfOrbitalTargeting.hpp"

#include <algorithm>

#include "UtGoldenSectionSearch.hpp"
#include "UtLambertProblem.hpp"
#include "UtMemory.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "WsfOrbitalTargetPoint.hpp"
#include "WsfOrbitalTargetingCost.hpp"

WsfOrbitalTargeting::WsfOrbitalTargeting(const UtCalendar&                     aTime,
                                         UtOrbitalPropagatorBase&              aInterceptorProp,
                                         const wsf::space::OrbitalTargetPoint& aTarget)
   : mTime{aTime}
   , mInterceptorProp(aInterceptorProp)
   , mTargetPtr{ut::clone(aTarget)}
{
}

//! Construct the object with the option of relaxing solution constraints.
//! The constructor allows users to explicitly allow for solutions that have transfer orbits that are
//! hyperbolic, or which intersect the Earth.
//! @param aTime            The time at which to begin the targeting operation.
//! @param aInterceptorProp A propagator representing the motion of the intercepting spacecraft.
//! @param aTarget          A target point to be intercepted.
//! @param aAllowHyperbolic Indicates if hyperbolic transfer orbits are allowed.
//! @param aAllowEarthHit   Indicates if Earth intersecting transfer orbits are allowed.
WsfOrbitalTargeting::WsfOrbitalTargeting(const UtCalendar&                     aTime,
                                         UtOrbitalPropagatorBase&              aInterceptorProp,
                                         const wsf::space::OrbitalTargetPoint& aTarget,
                                         bool                                  aAllowHyperbolic,
                                         bool                                  aAllowEarthHit)
   : mTime{aTime}
   , mInterceptorProp(aInterceptorProp)
   , mTargetPtr{ut::clone(aTarget)}
   , mAllowEarthHit{aAllowEarthHit}
{
}

//! Solve the targeting problem to minimize delta V of the transfer.
//! This solution will impose a number of conditions on the solution. They include those of the LambertUniversal function,
//! as well as that the maximum allowed time is not exceeded, and that the solution can be accomplished without exceeding
//! the provided @p aDvMax. Unless the user of this object explicitly allows it during construction, solutions may not
//! have transfer orbits that intersect the central body.
//! @param aDtMax         The maximum allowable targeting solution time.
//! @param aDvMax         The maximum allowable targeting delta v for the solution.
//! @param aRendezvous    Indicates if this targeting is a rendezvous.
//! @param aTimeTolerance The desired precision in seconds of the solution (i.e., valid to within aTimeTolerance seconds).
//!                       A value of <=1E-4 s will generally provide an error of approximately one meter or less.
//! @param [out] aDt      The transit time of the minimum delta v solution.
//! @param [out] aDv      The minimum delta v.
//! @return               The results of the optimal delta-v solution.
UtLambertProblem::Result WsfOrbitalTargeting::MinimizeDeltaV(double  aDtMax,
                                                             double  aDvMax,
                                                             bool    aRendezvous,
                                                             double  aTimeTolerance,
                                                             double& aDt,
                                                             double& aDv) const
{
   return MinimizeCost(wsf::space::BlendedCost{}, aDtMax, aDvMax, aRendezvous, aTimeTolerance, aDt, aDv);
}

//! Solve the targeting problem to minimize delta t of the transfer.
//! This solution will impose a number of conditions on the solution. They include those of the LambertUniversal
//! function, as well as that the maximum allowed delta t is not exceeded, and that the solution can be accomplished
//! without exceeding the provided @p aDvMax. Unless the user of this object explicitly allows it during construction,
//! solutions may not have transfer orbits that intersect the Earth.
//! @param aDtMax         The maximum allowable targeting solution time.
//! @param aDvMax         The maximum allowable targeting delta v for the solution.
//! @param aRendezvous    Indicates if this targeting is a rendezvous.
//! @param aTimeTolerance The desired precision in seconds of the solution
//!                       (i.e., there is no earlier constrained solution to within aTimeTolerance).
//! @param [out] aDt      The minimum time of the solution.
//! @param [out] aDv      The delta v of the minimum time solution.
//! @return               The result of the solution of the targeting problem for the minimum time solution.
UtLambertProblem::Result WsfOrbitalTargeting::MinimizeDeltaT(double  aDtMax,
                                                             double  aDvMax,
                                                             bool    aRendezvous,
                                                             double  aTimeTolerance,
                                                             double& aDt,
                                                             double& aDv) const
{
   // The goal is to find the allowed solution closest to a delta-t of zero. Any targeting problem will
   // be impossible at a delta-t of zero, so as delta-t is changed from zero to larger values, if there is
   // a possible transfer at all, the solution must go from being disallowed to allowed.

   // First, find an interval in which the solution changes from disallowed to allowed.
   static constexpr int    cN_SAMPLES     = 40;
   static constexpr double cMAX_STEP_SIZE = 3600.0;
   double                  searchStep     = aDtMax / cN_SAMPLES;
   double                  lowerBound     = 0.0;
   double                  upperBound     = searchStep;

   double                   bestDv;
   UtLambertProblem::Result bestSolution = Solve(upperBound, aRendezvous, bestDv);
   while (!bestSolution.Assess(mInterceptorProp.HyperbolicPropagationAllowed(), mAllowEarthHit) || bestDv > aDvMax)
   {
      // Upper bound is not yet a valid solution.
      searchStep   = std::min(searchStep * 2.0, cMAX_STEP_SIZE);
      lowerBound   = upperBound;
      upperBound   = std::min(upperBound + searchStep, aDtMax); // Do not search past the maximum allowed delta-t.
      bestSolution = Solve(upperBound, aRendezvous, bestDv);
      if (upperBound == aDtMax &&
          (!bestSolution.Assess(mInterceptorProp.HyperbolicPropagationAllowed(), mAllowEarthHit) || bestDv > aDvMax))
      {
         // If we are at the upper bound of delta-t and there was no solution,
         // we have no solution, so return false.
         return UtLambertProblem::Result{};
      }
   }

   // At this point upperBound is past the transition, lowerBound is before, bestSolution must
   // evaluate to true, and it must satisfy the delta-v constraint.

   // Second, bisect on that interval to find the point at lowest delta-t where the transition
   // to allowable occurs.
   double rangeT = (upperBound - lowerBound) / 2.0;
   while (rangeT > aTimeTolerance)
   {
      double deltaT = (upperBound + lowerBound) / 2.0;
      double deltaV;
      auto   result = Solve(deltaT, aRendezvous, deltaV);
      if (result.Assess(mInterceptorProp.HyperbolicPropagationAllowed(), mAllowEarthHit) && deltaV <= aDvMax)
      {
         // The midpoint is a solution, so move upper limit down to midpoint.
         bestSolution = result;
         bestDv       = deltaV;
         upperBound   = deltaT;
      }
      else
      {
         // The midpoint is not a solution, so move lower limit up to midpoint.
         lowerBound = deltaT;
      }
      rangeT = (upperBound - lowerBound) / 2.0;
   }

   aDt = bestSolution.GetTransferTime();
   aDv = bestDv;
   return bestSolution;
}

UtLambertProblem::Result WsfOrbitalTargeting::MinimizeCost(const wsf::space::OrbitalTargetingCost& aCost,
                                                           double                                  aDtMax,
                                                           double                                  aDvMax,
                                                           bool                                    aRendezvous,
                                                           double                                  aTimeTolerance,
                                                           double&                                 aDt,
                                                           double&                                 aDv) const
{
   double                   minCost{std::numeric_limits<double>::max()};
   double                   bestDv{std::numeric_limits<double>::max()};
   UtLambertProblem::Result bestResult;
   auto                     ranges = FindAllRangesCost(aCost, 0.0, aDtMax, aDvMax, aRendezvous);
   for (const auto& range : ranges)
   {
      double deltaV;
      auto   local = SearchCost(aCost, range.first, range.second, aRendezvous, aDvMax, aTimeTolerance, deltaV);
      double cost  = aCost(local.GetTransferTime(), deltaV);
      if (local.Assess(mInterceptorProp.HyperbolicPropagationAllowed(), mAllowEarthHit) && cost < minCost)
      {
         minCost    = cost;
         bestDv     = deltaV;
         bestResult = local;
      }
   }
   if (bestResult.Assess(mInterceptorProp.HyperbolicPropagationAllowed(), mAllowEarthHit))
   {
      aDt = bestResult.GetTransferTime();
      aDv = bestDv;
   }
   return bestResult;
}

//! Decide if the Lambert solution should proceed the short or long way
//! This is taken from Vallado, p. 506.
//! @param  aLocationInterceptor The ECI location of the interceptor
//! @param  aVelocityInterceptor The ECI velocity of the interceptor
//! @param  aLocationTarget      The ECI location of the target.
//! @return                      true if the short way is to be taken; false if the long way is to be taken.
bool WsfOrbitalTargeting::GoShortWay(const UtVec3d& aLocationInterceptor,
                                     const UtVec3d& aVelocityInterceptor,
                                     const UtVec3d& aLocationTarget)
{
   UtVec3d h;
   h.CrossProduct(aLocationInterceptor, aVelocityInterceptor);
   UtVec3d trans;
   trans.CrossProduct(aLocationInterceptor, aLocationTarget);
   return (trans.DotProduct(h) > 0.0);
}

//! Solve the targeting problem.
//! This will solve a single instance of the targeting problem for a given time to intercept, @p aDt.
//! Unless the user allows for it during construction of this object, this method will return false
//! if the resulting solution intersects the Earth on the transfer orbit.
//! This is essentially Algorithm 61 from Vallado (pp. 503-504).
//! @param aDt           The time of the targeting solution.
//! @param aRendezvous   Indicates if the solution is for a rendezvous.
//! @param [out] aDeltaV The resulting delta V for the solution.
//! @return              The results of the solution of the targeting problem.
UtLambertProblem::Result WsfOrbitalTargeting::Solve(double aDt, bool aRendezvous, double& aDeltaV) const
{
   UtVec3d deltaV_Intercept;
   UtVec3d deltaV_Rendezvous;
   auto    retval = Solve(aDt, deltaV_Intercept, deltaV_Rendezvous);
   aDeltaV        = deltaV_Intercept.Magnitude();
   if (aRendezvous)
   {
      aDeltaV += deltaV_Rendezvous.Magnitude();
   }
   return retval;
}

//! Solve the targeting problem.
//! This will solve a single instance of the targeting problem for a given time to intercept, @p aDt.
//! Unless the user allows for it during construction of this object, this method will return false
//! if the resulting solution is hyperbolic, or if it intersects the Earth on the transfer orbit.
//! This is essentially Algorithm 61 from Vallado (pp. 503-504). This routine computes both vector
//! delta-V values; the caller is responsible for including the rendezvous portion if needed.
//! @param aDt                      The time of the targeting solution.
//! @param [out] aDeltaV_Intercept  The delta V vector for the intercept portion of the maneuver.
//! @param [out] aDeltaV_Rendezvous The delta V vector for the rendezvous portion of the maneuver.
//! @return                         The results of the targeting problem.
UtLambertProblem::Result WsfOrbitalTargeting::Solve(double aDt, UtVec3d& aDeltaV_Intercept, UtVec3d& aDeltaV_Rendezvous) const
{
   UtCalendar then = mTime;
   then.AdvanceTimeBy(aDt);
   mTargetPtr->Update(then);
   ut::OrbitalState::Vector osvInertial    = mTargetPtr->GetOrbitalStateVectorInertial();
   UtVec3d                  locationTarget = osvInertial.GetLocation();
   UtVec3d                  velocityTarget = osvInertial.GetVelocity();
   mTargetPtr->Update(mTime);

   mInterceptorProp.Update(mTime);
   osvInertial                 = mInterceptorProp.GetOrbitalState().GetOrbitalStateVectorInertial();
   UtVec3d locationInterceptor = osvInertial.GetLocation();
   UtVec3d velocityInterceptor = osvInertial.GetVelocity();
   bool    shortWay            = GoShortWay(locationInterceptor, velocityInterceptor, locationTarget);

   auto result = mInterceptorProp.SolveLambertProblem(mInterceptorProp.GetCentralBody(),
                                                      locationInterceptor,
                                                      locationTarget,
                                                      aDt,
                                                      shortWay,
                                                      mAllowEarthHit,
                                                      mTolerance);

   UtVec3d deltaV_1;
   deltaV_1.Subtract(result.GetInitialVelocity(), velocityInterceptor);
   UtVec3d deltaV_2;
   deltaV_2.Subtract(velocityTarget, result.GetFinalVelocity());

   aDeltaV_Intercept  = deltaV_1;
   aDeltaV_Rendezvous = deltaV_2;

   return result;
}

//! Solve the Lambert problem.
//! This will solve the lambert problem for the given time to intercept or rendezvous, @p aDt.
//! @param aDt         The transfer time for which a solution delta-v is sought.
//! @param aRendezvous true if the solution should be for a rendezvous; false for an intercept.
//! @return            The results of the solution.
WsfOrbitalTargeting::Sample WsfOrbitalTargeting::Solve(double aDt, bool aRendezvous) const
{
   double dv;
   auto   result = Solve(aDt, aRendezvous, dv);
   return Sample{aDt, dv, result.IsSolution()};
}

std::vector<std::pair<double, double>> WsfOrbitalTargeting::FindAllRangesCost(const wsf::space::OrbitalTargetingCost& aCost,
                                                                              double aDtZero,
                                                                              double aDtMax,
                                                                              double aDvMax,
                                                                              bool   aRendezvous) const
{
   std::vector<std::pair<double, double>> retval{};

   // Use cN_SAMPLES samples, but allow step sizes to be no larger than cMAX_STEP_SIZE.
   static constexpr int    cN_SAMPLES     = 40;
   static constexpr double cMAX_STEP_SIZE = 3600.0;
   double                  dtStep         = std::min((aDtMax - aDtZero) / cN_SAMPLES, cMAX_STEP_SIZE);
   auto                    nSamples       = static_cast<std::size_t>(std::floor((aDtMax - aDtZero) / dtStep));

   // Collect results from the samples
   std::vector<Sample> results(nSamples + 1);
   for (std::size_t i = 0; i <= nSamples; ++i)
   {
      double dt        = aDtZero + dtStep * i;
      results[i]       = Solve(dt, aRendezvous);
      results[i].mCost = aCost(results[i].mDt, results[i].mDv);
   }

   // Step over the samples, examining the pattern of three consecutive samples. This looks
   // for patterns that suggest a local minimum (e.g., the central sample is lower than the
   // samples to the left or right). For any found the interval is added to the return
   // vector.
   for (std::size_t i = 1; i < nSamples; ++i)
   {
      // Analyze point only if the point is a solution satisfying the constraints
      const Sample& center = results[i];
      const Sample& left   = results[i - 1];
      const Sample& right  = results[i + 1];
      if (center.mHasSolution && center.mDv <= aDvMax)
      {
         bool leftOk  = left.mDv <= aDvMax;
         bool rightOk = right.mDv <= aDvMax;

         bool isOfInterest{false};
         if (leftOk)
         {
            if (rightOk)
            {
               if (center.mCost < left.mCost && center.mCost < right.mCost)
               {
                  // center is lowest of the three, suggesting a local minimum is somewhere between left and right.
                  isOfInterest = true;
               }
               else if (center.mCost == left.mCost && center.mCost < right.mCost)
               {
                  // center and left are equal and lower than right, suggesting a local minimum is somewhere
                  // between left and right.
                  isOfInterest = true;
               }
               else if (center.mCost < left.mCost && center.mCost == right.mCost)
               {
                  // center and right are equal and lower than left, suggesting a local minimum is somewhere
                  // between left and right.
                  isOfInterest = true;
               }
               else if (left.mCost < center.mCost && center.mCost < right.mCost)
               {
                  // The cost values are monotonically increasing. If this occurs at a boundary,
                  // either the overall boundary of the interval, or a transition from regions of
                  // no solution to regions with a solution, a local minimum is likely to exist at
                  // the transition point.
                  isOfInterest = i == 1 || (!left.mHasSolution && right.mHasSolution);
               }
               else if (left.mCost > center.mCost && center.mCost > right.mCost)
               {
                  // The cost values are monotonically decreasing. If this occurs at a boundary,
                  // either the overall boundary of the interval, or a transition from regions with
                  // solutions to regions with no solution, a local minimum is likely to exist at
                  // the transition point.
                  isOfInterest = i == nSamples - 1 || (!right.mHasSolution && left.mHasSolution);
               }
            }
            else
            {
               // This is an interval of interest if the center is less than or equal to the left
               isOfInterest = center.mCost <= left.mCost;
            }
         }
         else
         {
            if (rightOk)
            {
               // This is an interval of interest if the center is less than or equal to the right
               isOfInterest = center.mCost <= right.mCost;
            }
            else
            {
               // If the center point satisfies the constraints, but neither left nor right do,
               // the there should be some point between left and right which satisfies all the
               // constraints which has a minimum delta-v.
               isOfInterest = true;
            }
         }

         if (isOfInterest)
         {
            retval.emplace_back(std::make_pair(left.mDt, right.mDt));
         }
      }
   }

   return retval;
}

UtLambertProblem::Result WsfOrbitalTargeting::SearchCost(const wsf::space::OrbitalTargetingCost& aCost,
                                                         double                                  aDtLow,
                                                         double                                  aDtHigh,
                                                         bool                                    aRendezvous,
                                                         double                                  aDvMax,
                                                         double                                  aTimeTolerance,
                                                         double&                                 aDeltaV) const
{
   auto func = [&aCost, aRendezvous, aDvMax, this](double aTime) -> double
   {
      double deltaV;
      if (!Solve(aTime, aRendezvous, deltaV).IsSolution() || deltaV > aDvMax)
      {
         return std::numeric_limits<double>::max();
      }
      return aCost(aTime, deltaV);
   };

   return Solve(UtGoldenSectionSearch(func, aDtLow, aDtHigh, aTimeTolerance), aRendezvous, aDeltaV);
}
