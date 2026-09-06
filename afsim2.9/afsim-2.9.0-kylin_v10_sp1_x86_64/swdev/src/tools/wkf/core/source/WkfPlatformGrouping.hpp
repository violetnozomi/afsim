// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKFPLATFORMGROUPING_HPP
#define WKFPLATFORMGROUPING_HPP

#include "wkf_export.h"

#include <string>

#include <QMetaType>

// this provides the data types and functions to operate on groupings of platforms
namespace wkf
{
class WKF_EXPORT PlatformGrouping
{
public:
   enum TypeEnum
   {
      eCategory          = 0,
      eType              = 1,
      eSide              = 2,
      eGroupingTypeCount = 3,
      eIndividual        = 3
   };

   PlatformGrouping()
      : PlatformGrouping(eType, "WSF_PLATFORM")
   {
   }

   PlatformGrouping(int aType, const std::string& aName)
      : mType(aType)
      , mName(aName)
   {
   }

   void Set(int aType, const std::string& aName)
   {
      mType = aType;
      mName = aName;
   }
   int                      Type() const { return mType; }
   const std::string&       Name() const { return mName; }
   std::string              TypeString() const { return GetTypeString(Type()); }
   bool                     IsGlobal() const { return ((mType == eType) && (mName == "WSF_PLATFORM")); }
   bool                     IsIndividual() const { return (mType == eIndividual); }
   static const std::string GetTypeString(int aTypeId)
   {
      switch (aTypeId)
      {
      case (eCategory):
         return "Category";
         break;
      case (eType):
         return "Platform Type";
         break;
      case (eSide):
         return "Side";
         break;
      case (eIndividual):
         return "Platform";
         break;
      default:
         return "Undefined";
         break;
      }
   }

   bool operator==(const PlatformGrouping& aRHS) const { return (mType == aRHS.mType) && (mName == aRHS.mName); }
   bool operator<(const PlatformGrouping& aRHS) const
   {
      return (mType == aRHS.mType) ? (mName < aRHS.mName) : (mType < aRHS.mType);
   }

private:
   int         mType;
   std::string mName;
};
} // namespace wkf

// This should allow Qt to pass a wkf::PlatformGrouping through its Meta Object system, however
// it was not necessary when building Warlock or Wizard and wkf::PlatformGroup was able to be handled properly.
// In Results Viz, wkf::PlatformGrouping was not registered, and this line still did not register it.
// We have no explanation for why this does not work, when it should, and clearly does in many other places in code.
Q_DECLARE_METATYPE(wkf::PlatformGrouping)

#endif
