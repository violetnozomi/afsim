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

#ifndef UTSPLINE_HPP
#define UTSPLINE_HPP

#include <array>
#include <cmath>
#include <numeric>
#include <vector>

#include "UtPolynomial.hpp"

//! A Wrapper class used to delegate root finding to the correct method in UtPolynomial.
//! The general case here has a deleted member, because unless N is 1, 2, 3 or 4,
//! there is no general closed form solution to the problem of rootfinding.
template<std::size_t N>
class UtInterpolatingPolynomialRoots
{
public:
   static int Find(const double aCoefficients[N + 1], double aSolutions[N]) = delete;
};

//! Linear polynomial specialization.
template<>
class UtInterpolatingPolynomialRoots<1>
{
public:
   static int Find(const double aCoefficients[2], double aSolutions[1])
   {
      return UtPolynomial::SolveLinear(aCoefficients, aSolutions);
   }
};

//! Quadric polynomial specialization.
template<>
class UtInterpolatingPolynomialRoots<2>
{
public:
   static int Find(const double aCoefficients[3], double aSolutions[2])
   {
      return UtPolynomial::SolveQuadric(aCoefficients, aSolutions);
   }
};

//! Cubic polynomial specialization.
template<>
class UtInterpolatingPolynomialRoots<3>
{
public:
   static int Find(const double aCoefficients[4], double aSolutions[3])
   {
      return UtPolynomial::SolveCubic(aCoefficients, aSolutions);
   }
};

//! Quartic polynomial specialization.
template<>
class UtInterpolatingPolynomialRoots<4>
{
public:
   static int Find(const double aCoefficients[5], double aSolutions[4])
   {
      return UtPolynomial::SolveQuartic(aCoefficients, aSolutions);
   }
};

//! A polynomial that can be used to interpolate to various degrees.
//! The requirements on the data type T are: T must have a default
//! constructor that creates an object that is the additive identity for
//! the addition operation defined for T; T must support + and +=;
//! T must support -; and T must support * and *= where a T is scaled by
//! a double.
template<typename T, std::size_t N>
class UtInterpolatingPolynomial
{
public:
   UtInterpolatingPolynomial(const std::array<T, N + 1>& aCoeff, double aInitTime)
      : mCoeff(aCoeff)
      , mInitTime(aInitTime)
   {
   }

   T operator()(double aTime) const
   {
      T      retval{};
      double delta = aTime - mInitTime;
      for (int i = N; i >= 0; --i)
      {
         retval *= delta;
         retval += mCoeff[i];
      }
      return retval;
   }

   UtInterpolatingPolynomial<T, N - 1> Derivative() const
   {
      std::array<T, N> values{};
      for (std::size_t i = 1; i <= N; ++i)
      {
         values[i - 1] = mCoeff[i] * static_cast<double>(i);
      }
      return UtInterpolatingPolynomial<T, N - 1>(values, mInitTime);
   }

   std::vector<double> Zeros(double aMinTime, double aMaxTime)
   {
      double solutions[N];
      int    numZeros = UtInterpolatingPolynomialRoots<N>::Find(mCoeff.data(), solutions);
      for (int i = 0; i < numZeros; ++i)
      {
         solutions[i] += aMinTime;
      }

      std::vector<double> retval{};
      for (int i = 0; i < numZeros; ++i)
      {
         if (solutions[i] <= aMaxTime && solutions[i] >= aMinTime)
         {
            retval.push_back(solutions[i]);
         }
      }
      return retval;
   }

private:
   std::array<T, N + 1> mCoeff;
   double               mInitTime;
};


//! This is a template specialization for degree 0 polynomials.
template<typename T>
class UtInterpolatingPolynomial<T, 0>
{
public:
   UtInterpolatingPolynomial(const std::array<T, 1>& aCoeff, double aInitTime)
      : mCoeff{aCoeff[0]}
   {
   }

   T operator()(double aParam) const { return mCoeff; }

   UtInterpolatingPolynomial<T, 0> Derivative() const { return UtInterpolatingPolynomial<T, 0>({T{}, 0.0}); }

private:
   T mCoeff;
};

namespace UtCubicSpline
{
//! Construct a cubic interpolating polynomial from values and derivatives.
//! The values and derivatives are specified at the endpoints of the interval
//! over which interpolation is to occur. In this routine, the first arguments
//! must be for the lowest parameter, and the second for the highest parameter
//! of the interval.
template<typename T>
UtInterpolatingPolynomial<T, 3> TwoPoint(double aTime0, T aValue0, T aDeriv0, double aTime1, T aValue1, T aDeriv1)
{
   double           deltaTime = aTime1 - aTime0;
   std::array<T, 4> coeff{};

   coeff[0] = aValue0;
   coeff[1] = deltaTime * aDeriv0;
   coeff[2] = -3.0 * aValue0 - (2.0 * deltaTime) * aDeriv0 + 3.0 * aValue1 - deltaTime * aDeriv1;
   coeff[3] = 2.0 * aValue0 + deltaTime * aDeriv0 - 2.0 * aValue1 + deltaTime * aDeriv1;

   coeff[1] *= 1.0 / deltaTime;
   coeff[2] *= 1.0 / (deltaTime * deltaTime);
   coeff[3] *= 1.0 / (deltaTime * deltaTime * deltaTime);

   return UtInterpolatingPolynomial<T, 3>(coeff, aTime0);
}

//! Construct a cubic interpolating polynomial from four values.
//! The four values must be presented in increasing order of the
//! parameter. The resulting polynomial will pass through each
//! given value at the given parameter.
template<typename T>
UtInterpolatingPolynomial<T, 3>
FourPoint(double aTime0, T aValue0, double aTime1, T aValue1, double aTime2, T aValue2, double aTime3, T aValue3)
{
   double deltaTime = aTime3 - aTime0;

   double tau1_1 = (aTime1 - aTime0) / deltaTime;
   double tau1_2 = tau1_1 * tau1_1;
   double tau1_3 = tau1_2 * tau1_1;

   double tau2_1 = (aTime2 - aTime0) / deltaTime;
   double tau2_2 = tau2_1 * tau2_1;
   double tau2_3 = tau2_2 * tau2_1;

   double det = tau1_3 * tau2_2 + tau1_2 * tau2_1 + tau1_1 * tau2_3 - tau1_3 * tau2_1 - tau1_2 * tau2_3 - tau1_1 * tau2_2;
   det        = 1.0 / det;

   T p1mp0 = aValue1 - aValue0;
   T p2mp0 = aValue2 - aValue0;
   T p3mp0 = aValue3 - aValue0;

   std::array<T, 4> coeff{};
   coeff[0] = aValue0;
   coeff[1] = det * ((tau2_3 - tau2_2) * p1mp0 + (tau1_2 - tau1_3) * p2mp0 + (tau1_3 * tau2_2 - tau1_2 * tau2_3) * p3mp0);
   coeff[2] = det * ((tau2_1 - tau2_3) * p1mp0 + (tau1_3 - tau1_1) * p2mp0 + (tau1_1 * tau2_3 - tau1_3 * tau2_1) * p3mp0);
   coeff[3] = det * ((tau2_2 - tau2_1) * p1mp0 + (tau1_1 - tau1_2) * p2mp0 + (tau1_2 * tau2_1 - tau1_1 * tau2_2) * p3mp0);

   coeff[1] *= 1.0 / deltaTime;
   coeff[2] *= 1.0 / (deltaTime * deltaTime);
   coeff[3] *= 1.0 / (deltaTime * deltaTime * deltaTime);

   return UtInterpolatingPolynomial<T, 3>(coeff, aTime0);
}
} // namespace UtCubicSpline

namespace UtQuinticSpline
{
//! Construct a fifth degree interpolating polynomial.
//! The resulting interpolation will match the provided
//! value, derivatives and second derivatives at the two
//! bounds of the provided interval. The requirements on T
//! are the same for those of the UtInterpolatingPolynomial.
//! The provided boundary values should be provided with the
//! smallest parameter values provided first.
template<typename T>
UtInterpolatingPolynomial<T, 5>
TwoPoint(double aTime0, T aValue0, T aDeriv0, T aSecond0, double aTime1, T aValue1, T aDeriv1, T aSecond1)
{
   double deltaTime = aTime1 - aTime0;
   T      a0        = aValue0;
   T      a1        = deltaTime * aDeriv0;
   T      a2        = deltaTime * deltaTime * aSecond0;
   T      a3        = aValue1;
   T      a4        = deltaTime * aDeriv1;
   T      a5        = deltaTime * deltaTime * aSecond1;

   std::array<T, 6> coeff{};
   coeff[0] = a0;
   coeff[1] = a1;
   coeff[2] = 0.5 * a2;
   coeff[3] = -10.0 * a0 - 6.0 * a1 - 1.5 * a2 + 10.0 * a3 - 4.0 * a4 + 0.5 * a5;
   coeff[4] = 15.0 * a0 + 8.0 * a1 + 1.5 * a2 - 15.0 * a3 + 7.0 * a4 - a5;
   coeff[5] = -6.0 * a0 - 3.0 * a1 - 0.5 * a2 + 6.0 * a3 - 3.0 * a4 + 0.5 * a5;

   coeff[1] *= pow(deltaTime, -1.0);
   coeff[2] *= pow(deltaTime, -2.0);
   coeff[3] *= pow(deltaTime, -3.0);
   coeff[4] *= pow(deltaTime, -4.0);
   coeff[5] *= pow(deltaTime, -5.0);

   return UtInterpolatingPolynomial<T, 5>(coeff, aTime0);
}
} // namespace UtQuinticSpline

#endif
