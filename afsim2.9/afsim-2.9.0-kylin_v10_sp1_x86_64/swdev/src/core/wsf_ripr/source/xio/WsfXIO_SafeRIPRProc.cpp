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
#include "xio/WsfXIO_SafeRIPRProc.hpp"

#include "SafeRIPRProc.hpp"
#include "WsfPlatform.hpp"
#include "WsfRIPRJob.hpp"
#include "WsfRIPRProcessor.hpp"

WsfXIO_SafeRIPRProc::WsfXIO_SafeRIPRProc(const WsfRIPRProcessor* aProc)
{
   mPlatformIndex = aProc->GetPlatform()->GetIndex();
   mProcessorName = aProc->GetNameStringId();
}

WsfXIO_SafeRIPRProc::WsfXIO_SafeRIPRProc(const SafeRIPRProc& aProc)
{
   WsfRIPRProcessor* procPtr = aProc.get();
   mPlatformIndex            = procPtr->GetPlatform()->GetIndex();
   mProcessorName            = procPtr->GetNameStringId();
}
