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

#ifndef WSFPPROXYSTRUCTVALUE_HPP
#define WSFPPROXYSTRUCTVALUE_HPP

#include "wsf_parser_export.h"

#include "WsfPProxyStructHeader.hpp"

class WsfPProxyStructType;

// A wrapper for storing / operating on a struct instance.
class WSF_PARSER_EXPORT WsfPProxyStructValue : public WsfPProxyValue
{
public:
   WsfPProxyStructValue()
      : WsfPProxyValue()
   {
   }
   WsfPProxyStructValue(const WsfPProxyValue& aValue)
      : WsfPProxyValue(aValue)
   {
      if (!mTypePtr || !mTypePtr->IsStruct())
      {
         mTypePtr = nullptr;
      }
      // if the data type can be derived from, make sure we have the most
      // specialized struct type
      else if (mTypePtr->mTypeFlags & WsfProxy::cPOLYMORPHIC)
      {
         mTypePtr = (WsfPProxyType*)static_cast<WsfPProxyStructHeader*>(mDataPtr)->mStructPtr;
      }
   }

   WsfPProxyStructValue(WsfPProxyStructHeader* aPtr);

   size_t GetMemberIndex(const std::string& aName) const override;

   WsfPProxyType* GetMemberType(const std::string& aName) const override;

   WsfPProxyType* GetMemberType(size_t aMemberIndex) const override;

   WsfPProxyStructHeader* GetHeader() const { return (WsfPProxyStructHeader*)mDataPtr; }
   WsfPProxyPath*         GetBase() const
   {
      WsfPProxyStructHeader* hdr = GetHeader();
      if (hdr && !hdr->GetBasePath().Empty())
      {
         return &hdr->GetBasePath();
      }
      return nullptr;
   }
   void SetBase(const WsfPProxyPath& aPath)
   {
      WsfPProxyStructHeader* hdr = GetHeader();
      if (hdr)
      {
         hdr->mBasePath = aPath;
      }
   }
   const WsfPProxyStructType* GetStruct() const { return (WsfPProxyStructType*)mTypePtr; }
   void*                      GetMemberData() const { return GetHeader()->GetStart(); }
   size_t                     GetMemberCount();
   void*                      GetMember(size_t aMemberIndex);
   void*                      GetMember(const std::string& aName);

   std::string GetMemberName(size_t aMemberIndex) const;
};

#endif
