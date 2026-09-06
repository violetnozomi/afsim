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

#include "WsfNORAD_PropagatorInverter.hpp"

#include <algorithm>
#include <functional>

#include "UtEarth.hpp"
#include "UtMath.hpp"
#include "WsfNORAD_OrbitalPropagator.hpp"

namespace
{
static constexpr double cECCENTRIC_LIMIT  = 0.75;
static constexpr double cCIRCULAR_LIMIT   = 1.0e-7;
static constexpr double cEQUATORIAL_LIMIT = 1.0e-8;
static constexpr double cSOLVE_TOLERANCE  = 1.0e-6;
} // namespace

WsfNORAD_PropagatorInverter::WsfNORAD_PropagatorInverter(const ut::OrbitalState& aOsculatingInitialState,
                                                         std::unique_ptr<WsfNORAD_OrbitalPropagator> aPropPtr)
   : mPropPtr{std::move(aPropPtr)}
   , mEpoch{aOsculatingInitialState.GetEpoch()}
   , mECI_TEME_Converter(mEpoch, ut::EarthWGS84())
   , mTargetStateTEME{aOsculatingInitialState.GetOrbitalStateVectorTEME()}
   , mSearchSolution{0.0}
   , mSolutionFound{false}
   , mReferenceTLE(aOsculatingInitialState.GetTwoLineElement())
{
   // NOTE: There is some issue with the NORAD propagator at high eccentricities. This is either a problem in the
   // implementation or the model does not apply to such an eccentric orbit.
   UtTwoLineElement initialGuess(mReferenceTLE);

   initialGuess.Publish(mTargetStateTEME.Position(), mTargetStateTEME.Velocity(), mEpoch);

   if (initialGuess.GetEccentricity() < cECCENTRIC_LIMIT)
   {
      SearchVector guessVector = InitialOsculatingElements(mTargetStateTEME);

      // These two modifications allow for a initial guess that lies on either of the two
      // degenerate surfaces the ability to wander off of them if that is where the solution
      // lies.
      if (initialGuess.GetEccentricity() < cCIRCULAR_LIMIT)
      {
         guessVector += SearchVector::UnitVector(0, 0.001);
      }
      if (initialGuess.GetInclination() < cEQUATORIAL_LIMIT)
      {
         guessVector += SearchVector::UnitVector(1, 0.001);
      }

      auto func      = [this](const SearchVector& aInput) -> StateVector { return Propagate(aInput); };
      mSolutionFound = FullStateSolver::SolveState(guessVector, mTargetStateTEME, func, cSOLVE_TOLERANCE, mSearchSolution);
   }
}

ut::OrbitalState WsfNORAD_PropagatorInverter::GetOrbitalState() const
{
   return ut::OrbitalState(ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                           ut::OrbitalState::ReferenceFrame::cTEME,
                           mSearchSolution.GetTLE(mEpoch, mReferenceTLE));
}

UtTwoLineElement WsfNORAD_PropagatorInverter::GetTwoLineElement() const
{
   return mSearchSolution.GetTLE(mEpoch, mReferenceTLE);
}

//! Apply the propagation function to the given elements.
//! \param aElements - The orbital elements to be propagated.
//! \returns         - The state of the system after propagation
WsfNORAD_PropagatorInverter::StateVector WsfNORAD_PropagatorInverter::Propagate(const SearchVector& aElems) const
{
   mPropPtr->Initialize(ut::OrbitalState(ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                         ut::OrbitalState::ReferenceFrame::cTEME,
                                         aElems.GetTLE(mEpoch, mReferenceTLE)));
   mPropPtr->Update(mEpoch);

   return StateVector(mPropPtr->GetOrbitalState().GetOrbitalStateVectorTEME());
}

//! Form an initial guess for the solution.
//! This uses the osculating elements to form the initial guess.
//! \param aState - The state from which to compute the initial guess.
//! \returns      - The initial guess.
WsfNORAD_PropagatorInverter::SearchVector WsfNORAD_PropagatorInverter::InitialOsculatingElements(const StateVector& aState) const
{
   UtTwoLineElement tle{};
   tle.Publish(aState.Position(), aState.Velocity(), mEpoch);
   return SearchVector(tle);
}

WsfNORAD_PropagatorInverter::StateVector::StateVector(const ut::OrbitalState::Vector& aVector)
   : mVec(cDIMENSION)
{
   mVec(0) = aVector.GetLocation()[0];
   mVec(1) = aVector.GetLocation()[1];
   mVec(2) = aVector.GetLocation()[2];
   mVec(3) = aVector.GetVelocity()[0];
   mVec(4) = aVector.GetVelocity()[1];
   mVec(5) = aVector.GetVelocity()[2];
}

WsfNORAD_PropagatorInverter::StateVector::StateVector(const UtMatrixd& aMatrix)
   : mVec{aMatrix}
{
}

//! Return the position part of the state.
UtVec3d WsfNORAD_PropagatorInverter::StateVector::Position() const
{
   return UtVec3d(mVec(0), mVec(1), mVec(2));
}

//! Return the velocity part of the state.
UtVec3d WsfNORAD_PropagatorInverter::StateVector::Velocity() const
{
   return UtVec3d(mVec(3), mVec(4), mVec(5));
}

//! Returns the difference of this state with another.
//! This returns a new StateVector with the result.
WsfNORAD_PropagatorInverter::StateVector
WsfNORAD_PropagatorInverter::StateVector::operator-(const WsfNORAD_PropagatorInverter::StateVector& aOther) const
{
   return StateVector{mVec - aOther.mVec};
}

//! Return this state vector scaled by a constant.
WsfNORAD_PropagatorInverter::StateVector WsfNORAD_PropagatorInverter::StateVector::operator*(double aScalar) const
{
   return StateVector{aScalar * mVec};
}

WsfNORAD_PropagatorInverter::SearchVector::SearchVector(const UtTwoLineElement& aElements)
   : mVec(cDIMENSION)
{
   mVec(0) = aElements.GetEccentricity();
   mVec(1) = aElements.GetInclination();
   mVec(2) = aElements.GetRAAN();
   mVec(3) = aElements.GetArgumentOfPeriapsis();
   mVec(4) = aElements.GetMeanAnomaly();
   mVec(5) = aElements.GetSemiMajorAxis();
}

WsfNORAD_PropagatorInverter::SearchVector::SearchVector(const UtMatrixd& aMatrix)
   : mVec{aMatrix}
{
}

WsfNORAD_PropagatorInverter::SearchVector::SearchVector(double aFillValue)
   : mVec(cDIMENSION)
{
   mVec.Fill(aFillValue);
}

//! Return a TLE corresponding to this SearchVector.
//! The search space does not modify some elements of the TLE, so these are set back in via arguments to
//! this routine.
UtTwoLineElement WsfNORAD_PropagatorInverter::SearchVector::GetTLE(const UtCalendar&       aEpoch,
                                                                   const UtTwoLineElement& aReferenceTLE) const
{
   UtTwoLineElement retval(aReferenceTLE);
   retval.SetEccentricity(mVec(0));
   retval.SetInclination(mVec(1));
   retval.SetRAAN(mVec(2));
   retval.SetArgumentOfPeriapsis(mVec(3));
   retval.SetMeanAnomaly(mVec(4));
   retval.SetSemiMajorAxis(mVec(5));
   retval.SetEpoch(aEpoch);
   retval.SetHasMeanElements(true);
   retval.SetEphemerisType(UtOrbitalElements::EphemerisType::cNORAD);
   return retval;
}

//! Add the given SearchVector to this object.
WsfNORAD_PropagatorInverter::SearchVector& WsfNORAD_PropagatorInverter::SearchVector::operator+=(const SearchVector& aOther)
{
   mVec += aOther.mVec;
   return *this;
}

//! Subtract the given SearchVector to this object.
WsfNORAD_PropagatorInverter::SearchVector& WsfNORAD_PropagatorInverter::SearchVector::operator-=(const SearchVector& aOther)
{
   // The result must be constrained such that the eccentricity is between 0 and 1,
   // the inclination is between 0 and 180 degrees, and the semi-major axis is greater than zero.
   // This protection is only included in this operation because this is the operation called from
   // UtNewtonRaphson that has been observed to produce out-of-constraint results.
   // In order to do this, an approach is adopted whereby a smaller and smaller fraction of aOther is subtracted
   // (mVec is assumed within constraints), so that eventually the difference is guaranteed to be within constraints.
   auto   diff = mVec - aOther.mVec;
   double frac = 1.0;
   while ((diff(0) < 0.0) || (diff(0) >= 1.0)) // eccentricity constraints
   {
      frac -= 0.1;
      diff(0) = mVec(0) - aOther.mVec(0) * frac;
   }
   frac = 1.0;
   while ((diff(1) < 0.0) || (diff(1) > UtMath::cPI)) // inclination constraints
   {
      frac -= 0.1;
      diff(1) = mVec(1) - aOther.mVec(1) * frac;
   }
   frac = 1.0;
   while (diff(5) <= 0.0) // SMA constraint
   {
      frac -= 0.1;
      diff(5) = mVec(5) - aOther.mVec(5) * frac;
   }
   mVec = diff;
   return *this;
}

//! Return the sum of this object and another SearchVector.
WsfNORAD_PropagatorInverter::SearchVector WsfNORAD_PropagatorInverter::SearchVector::operator+(const SearchVector& aOther) const
{
   SearchVector retval{mVec};
   retval += aOther;
   return retval;
}

//! Return a scaled version of this object.
WsfNORAD_PropagatorInverter::SearchVector WsfNORAD_PropagatorInverter::SearchVector::operator*(double aScalar) const
{
   auto retval = SearchVector{mVec * aScalar};
   return retval;
}

//! Return the element at the given index.
double WsfNORAD_PropagatorInverter::SearchVector::Component(int aIndex) const
{
   return mVec(aIndex);
}

//! Return a unit SeachVector.
//! This will return a SearchVector with a single non-zero entry in the position given by @p aIndex, which will have the
//! value @aValue.
WsfNORAD_PropagatorInverter::SearchVector WsfNORAD_PropagatorInverter::SearchVector::UnitVector(int aIndex, double aValue)
{
   SearchVector retval{UtMatrixd(6)};
   retval.mVec.Fill(0.0);
   retval.mVec(aIndex) = aValue;
   return retval;
}
