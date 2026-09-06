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

#ifndef UTCONTROL_HPP
#define UTCONTROL_HPP

#include "ut_export.h"

// =================================================================================================

//! Namespace containing classes supporting simple control systems.
namespace UtControl
{
//! Transfer function of the form 1 / (S + 1), where the timestep is variable.
class UT_EXPORT TF_1V
{
public:
   TF_1V();
   void   SetTimeConstant(double aTimeConstant) { mTimeConstant = aTimeConstant; }
   void   Reset(double aTime, double aInitialValue = 0.0);
   double Update(double aTime, double aInput);

private:
   double mTimeConstant;
   double mLastTime;
   double mLastOutput;
};
} // namespace UtControl

#endif
