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

#include "WsfPProxyStructType.hpp"

#include "UtCast.hpp"
#include "WsfParseRule.hpp"

WsfPProxyStructType::WsfPProxyStructType(WsfParseStruct*                   aParseStructPtr,
                                         WsfPProxyStructType*              aBasePtr,
                                         const std::vector<std::string>&   aNames,
                                         const std::vector<AttrData>&      aLocalAttributes,
                                         const std::map<std::string, int>& aNameToIndexMap)
   : WsfPProxyType(WsfProxy::cSTRUCT)
   , mBasePtr(aBasePtr)
   , mLocalAttributes(aLocalAttributes)
   , mNameToIndex(aNameToIndexMap)
   , mAllNames(aNames)
   , mInitialized(false)
   , mEmbedded(false)
   , mParseStructPtr(aParseStructPtr)
{
}

WsfPProxyStructType::~WsfPProxyStructType()
{
   mDefaultStruct.Delete();
}

void WsfPProxyStructType::Construct(void* aValuePtr) const
{
   WsfPProxyStructHeader* hdr = new (aValuePtr) WsfPProxyStructHeader;
   hdr->mStructPtr            = this;
   hdr->mStructFlags          = WsfPProxyStructHeader::cSF_EXPANDED_FMT;

   ConstructP(hdr);
}

void WsfPProxyStructType::Initialize()
{
   // We can't store the struct embedded in another struct if it is polymorphic
   if (mBasePtr != nullptr)
   {
      mTypeFlags |= WsfProxy::cPOLYMORPHIC;
   }
   mAttrData.resize(InitGetAttrCount());

   size_t offsetAdjust = 0;
   size_t attrIndex    = 0;
   InitializeBase(this, offsetAdjust, attrIndex);
   mStructSize = offsetAdjust;
   assert(mStructSize <= 100000); // arbitrary max size for sanity

   mDataSize    = sizeof(WsfPProxyStructHeader);
   mInitialized = true;
}

void WsfPProxyStructType::InitializeBase(WsfPProxyStructType* aBasePtr, size_t& aCurrentOffset, size_t& aAttributeIndex)
{
   assert(aCurrentOffset <= 100000); // arbitrary max size for sanity

   if (aBasePtr->mBasePtr)
   {
      InitializeBase(aBasePtr->mBasePtr, aCurrentOffset, aAttributeIndex);
   }

   for (size_t i = 0; i < aBasePtr->mLocalAttributes.size(); ++i)
   {
      size_t         attrIndex = i + aAttributeIndex;
      WsfPProxyType* typePtr   = aBasePtr->mLocalAttributes[i].mTypePtr;
      AttrData&      attrDat   = mAttrData[attrIndex];
      attrDat.mTypePtr         = typePtr;
      // attrDat.mCreateAsPointer = aBasePtr->mLocalAttributes[i].mCreateAsPointer;
      if (attrDat.mOffset == ut::npos)
      {
         attrDat.mOffset = aCurrentOffset;
         aCurrentOffset += ut::cast_to_int(typePtr->mDataSize);
         mDynamicAttributes.push_back(attrIndex);
      }
   }
   aAttributeIndex += aBasePtr->mLocalAttributes.size();
}

size_t WsfPProxyStructType::InitGetAttrCount()
{
   if (mBasePtr)
   {
      return mBasePtr->InitGetAttrCount() + mLocalAttributes.size();
   }
   return mLocalAttributes.size();
}

size_t WsfPProxyStructType::MemoryUsage(void* aDataPtr) const
{
   WsfPProxyStructValue inst  = GetInstance((WsfPProxyStructHeader*)aDataPtr);
   size_t               count = inst.GetMemberCount();
   size_t               usage = 0;
   if (inst.GetHeader()->GetStart() != nullptr)
   {
      for (size_t i = 0; i < count; ++i)
      {
         WsfPProxyValue member = inst.GetAtIndex(i);
         usage += member.GetType()->MemoryUsage(member.GetDataPtr());
      }
   }
   usage += sizeof(void*) * 2; // header size
   auto* eh = inst.GetHeader();
   usage += eh->mBasePath.MemoryUsage();
   return usage;
}

void WsfPProxyStructType::Copy(void* aDest, void* aSrc, int copyFlags) const
{
   WsfPProxyStructHeader*     destHdr = (WsfPProxyStructHeader*)aDest;
   WsfPProxyStructHeader*     srcHdr  = (WsfPProxyStructHeader*)aSrc;
   WsfPProxyStructValue       srcInst(GetInstance(srcHdr));
   const WsfPProxyStructType* srcStruct = srcInst.GetStruct();
   assert(srcStruct->IsTypeOf(this));
   Destroy(aDest);
   destHdr->mStructPtr = srcStruct;
   srcStruct->ConstructP(destHdr, &srcInst, copyFlags);
   if ((copyFlags & WsfProxy::cCOPY_BASE))
   {
      destHdr->mBasePath = srcHdr->mBasePath;
   }
   destHdr->mStructFlags = (destHdr->mStructFlags & ~WsfPProxyStructHeader::cSF_INSTANTIATION_MASK) |
                           (srcHdr->mStructFlags & WsfPProxyStructHeader::cSF_INSTANTIATION_MASK);
}

void WsfPProxyStructType::Destroy(void* aValuePtr) const
{
   WsfPProxyStructHeader* hdr = (WsfPProxyStructHeader*)aValuePtr;
   DestroyInstance(WsfPProxyStructValue(hdr));
   hdr->~WsfPProxyStructHeader();
   delete[](char*) hdr->mStructDataPtr;
}

void WsfPProxyStructType::ClearUnset(void* aValuePtr) const
{
   WsfPProxyStructHeader* hdr = (WsfPProxyStructHeader*)aValuePtr;
   hdr->mStructFlags &= ~WsfPProxyStructHeader::cSF_UNSET;
}

bool WsfPProxyStructType::IsUnset(void* aValuePtr) const
{
   WsfPProxyStructHeader* hdr = (WsfPProxyStructHeader*)aValuePtr;
   return 0 != (hdr->mStructFlags & WsfPProxyStructHeader::cSF_UNSET);
}

void WsfPProxyStructType::ConstructP(WsfPProxyStructHeader*      aHdr,
                                     const WsfPProxyStructValue* aCopyFromPtr /*= 0*/,
                                     int                         copyFlags /* WsfProxy::cCOPY_ALL */) const
{
   if (!aCopyFromPtr && mDefaultStruct)
   {
      aCopyFromPtr = &mDefaultStruct;
   }
   auto* header           = new ((void*)aHdr) WsfPProxyStructHeader;
   header->mStructDataPtr = new char[mStructSize];
   WsfPProxyStructValue inst(header);

   if (aCopyFromPtr)
   {
      CopyConstructInstance(inst, *aCopyFromPtr, copyFlags);
   }
   else
   {
      ConstructInstance(inst);
   }
}

void WsfPProxyStructType::CopyConstructInstance(WsfPProxyStructValue aInstance, WsfPProxyStructValue aSource, int copyFlags) const
{
   aInstance.GetHeader()->Constructing();
   memcpy((char*)aInstance.GetMemberData(), (char*)aSource.GetMemberData(), mStructSize);
   for (size_t idx : mDynamicAttributes)
   {
      const AttrData& attrDat   = mAttrData[idx];
      void*           memberPtr = (char*)aInstance.GetMemberData() + attrDat.mOffset;
      WsfPProxyType*  attrType  = attrDat.mTypePtr;
#ifndef WSF_PARSE_DEBUG_MEMORY
      if (0 == (attrType->mTypeFlags & WsfProxy::cPOD))
#endif
      {
         ConstructAttr(attrDat, memberPtr);
         attrType->Copy(memberPtr, aSource.GetMember(idx), copyFlags);
      }
   }
}

void WsfPProxyStructType::ConstructInstance(WsfPProxyStructValue aInstance) const
{
   aInstance.GetHeader()->Constructing();
   for (size_t idx : mDynamicAttributes)
   {
      const AttrData& attrDat   = mAttrData[idx];
      void*           memberPtr = (char*)aInstance.GetMemberData() + attrDat.mOffset;
      ConstructAttr(attrDat, memberPtr);
   }
}

// int g_VALGRIND_REMEMBER = 0;
void WsfPProxyStructType::ConstructAttr(const AttrData& aAttr, void* aValuePtr) const
{
   aAttr.mTypePtr->Construct(aValuePtr);
   //      if (aAttr.mTypePtr->mTypeFlags & WsfPProxyType::cPOD) {
   //         // Check that the bytes in the value are all initialized when running VALGRIND
   //         int byteSum = 0;
   //         for (int i = 0; i < aAttr.mTypePtr->mDataSize; ++i) {
   //            if (((char*)aValuePtr)[i] > 0) {
   //               g_VALGRIND_REMEMBER += 1;
   //            }
   //         }
   //      }
}

void WsfPProxyStructType::DestroyInstance(WsfPProxyStructValue aInstance) const
{
   const WsfPProxyStructType* specializedType = aInstance.GetStruct();
   if (specializedType != this)
   {
      specializedType->DestroyInstance(aInstance);
      return;
   }

   aInstance.GetHeader()->Destroying();
   for (size_t idx : mDynamicAttributes)
   {
      void* memberPtr = static_cast<char*>(aInstance.GetMemberData()) + mAttrData[idx].mOffset;
      mAttrData[idx].mTypePtr->Destroy(memberPtr);
   }
}

void WsfPProxyStructType::CopyInstance(WsfPProxyStructValue& aDest, WsfPProxyStructValue& aSrc, int copyFlags) const
{
   if (aSrc.IsUnset())
   {
      aDest.SetUnset();
   }
   else
   {
      aDest.ClearUnset();
   }

   assert(aSrc.GetHeader()->mStructFlags & WsfPProxyStructHeader::cSF_CONSTRUCTED);
   assert(aDest.GetHeader()->mStructFlags & WsfPProxyStructHeader::cSF_CONSTRUCTED);
   size_t srcmembers = aSrc.GetMemberCount();
   size_t dstmembers = aDest.GetMemberCount();
   for (size_t i = 0; i < srcmembers; ++i)
   {
      if (i < dstmembers)
      {
         aDest.GetMemberType(i)->Copy(aDest.GetMember(i), aSrc.GetMember(i), copyFlags);
      }
   }
}

WsfPProxyValue WsfPProxyStructType::GetMemberValue(void* aPtr, size_t aIndex) const
{
   if (aIndex < mAttrData.size())
   {
      assert(aPtr != nullptr);
      WsfPProxyStructHeader* hdr = (WsfPProxyStructHeader*)aPtr;
      assert(hdr->mStructFlags & WsfPProxyStructHeader::cSF_CONSTRUCTED);
      WsfPProxyType* attrType = mAttrData[aIndex].mTypePtr;
      WsfPProxyValue attrVal((void*)(mAttrData[aIndex].mOffset + (char*)hdr->GetStart()), attrType);
      if (!attrType->IsStruct())
      {
         return attrVal;
      }
      else
      {
         return WsfPProxyStructValue(attrVal);
      }
   }
   return WsfPProxyValue();
}

size_t WsfPProxyStructType::GetAttrCount(void* aPtr) const
{
   return mAttrData.size();
}

WsfPProxyValue WsfPProxyStructType::GetAttr(void* aPtr, const std::string& aName) const
{
   WsfPProxyStructHeader* hdr = (WsfPProxyStructHeader*)aPtr;
   assert(hdr->mStructFlags & WsfPProxyStructHeader::cSF_CONSTRUCTED);
   // DEH: this should not be needed.  GetAttr() should be called from a safer place
   //          if (this != hdr->mStructPtr) // for polymorphism
   //          {
   //             return hdr->mStructPtr->GetAttr(aPtr, aName);
   //          }
   size_t idx = GetMemberIndex(aName);
   if (idx != ut::npos)
   {
      assert(aPtr != nullptr);
      WsfPProxyType* attrType = mAttrData[idx].mTypePtr;
      WsfPProxyValue attrVal((void*)(mAttrData[idx].mOffset + (char*)hdr->GetStart()), attrType);
      if (!attrType->IsStruct())
      {
         return attrVal;
      }
      else
      {
         return WsfPProxyStructValue(attrVal);
      }
   }
   return WsfPProxyValue();
}

bool WsfPProxyStructType::SwapAttr(void* aPtr, const WsfPProxyKey& aKey, WsfPProxyValue& aValue) const
{
   size_t idx = aKey.GetIndex();
   if (idx != ut::npos)
   {
      if (!aValue.IsValid())
      {
         WsfPProxyValue currentVal = GetMemberValue(aPtr, idx);
         aValue.Delete();
         aValue = currentVal.Copy();
         currentVal.SetUnset();
         return true;
      }
      else
      {
         WsfPProxyType* attrType = mAttrData[idx].mTypePtr;
         if (aValue.GetType()->IsTypeOf(attrType))
         {
            WsfPProxyValue member = GetMemberValue(aPtr, idx);
            WsfPProxyValue oldVal = member.Copy();
            member.Copy(aValue);
            aValue.Delete();
            aValue = oldVal;
            return true;
         }
      }
   }
   return false;
}

std::string WsfPProxyStructType::GetNameAtIndex(size_t aIndex) const
{
   if (aIndex < mAllNames.size())
   {
      return mAllNames[aIndex];
   }
   return "";
}
