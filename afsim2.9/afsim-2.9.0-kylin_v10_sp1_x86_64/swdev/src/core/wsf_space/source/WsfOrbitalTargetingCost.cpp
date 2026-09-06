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

#include "WsfOrbitalTargetingCost.hpp"

#include <string>

#include "UtInput.hpp"

namespace wsf
{
namespace space
{

bool BlendedCost::ProcessInput(UtInput& aInput)
{
   aInput.ReadValue(mA_Value);
   aInput.ReadValue(mB_Value);
   aInput.ReadValue(mC_Value);
   return true;
}

double BlendedCost::operator()(double aDt, double aDv) const
{
   // There is no constant term because when looking for a minimum, the overall
   // reference level does not matter.
   return mA_Value * aDt + mB_Value * aDv + mC_Value * aDt * aDv;
}

bool BlendedCost::IsLeastTime() const
{
   return mA_Value != 0.0 && mB_Value == 0.0 && mC_Value == 0.0;
}

bool BlendedCost::IsValid() const
{
   // Only a cost function will all three coefficients set to zero would be invalid.
   return mA_Value != 0.0 || mB_Value != 0.0 || mC_Value != 0.0;
}

std::string BlendedCost::GetConfigurationRequirements() const
{
   return "'blended' cost functions must have at least one non-zero coefficient.";
}

} // namespace space
} // namespace wsf
