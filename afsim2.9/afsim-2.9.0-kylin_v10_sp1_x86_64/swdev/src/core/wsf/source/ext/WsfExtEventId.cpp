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


#include "ext/WsfExtEventId.hpp"

WsfExtEventId::WsfExtEventId()
   : mSite()
   , mApplication()
   , mEventNumber()
{
}

WsfExtEventId::WsfExtEventId(uint16_t aSite, uint16_t aApplication, uint16_t aEventNumber)
   : mSite(aSite)
   , mApplication(aApplication)
   , mIssuer()
   , mEventNumber(aEventNumber)
{
}

WsfExtEventId::WsfExtEventId(std::string aIssuer, uint16_t aEventNumber)
   : mSite()
   , mApplication()
   , mIssuer(aIssuer)
   , mEventNumber(aEventNumber)
{
}
