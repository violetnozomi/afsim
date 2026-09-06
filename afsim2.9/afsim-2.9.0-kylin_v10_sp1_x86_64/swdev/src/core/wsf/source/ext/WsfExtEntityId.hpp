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


#ifndef WSFEXTENTITYID_HPP
#define WSFEXTENTITYID_HPP

#include "wsf_export.h"

#include <iosfwd>
#include <string>

class WSF_EXPORT WsfExtEntityId
{
public:
   enum
   {
      ALL_SITES      = 0xFFFF,
      ALL_APPLIC     = 0xFFFF,
      ALL_ENTITIES   = 0xFFFF,
      NO_SITES       = 0,
      NO_APPLIC      = 0,
      NO_ENTITY      = 0,
      RQST_ASSIGN_ID = 0xFFFE
   };

   WsfExtEntityId()
      : mSite(0)
      , mApplication(0)
      , mEntity(0)
   {
   }

   WsfExtEntityId(int aSite, int aApplication, int aEntity)
      : mSite(aSite)
      , mApplication(aApplication)
      , mEntity(aEntity)
   {
   }


   bool IsDefined() const { return (mSite != 0 && mApplication != 0 && mEntity != 0); }

   bool operator<(const WsfExtEntityId& aRhs) const;

   bool operator==(const WsfExtEntityId& aRhs) const;

   int GetSite() const { return mSite; }
   int GetApplication() const { return mApplication; }
   int GetEntity() const { return mEntity; }

   void SetSite(int aSite) { mSite = aSite; }
   void SetApplication(int aApplication) { mApplication = aApplication; }
   void SetEntity(int aEntity) { mEntity = aEntity; }

protected:
   int mSite;
   int mApplication;
   int mEntity;
};

WSF_EXPORT std::ostream& operator<<(std::ostream& aOut, const WsfExtEntityId& aId);

#endif
