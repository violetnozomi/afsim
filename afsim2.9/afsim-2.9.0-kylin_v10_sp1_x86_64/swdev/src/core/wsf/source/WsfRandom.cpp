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

#include "WsfRandom.hpp"

#include "UtMemory.hpp"
#include "UtRandom.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptObjectClass.hpp"

namespace wsf
{

namespace script
{
// =================================================================================================
RandomClass::RandomClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfRandom");
   mConstructible = false;
   AddStaticMethod(ut::make_unique<Uniform_1>("Uniform"));
   AddStaticMethod(ut::make_unique<Uniform_2>("Uniform"));
   AddStaticMethod(ut::make_unique<Bernoulli_1>("Bernoulli"));
   AddStaticMethod(ut::make_unique<Bernoulli_2>("Bernoulli"));
   AddStaticMethod(ut::make_unique<Binomial_1>("Binomial"));
   AddStaticMethod(ut::make_unique<Binomial_2>("Binomial"));
   AddStaticMethod(ut::make_unique<Binomial_3>("Binomial"));
   AddStaticMethod(ut::make_unique<NegativeBinomial_1>("NegativeBinomial"));
   AddStaticMethod(ut::make_unique<NegativeBinomial_2>("NegativeBinomial"));
   AddStaticMethod(ut::make_unique<NegativeBinomial_3>("NegativeBinomial"));
   AddStaticMethod(ut::make_unique<Poisson_1>("Poisson"));
   AddStaticMethod(ut::make_unique<Poisson_2>("Poisson"));
   AddStaticMethod(ut::make_unique<Exponential_1>("Exponential"));
   AddStaticMethod(ut::make_unique<Exponential_2>("Exponential"));
   AddStaticMethod(ut::make_unique<Gamma_1>("Gamma"));
   AddStaticMethod(ut::make_unique<Gamma_2>("Gamma"));
   AddStaticMethod(ut::make_unique<Gamma_3>("Gamma"));
   AddStaticMethod(ut::make_unique<Weibull_1>("Weibull"));
   AddStaticMethod(ut::make_unique<Weibull_2>("Weibull"));
   AddStaticMethod(ut::make_unique<Weibull_3>("Weibull"));
   AddStaticMethod(ut::make_unique<ExtremeValue_1>("ExtremeValue"));
   AddStaticMethod(ut::make_unique<ExtremeValue_2>("ExtremeValue"));
   AddStaticMethod(ut::make_unique<ExtremeValue_3>("ExtremeValue"));
   AddStaticMethod(ut::make_unique<Gaussian_1>("Gaussian"));
   AddStaticMethod(ut::make_unique<Gaussian_2>("Gaussian"));
   AddStaticMethod(ut::make_unique<Gaussian_3>("Gaussian"));
   AddStaticMethod(ut::make_unique<Normal_1>("Normal"));
   AddStaticMethod(ut::make_unique<Normal_2>("Normal"));
   AddStaticMethod(ut::make_unique<Normal_3>("Normal"));
   AddStaticMethod(ut::make_unique<LogNormal_1>("LogNormal"));
   AddStaticMethod(ut::make_unique<LogNormal_2>("LogNormal"));
   AddStaticMethod(ut::make_unique<LogNormal_3>("LogNormal"));
   AddStaticMethod(ut::make_unique<ChiSquared_1>("ChiSquared"));
   AddStaticMethod(ut::make_unique<ChiSquared_2>("ChiSquared"));
   AddStaticMethod(ut::make_unique<Cauchy_1>("Cauchy"));
   AddStaticMethod(ut::make_unique<Cauchy_2>("Cauchy"));
   AddStaticMethod(ut::make_unique<Cauchy_3>("Cauchy"));
   AddStaticMethod(ut::make_unique<FisherF_1>("FisherF"));
   AddStaticMethod(ut::make_unique<FisherF_2>("FisherF"));
   AddStaticMethod(ut::make_unique<FisherF_3>("FisherF"));
   AddStaticMethod(ut::make_unique<StudentT_1>("StudentT"));
   AddStaticMethod(ut::make_unique<StudentT_2>("StudentT"));
   AddStaticMethod(ut::make_unique<Rayleigh>());
}

#define GETRAND WsfScriptContext::GetSIMULATION(aContext)->GetRandom()

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Uniform_1, 2, "double", "double, double")
{
   aReturnVal.SetDouble(GETRAND.Uniform(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Uniform_2, 0, "double", "")
{
   aReturnVal.SetDouble(GETRAND.Uniform<double>());
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Bernoulli_1, 0, "bool", "")
{
   aReturnVal.SetBool(GETRAND.Bernoulli());
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Bernoulli_2, 1, "bool", "double")
{
   aReturnVal.SetBool(GETRAND.Bernoulli(aVarArgs[0].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Binomial_1, 0, "int", "")
{
   aReturnVal.SetInt(GETRAND.Binomial());
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Binomial_2, 1, "int", "int")
{
   int t = aVarArgs[0].GetInt();
   aReturnVal.SetInt(GETRAND.Binomial(t));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Binomial_3, 2, "int", "int, double")
{
   int    t = aVarArgs[0].GetInt();
   double p = aVarArgs[1].GetDouble();
   aReturnVal.SetInt(GETRAND.Binomial(t, p));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, NegativeBinomial_1, 0, "int", "")
{
   aReturnVal.SetInt(GETRAND.NegativeBinomial());
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, NegativeBinomial_2, 1, "int", "int")
{
   int k = aVarArgs[0].GetInt();
   aReturnVal.SetInt(GETRAND.NegativeBinomial(k));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, NegativeBinomial_3, 2, "int", "int, double")
{
   int    k = aVarArgs[0].GetInt();
   double p = aVarArgs[1].GetDouble();
   aReturnVal.SetInt(GETRAND.NegativeBinomial(k, p));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Geometric_1, 0, "int", "")
{
   aReturnVal.SetInt(GETRAND.Geometric());
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Geometric_2, 1, "int", "double")
{
   double p = aVarArgs[0].GetDouble();
   aReturnVal.SetInt(GETRAND.Geometric(p));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Poisson_1, 0, "int", "")
{
   aReturnVal.SetInt(GETRAND.Poisson());
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Poisson_2, 1, "int", "double")
{
   double mean = aVarArgs[0].GetDouble();
   aReturnVal.SetInt(GETRAND.Poisson(mean));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Exponential_1, 0, "double", "")
{
   aReturnVal.SetDouble(GETRAND.Exponential());
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Exponential_2, 1, "double", "double")
{
   double lambda = aVarArgs[0].GetDouble();
   aReturnVal.SetDouble(GETRAND.Exponential(lambda));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Gamma_1, 0, "double", "")
{
   aReturnVal.SetDouble(GETRAND.Gamma());
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Gamma_2, 1, "double", "double")
{
   double alpha = aVarArgs[0].GetDouble();
   aReturnVal.SetDouble(GETRAND.Gamma(alpha));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Gamma_3, 2, "double", "double, double")
{
   double alpha = aVarArgs[0].GetDouble();
   double beta  = aVarArgs[1].GetDouble();
   aReturnVal.SetDouble(GETRAND.Gamma(alpha, beta));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Weibull_1, 0, "double", "")
{
   aReturnVal.SetDouble(GETRAND.Weibull());
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Weibull_2, 1, "double", "double")
{
   double a = aVarArgs[0].GetDouble();
   aReturnVal.SetDouble(GETRAND.Weibull(a));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Weibull_3, 2, "double", "double, double")
{
   double a = aVarArgs[0].GetDouble();
   double b = aVarArgs[1].GetDouble();
   aReturnVal.SetDouble(GETRAND.Weibull(a, b));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, ExtremeValue_1, 0, "double", "")
{
   aReturnVal.SetDouble(GETRAND.ExtremeValue());
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, ExtremeValue_2, 1, "double", "double")
{
   double a = aVarArgs[0].GetDouble();
   aReturnVal.SetDouble(GETRAND.ExtremeValue(a));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, ExtremeValue_3, 2, "double", "double, double")
{
   double a = aVarArgs[0].GetDouble();
   double b = aVarArgs[1].GetDouble();
   aReturnVal.SetDouble(GETRAND.ExtremeValue(a, b));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Gaussian_1, 0, "double", "")
{
   aReturnVal.SetDouble(GETRAND.Gaussian());
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Gaussian_2, 1, "double", "double")
{
   aReturnVal.SetDouble(GETRAND.Gaussian(aVarArgs[0].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Gaussian_3, 2, "double", "double, double")
{
   aReturnVal.SetDouble(GETRAND.Gaussian(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Normal_1, 2, "double", "double, double")
{
   aReturnVal.SetDouble(GETRAND.Normal(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Normal_2, 1, "double", "double")
{
   aReturnVal.SetDouble(GETRAND.Normal(aVarArgs[0].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Normal_3, 0, "double", "")
{
   aReturnVal.SetDouble(GETRAND.Normal());
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, LogNormal_1, 0, "double", "")
{
   aReturnVal.SetDouble(GETRAND.LogNormal());
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, LogNormal_2, 1, "double", "double")
{
   aReturnVal.SetDouble(GETRAND.LogNormal(aVarArgs[0].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, LogNormal_3, 2, "double", "double, double")
{
   aReturnVal.SetDouble(GETRAND.LogNormal(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, ChiSquared_1, 0, "double", "")
{
   aReturnVal.SetDouble(GETRAND.ChiSquared());
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, ChiSquared_2, 1, "double", "double")
{
   double n = aVarArgs[0].GetDouble();
   aReturnVal.SetDouble(GETRAND.ChiSquared(n));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Cauchy_1, 0, "double", "")
{
   aReturnVal.SetDouble(GETRAND.Cauchy());
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Cauchy_2, 1, "double", "double")
{
   double a = aVarArgs[0].GetDouble();
   aReturnVal.SetDouble(GETRAND.Cauchy(a));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Cauchy_3, 2, "double", "double, double")
{
   double a = aVarArgs[0].GetDouble();
   double b = aVarArgs[1].GetDouble();
   aReturnVal.SetDouble(GETRAND.Cauchy(a, b));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, FisherF_1, 0, "double", "")
{
   aReturnVal.SetDouble(GETRAND.FisherF());
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, FisherF_2, 1, "double", "double")
{
   double m = aVarArgs[0].GetDouble();
   aReturnVal.SetDouble(GETRAND.FisherF(m));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, FisherF_3, 2, "double", "double, double")
{
   double m = aVarArgs[0].GetDouble();
   double n = aVarArgs[1].GetDouble();
   aReturnVal.SetDouble(GETRAND.FisherF(m, n));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, StudentT_1, 0, "double", "")
{
   aReturnVal.SetDouble(GETRAND.StudentT());
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, StudentT_2, 1, "double", "double")
{
   double n = aVarArgs[0].GetDouble();
   aReturnVal.SetDouble(GETRAND.StudentT(n));
}

UT_DEFINE_SCRIPT_METHOD(RandomClass, WsfSimulation, Rayleigh, 1, "double", "double")
{
   double radius = aVarArgs[0].GetDouble();
   aReturnVal.SetDouble(GETRAND.Rayleigh(radius));
}

} // namespace script
} // namespace wsf
