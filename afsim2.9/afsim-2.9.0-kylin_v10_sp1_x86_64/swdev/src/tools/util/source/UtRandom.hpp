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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UTRANDOM_HPP
#define UTRANDOM_HPP

#include "ut_export.h"

#include <cmath>
#include <random>

#include "UtTypeTraits.hpp"

namespace ut
{

class UT_EXPORT Random
{
public:
   void SetSeed(unsigned int aSeed)
   {
      mSeed = aSeed;
      mGen.seed(aSeed);
   }

   unsigned int GetSeed() const { return mSeed; }

   // Uniform Distributions
   template<class T = double>
   ut::enable_if_t<std::is_floating_point<T>::value, T> Uniform(T aMin = 0.0, T aMax = 1.0)
   {
      return std::uniform_real_distribution<T>{aMin, aMax}(mGen);
   }

   template<class T = int>
   ut::enable_if_t<std::is_integral<T>::value, T> Uniform(T aMin = 0, T aMax = std::numeric_limits<T>::max())
   {
      return std::uniform_int_distribution<T>{aMin, aMax}(mGen);
   }

   // Bernoulli Distributions

   bool Bernoulli(double aP = 0.5) { return std::bernoulli_distribution(aP)(mGen); }

   template<class T = int>
   T Binomial(T aT = 1, double aP = 0.5)
   {
      static_assert(std::is_integral<T>::value, "Binomial distribution type must be an integral type");
      return std::binomial_distribution<T>{aT, aP}(mGen);
   }

   template<class T = int>
   T NegativeBinomial(T aK = 1, double aP = 0.5)
   {
      static_assert(std::is_integral<T>::value, "NegativeBinomial distribution type must be an integral type");
      return std::negative_binomial_distribution<T>{aK, aP}(mGen);
   }

   template<class T = int>
   T Geometric(double aP = 0.5)
   {
      static_assert(std::is_integral<T>::value, "Geometric distribution type must be an integral type");
      return std::geometric_distribution<T>{aP}(mGen);
   }

   // Poisson Distributions
   template<class T = int>
   T Poisson(double aMean = 1.0)
   {
      static_assert(std::is_integral<T>::value, "Poisson distribution type must be an integral type");
      return std::poisson_distribution<T>{aMean}(mGen);
   }

   template<class T = double>
   T Exponential(T aLambda = 1.0)
   {
      static_assert(std::is_floating_point<T>::value, "Exponential distribution type must be a floating point type");
      return std::exponential_distribution<T>{aLambda}(mGen);
   }

   template<class T = double>
   T Gamma(T aAlpha = 1.0, T aBeta = 1.0)
   {
      static_assert(std::is_floating_point<T>::value, "Gamma distribution type must be a floating point type");
      return std::gamma_distribution<T>{aAlpha, aBeta}(mGen);
   }

   template<class T = double>
   T Weibull(T aA = 1.0, T aB = 1.0)
   {
      static_assert(std::is_floating_point<T>::value, "Weibull distribution type must be a floating point type");
      return std::weibull_distribution<T>{aA, aB}(mGen);
   }

   template<class T = double>
   T ExtremeValue(T aA = 0.0, T aB = 1.0)
   {
      static_assert(std::is_floating_point<T>::value, "ExtremeValue distribution type must be a floating point type");
      return std::extreme_value_distribution<T>{aA, aB}(mGen);
   }

   // Normal Distributions
   template<class T = double>
   T Gaussian(T aMean = 0.0, T aStdDev = 1.0)
   {
      return Normal<T>(aMean, aStdDev);
   }

   template<class T = double>
   T Normal(T aMean = 0.0, T aStdDev = 1.0)
   {
      static_assert(std::is_floating_point<T>::value, "Normal distribution type must be a floating point type");
      return std::normal_distribution<T>{aMean, aStdDev}(mGen);
   }

   template<class T = double>
   T LogNormal(T aM = 0.0, T aS = 1.0)
   {
      static_assert(std::is_floating_point<T>::value, "LogNormal distribution type must be a floating point type");
      return std::lognormal_distribution<T>{aM, aS}(mGen);
   }

   template<class T = double>
   T ChiSquared(T aN = 1.0)
   {
      static_assert(std::is_floating_point<T>::value, "ChiSquared distribution type must be a floating point type");
      return std::chi_squared_distribution<T>{aN}(mGen);
   }

   template<class T = double>
   T Cauchy(T aA = 0.0, T aB = 1.0)
   {
      static_assert(std::is_floating_point<T>::value, "Cauchy distribution type must be a floating point type");
      return std::cauchy_distribution<T>{aA, aB}(mGen);
   }

   template<class T = double>
   T FisherF(T aM = 1.0, T aN = 1.0)
   {
      static_assert(std::is_floating_point<T>::value, "FisherF distribution type must be a floating point type");
      return std::fisher_f_distribution<T>{aM, aN}(mGen);
   }

   template<class T = double>
   T StudentT(T aN = 1)
   {
      static_assert(std::is_floating_point<T>::value, "StudentT distribution type must be a floating point type");
      return std::student_t_distribution<T>{aN}(mGen);
   }

   template<class T = double>
   T Rayleigh(T aRadius)
   {
      static_assert(std::is_floating_point<T>::value, "Rayleigh distribution type must be a floating point type");
      return std::sqrt(-aRadius * aRadius * std::log(Uniform<T>()) / std::log(2.0));
   }

private:
   unsigned int mSeed{1};
   std::mt19937 mGen{1};
};

} // namespace ut

#endif
