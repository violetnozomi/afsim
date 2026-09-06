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

#ifndef UTSLEEP_HPP
#define UTSLEEP_HPP

#include "ut_export.h"

/** Defines a system-independent "sleep" method.
   @note Original code is from the IWARS-SysUtil class.
*/
namespace UtSleep
{
//! The minimum value allowed as an argument to Sleep(double).
//! @note This constant is provided as a convenience to the user,
//!   but it is not used in the implementation of UtSleep.
extern const double cMIN_SLEEP;
void UT_EXPORT      Sleep(double aSleepTime);
//! Sleep the minimal amount of time allowed
void UT_EXPORT SleepMinimum();
} // namespace UtSleep
#endif
