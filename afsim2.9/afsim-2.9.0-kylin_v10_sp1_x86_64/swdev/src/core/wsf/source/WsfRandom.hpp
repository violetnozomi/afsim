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

#ifndef WSFRANDOM_HPP
#define WSFRANDOM_HPP

#include "wsf_export.h"

#include <string>

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"
#include "UtScriptTypes.hpp"

namespace wsf
{

namespace script
{

class WSF_EXPORT RandomClass : public UtScriptClass
{
public:
   RandomClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(Uniform_1);
   UT_DECLARE_SCRIPT_METHOD(Uniform_2);
   UT_DECLARE_SCRIPT_METHOD(Bernoulli_1);
   UT_DECLARE_SCRIPT_METHOD(Bernoulli_2);
   UT_DECLARE_SCRIPT_METHOD(Binomial_1);
   UT_DECLARE_SCRIPT_METHOD(Binomial_2);
   UT_DECLARE_SCRIPT_METHOD(Binomial_3);
   UT_DECLARE_SCRIPT_METHOD(NegativeBinomial_1);
   UT_DECLARE_SCRIPT_METHOD(NegativeBinomial_2);
   UT_DECLARE_SCRIPT_METHOD(NegativeBinomial_3);
   UT_DECLARE_SCRIPT_METHOD(Geometric_1);
   UT_DECLARE_SCRIPT_METHOD(Geometric_2);
   UT_DECLARE_SCRIPT_METHOD(Poisson_1);
   UT_DECLARE_SCRIPT_METHOD(Poisson_2);
   UT_DECLARE_SCRIPT_METHOD(Exponential_1);
   UT_DECLARE_SCRIPT_METHOD(Exponential_2);
   UT_DECLARE_SCRIPT_METHOD(Gamma_1);
   UT_DECLARE_SCRIPT_METHOD(Gamma_2);
   UT_DECLARE_SCRIPT_METHOD(Gamma_3);
   UT_DECLARE_SCRIPT_METHOD(Weibull_1);
   UT_DECLARE_SCRIPT_METHOD(Weibull_2);
   UT_DECLARE_SCRIPT_METHOD(Weibull_3);
   UT_DECLARE_SCRIPT_METHOD(ExtremeValue_1);
   UT_DECLARE_SCRIPT_METHOD(ExtremeValue_2);
   UT_DECLARE_SCRIPT_METHOD(ExtremeValue_3);
   UT_DECLARE_SCRIPT_METHOD(Gaussian_1);
   UT_DECLARE_SCRIPT_METHOD(Gaussian_2);
   UT_DECLARE_SCRIPT_METHOD(Gaussian_3);
   UT_DECLARE_SCRIPT_METHOD(Normal_1);
   UT_DECLARE_SCRIPT_METHOD(Normal_2);
   UT_DECLARE_SCRIPT_METHOD(Normal_3);
   UT_DECLARE_SCRIPT_METHOD(LogNormal_1);
   UT_DECLARE_SCRIPT_METHOD(LogNormal_2);
   UT_DECLARE_SCRIPT_METHOD(LogNormal_3);
   UT_DECLARE_SCRIPT_METHOD(ChiSquared_1);
   UT_DECLARE_SCRIPT_METHOD(ChiSquared_2);
   UT_DECLARE_SCRIPT_METHOD(Cauchy_1);
   UT_DECLARE_SCRIPT_METHOD(Cauchy_2);
   UT_DECLARE_SCRIPT_METHOD(Cauchy_3);
   UT_DECLARE_SCRIPT_METHOD(FisherF_1);
   UT_DECLARE_SCRIPT_METHOD(FisherF_2);
   UT_DECLARE_SCRIPT_METHOD(FisherF_3);
   UT_DECLARE_SCRIPT_METHOD(StudentT_1);
   UT_DECLARE_SCRIPT_METHOD(StudentT_2);
   UT_DECLARE_SCRIPT_METHOD(Rayleigh);
};

} // namespace script
} // namespace wsf

#endif
