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

#include <array>
#include <cmath>
#include <stdexcept>

#include <gtest/gtest.h>

#include "UtNewtonRaphson.hpp"

// define some types here for the tests
// Note that we break the encapsulation of these so we can test the bare minimum
// interface required by UtNewtonRaphson

class Jacobian;

class SearchVector
{
public:
   SearchVector(double aX = 0.0, double aY = 0.0)
      : mVec{{aX, aY}}
   {
   }

   SearchVector& operator-=(const SearchVector& aOther)
   {
      mVec[0] -= aOther.mVec[0];
      mVec[1] -= aOther.mVec[1];
      return *this;
   }

   SearchVector operator+(const SearchVector& aOther) const
   {
      SearchVector retval{};
      retval.mVec[0] = mVec[0] + aOther.mVec[0];
      retval.mVec[1] = mVec[1] + aOther.mVec[1];
      return retval;
   }

   double Component(int aIndex) const { return mVec.at(aIndex); }

   static SearchVector UnitVector(int aIndex, double aValue)
   {
      SearchVector retval{};
      retval.mVec.at(aIndex) = aValue;
      return retval;
   }

   static constexpr int cDIMENSION = 2;

   friend class Jacobian;

private:
   std::array<double, 2> mVec;
};

class StateVector
{
public:
   StateVector(double aX = 0.0, double aY = 0.0)
      : mVec{{aX, aY}}
   {
   }

   StateVector operator-(const StateVector& aOther) const
   {
      StateVector retval{};
      retval.mVec[0] = mVec[0] - aOther.mVec[0];
      retval.mVec[1] = mVec[1] - aOther.mVec[1];
      return retval;
   }

   StateVector operator*(double aScale) const
   {
      StateVector retval{*this};
      retval.mVec[0] *= aScale;
      retval.mVec[1] *= aScale;
      return retval;
   }

   static constexpr int cDIMENSION = 2;

   friend class Jacobian;

private:
   std::array<double, 2> mVec;
};

class Jacobian
{
public:
   Jacobian()
      : mMat{{0.0, 0.0, 0.0, 0.0}}
   {
   }

   Jacobian(const Jacobian& aOther)
   {
      mMat.at(0) = aOther.mMat.at(0);
      mMat.at(1) = aOther.mMat.at(1);
      mMat.at(2) = aOther.mMat.at(2);
      mMat.at(3) = aOther.mMat.at(3);
   }

   SearchVector InverseProduct(const StateVector& aState)
   {
      double det = mMat[3] * mMat[0] - mMat[1] * mMat[2];
      if (fabs(det) < 1.0e-9)
      {
         throw std::runtime_error("Singular matrix");
      }
      SearchVector retval{};
      retval.mVec[0] = (mMat[3] * aState.mVec[0] - mMat[1] * aState.mVec[1]) / det;
      retval.mVec[1] = (mMat[0] * aState.mVec[1] - mMat[2] * aState.mVec[0]) / det;
      return retval;
   }

   void SetColumn(int aColumnIndex, const StateVector& aColumn)
   {
      mMat.at(aColumnIndex + 2 * 0) = aColumn.mVec.at(0);
      mMat.at(aColumnIndex + 2 * 1) = aColumn.mVec.at(1);
   }

   static constexpr int cDIMENSION = 2;

private:
   std::array<double, 4> mMat;
};

TEST(UtNewtonRaphson, LinearCase)
{
   int  counter{0};
   auto testFunc = [&counter](const SearchVector& aSearch) -> StateVector
   {
      ++counter;
      return StateVector{aSearch.Component(0) * 2 + aSearch.Component(1) * 7,
                         aSearch.Component(0) * 1 + aSearch.Component(1) * 3};
   };

   SearchVector guess{};
   StateVector  target{1.0, 1.0};
   SearchVector solution{};
   bool ok = ut::NewtonRaphson<SearchVector, StateVector, Jacobian>::Solve(guess, target, testFunc, 1.0e-12, solution);

   EXPECT_TRUE(ok);
   EXPECT_EQ(counter, 7);
   EXPECT_NEAR(solution.Component(0), 4.0, 1.0e12);
   EXPECT_NEAR(solution.Component(1), -1.0, 1.0e12);
}

TEST(UtNewtonRaphson, NonLinearCase)
{
   int  counter{0};
   auto testFunc = [&counter](const SearchVector& aSearch) -> StateVector
   {
      ++counter;
      double r = sqrt(aSearch.Component(0) * aSearch.Component(0) + aSearch.Component(1) * aSearch.Component(1));
      return StateVector{r, aSearch.Component(0) / r};
   };

   SearchVector guess{1.0, 1.0};
   StateVector  target{2.0, 0.5};
   SearchVector solution{};
   bool ok = ut::NewtonRaphson<SearchVector, StateVector, Jacobian>::Solve(guess, target, testFunc, 1.0e-12, solution);

   EXPECT_TRUE(ok);
   EXPECT_EQ(counter, 19);
   EXPECT_NEAR(solution.Component(0), 1.0, 1.0e-12);
   EXPECT_NEAR(solution.Component(1), sqrt(3.0), 1.0e-12);
}

TEST(UtNewtonRaphson, NoSolutionCase)
{
   int  counter{0};
   auto testFunc = [&counter](const SearchVector& aSearch) -> StateVector
   {
      ++counter;
      double r = sqrt(aSearch.Component(0) * aSearch.Component(0) + aSearch.Component(1) * aSearch.Component(1));
      return StateVector{r, aSearch.Component(0) / r};
   };

   SearchVector guess{1.0, 1.0};
   StateVector  target{-2.0, 0.5};
   SearchVector solution{};
   bool ok = ut::NewtonRaphson<SearchVector, StateVector, Jacobian>::Solve(guess, target, testFunc, 1.0e-12, solution);

   EXPECT_FALSE(ok);
   // The solution search currently terminates on a singular matrix exception at 115 iterations.
   // Three evaluations per iteration on iterations 0-114, plus one during initialization,
   // plus two on the final iteration, gives 345.
   EXPECT_EQ(counter, 345);
}

TEST(UtNewtonRaphson, WorkingOneD_Case)
{
   // The roots are +/- 1
   auto func  = [](double aValue) { return aValue * aValue - 1.0; };
   auto deriv = [](double aValue) { return 2.0 * aValue; };

   static constexpr double cROOT      = 1.0;
   static constexpr double cINITIAL   = 4.0;
   static constexpr double cTOLERANCE = 1.0e-15;

   auto result = ut::NewtonRaphson1D(func, deriv, cINITIAL, cTOLERANCE);
   EXPECT_TRUE(result.second);
   EXPECT_NEAR(result.first, cROOT, cTOLERANCE);
   EXPECT_NEAR(func(result.first), 0.0, cTOLERANCE);
}

TEST(UtNewtonRaphson, NonConvergentOneD_Case)
{
   // Starting at 0 or 1 for this should never converge.
   auto func  = [](double aValue) { return aValue * aValue * aValue - 2.0 * aValue + 2.0; };
   auto deriv = [](double aValue) { return 3.0 * aValue * aValue - 2.0; };

   static constexpr double cINITIAL   = 0.0;
   static constexpr double cTOLERANCE = 1.0e-15;

   auto result = ut::NewtonRaphson1D(func, deriv, cINITIAL, cTOLERANCE);
   EXPECT_FALSE(result.second);
   // The method will give a cycle jumping back and forth from 0 -> 1 -> 0. With
   // the current limit of 100 iterations, this should end where it started.
   EXPECT_NEAR(result.first, cINITIAL, cTOLERANCE);
}
