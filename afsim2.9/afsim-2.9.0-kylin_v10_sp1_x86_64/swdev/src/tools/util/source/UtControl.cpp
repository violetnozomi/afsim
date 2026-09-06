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

#include "UtControl.hpp"

#include <cmath>
#include <iostream>

// ================================================================================================
UtControl::TF_1V::TF_1V()
   : mTimeConstant(0.0)
   , mLastTime(0.0)
   , mLastOutput(0.0)
{
}

// ================================================================================================
void UtControl::TF_1V::Reset(double aTime, double aInitialValue /* = 0.0 */)
{
   mLastTime   = aTime;
   mLastOutput = aInitialValue;
}

// ================================================================================================
double UtControl::TF_1V::Update(double aTime, double aInput)
{
   double dt = aTime - mLastTime;
   if (dt > 0.0)
   {
      if (mTimeConstant > 0.0)
      {
         double s    = exp(-dt / mTimeConstant);
         mLastOutput = ((mLastOutput - aInput) * s) + aInput;
      }
      else
      {
         mLastOutput = aInput / dt;
      }
      mLastTime = aTime;
   }
   return mLastOutput;
}
