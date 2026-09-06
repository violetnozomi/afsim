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

#include "WsfNonClassicalOrbitalPropagator.hpp"

#include "UtLambertProblem.hpp"
#include "UtNewtonRaphson.hpp"
#include "UtOrbitalPropagator.hpp"
#include "UtTwoLineElement.hpp"

WsfNonClassicalOrbitalPropagator::WsfNonClassicalOrbitalPropagator(std::unique_ptr<ut::OrbitalState> aInitialOrbitalStatePtr)
   : wsf::space::OrbitalPropagator(std::move(aInitialOrbitalStatePtr))
{
}

std::unique_ptr<UtOrbitalPropagatorBase> WsfNonClassicalOrbitalPropagator::GetManeuveringPropagator() const
{
   // Return a Keplerian propagator with the same instantaneous state.
   auto retvalPtr = ut::make_unique<UtOrbitalPropagator>(ut::make_unique<ut::OrbitalState>(GetOrbitalState()));
   retvalPtr->Initialize(GetCurrentTime());
   return std::move(retvalPtr);
}

UtLambertProblem::Result WsfNonClassicalOrbitalPropagator::SolveLambertProblem(const ut::CentralBody& aCentralBody,
                                                                               const UtVec3d&         aLocationECI_1,
                                                                               const UtVec3d&         aLocationECI_2,
                                                                               double                 aDt,
                                                                               bool                   aShortWay,
                                                                               bool                   aAllowHitsEarth,
                                                                               double aConvergenceTolerance) const
{
   auto result = UtLambertProblem::Universal(aLocationECI_1,
                                             aLocationECI_2,
                                             aDt,
                                             aCentralBody.GetEllipsoid(),
                                             aShortWay,
                                             aConvergenceTolerance);
   bool ok     = result.Assess(HyperbolicPropagationAllowed(), aAllowHitsEarth);
   if (ok)
   {
      UtCalendar now = GetCurrentTime();
      UtCalendar then(now);
      then.AdvanceTimeBy(aDt);

      // This function will return the final position given the (fixed) initial position and (variable) initial velocity.
      auto propPtr = std::unique_ptr<UtOrbitalPropagatorBase>(Clone());
      propPtr->Initialize(now);
      auto func = [&propPtr, &aLocationECI_1, &now, &then](const SearchVector& aSearch) -> StateVector
      {
         ut::OrbitalState initialState =
            propPtr->GetInitialOrbitalState(); // Copy the initial state to capture the non-osculating TLE values.
         initialState.SetReferenceFrame(ut::OrbitalState::ReferenceFrame::cECI);
         initialState.Set(now, ut::OrbitalState::Vector(aLocationECI_1, aSearch.Velocity()));
         if (!propPtr->SetInitialOrbitalState(initialState))
         {
            throw std::runtime_error("Unable to solve for initial state.");
         }
         if (!propPtr->Initialize(now))
         {
            throw std::runtime_error("Unable to initialize propagator.");
         }
         propPtr->Update(then);
         return StateVector(propPtr->GetOrbitalState().GetOrbitalStateVectorInertial().GetLocation());
      };

      // Solve for the initial velocity that gets the object to the right location at the right time.
      SearchVector initialGuess(result.GetInitialVelocity());
      StateVector  targetState(aLocationECI_2);
      SearchVector solution{0.0};
      try
      {
         ok = ut::NewtonRaphson<SearchVector, StateVector, Jacobian>::Solve(initialGuess,
                                                                            targetState,
                                                                            func,
                                                                            aConvergenceTolerance,
                                                                            solution);
      }
      catch (std::runtime_error&)
      {
         ok = false;
      }

      if (ok)
      {
         // If the solution succeeds, collect the results.
         ut::OrbitalState initialState = propPtr->GetInitialOrbitalState();
         propPtr->SetInitialOrbitalState(ut::OrbitalState(now,
                                                          initialState.GetCentralPoint(),
                                                          initialState.GetCoordinateSystem(),
                                                          ut::OrbitalState::ReferenceFrame::cECI,
                                                          ut::OrbitalState::Vector(aLocationECI_1, solution.Velocity())));
         propPtr->Initialize(now);
         propPtr->Update(then);
         result.SetSolution(ok,
                            solution.Velocity(),
                            propPtr->GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity());
      }
      else
      {
         result.SetSolution(ok, result.GetInitialVelocity(), result.GetFinalVelocity());
      }
   }

   return result;
}

WsfNonClassicalOrbitalPropagator::StateVector::StateVector(const UtVec3d& aPosition)
   : mVec(3)
{
   mVec(0) = aPosition[0];
   mVec(1) = aPosition[1];
   mVec(2) = aPosition[2];
}

WsfNonClassicalOrbitalPropagator::StateVector::StateVector(const UtMatrixd& aMatrix)
   : mVec{aMatrix}
{
}

WsfNonClassicalOrbitalPropagator::StateVector
WsfNonClassicalOrbitalPropagator::StateVector::operator-(const WsfNonClassicalOrbitalPropagator::StateVector& aOther) const
{
   return StateVector{mVec - aOther.mVec};
}

WsfNonClassicalOrbitalPropagator::StateVector WsfNonClassicalOrbitalPropagator::StateVector::operator*(double aScalar) const
{
   return StateVector{aScalar * mVec};
}

double WsfNonClassicalOrbitalPropagator::StateVector::Component(int aIndex) const
{
   return mVec(aIndex);
}

WsfNonClassicalOrbitalPropagator::SearchVector::SearchVector(const UtVec3d& aVelocity)
   : mVec(3)
{
   mVec(0) = aVelocity[0];
   mVec(1) = aVelocity[1];
   mVec(2) = aVelocity[2];
}

WsfNonClassicalOrbitalPropagator::SearchVector::SearchVector(const UtMatrixd& aMatrix)
   : mVec{aMatrix}
{
}

WsfNonClassicalOrbitalPropagator::SearchVector::SearchVector(double aFillValue)
   : mVec(3)
{
   mVec.Fill(aFillValue);
}

WsfNonClassicalOrbitalPropagator::SearchVector&
WsfNonClassicalOrbitalPropagator::SearchVector::operator+=(const WsfNonClassicalOrbitalPropagator::SearchVector& aOther)
{
   mVec += aOther.mVec;
   return *this;
}

WsfNonClassicalOrbitalPropagator::SearchVector&
WsfNonClassicalOrbitalPropagator::SearchVector::operator-=(const WsfNonClassicalOrbitalPropagator::SearchVector& aOther)
{
   mVec -= aOther.mVec;
   return *this;
}

WsfNonClassicalOrbitalPropagator::SearchVector WsfNonClassicalOrbitalPropagator::SearchVector::operator+(
   const WsfNonClassicalOrbitalPropagator::SearchVector& aOther) const
{
   SearchVector retval{mVec};
   retval += aOther;
   return retval;
}

WsfNonClassicalOrbitalPropagator::SearchVector WsfNonClassicalOrbitalPropagator::SearchVector::operator*(double aScalar) const
{
   return SearchVector{mVec * aScalar};
}

double WsfNonClassicalOrbitalPropagator::SearchVector::Component(int aIndex) const
{
   return mVec(aIndex);
}

UtVec3d WsfNonClassicalOrbitalPropagator::SearchVector::Velocity() const
{
   return UtVec3d(mVec(0), mVec(1), mVec(2));
}

WsfNonClassicalOrbitalPropagator::SearchVector WsfNonClassicalOrbitalPropagator::SearchVector::UnitVector(int    aIndex,
                                                                                                          double aValue)
{
   SearchVector retval{UtMatrixd(3)};
   retval.mVec.Fill(0.0);
   retval.mVec(aIndex) = aValue;
   return retval;
}

WsfNonClassicalOrbitalPropagator::Jacobian::Jacobian()
   : mMat(3, 3)
{
}

void WsfNonClassicalOrbitalPropagator::Jacobian::SetColumn(int aIndex, const StateVector& aColumn)
{
   for (int i = 0; i < cDIMENSION; ++i)
   {
      mMat(i, aIndex) = aColumn.Vector()(i);
   }
}

WsfNonClassicalOrbitalPropagator::SearchVector
WsfNonClassicalOrbitalPropagator::Jacobian::InverseProduct(const StateVector& aState) const
{
   UtMatrixd Jinv;
   Jinv.Invert(mMat);

   UtMatrixd retval;
   retval.Multiply(Jinv, aState.Vector());

   return SearchVector{retval};
}
