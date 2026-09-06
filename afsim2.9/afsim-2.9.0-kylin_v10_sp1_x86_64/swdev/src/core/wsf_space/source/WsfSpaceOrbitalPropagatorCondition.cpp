// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2022 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSpaceOrbitalPropagatorCondition.hpp"

#include "UtBinarySearch.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "UtTwoLineElement.hpp"
#include "WsfSpaceOrbitalPropagatorConditionVisitor.hpp"

namespace
{
constexpr size_t cMAX_ITERATIONS{60};
constexpr double cSEARCH_TOLERANCE{1.0e-8};
constexpr double cINCREMENT_RATIO{1.5};

} // namespace

namespace wsf
{
namespace space
{
bool OrbitalPropagatorCondition::ProcessInput(UtInput& aInput)
{
   // The base class does not currently handle processing the orbit number input
   // for the condition. The current grammar for the condition is not in a block
   // and the 'orbit nn' input command would precede the token that specifies
   // the type of the condition, so the specific type would need to exist before
   // the specific type has been read from the input. The solution currently is
   // to have WsfEvent::ProcessInput call SetOrbitNumber on the specific type
   // created when needed.
   return true;
}

// OPTIMIZING CONDITION

bool OrbitalPropagatorOptimizingCondition::GetTimeToCondition(const UtOrbitalPropagatorBase& aPropagator,
                                                              double&                        aTimeToCondition) const
{
   // The optimizing conditions are currently only defined for non-hyperbolic orbits.
   if (aPropagator.GetOrbitalState().OrbitIsHyperbolic())
   {
      aTimeToCondition = -1.0;
      return false;
   }

   if (HandleSpecialCases(aPropagator, aTimeToCondition))
   {
      return true;
   }

   auto initialGuess = InitialGuess(aPropagator);
   if (!initialGuess.first)
   {
      // Cases that are unable to get an initial guess are typically
      // deficient in some way that makes it impossible to determine
      // when the condition is satisfied (e.g., RAAN for an equatorial
      // orbit).
      aTimeToCondition = -1.0;
      return false;
   }

   auto propPtr = ut::clone(aPropagator);
   propPtr->Initialize(aPropagator.GetOrbitalState());

   // Handle the offset by the number of orbits here. Then later the search
   // does not need to account for this offset.
   UtCalendar baseEpoch = propPtr->GetCurrentTime();
   double     period    = propPtr->GetOrbitalState().GetOrbitalElements().GetPeriod();
   if (mOrbitNumber > 0)
   {
      baseEpoch.AdvanceTimeBy(mOrbitNumber * period);
      propPtr->Update(baseEpoch);
      initialGuess.second -= mOrbitNumber * period;
   }

   UtBinarySearch search{};
   bool           searchFailed;
   bool           searchConverged;
   bool           valueBracketed;
   double         nextTrialValue;

   // Perform some initial evaluations of the condition to bracked the solution.
   double delta = 0.125 * period;
   double firstTrial{};
   double secondTrial{};
   if (initialGuess.second > 0.0)
   {
      firstTrial  = std::max(initialGuess.second - delta, 0.0);
      secondTrial = initialGuess.second + delta;
   }
   else
   {
      // Shortcut if the initial guess returns zero as the time to the condition
      // being satisfied.
      aTimeToCondition = 0.0;
      return true;
   }
   bool firstResult  = Objective(*propPtr, baseEpoch, firstTrial);
   bool secondResult = Objective(*propPtr, baseEpoch, secondTrial);
   if (firstResult == secondResult)
   {
      // In some cases, the initial guess is bad, or the initial range is insufficient to
      // bracket the solution.
      firstTrial   = initialGuess.second - 0.5 * delta;
      secondTrial  = initialGuess.second + 2.0 * delta;
      firstResult  = Objective(*propPtr, baseEpoch, firstTrial);
      secondResult = Objective(*propPtr, baseEpoch, secondTrial);
      if (firstResult == secondResult)
      {
         auto msg = ut::log::error() << "Unable to bracked solution with initial guess.";
         msg.AddNote() << "Condition type: " << GetConditionString();
         aTimeToCondition = -1.0;
         return false;
      }
   }

   // Setup the binary seach to look for a maximum where the condition is true, or
   // a minimum where the condition is true, based on the two initial values.
   if (firstResult && !secondResult)
   {
      search.Initialize(true, cMAX_ITERATIONS, cSEARCH_TOLERANCE, cINCREMENT_RATIO);
   }
   else if (!firstResult && secondResult)
   {
      search.Initialize(false, cMAX_ITERATIONS, cSEARCH_TOLERANCE, cINCREMENT_RATIO);
      // UtBinary search will not log a failure until a success has been logged.
      // So the two values are swapped to assure that the solution is bracketed.
      std::swap(firstTrial, secondTrial);
      std::swap(firstResult, secondResult);
   }
   search.Update(firstTrial, firstResult, searchFailed, searchConverged, valueBracketed, nextTrialValue);
   search.Update(secondTrial, secondResult, searchFailed, searchConverged, valueBracketed, nextTrialValue);
   // After the previous, nextTrialValue will make a good next step

   while (!searchFailed && !searchConverged)
   {
      bool nextResult = Objective(*propPtr, baseEpoch, nextTrialValue);
      search.Update(nextTrialValue, nextResult, searchFailed, searchConverged, valueBracketed, nextTrialValue);
   }

   bool retval{true};
   if (searchConverged)
   {
      aTimeToCondition = nextTrialValue + mOrbitNumber * period;
   }
   else if (!valueBracketed)
   {
      auto msg = ut::log::error() << "Unable to bracket orbital propagator condition solution.";
      msg.AddNote() << "Condition type: " << GetConditionString();
      retval = false;
   }
   else // bracketed but not converged
   {
      auto msg = ut::log::error() << "Unable to converge on orbital propagator condition solution.";
      msg.AddNote() << "Condition type: " << GetConditionString();
      msg.AddNote() << "Max. iterations: " << cMAX_ITERATIONS;
      msg.AddNote() << "Tolerance: " << cSEARCH_TOLERANCE;
      retval = false;
   }
   return retval;
}

// NONE

bool NoneCondition::GetTimeToCondition(const UtOrbitalPropagatorBase& aPropagator, double& aTimeToCondition) const
{
   aTimeToCondition = 0.0;
   if (!aPropagator.GetOrbitalState().OrbitIsHyperbolic())
   {
      aTimeToCondition += mOrbitNumber * aPropagator.GetOrbitalState().GetOrbitalElements().GetPeriod();
   }
   return true;
}

void NoneCondition::Accept(OrbitalPropagatorConditionVisitor& aVisitor)
{
   aVisitor.Accept(*this);
}

// RELATIVE TIME

bool RelativeTimeCondition::ProcessInput(UtInput& aInput)
{
   aInput.ReadUnitValue(mOffsetTime);
   aInput.ValueGreaterOrEqual<double>(mOffsetTime, 0.0);
   return true;
}

bool RelativeTimeCondition::ValidateParameterRanges() const
{
   bool retval{true};
   if (mOffsetTime < 0.0)
   {
      ut::log::error() << "relative_time constraint must have a non-negative offset time.";
      retval = false;
   }
   return retval;
}

bool RelativeTimeCondition::GetTimeToCondition(const UtOrbitalPropagatorBase& aPropagator, double& aTimeToCondition) const
{
   aTimeToCondition = mOffsetTime;
   if (!aPropagator.GetOrbitalState().OrbitIsHyperbolic())
   {
      aTimeToCondition += mOrbitNumber * aPropagator.GetOrbitalState().GetOrbitalElements().GetPeriod();
   }
   return true;
}

void RelativeTimeCondition::Accept(OrbitalPropagatorConditionVisitor& aVisitor)
{
   aVisitor.Accept(*this);
}

// PERIAPSIS

std::pair<bool, double> PeriapsisCondition::InitialGuess(const UtOrbitalPropagatorBase& aPropagator) const
{
   auto   predictorPtr = aPropagator.GetManeuveringPropagator();
   double prediction   = predictorPtr->GetTimeToPeriapsisPassage(mOrbitNumber);
   return std::make_pair(true, prediction);
}

bool PeriapsisCondition::Objective(UtOrbitalPropagatorBase& aPropagator, const UtCalendar& aBaseEpoch, double aOffsetTime) const
{
   UtCalendar then{aBaseEpoch};
   then.AdvanceTimeBy(aOffsetTime);
   aPropagator.Update(then);

   // When the radial velocity changes from negative to positive we have reached the point of
   // periapsis.
   UtVec3d rHat  = aPropagator.GetOrbitalState().GetOrbitalStateVector().GetLocation().GetNormal();
   UtVec3d vel   = aPropagator.GetOrbitalState().GetOrbitalStateVector().GetVelocity();
   double  vDotR = vel.DotProduct(rHat);
   return vDotR < 0.0;
}

bool PeriapsisCondition::HandleSpecialCases(const UtOrbitalPropagatorBase& aPropagator, double& aTimeToCondition) const
{
   bool retval{false};
   if (aPropagator.OrbitIsCircular())
   {
      aTimeToCondition = mOrbitNumber * aPropagator.GetInitialOrbitalState().GetOrbitalElements().GetPeriod();
      retval           = true;
   }
   return retval;
}

void PeriapsisCondition::Accept(OrbitalPropagatorConditionVisitor& aVisitor)
{
   aVisitor.Accept(*this);
}

// APOAPSIS

std::pair<bool, double> ApoapsisCondition::InitialGuess(const UtOrbitalPropagatorBase& aPropagator) const
{
   auto   predictorPtr = aPropagator.GetManeuveringPropagator();
   double prediction   = predictorPtr->GetTimeToApoapsisPassage(mOrbitNumber);
   return std::make_pair(true, prediction);
}

bool ApoapsisCondition::Objective(UtOrbitalPropagatorBase& aPropagator, const UtCalendar& aBaseEpoch, double aOffsetTime) const
{
   UtCalendar then{aBaseEpoch};
   then.AdvanceTimeBy(aOffsetTime);
   aPropagator.Update(then);

   // When the radial velocity changes from positive to negative we have reached the point of
   // apoapsis.
   UtVec3d rHat  = aPropagator.GetOrbitalState().GetOrbitalStateVector().GetLocation().GetNormal();
   UtVec3d vel   = aPropagator.GetOrbitalState().GetOrbitalStateVector().GetVelocity();
   double  vDotR = vel.DotProduct(rHat);
   return vDotR > 0.0;
}

bool ApoapsisCondition::HandleSpecialCases(const UtOrbitalPropagatorBase& aPropagator, double& aTimeToCondition) const
{
   bool retval{false};
   if (aPropagator.OrbitIsCircular())
   {
      aTimeToCondition = mOrbitNumber * aPropagator.GetInitialOrbitalState().GetOrbitalElements().GetPeriod();
      retval           = true;
   }
   return retval;
}

void ApoapsisCondition::Accept(OrbitalPropagatorConditionVisitor& aVisitor)
{
   aVisitor.Accept(*this);
}

// ASCENDING NODE

std::pair<bool, double> AscendingNodeCondition::InitialGuess(const UtOrbitalPropagatorBase& aPropagator) const
{
   auto   predictorPtr = aPropagator.GetManeuveringPropagator();
   double prediction   = predictorPtr->GetTimeToAscendingNodePassage(mOrbitNumber);
   return std::make_pair(true, prediction);
}

bool AscendingNodeCondition::Objective(UtOrbitalPropagatorBase& aPropagator,
                                       const UtCalendar&        aBaseEpoch,
                                       double                   aOffsetTime) const
{
   UtCalendar then{aBaseEpoch};
   then.AdvanceTimeBy(aOffsetTime);
   aPropagator.Update(then);

   double z = aPropagator.GetOrbitalState().GetOrbitalStateVectorTOD().GetLocation().Get(2);
   return z < 0.0;
}

bool AscendingNodeCondition::HandleSpecialCases(const UtOrbitalPropagatorBase& aPropagator, double& aTimeToCondition) const
{
   bool retval{false};
   if (aPropagator.OrbitIsEquatorial())
   {
      aTimeToCondition = mOrbitNumber * aPropagator.GetInitialOrbitalState().GetOrbitalElements().GetPeriod();
      retval           = true;
   }
   return retval;
}

void AscendingNodeCondition::Accept(OrbitalPropagatorConditionVisitor& aVisitor)
{
   aVisitor.Accept(*this);
}

// DESCENDING NODE

std::pair<bool, double> DescendingNodeCondition::InitialGuess(const UtOrbitalPropagatorBase& aPropagator) const
{
   auto   predictorPtr = aPropagator.GetManeuveringPropagator();
   double prediction   = predictorPtr->GetTimeToDescendingNodePassage(mOrbitNumber);
   return std::make_pair(true, prediction);
}

bool DescendingNodeCondition::Objective(UtOrbitalPropagatorBase& aPropagator,
                                        const UtCalendar&        aBaseEpoch,
                                        double                   aOffsetTime) const
{
   UtCalendar then{aBaseEpoch};
   then.AdvanceTimeBy(aOffsetTime);
   aPropagator.Update(then);

   double z = aPropagator.GetOrbitalState().GetOrbitalStateVectorTOD().GetLocation().Get(2);
   return z > 0.0;
}

bool DescendingNodeCondition::HandleSpecialCases(const UtOrbitalPropagatorBase& aPropagator, double& aTimeToCondition) const
{
   bool retval{false};
   if (aPropagator.OrbitIsEquatorial())
   {
      aTimeToCondition = mOrbitNumber * aPropagator.GetInitialOrbitalState().GetOrbitalElements().GetPeriod();
      retval           = true;
   }
   return retval;
}

void DescendingNodeCondition::Accept(OrbitalPropagatorConditionVisitor& aVisitor)
{
   aVisitor.Accept(*this);
}

// ECLIPSE ENTRY

bool EclipseEntryCondition::GetTimeToCondition(const UtOrbitalPropagatorBase& aPropagator, double& aTimeToCondition) const
{
   if (aPropagator.GetOrbitalState().OrbitIsHyperbolic())
   {
      return false;
   }
   double     timeToEntry, timeToExit;
   double     orbitTimeDelay = GetOrbitNumber() * aPropagator.GetOrbitalState().GetOrbitalElements().GetPeriod();
   UtCalendar time           = aPropagator.GetCurrentTime();
   time.AdvanceTimeBy(orbitTimeDelay);
   bool ok = aPropagator.GetEclipseTimes(time, timeToEntry, timeToExit);
   if (ok)
   {
      aTimeToCondition = timeToEntry + orbitTimeDelay;
   }
   else
   {
      auto logger = ut::log::warning() << "Satellite does not enter eclipse.";
      logger.AddNote() << "Satellite: " << aPropagator.GetDescriptor();
   }
   return ok;
}

void EclipseEntryCondition::Accept(OrbitalPropagatorConditionVisitor& aVisitor)
{
   aVisitor.Accept(*this);
}

// ECLIPSE EXIT

bool EclipseExitCondition::GetTimeToCondition(const UtOrbitalPropagatorBase& aPropagator, double& aTimeToCondition) const
{
   if (aPropagator.GetOrbitalState().OrbitIsHyperbolic())
   {
      return false;
   }
   double     timeToEntry, timeToExit;
   double     orbitTimeDelay = GetOrbitNumber() * aPropagator.GetOrbitalState().GetOrbitalElements().GetPeriod();
   UtCalendar time           = aPropagator.GetCurrentTime();
   time.AdvanceTimeBy(orbitTimeDelay);
   bool ok = aPropagator.GetEclipseTimes(time, timeToEntry, timeToExit);
   if (ok)
   {
      aTimeToCondition = timeToExit + orbitTimeDelay;
   }
   else
   {
      auto logger = ut::log::warning() << "Satellite does not exit eclipse.";
      logger.AddNote() << "Satellite: " << aPropagator.GetDescriptor();
   }
   return ok;
}

void EclipseExitCondition::Accept(OrbitalPropagatorConditionVisitor& aVisitor)
{
   aVisitor.Accept(*this);
}

// RADIUS NEEDING CONDITION

bool RadiusCondition::ProcessInput(UtInput& aInput)
{
   aInput.ReadUnitValue(mRadius);
   aInput.ValueGreater<double>(mRadius, 0.0);
   return true;
}

bool RadiusCondition::ValidateParameterRanges() const
{
   bool retval{true};
   if (mRadius <= 0.0)
   {
      ut::log::error() << GetType() << " constraint must specify a radius greater than 0.";
      retval = false;
   }
   return retval;
}

// ASCENDING RADIUS

std::pair<bool, double> AscendingRadiusCondition::InitialGuess(const UtOrbitalPropagatorBase& aPropagator) const
{
   auto   predictorPtr = aPropagator.GetManeuveringPropagator();
   double prediction;
   bool   ok = predictorPtr->GetTimeToAscendingRadius(mRadius, prediction, mOrbitNumber);
   return std::make_pair(ok, prediction);
}

bool AscendingRadiusCondition::Objective(UtOrbitalPropagatorBase& aPropagator,
                                         const UtCalendar&        aBaseEpoch,
                                         double                   aOffsetTime) const
{
   UtCalendar then{aBaseEpoch};
   then.AdvanceTimeBy(aOffsetTime);
   aPropagator.Update(then);

   double r = aPropagator.GetOrbitalState().GetOrbitalStateVector().GetLocation().Magnitude();
   return r < mRadius;
}

void AscendingRadiusCondition::Accept(OrbitalPropagatorConditionVisitor& aVisitor)
{
   aVisitor.Accept(*this);
}

// DESCENDING RADIUS

std::pair<bool, double> DescendingRadiusCondition::InitialGuess(const UtOrbitalPropagatorBase& aPropagator) const
{
   auto   predictorPtr = aPropagator.GetManeuveringPropagator();
   double prediction;
   bool   ok = predictorPtr->GetTimeToDescendingRadius(mRadius, prediction, mOrbitNumber);
   return std::make_pair(ok, prediction);
}

bool DescendingRadiusCondition::Objective(UtOrbitalPropagatorBase& aPropagator,
                                          const UtCalendar&        aBaseEpoch,
                                          double                   aOffsetTime) const
{
   UtCalendar then{aBaseEpoch};
   then.AdvanceTimeBy(aOffsetTime);
   aPropagator.Update(then);

   double r = aPropagator.GetOrbitalState().GetOrbitalStateVector().GetLocation().Magnitude();
   return r > mRadius;
}

void DescendingRadiusCondition::Accept(OrbitalPropagatorConditionVisitor& aVisitor)
{
   aVisitor.Accept(*this);
}

// INTERSECTION

void IntersectionCondition::SetRAAN(double aRAAN)
{
   mRAAN = aRAAN;
}

void IntersectionCondition::SetInclination(double aInclination)
{
   mInclination = aInclination;
}

void IntersectionCondition::SetIntersectionRAAN_Inclination(double aRAAN, double aInclination)
{
   mRAAN        = aRAAN;
   mInclination = aInclination;

   // If the orbit is equatorial, RAAN loses meaning. Set it to zero.
   if (UtMath::NearlyEqual(aInclination, 0.0) || UtMath::NearlyEqual(aInclination, UtMath::cPI))
   {
      mRAAN = 0.0;
   }
}

bool IntersectionCondition::HandleSpecialCases(const UtOrbitalPropagatorBase& aPropagator, double& aTimeToCondition) const
{
   static constexpr double cTOLERANCE{1.0e-7};
   bool                    retval{false};
   const auto&             elem = aPropagator.GetOrbitalState().GetOrbitalElementsTOD();
   // When the initial and final orbit normals are anti-parallel, all points on the orbit are
   // intersections, so do the maneuver immediately.
   auto n1 = OrbitNormal(elem.GetRAAN(), elem.GetInclination());
   auto n2 = OrbitNormal(mRAAN, mInclination);
   if (UtMath::NearlyEqual(n1.DotProduct(n2), -1.0, cTOLERANCE))
   {
      aTimeToCondition = mOrbitNumber * elem.GetPeriod();
      retval           = true;
   }
   return retval;
}

UtVec3d IntersectionCondition::OrbitNormal(double aRAAN, double aInclination) const
{
   return UtVec3d{std::sin(aInclination) * std::sin(aRAAN),
                  -std::sin(aInclination) * std::cos(aRAAN),
                  std::cos(aInclination)};
}

// NORTHERN INTERSECTION

std::pair<bool, double> NorthernIntersectionCondition::InitialGuess(const UtOrbitalPropagatorBase& aPropagator) const
{
   // If both orbits are equatorial, there is no unique way to define RAAN for either orbit, and so the
   // condition does not ever obtain.
   if (aPropagator.OrbitIsEquatorial() && (mInclination == 0.0 || mInclination == UtMath::cPI))
   {
      return std::make_pair(false, -1.0);
   }
   auto   predictorPtr = aPropagator.GetManeuveringPropagator();
   double prediction   = predictorPtr->GetTimeToNorthernIntersection(mRAAN, mInclination, mOrbitNumber);
   return std::make_pair(true, prediction);
}

bool NorthernIntersectionCondition::Objective(UtOrbitalPropagatorBase& aPropagator,
                                              const UtCalendar&        aBaseEpoch,
                                              double                   aOffsetTime) const
{
   UtCalendar then{aBaseEpoch};
   then.AdvanceTimeBy(aOffsetTime);
   aPropagator.Update(then);

   // The point of intersection is when the propagator is at a location in both the initial
   // and final orbital planes. Because the propagator already is restricted to the orbital
   // plane of the initial orbit, the only thing to compare is when the state is perpendicular
   // to the final orbital plane's normal.
   auto    n2   = OrbitNormal(mRAAN, mInclination);
   UtVec3d rHat = aPropagator.GetOrbitalState().GetOrbitalStateVectorTOD().GetLocation().GetNormal();
   double  dot  = rHat.DotProduct(n2);

   return dot > 0.0;
}

void NorthernIntersectionCondition::Accept(OrbitalPropagatorConditionVisitor& aVisitor)
{
   aVisitor.Accept(*this);
}

// SOUTHERN INTERSECTION

std::pair<bool, double> SouthernIntersectionCondition::InitialGuess(const UtOrbitalPropagatorBase& aPropagator) const
{
   // If both orbits are equatorial, there is no unique way to define RAAN for either orbit, and so the
   // condition does not ever obtain.
   if (aPropagator.OrbitIsEquatorial() && (mInclination == 0.0 || mInclination == UtMath::cPI))
   {
      return std::make_pair(false, -1.0);
   }
   auto   predictorPtr = aPropagator.GetManeuveringPropagator();
   double prediction   = predictorPtr->GetTimeToSouthernIntersection(mRAAN, mInclination, mOrbitNumber);
   return std::make_pair(true, prediction);
}

bool SouthernIntersectionCondition::Objective(UtOrbitalPropagatorBase& aPropagator,
                                              const UtCalendar&        aBaseEpoch,
                                              double                   aOffsetTime) const
{
   UtCalendar then{aBaseEpoch};
   then.AdvanceTimeBy(aOffsetTime);
   aPropagator.Update(then);

   // The point of intersection is when the propagator is at a location in both the initial
   // and final orbital planes. Because the propagator already is restricted to the orbital
   // plane of the initial orbit, the only thing to compare is when the state is perpendicular
   // to the final orbital plane's normal.
   auto    n2   = OrbitNormal(mRAAN, mInclination);
   UtVec3d rHat = aPropagator.GetOrbitalState().GetOrbitalStateVectorTOD().GetLocation().GetNormal();
   double  dot  = rHat.DotProduct(n2);

   return dot > 0.0;
}

void SouthernIntersectionCondition::Accept(OrbitalPropagatorConditionVisitor& aVisitor)
{
   aVisitor.Accept(*this);
}

} // namespace space
} // namespace wsf
