// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "UtScriptMath.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>

#include "UtEarth.hpp"
#include "UtLatPos.hpp"
#include "UtLonPos.hpp"
#include "UtMath.hpp"
#include "UtScriptMethodDefine.hpp"
#include "UtScriptVector.hpp"

UT_MAP_TYPE_TO_SCRIPT_NAME(UtScriptMath, "Math");

UtScriptMath::UtScriptMath(UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass("Math", aScriptTypesPtr)
{
   UT_SCRIPT_WRAP_CLASS(UtScriptMath);

   // Math Constants.
   UT_SCRIPT_CONSTEXPR_VALUE(E, UtMath::cE);
   UT_SCRIPT_CONSTEXPR_VALUE(PI, UtMath::cPI);
   UT_SCRIPT_CONSTEXPR_VALUE(PI_OVER_2, UtMath::cPI_OVER_2);
   UT_SCRIPT_CONSTEXPR_VALUE(PI_OVER_4, UtMath::cPI_OVER_4);
   UT_SCRIPT_CONSTEXPR_VALUE(TWO_PI, UtMath::cTWO_PI);
   UT_SCRIPT_CONSTEXPR_VALUE(FOUR_PI, UtMath::cFOUR_PI);
   UT_SCRIPT_CONSTEXPR_VALUE(RAD_PER_DEG, UtMath::cRAD_PER_DEG);
   UT_SCRIPT_CONSTEXPR_VALUE(DEG_PER_RAD, UtMath::cDEG_PER_RAD);
   UT_SCRIPT_CONSTEXPR_VALUE(M_PER_FT, UtMath::cM_PER_FT);
   UT_SCRIPT_CONSTEXPR_VALUE(M_PER_MI, UtMath::cM_PER_MI);
   UT_SCRIPT_CONSTEXPR_VALUE(M_PER_NM, UtMath::cM_PER_NM);
   UT_SCRIPT_CONSTEXPR_VALUE(NM_PER_M, UtMath::cNM_PER_M);
   UT_SCRIPT_CONSTEXPR_VALUE(FT_PER_M, UtMath::cFT_PER_M);
   UT_SCRIPT_CONSTEXPR_VALUE(FT_PER_MI, UtMath::cFT_PER_MI);
   UT_SCRIPT_CONSTEXPR_VALUE(FT_PER_NM, UtMath::cFT_PER_NM);
   UT_SCRIPT_CONSTEXPR_VALUE(NM_PER_FT, UtMath::cNM_PER_FT);
   UT_SCRIPT_CONSTEXPR_VALUE(MPS_PER_MPH, UtMath::cMPS_PER_MPH);
   UT_SCRIPT_CONSTEXPR_VALUE(MPH_PER_MPS, UtMath::cMPH_PER_MPS);
   UT_SCRIPT_CONSTEXPR_VALUE(MPS_PER_NMPH, UtMath::cMPS_PER_NMPH);
   UT_SCRIPT_CONSTEXPR_VALUE(NMPH_PER_MPS, UtMath::cNMPH_PER_MPS);
   UT_SCRIPT_CONSTEXPR_VALUE(LB_PER_KG, UtMath::cLB_PER_KG);
   UT_SCRIPT_CONSTEXPR_VALUE(LB_PER_NT, UtMath::cLB_PER_NT);
   UT_SCRIPT_CONSTEXPR_VALUE(KG_PER_LB, UtMath::cKG_PER_LB);
   UT_SCRIPT_CONSTEXPR_VALUE(NT_PER_LB, UtMath::cNT_PER_LB);
   UT_SCRIPT_CONSTEXPR_VALUE(LIGHT_SPEED, UtMath::cLIGHT_SPEED);
   UT_SCRIPT_CONSTEXPR_VALUE(BOLTZMANN_CONSTANT, UtMath::cBOLTZMANN_CONSTANT);
   UT_SCRIPT_CONSTEXPR_VALUE(GRAVITATIONAL_CONSTANT, UtMath::cGRAVITATIONAL_CONSTANT);
   UT_SCRIPT_CONSTEXPR_VALUE(DOUBLE_MIN, UtMath::cDOUBLE_MIN);
   UT_SCRIPT_CONSTEXPR_VALUE(DOUBLE_MAX, UtMath::cDOUBLE_MAX);
   UT_SCRIPT_CONSTEXPR_VALUE(INTEGER_MIN, UtMath::cINTEGER_MIN);
   UT_SCRIPT_CONSTEXPR_VALUE(INTEGER_MAX, UtMath::cINTEGER_MAX);

   // Math Functions.
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, Sin, (double deg)) { return sin(deg * UtMath::cRAD_PER_DEG); };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, ASin, (double val)) { return asin(val) * UtMath::cDEG_PER_RAD; };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, Cos, (double deg)) { return cos(deg * UtMath::cRAD_PER_DEG); };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, ACos, (double val)) { return acos(val) * UtMath::cDEG_PER_RAD; };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, Tan, (double deg)) { return tan(deg * UtMath::cRAD_PER_DEG); };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, ATan, (double val)) { return atan(val) * UtMath::cDEG_PER_RAD; };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, ATan2, (double y, double x)) { return atan2(y, x) * UtMath::cDEG_PER_RAD; };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, Sqrt, (double val)) { return sqrt(val); };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, Fabs, (double val)) { return fabs(val); };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, Log10, (double val)) { return log10(val); };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, Log, (double val)) { return log(val); };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, Ln, (double val)) { return log(val); };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, Log, (double val, double base)) { return log(val) / log(base); };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, Pow, (double val, double exponent)) { return pow(val, exponent); };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, Floor, (double val)) { return floor(val); };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, Ceil, (double val)) { return ceil(val); };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, Fmod, (double x, double y)) { return fmod(x, y); };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(int, Mod, (int val, int modulo)) { return val % modulo; };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, Max, (double a, double b)) { return std::max(a, b); };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, Min, (double a, double b)) { return std::min(a, b); };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double,
                           Lerp,
                           (double aValue, double aValueMin, double aValueMax, double aRangeMin, double aRangeMax))
   {
      return UtMath::Lerp(aValue, aValueMin, aValueMax, aRangeMin, aRangeMax);
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, Limit, (double val, double limit)) { return UtMath::Limit(val, limit); };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, Limit, (double val, double lowerLimit, double upperLimit))
   {
      return UtMath::Limit(val, lowerLimit, upperLimit);
   };

   // Angle functions
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(bool, AngleIsBetween, (double angle_deg, double lowerLimit_deg, double upperLimit_deg))
   {
      return UtMath::AngleIsBetween(angle_deg * UtMath::cRAD_PER_DEG, // Convert from degrees to radians
                                    lowerLimit_deg * UtMath::cRAD_PER_DEG,
                                    upperLimit_deg * UtMath::cRAD_PER_DEG);
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(bool, AngleWithinTolerance, (double angle_deg, double angle_tolerance_deg))
   {
      return UtMath::AngleWithinTolerance(angle_deg * UtMath::cRAD_PER_DEG, // Convert from degrees to radians
                                          angle_tolerance_deg * UtMath::cRAD_PER_DEG);
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, NormalizeAngleMinus180_180, (double deg))
   {
      return UtMath::NormalizeAngleMinus180_180(deg);
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, NormalizeAngle0_360, (double deg)) { return UtMath::NormalizeAngle0_360(deg); };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, Sign, (double val)) { return UtMath::Sign(val); };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, LinearToDB, (double val)) { return UtMath::LinearToDB(val); };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, DB_ToLinear, (double val)) { return UtMath::DB_ToLinear(val); };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, SafeLinearToDB, (double val)) { return UtMath::SafeLinearToDB(val); };

   // Random Number Generator
   UT_SCRIPT_METHOD(void, SetSeed, (int seed)) { self.GetRandom().SetSeed(seed); };
   UT_SCRIPT_METHOD(int, Seed, ()) { return self.GetRandom().GetSeed(); };

   UT_SCRIPT_METHOD(double, RandomUniform, ()) { return self.GetRandom().template Uniform<double>(); };

   UT_SCRIPT_METHOD(double, RandomUniform, (double min, double max)) { return self.GetRandom().Uniform(min, max); };

   UT_SCRIPT_METHOD(bool, RandomBernoulli, ()) { return self.GetRandom().Bernoulli(); };

   UT_SCRIPT_METHOD(double, RandomBernoulli, (double p)) { return self.GetRandom().Bernoulli(p); };

   UT_SCRIPT_METHOD(int, RandomBinomial, ()) { return self.GetRandom().Binomial(); };

   UT_SCRIPT_METHOD(int, RandomBinomial, (int t)) { return self.GetRandom().Binomial(t); };

   UT_SCRIPT_METHOD(int, RandomBinomial, (int t, double p)) { return self.GetRandom().Binomial(t, p); };

   UT_SCRIPT_METHOD(int, RandomNegativeBinomial, ()) { return self.GetRandom().NegativeBinomial(); };

   UT_SCRIPT_METHOD(int, RandomNegativeBinomial, (int k)) { return self.GetRandom().NegativeBinomial(k); };

   UT_SCRIPT_METHOD(int, RandomNegativeBinomial, (int k, double p)) { return self.GetRandom().NegativeBinomial(k, p); };

   UT_SCRIPT_METHOD(int, RandomGeometric, ()) { return self.GetRandom().Geometric(); };

   UT_SCRIPT_METHOD(int, RandomGeometric, (double p)) { return self.GetRandom().Geometric(p); };

   UT_SCRIPT_METHOD(int, RandomPoisson, ()) { return self.GetRandom().Poisson(); };

   UT_SCRIPT_METHOD(int, RandomPoisson, (double mean)) { return self.GetRandom().Poisson(mean); };

   UT_SCRIPT_METHOD(double, RandomExponential, ()) { return self.GetRandom().Exponential(); };

   UT_SCRIPT_METHOD(double, RandomExponential, (double lambda)) { return self.GetRandom().Exponential(lambda); };

   UT_SCRIPT_METHOD(double, RandomGamma, ()) { return self.GetRandom().Gamma(); };

   UT_SCRIPT_METHOD(double, RandomGamma, (double alpha)) { return self.GetRandom().Gamma(alpha); };

   UT_SCRIPT_METHOD(double, RandomGamma, (double alpha, double beta)) { return self.GetRandom().Gamma(alpha, beta); };

   UT_SCRIPT_METHOD(double, RandomWeibull, ()) { return self.GetRandom().Weibull(); };

   UT_SCRIPT_METHOD(double, RandomWeibull, (double a)) { return self.GetRandom().Weibull(a); };

   UT_SCRIPT_METHOD(double, RandomWeibull, (double a, double b)) { return self.GetRandom().Weibull(a, b); };

   UT_SCRIPT_METHOD(double, RandomExtremeValue, ()) { return self.GetRandom().ExtremeValue(); };

   UT_SCRIPT_METHOD(double, RandomExtremeValue, (double a)) { return self.GetRandom().ExtremeValue(a); };

   UT_SCRIPT_METHOD(double, RandomExtremeValue, (double a, double b)) { return self.GetRandom().ExtremeValue(a, b); };

   UT_SCRIPT_METHOD(double, RandomGaussian, ()) { return self.GetRandom().Gaussian(); };

   UT_SCRIPT_METHOD(double, RandomGaussian, (double mean)) { return self.GetRandom().Gaussian(mean); };

   UT_SCRIPT_METHOD(double, RandomGaussian, (double mean, double stdDev))
   {
      return self.GetRandom().Gaussian(mean, stdDev);
   };

   UT_SCRIPT_METHOD(double, RandomNormal, ()) { return self.GetRandom().Normal(); };

   UT_SCRIPT_METHOD(double, RandomNormal, (double mean)) { return self.GetRandom().Normal(mean); };

   UT_SCRIPT_METHOD(double, RandomNormal, (double mean, double stdDev))
   {
      return self.GetRandom().Normal(mean, stdDev);
   };

   UT_SCRIPT_METHOD(double, RandomLogNormal, ()) { return self.GetRandom().LogNormal(); };

   UT_SCRIPT_METHOD(double, RandomLogNormal, (double m)) { return self.GetRandom().LogNormal(m); };

   UT_SCRIPT_METHOD(double, RandomLogNormal, (double m, double s)) { return self.GetRandom().LogNormal(m, s); };

   UT_SCRIPT_METHOD(double, RandomChiSquared, ()) { return self.GetRandom().ChiSquared(); };

   UT_SCRIPT_METHOD(double, RandomChiSquared, (double n)) { return self.GetRandom().ChiSquared(n); };

   UT_SCRIPT_METHOD(double, RandomCauchy, ()) { return self.GetRandom().Cauchy(); };

   UT_SCRIPT_METHOD(double, RandomCauchy, (double a)) { return self.GetRandom().Cauchy(a); };

   UT_SCRIPT_METHOD(double, RandomCauchy, (double a, double b)) { return self.GetRandom().Cauchy(a, b); };

   UT_SCRIPT_METHOD(double, RandomFisherF, ()) { return self.GetRandom().FisherF(); };

   UT_SCRIPT_METHOD(double, RandomFisherF, (double m)) { return self.GetRandom().FisherF(m); };

   UT_SCRIPT_METHOD(double, RandomFisherF, (double m, double n)) { return self.GetRandom().FisherF(m, n); };

   UT_SCRIPT_METHOD(double, RandomStudentT, ()) { return self.GetRandom().StudentT(); };

   UT_SCRIPT_METHOD(double, RandomStudentT, (double n)) { return self.GetRandom().StudentT(n); };

   UT_SCRIPT_METHOD(double, RandomRayleigh, (double radius)) { return self.GetRandom().Rayleigh(radius); };

   UT_SCRIPT_METHOD(int, Roll, (int lo, int hi)) { return self.GetRandom().Uniform(lo, hi); };

   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, StringToLat, (const std::string& str))
   {
      UtLatPos           value;
      std::istringstream iss(str);
      if (iss >> value)
      {
         return value;
      }
      return 0.0;
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, StringToLon, (const std::string& str))
   {
      UtLonPos           value;
      std::istringstream iss(str);
      if (iss >> value)
      {
         return value;
      }
      return 0.0;
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(std::string, LatToString, (double lat))
   {
      UtLatPos           value(lat);
      std::ostringstream oss;
      oss << value;
      return oss.str();
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(std::string, LonToString, (double lon))
   {
      UtLonPos           value(lon);
      std::ostringstream oss;
      oss << value;
      return oss.str();
   };

   UT_SCRIPT_METHOD(std::vector<int>, AssignMaxProfit, (const UtScriptVector<UtScriptVector<int>>& inputRows))
   {
      // populate "values" from input argument
      std::vector<std::vector<int>> values;
      values.reserve(inputRows.size());
      for (const auto& row : inputRows)
      {
         values.push_back(static_cast<std::vector<int>>(row));
      };
      // find assignments
      std::vector<int> assigns = self.GetOptimalAssigner().AssignMaxProfit(values);
      return assigns;
   };

   UT_SCRIPT_METHOD(std::vector<int>, AssignMaxProfit, (const UtScriptVector<UtScriptVector<double>>& inputRows))
   {
      // populate "values" from input argument
      std::vector<std::vector<double>> values;
      values.reserve(inputRows.size());
      for (const auto& row : inputRows)
      {
         values.push_back(static_cast<std::vector<double>>(row));
      };

      // find assignments
      std::vector<int> assigns = self.GetOptimalAssigner().AssignMaxProfit(values);
      return assigns;
   };

   UT_SCRIPT_METHOD(std::vector<int>, AssignMinCost, (const UtScriptVector<UtScriptVector<int>>& inputRows))
   {
      // populate "values" from input argument
      std::vector<std::vector<int>> values;
      values.reserve(inputRows.size());
      for (const auto& row : inputRows)
      {
         values.push_back(static_cast<std::vector<int>>(row));
      };
      // find assignments
      std::vector<int> assigns = self.GetOptimalAssigner().AssignMinCost(values);
      return assigns;
   };

   UT_SCRIPT_METHOD(std::vector<int>, AssignMinCost, (const UtScriptVector<UtScriptVector<double>>& inputRows))
   {
      // populate "values" from input argument
      std::vector<std::vector<double>> values;
      values.reserve(inputRows.size());
      for (const auto& row : inputRows)
      {
         values.push_back(static_cast<std::vector<double>>(row));
      };
      // find assignments
      std::vector<int> assigns = self.GetOptimalAssigner().AssignMinCost(values);
      return assigns;
   };
}
