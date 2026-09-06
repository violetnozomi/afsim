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

#ifndef VIPROXYSTRUCTTYPE_HPP
#define VIPROXYSTRUCTTYPE_HPP

#include <cassert>
#include <functional>
#include <map>
#include <string.h>
#include <vector>

#include "UtCast.hpp"
#include "UtMemory.hpp"
#include "WsfPProxyStructValue.hpp"
#include "WsfPProxyType.hpp"
#include "WsfParseRule.hpp"
class ProxyWriter;

// A struct type is a proxy type composed of many member proxy types.
class WsfPProxyStructType : public WsfPProxyType
{
public:
   struct AttrData
   {
      AttrData(WsfPProxyType* aTypePtr = nullptr)
         : mTypePtr(aTypePtr)
         , mOffset(ut::npos)
      {
      }
      WsfPProxyType* mTypePtr;
      size_t         mOffset;
   };

   class Builder;

   ~WsfPProxyStructType() override;
   // returns true if the struct instance is in the unset state
   bool IsUnset(void* aValuePtr) const override;
   // Sets the struct instance to the 'unset' state
   void SetUnset(void* aValuePtr) const override
   {
      WsfPProxyStructHeader* hdr = (WsfPProxyStructHeader*)aValuePtr;
      hdr->mStructFlags |= WsfPProxyStructHeader::cSF_UNSET;
   }
   bool IsInherited(void* aValuePtr) const override
   {
      WsfPProxyStructHeader* hdr = (WsfPProxyStructHeader*)aValuePtr;
      return 0 != (hdr->mStructFlags & WsfPProxyStructHeader::cSF_INHERITED);
   }
   void SetInherited(void* aValuePtr, bool aIsInherited) const override
   {
      WsfPProxyStructHeader* hdr = (WsfPProxyStructHeader*)aValuePtr;
      if (aIsInherited)
      {
         hdr->mStructFlags |= WsfPProxyStructHeader::cSF_INHERITED;
      }
      else
      {
         hdr->mStructFlags &= ~WsfPProxyStructHeader::cSF_INHERITED;
      }
   }

   bool IsOfBasicType(const std::string& aTypeName) const override
   {
      const WsfPProxyStructType* pStruct  = this;
      bool                       isTypeOf = false;

      while ((pStruct != nullptr) && !isTypeOf)
      {
         isTypeOf = (aTypeName == pStruct->mTypeName);
         pStruct  = pStruct->mBasePtr;
      }

      return isTypeOf;
   }

   WsfParseStruct* GetParseStruct() { return mParseStructPtr; }

   void           ClearUnset(void* aValuePtr) const override;
   WsfPProxyType* GetContainedType(size_t aIndex) const override
   {
      return (aIndex < mAttrData.size()) ? mAttrData[aIndex].mTypePtr : nullptr;
   }
   // Returns a member of a struct by index
   WsfPProxyValue GetAtIndex(void* aDataPtr, size_t aIndex) const override { return GetMemberValue(aDataPtr, aIndex); }
   size_t         GetAttrCount(void* aPtr) const override;

   void Initialize();

   // Create a struct given a pre-allocated buffer
   void Construct(void* aValuePtr) const override;

   // Free the struct's memory
   void Destroy(void* aValuePtr) const override;
   // virtual WsfPProxyHash Hash(void* aDataPtr) const;
   size_t MemoryUsage(void* aDataPtr) const override;

private:
   WsfPProxyStructType(WsfParseStruct*                   aParseStructPtr,
                       WsfPProxyStructType*              aBasePtr,
                       const std::vector<std::string>&   aNames,
                       const std::vector<AttrData>&      aLocalAttributes,
                       const std::map<std::string, int>& aNameToIndexMap);

   // Usually structs are copy-constructed from the default value
   void CopyConstructInstance(WsfPProxyStructValue aInstance, WsfPProxyStructValue aSource, int copyFlags) const;

   void ConstructInstance(WsfPProxyStructValue aInstance) const;

   ptrdiff_t GetStructSize() const { return mStructSize; }

   void ConstructP(WsfPProxyStructHeader*      aHdr,
                   const WsfPProxyStructValue* aCopyFromPtr = nullptr,
                   int                         copyFlags    = WsfProxy::cCOPY_ALL) const;

   // Returns the number of attributes. To be used prior to initialization.
   size_t InitGetAttrCount();

   // Initialize attributes that are dynamic, and do not exist a c++ struct
   void InitializeBase(WsfPProxyStructType* aBasePtr, size_t& aCurrentOffset, size_t& aAttributeIndex);

public:
   void ConstructAttr(const AttrData& aAttr, void* aValuePtr) const;
   void DestroyInstance(WsfPProxyStructValue aInstance) const;

   void                 CopyInstance(WsfPProxyStructValue& aDest, WsfPProxyStructValue& aSrc, int copyFlags) const;
   WsfPProxyStructValue GetInstance(WsfPProxyStructHeader* aHeader) const { return WsfPProxyStructValue(aHeader); }

   void           Copy(void* aDest, void* aSrc, int copyFlags) const override;
   WsfPProxyValue GetMemberValue(void* aPtr, size_t aIndex) const;
   WsfPProxyValue GetAttr(void* aPtr, const std::string& aName) const override;
   bool           SwapAttr(void* aPtr, const WsfPProxyKey& aKey, WsfPProxyValue& aValue) const override;

   // Returns the total number of members
   size_t GetMemberCount() const { return mAttrData.size(); }

   // Returns the value name associated with the given index, returns an empty string if the index is out of bounds
   std::string GetNameAtIndex(size_t aIndex) const override;
   // Returns the index of the member with the name, -1 if none exists with that name
   size_t GetMemberIndex(const std::string& aName) const override
   {
      auto i = mNameToIndex.find(aName);
      if (i != mNameToIndex.end())
      {
         return ut::cast_to_size_t(i->second);
      }
      return ut::npos;
   }

   bool IsTypeOf(const WsfPProxyType* aOtherType) const override
   {
      if (this == aOtherType)
      {
         return true;
      }
      if (!mBasePtr)
      {
         return false;
      }
      return mBasePtr->IsTypeOf(aOtherType);
   }

   // Returns a struct instance filled with the default values
   WsfPProxyStructValue GetDefaultValue() const { return mDefaultStruct; }

   virtual bool IsInitialized() const { return mInitialized; }

   //! Size in bytes of the actual structure
   ptrdiff_t mStructSize;
   //! Pointer to base struct
   WsfPProxyStructType* mBasePtr;
   //! Attributes for this struct, not including base classes
   // std::vector<WsfPProxyType*>            mLocalAttributes;
   std::vector<AttrData> mLocalAttributes;

   //! Byte-offset of attribute ordered by attribute index
   //! These are not strictly increasing
   std::vector<AttrData> mAttrData;

   //! Indices of attributes that are not in the real struct
   std::vector<size_t> mDynamicAttributes;

   //! Map an attribute name to the index
   using NameToIndexMap = std::map<std::string, int>;
   NameToIndexMap           mNameToIndex;
   std::vector<std::string> mAllNames;


   WsfPProxyStructValue mDefaultStruct;

   bool mInitialized;
   // If true, the struct's contents are always stored inline with the header.  (WsfPProxyStructInlineHeader)
   // If false, the struct's contents are always stored in another location.  (WsfPProxyStructExpandedHeader)
   bool            mEmbedded;
   WsfParseStruct* mParseStructPtr;
};

class WSF_PARSER_EXPORT WsfPProxyStructType::Builder
{
public:
   Builder()
      : mParseStructPtr(nullptr)
      , mBasePtr(nullptr)
   {
   }

   Builder& SetBaseStruct(WsfPProxyStructType* aBasePtr)
   {
      mBasePtr = aBasePtr;
      if (mBasePtr)
      {
         mNameToIndex = mBasePtr->mNameToIndex;
         mNames       = mBasePtr->mAllNames;
      }
      return *this;
   }

   Builder& SetParseStruct(WsfParseStruct* aParseStruct)
   {
      mParseStructPtr = aParseStruct;
      return *this;
   }

   Builder& AddAttribute(const std::string& aName, WsfPProxyType* aType)
   {
      if (aType)
      {
         mNameToIndex[aName] = ut::cast_to_int(mNames.size());
         mNames.push_back(aName);
         mLocalAttributes.emplace_back(aType);
      }
      return *this;
   }

   std::unique_ptr<WsfPProxyStructType> build()
   {
      return std::unique_ptr<WsfPProxyStructType>(
         new WsfPProxyStructType(mParseStructPtr, mBasePtr, mNames, mLocalAttributes, mNameToIndex));
   }

private:
   WsfParseStruct*            mParseStructPtr;
   WsfPProxyStructType*       mBasePtr;
   std::vector<std::string>   mNames;
   std::vector<AttrData>      mLocalAttributes;
   std::map<std::string, int> mNameToIndex;
};

#endif
