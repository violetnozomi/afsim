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


#ifndef WSFEXTEVENTID_HPP
#define WSFEXTEVENTID_HPP

#include "wsf_export.h"

#include <cstdint>
#include <string>

//! Represents a unique ID for an event owned by this simulation.
class WSF_EXPORT WsfExtEventId
{
public:
   WsfExtEventId();
   WsfExtEventId(uint16_t aSite, uint16_t aApplication, uint16_t aEventNumber);
   WsfExtEventId(std::string aIssuer, uint16_t aEventNumber);


   uint16_t           GetSite() const { return mSite; }
   uint16_t           GetApplication() const { return mApplication; }
   const std::string& GetIssuer() const { return mIssuer; }
   uint16_t           GetNumber() const { return mEventNumber; }

   // Mutators

   void SetSite(uint16_t aSite) { mSite = aSite; }
   void SetApplication(uint16_t aApplication) { mApplication = aApplication; }
   void SetIssuer(const std::string& aIssuer) { mIssuer = aIssuer; }
   void SetNumber(uint16_t aNumber) { mEventNumber = aNumber; }

   // Either mSite and mApplication OR mIssuer must be set.
   uint16_t    mSite;
   uint16_t    mApplication;
   std::string mIssuer;
   // Locally unique ID
   uint16_t mEventNumber;
};
#endif
