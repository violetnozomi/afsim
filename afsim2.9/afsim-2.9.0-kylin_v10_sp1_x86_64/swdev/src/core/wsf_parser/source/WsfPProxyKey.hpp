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

#ifndef WSFPPROXYKEY_HPP
#define WSFPPROXYKEY_HPP
#include "wsf_parser_export.h"

#include <string>

//! WsfPProxyPath is composed of a list of entries.  Entries are integer indices
//! when possible.  For map keys, a string is stored instead.
class WSF_PARSER_EXPORT WsfPProxyKey
{
public:
   enum class KeyType
   {
      cSTRING,
      cINDEX
   };

   WsfPProxyKey();
   WsfPProxyKey(size_t i);
   WsfPProxyKey(const std::string& s);

   void SetIndex(size_t aIndex)
   {
      mIndex   = aIndex;
      mKeyType = KeyType::cINDEX;
   }
   void               SetMapKey(const std::string& aMapKey) { mMapKey = aMapKey; }
   size_t             GetIndex() const { return mIndex; }
   const std::string& GetMapKey() const { return mMapKey; }
   std::string        ToString() const;

   void ClearMapKey() { mMapKey.clear(); }

   bool IsString() const;
   bool IsIndex() const;
   bool operator<(const WsfPProxyKey& aRhs) const;
   bool operator==(const WsfPProxyKey& aRhs) const;


private:
   // Key for map entry
   std::string mMapKey;

   // Index of list entry or struct entry
   size_t mIndex;

   KeyType mKeyType;
};
#endif
