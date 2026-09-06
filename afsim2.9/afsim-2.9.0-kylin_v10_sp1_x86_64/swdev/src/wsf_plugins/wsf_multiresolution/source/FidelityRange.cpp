// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "FidelityRange.hpp"

#include "UtInput.hpp"

namespace wsf
{
namespace multiresolution
{
FidelityRange ProcessFidelityRange(UtInput& aInput)
{
   const FidelityRange fidelityRange = [&aInput]
   {
      FidelityRange fidelityRange;
      aInput.ReadValue(fidelityRange.mLowerBound);
      aInput.ReadValue(fidelityRange.mUpperBound);
      return fidelityRange;
   }();
   // Affirm range and order
   aInput.ValueInClosedRange(fidelityRange.mLowerBound, 0.0, 1.0);
   aInput.ValueInClosedRange(fidelityRange.mUpperBound, 0.0, 1.0);
   aInput.ValueLessOrEqual(fidelityRange.mLowerBound, fidelityRange.mUpperBound);
   return fidelityRange;
}
} // namespace multiresolution
} // namespace wsf
