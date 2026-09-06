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

#ifndef WSFORBITALTARGETINGCOST_HPP
#define WSFORBITALTARGETINGCOST_HPP

#include "wsf_space_export.h"

#include <string>

class UtInput;
#include "UtScriptAccessible.hpp"

namespace wsf
{
namespace space
{

//! A cost function to minimize when computing a solution for an orbital targeting operation.
class OrbitalTargetingCost : public UtScriptAccessible
{
public:
   virtual OrbitalTargetingCost* Clone() const = 0;
   virtual bool                  ProcessInput(UtInput& aInput) { return false; }

   //! The cost function is a mapping from a delta-t, delta-v value pair into the real numbers.
   virtual double operator()(double aDt, double aDv) const = 0;

   //! Return if this cost is equivalent to a least time optimization.
   virtual bool IsLeastTime() const = 0;

   //! Return if the cost function has a valid configuration.
   virtual bool IsValid() const = 0;

   //! Return a text description of the requirements on the cost function to be considered valid.
   virtual std::string GetConfigurationRequirements() const = 0;
};

//! The blended cost function contains polynomials in delta-t and delta-v up to first order in each.
//! The function depends on three parameters, A, B, and C as follows:
//!     g(dt, dv) = A * dt + B * dv + C * dt * dv.
//! A valid configuration of this object will have at least one of {A, B, C} non-zero. This contains
//! least-time and least-delta-v as special cases.
class WSF_SPACE_EXPORT BlendedCost : public OrbitalTargetingCost
{
public:
   BlendedCost()                          = default;
   BlendedCost(const BlendedCost& aOther) = default;
   BlendedCost(double aA_Value, double aB_Value, double aC_Value)
      : mA_Value{aA_Value}
      , mB_Value{aB_Value}
      , mC_Value{aC_Value}
   {
   }

   BlendedCost* Clone() const override { return new BlendedCost{*this}; }

   bool ProcessInput(UtInput& aInput) override;

   double operator()(double aDt, double aDv) const override;

   const char* GetScriptClassName() const override { return "WsfOrbitalBlendedCost"; }

   bool IsLeastTime() const override;

   bool IsValid() const override;

   std::string GetConfigurationRequirements() const override;

   double GetA_Value() const { return mA_Value; }
   double GetB_Value() const { return mB_Value; }
   double GetC_Value() const { return mC_Value; }

   void SetA_Value(double aA_Value) { mA_Value = aA_Value; }
   void SetB_Value(double aB_Value) { mB_Value = aB_Value; }
   void SetC_Value(double aC_Value) { mC_Value = aC_Value; }

private:
   double mA_Value{0.0}; //! The coefficient for the delta-t term. [1/s]
   double mB_Value{1.0}; //! The coefficient for the delta-v term. [s/m]
   double mC_Value{0.0}; //! The coefficient for the delta-t x delta-v term. [1/m]
};

} // namespace space
} // namespace wsf

#endif // WSFORBITALTARGETINGCOST_HPP
