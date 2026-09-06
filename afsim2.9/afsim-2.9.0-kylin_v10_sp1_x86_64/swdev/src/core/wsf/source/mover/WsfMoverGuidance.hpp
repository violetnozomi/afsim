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

#ifndef WSFMOVERGUIDANCE_HPP
#define WSFMOVERGUIDANCE_HPP

#include "wsf_export.h"

//! An abstract interface between a mover and its source of guidance.
//! This is currently a placeholder.
class WSF_EXPORT WsfMoverGuidance
{
public:
   WsfMoverGuidance();
   virtual ~WsfMoverGuidance();

   virtual void UpdateGuidance(double aSimTime, double aCurTime, double aEndTime) = 0;
};

#endif
