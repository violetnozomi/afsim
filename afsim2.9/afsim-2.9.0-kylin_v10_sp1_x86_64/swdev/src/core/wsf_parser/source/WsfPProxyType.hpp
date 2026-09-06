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

#ifndef VIPROXYTYPE_HPP
#define VIPROXYTYPE_HPP

#include "wsf_parser_export.h"

#include <cassert>
#include <string>

#include "UtMemoryDebug.hpp"
#include "WsfPProxyCommon.hpp"
#include "WsfPProxyHash.hpp"
#include "WsfPProxyValue.hpp"

// For every value stored in the proxy, it must have a type class.  The type class defines how
// to do basic operations on values of that type.
// Type values must have a fixed size, and be able to be constructed in a preexisting memory buffer.
// There are 4 main kinds of types.
//    struct - stores a static set of members
//    list - stores a dynamic list of members that all share a single type
//    object map - stores a dynamic set of members sharing a single type, each with a string name
//    basic types - Stores a single value (integer, string, Latitude, etc...)
class WSF_PARSER_EXPORT WsfPProxyType
{
public:
   WsfPProxyType(WsfProxy::ValueKind aKind)
      : mTypeFlags(0)
      , mTypeKind(aKind)
      , mTypeStoredKind(aKind)
   {
      // Handle the general case here
      mTypeName = wsf::proxy::ValueKindToString(aKind);
      if (aKind >= WsfProxy::cENUMERATION_VALUE)
      {
         mTypeFlags |= WsfProxy::cVALUE_FLAG;
      }
   }
   virtual ~WsfPProxyType() {}

   const std::string& GetTypeName() const { return mTypeName; }
   size_t             GetDataSize() const { return mDataSize; }
   int                mTypeFlags;
   int                mTypeKind;
   int                mTypeStoredKind;
   size_t             mDataSize;
   std::string        mTypeName;

   // Construct a value at the given address
   virtual void Construct(void* aValuePtr) const = 0;

   // Destroy the value
   virtual void Destroy(void* aValuePtr) const = 0;

   // Copy aSrc to a Dest
   virtual void Copy(void* aDest, void* aSrc, int copyFlags) const = 0;

   // If this value is an object map, return the value mapped to the given name.
   // If this value is a struct, return the member with the given name
   virtual WsfPProxyValue GetAttr(void* aPtr, const std::string& aName) const = 0;

   virtual bool SwapAttr(void* aPtr, const WsfPProxyKey& aKey, WsfPProxyValue& aValue) const { return false; }

   // If the value contains other values which may be indexed, returns the contained value
   // at the given index.  Valid for lists and structs.
   virtual WsfPProxyValue GetAtIndex(void* aPtr, size_t aIndex) const { return WsfPProxyValue(); }
   virtual std::string    GetNameAtIndex(size_t aIndex) const { return std::string(); }
   virtual size_t         GetAttrCount(void* aPtr) const { return 0; }
   virtual size_t         GetMemberIndex(const std::string& aAttrName) const { return ut::npos; }

   // Returns 'true' if the value is in the unset state
   virtual bool IsUnset(void* aValuePtr) const = 0;
   // Sets the value to the 'unset' state
   virtual void SetUnset(void* aValuePtr) const = 0;
   virtual void ClearUnset(void* aValuePtr) const {}

   virtual bool IsInherited(void* aValuePtr) const                     = 0;
   virtual void SetInherited(void* aValuePtr, bool aIsInherited) const = 0;

   virtual bool IsOfBasicType(const std::string& aTypeName) const { return aTypeName == mTypeName; }

   // If this type can contain other types, this returns the type contained at the given index.
   // The index is only necessary for structs, maps and lists have only one contained type.
   virtual WsfPProxyType* GetContainedType(size_t aIndex) const { return nullptr; }

   bool IsBasicType() const { return 0 != (mTypeFlags & WsfProxy::cVALUE_FLAG); }
   bool IsStruct() const { return mTypeKind == WsfProxy::cSTRUCT; }
   bool IsList() const { return mTypeKind == WsfProxy::cLIST; }
   bool IsObjectMap() const { return mTypeKind == WsfProxy::cOBJECT_MAP; }
   // bool IsPointer() const { return mTypeKind == cPOINTER; }
   virtual bool          IsTypeOf(const WsfPProxyType* aOtherType) const { return this == aOtherType; }
   virtual WsfPProxyHash Hash(void* aDataPtr) const { return WsfPProxyHash(aDataPtr, mDataSize); }
   virtual size_t        MemoryUsage(void* aDataPtr) const = 0;
};


class WsfParseNode;

// Basic types are the most atomic type in the proxy.
// They must support this interface.
class WSF_PARSER_EXPORT WsfPProxyBasicType : public WsfPProxyType
{
public:
   // Store an extra byte at the end for flags
   static const int cBASIC_TYPE_HEADER_SIZE = 1;
   WsfPProxyBasicType(WsfProxy::ValueKind aKind)
      : WsfPProxyType(aKind)
   {
   }

   // Sets the value given a string representation
   virtual void SetValue(void* p, const std::string& aText) const = 0;
   // Reads the value from a node containing the result of the parse
   virtual void Read(void* aValuePtr, WsfParseNode* aNodePtr) const = 0;

   // Writes the value when serializing wsf core files
   // using aRulePtr allows for multiple formats
   virtual std::string Write(void* aValuePtr, WsfParseRule* aRulePtr) const = 0;

   // Writes the proxy value to a string for user presentation --
   // may or may not be wsf core formatting
   virtual std::string ToString(void* aDataPtr) const { return std::string(); }
   // Returns true if the two values pointed to are equal
   virtual bool Equal(void* dest, void* src) const = 0;
   // Returns true if dest points to a value which is less than src
   virtual bool Less(void* dest, void* src) const = 0;
   // Attempts to negate the value pointed to by p, returns true if successful
   virtual bool Negate(void* p) const { return false; }

   WsfPProxyValue GetAttr(void*, const std::string&) const override { return WsfPProxyValue(); }

   // Returns 'true' if the value is in the unset state
   bool IsUnset(void* aValuePtr) const override { return 0 != (BasicValueFlags(aValuePtr) & WsfProxy::cVALUE_UNSET); }
   // Sets the value to the 'unset' state
   void SetUnset(void* aValuePtr) const override { BasicValueFlags(aValuePtr) |= WsfProxy::cVALUE_UNSET; }
   void ClearUnset(void* aValuePtr) const override { BasicValueFlags(aValuePtr) &= ~WsfProxy::cVALUE_UNSET; }

   bool IsInherited(void* aValuePtr) const override
   {
      return 0 != (BasicValueFlags(aValuePtr) & WsfProxy::cVALUE_INHERITED);
   }
   void SetInherited(void* aValuePtr, bool aIsInherited) const override
   {
      if (aIsInherited)
      {
         BasicValueFlags(aValuePtr) |= WsfProxy::cVALUE_INHERITED;
      }
      else
      {
         BasicValueFlags(aValuePtr) &= ~WsfProxy::cVALUE_INHERITED;
      }
   }

   virtual int GetUnitTypeId() const { return -1; }

   size_t MemoryUsage(void* aDataPtr) const override { return mDataSize; }

protected:
   static unsigned char& BasicValueFlags(void* aValuePtr) { return *((unsigned char*)aValuePtr); }
};

#endif
