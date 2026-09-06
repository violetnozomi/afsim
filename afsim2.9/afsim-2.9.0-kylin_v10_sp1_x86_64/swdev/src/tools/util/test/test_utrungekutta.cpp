// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <array>
#include <cmath>

#include <gtest/gtest.h>

#include "UtMath.hpp"
#include "UtRungeKutta.hpp"

// The polynomial system here is chosen such that the exact solution of the
// differential equation is a polynomial of a given order. This is used to
// verify that the second and fourth order Runge-Kutta methods given exact
// solutions up to second and fourth order, respectively.

class PolynomialState
{
public:
   PolynomialState(double aTime, double aValue)
      : mTime{aTime}
      , mValue{aValue}
   {
   }

   double GetTime() const { return mTime; }
   double GetValue() const { return mValue; }

   PolynomialState Advance(double aDeltaT, double aKinematicState) const
   {
      return PolynomialState{mTime + aDeltaT, mValue + aDeltaT * aKinematicState};
   }

private:
   double mTime;
   double mValue;
};

template<unsigned int N>
double PolynomialSolution(double aTime)
{
   return PolynomialSolution<N - 1>(aTime) * aTime;
}

template<>
double PolynomialSolution<0>(double aTime)
{
   return 1.0;
}

template<unsigned int N>
double PolynomialRHS(const PolynomialState& aState)
{
   return PolynomialSolution<N - 1>(aState.GetTime()) * N;
}

template<>
double PolynomialRHS<0>(const PolynomialState& aState)
{
   return 0.0;
}

template<unsigned int N>
void RunRK2_PolynomialTest(double aInitialTime, double aDeltaT, bool aNear)
{
   constexpr double cTOLERANCE   = 1.0e-14;
   auto             initialState = PolynomialState{aInitialTime, PolynomialSolution<N>(aInitialTime)};
   auto             value        = ut::RK2_Step<PolynomialState, double>(aDeltaT, initialState, PolynomialRHS<N>);
   EXPECT_NEAR(value.GetTime(), aInitialTime + aDeltaT, cTOLERANCE);
   if (aNear)
   {
      EXPECT_NEAR(value.GetValue(), PolynomialSolution<N>(aInitialTime + aDeltaT), cTOLERANCE);
   }
   else
   {
      EXPECT_FALSE(std::fabs(value.GetValue() - PolynomialSolution<N>(aInitialTime + aDeltaT)) < cTOLERANCE);
   }
}

template<unsigned int N>
void RunRK4_PolynomialTest(double aInitialTime, double aDeltaT, bool aNear)
{
   constexpr double cTOLERANCE   = 1.0e-14;
   auto             initialState = PolynomialState{aInitialTime, PolynomialSolution<N>(aInitialTime)};
   auto             value        = ut::RK4_Step<PolynomialState, double>(aDeltaT, initialState, PolynomialRHS<N>);
   EXPECT_NEAR(value.GetTime(), aInitialTime + aDeltaT, cTOLERANCE);
   if (aNear)
   {
      EXPECT_NEAR(value.GetValue(), PolynomialSolution<N>(aInitialTime + aDeltaT), cTOLERANCE);
   }
   else
   {
      EXPECT_FALSE(std::fabs(value.GetValue() - PolynomialSolution<N>(aInitialTime + aDeltaT)) < cTOLERANCE);
   }
}

TEST(UtRungeKutta, PolynomialSystem)
{
   // RK2 should reproduce polynomials up to second order.
   RunRK2_PolynomialTest<0>(0.0, 1.0, true);
   RunRK2_PolynomialTest<1>(1.0, 0.5, true);
   RunRK2_PolynomialTest<2>(2.0, 1.0, true);
   RunRK2_PolynomialTest<3>(3.0, 2.0, false);

   // RK4 should reproduce polynomials up to fourth order.
   RunRK4_PolynomialTest<0>(0.0, 1.0, true);
   RunRK4_PolynomialTest<1>(0.0, 1.0, true);
   RunRK4_PolynomialTest<2>(0.0, 1.0, true);
   RunRK4_PolynomialTest<3>(0.0, 1.0, true);
   RunRK4_PolynomialTest<4>(0.0, 1.0, true);
   RunRK4_PolynomialTest<5>(0.0, 1.0, false);
}

// The exponential system uses the equation of motion: dx/dt = x, which has
// the exact solution: x(t) = x(0) * exp(t). This system easily demonstrates
// the divergence of the RK method from a simple Euler method.

class ExponentialState
{
public:
   explicit ExponentialState(double aInitialState)
      : mState{aInitialState}
   {
   }

   double GetState() const { return mState; }

   ExponentialState Advance(double aDeltaT, double aKinematicState) const
   {
      return ExponentialState(mState + aDeltaT * aKinematicState);
   }

private:
   double mState;
};

double ExponentialRHS(const ExponentialState& aState)
{
   return aState.GetState();
}

double ExponentialSolution(double aTime, double aInitialValue)
{
   return std::exp(aTime) * aInitialValue;
}

TEST(UtRungeKutta, ExponentialSystemRK2)
{
   constexpr double       cINITIAL_TIME  = 0.0;
   constexpr double       cFINAL_TIME    = 2.0;
   constexpr unsigned int cSTEP_COUNT    = 2000;
   constexpr double       cDELTA_T       = (cFINAL_TIME - cINITIAL_TIME) / cSTEP_COUNT;
   constexpr double       cINITIAL_VALUE = 1.0;
   constexpr double       cTOLERANCE     = 1.0e-12;

   ExponentialState currState{1.0};
   ExponentialState currEuler{1.0};

   for (unsigned int i = 0; i < cSTEP_COUNT; ++i)
   {
      auto nextState = ut::RK4_Step<ExponentialState, double>(cDELTA_T, currState, ExponentialRHS);
      currState      = nextState;

      auto nextEuler = currEuler.Advance(cDELTA_T, ExponentialRHS(currEuler));
      currEuler      = nextEuler;
   }

   // RK4 does a much better job integrating this than the Euler method.
   EXPECT_NEAR(currState.GetState(), ExponentialSolution(cFINAL_TIME, cINITIAL_VALUE), cTOLERANCE);
   EXPECT_FALSE(std::fabs(currEuler.GetState() - ExponentialSolution(cFINAL_TIME, cINITIAL_VALUE)) < cTOLERANCE);
}

// The final system is the simple harmonic oscillator (SHO). This is a
// 2d system when the equations of motion are cast in first order form.
// The equations of motion (with units chosen to eliminate constants) are
// dx/dt = p, dp/dx = - x. If we take the state to be a two component vector
// (x, p), then the right hand side is just (p, -x).
//
// The solution of this problem is x(t) = r_0 * cos(theta_0 + t),
// p(t) = -r_0 * sin(theta_0 + t), where r_0 = sqrt(x_0 * x_0 + p_0 * p_0),
// and theta_0 = arctan(-p_0 / x_0).
//
// This system has an integral of motion r = sqrt(x(t)^2 + p(t)^2), so the
// solution should be restricted to a submanifold of phase space.
//
// This test will verify that the RK routines work for multi-dimensional
// systems, and will also verify that the RK method performs better than a
// simple Euler step.

class SHO_KinematicState : public std::array<double, 2>
{
public:
   SHO_KinematicState(double aDx, double aDp)
      : std::array<double, 2>{{aDx, aDp}}
   {
   }

   double GetDeltaX() const { return at(0); }
   double GetDeltaP() const { return at(1); }

   SHO_KinematicState& operator*(double aScaling)
   {
      (*this)[0] *= aScaling;
      (*this)[1] *= aScaling;
      return *this;
   }

   SHO_KinematicState& operator+(const SHO_KinematicState& aOther)
   {
      (*this)[0] += aOther.GetDeltaX();
      (*this)[1] += aOther.GetDeltaP();
      return *this;
   }
};

class SHO_State
{
public:
   explicit SHO_State(double aInitialX, double aInitialP)
      : mX{aInitialX}
      , mP{aInitialP}
   {
   }

   double GetX() const { return mX; }
   double GetP() const { return mP; }

   double GetR() const { return std::sqrt(mX * mX + mP * mP); }
   double GetTheta() const { return std::atan2(-mP, mX); }

   SHO_State Advance(double aDeltaT, const SHO_KinematicState& aKinematicState) const
   {
      return SHO_State(mX + aDeltaT * aKinematicState.GetDeltaX(), mP + aDeltaT * aKinematicState.GetDeltaP());
   }

private:
   double mX;
   double mP;
};

SHO_KinematicState SHO_RightHandSide(const SHO_State& aState)
{
   return SHO_KinematicState{aState.GetP(), -aState.GetX()};
}

SHO_State SHO_Solution(const SHO_State& aInitialState, double aInitialTime, double aFinalTime)
{
   double r0     = aInitialState.GetR();
   double theta0 = aInitialState.GetTheta();
   return SHO_State{r0 * std::cos(theta0 + aFinalTime - aInitialTime), -r0 * std::sin(theta0 + aFinalTime - aInitialTime)};
}

TEST(UtRungeKutta, SHO_System)
{
   constexpr double       cINITIAL_TIME = 0.0;
   constexpr double       cFINAL_TIME   = UtMath::cTWO_PI;
   constexpr unsigned int cSTEP_COUNT   = 2000;
   constexpr double       cDELTA_T      = (cFINAL_TIME - cINITIAL_TIME) / cSTEP_COUNT;
   constexpr double       cTOLERANCE    = 1.0e-11;

   SHO_State initialState{1.0, 1.0};
   SHO_State currState = initialState;
   SHO_State currEuler = initialState;

   for (unsigned int i = 0; i < cSTEP_COUNT; ++i)
   {
      auto nextState = ut::RK4_Step<SHO_State, SHO_KinematicState>(cDELTA_T, currState, SHO_RightHandSide);
      currState      = nextState;

      auto nextEuler = currEuler.Advance(cDELTA_T, SHO_RightHandSide(currEuler));
      currEuler      = nextEuler;
   }

   // RK4 does a much better job integrating this than the Euler method.
   auto solution = SHO_Solution(initialState, cINITIAL_TIME, cFINAL_TIME);
   EXPECT_NEAR(currState.GetX(), solution.GetX(), cTOLERANCE);
   EXPECT_NEAR(currState.GetP(), solution.GetP(), cTOLERANCE);
   EXPECT_NEAR(currState.GetR(), solution.GetR(), cTOLERANCE);
   EXPECT_NEAR(currState.GetTheta(), solution.GetTheta(), cTOLERANCE);
   EXPECT_FALSE(std::fabs(currEuler.GetX() - solution.GetX()) < cTOLERANCE);
   EXPECT_FALSE(std::fabs(currEuler.GetP() - solution.GetP()) < cTOLERANCE);
   EXPECT_FALSE(std::fabs(currEuler.GetR() - solution.GetR()) < cTOLERANCE);
   EXPECT_FALSE(std::fabs(currEuler.GetTheta() - solution.GetTheta()) < cTOLERANCE);
}
