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

#ifndef WSFINTERCEPT_HPP
#define WSFINTERCEPT_HPP

#include "wsf_export.h"

//! Method(s) for performing intercept calculations.
//!
namespace WsfIntercept
{
WSF_EXPORT double Intercept(const double aBodyLoc[3],
                            double       aBodySpd,
                            const double aTgtLoc[3],
                            const double aTgtVel[3],
                            double       aIntLoc[3],
                            double       aPredictAheadTime = 0.0);
};

#endif
