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

#ifndef WSFDISVISUALIZATION_HPP
#define WSFDISVISUALIZATION_HPP

#include "wsf_export.h"

#include "DisOther.hpp"
#include "UtCallbackHolder.hpp"
class WsfPlatform;
class WsfDisInterface;
#include "WsfVisualization.hpp"

//! Sends DisOther PDU's with commands to change visualization parameters.
class WSF_EXPORT WsfDisVisualization
{
public:
   WsfDisVisualization(WsfDisInterface* aInterfacePtr);
   void VisibleChange(double aSimTime, WsfPlatform* aPlatformPtr, bool aIsVisible);
   void BehaviorChange(double aSimTime, WsfVisualization::Behavior aBehavior);

   void             Connected(double aSimTime);
   WsfDisInterface* mInterfacePtr;
   UtCallbackHolder mCallbacks;
};
#endif
