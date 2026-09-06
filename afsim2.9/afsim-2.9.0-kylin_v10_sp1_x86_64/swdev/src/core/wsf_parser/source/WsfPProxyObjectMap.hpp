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

#ifndef WSFPPROXYOBJECTMAP_HPP
#define WSFPPROXYOBJECTMAP_HPP

#include <cassert>
#include <map>

#include "UtMemoryDebug.hpp"

// clang-format off
#include "WsfPProxyValue.hpp"
#include "WsfPProxyType.hpp"
// clang-format on

class WSF_PARSER_EXPORT WsfPProxyObjectMapType;

//
// Proxy object maps are maps from string to a single proxy type.
//

// The proxy object map instance
class WSF_PARSER_EXPORT WsfPProxyObjectMap
{
public:
   friend class WsfPProxyObjectMapType;

   WsfPProxyObjectMap()
      : mTypePtr()
      , mIsInherited(false)
   {
      mFiller[0] = mFiller[1] = mFiller[2] = 0;
   }

   WsfPProxyObjectMap(const WsfPProxyObjectMapType* aDataTypePtr)
      : mTypePtr(aDataTypePtr)
      , mIsInherited(false)
   {
   }

   WsfPProxyObjectMap(const WsfPProxyObjectMap& aRhs);
   ~WsfPProxyObjectMap();

   WsfPProxyType* GetDataType();

   WsfPProxyValue AddNew(const std::string& aName, WsfPProxyValue aValue, bool aOverrideExisting = false);

   WsfPProxyValue GetAt(const std::string& aName);

   WsfPProxyValue* FindAt(const std::string& aName);

   bool Erase(const std::string& aName);
   bool EraseNoDelete(const std::string& aName);

   bool UserErase(const std::string& aName);
   bool UserEraseNoDelete(const std::string& aName);
   bool UserUndelete(const std::string& aName);
   bool HasUserDeleted(const std::string& aName) const;

   void Clear();

   std::map<std::string, WsfPProxyValue>& GetValues() { return mValues; }

   void SetInherited(bool aIsInherited) { mIsInherited = aIsInherited; }

   size_t MemoryUsage() const;

   std::vector<std::string> GetObjectNames() const
   {
      std::vector<std::string> names;
      for (auto i = mValues.begin(); i != mValues.end(); ++i)
      {
         names.push_back(i->first);
      }
      return names;
   }

   using iterator = std::map<std::string, WsfPProxyValue>::iterator;

protected:
   // The proxy type for this map
   const WsfPProxyObjectMapType* mTypePtr;

   // The values in the map
   std::map<std::string, WsfPProxyValue> mValues;
   std::set<std::string>                 mDeletedValues;
   bool                                  mIsInherited;
   char                                  mFiller[3];

private:
   WsfPProxyObjectMap& operator=(const WsfPProxyObjectMap& aRhs); // no implementation

   UT_MEMORY_DEBUG_MARKER(cMDB_PROXY_MAP);
};

// The proxy type representing the object map for a particular type
class WSF_PARSER_EXPORT WsfPProxyObjectMapType : public WsfPProxyType
{
public:
   WsfPProxyObjectMapType(WsfPProxyType* aDataTypePtr)
      : WsfPProxyType(WsfProxy::cOBJECT_MAP)
      , mTypePtr(aDataTypePtr)
   {
      mDataSize = sizeof(WsfPProxyObjectMap);
   }

   void Construct(void* aValuePtr) const override;

   void           Destroy(void* aValuePtr) const override;
   void           Copy(void* dest, void* src, int flags) const override;
   WsfPProxyValue GetAttr(void* aValuePtr, const std::string& aName) const override;
   bool           SwapAttr(void* aPtr, const WsfPProxyKey& aKey, WsfPProxyValue& aValue) const override;
   bool           IsUnset(void* aValuePtr) const override;
   void           SetUnset(void* aValuePtr) const override;
   bool           IsInherited(void* aValuePtr) const override
   {
      WsfPProxyObjectMap* ptr = (WsfPProxyObjectMap*)aValuePtr;
      return ptr->mIsInherited;
   }
   void SetInherited(void* aValuePtr, bool aIsInherited) const override
   {
      WsfPProxyObjectMap* ptr = (WsfPProxyObjectMap*)aValuePtr;
      ptr->mIsInherited       = aIsInherited;
   }

   WsfPProxyType* GetDataType() const { return mTypePtr; }
   WsfPProxyType* GetContainedType(size_t aIndex) const override { return mTypePtr; }
   //    virtual WsfPProxyHash Hash(void* aDataPtr) const;
   size_t MemoryUsage(void* aDataPtr) const override;

private:
   // The proxy type stored in the map
   WsfPProxyType* mTypePtr;
};

#endif
