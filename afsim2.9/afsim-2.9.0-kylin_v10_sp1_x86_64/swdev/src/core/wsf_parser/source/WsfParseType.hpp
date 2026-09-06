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

#ifndef WSFPARSETYPE_HPP
#define WSFPARSETYPE_HPP

#include "wsf_parser_export.h"

#include <algorithm>
#include <cassert>
#include <map>
#include <string>
#include <vector>

#include "UtCast.hpp"
#include "UtStringRef.hpp"
#include "WsfParseTypePath.hpp"

class WsfParseRule;
// IMPORTANT:
//  WsfParseSymbolTable needs to be handled carefully.
//  In wsf core inputs, derived types are generally a copy of the base
//  type.  Most of the time, the base type isn't going to change,
//  so to avoid the cost of copying entire nested type-trees,
//  we just maintain references to the base type's symbol table.


class WSF_PARSER_EXPORT WsfParseType
{
   bool operator==(const WsfParseType& aRhs) const;

public:
   using SymbolTable = std::map<UtStringRef, WsfParseType*>;

   const WsfParseType* FindNestedSymbol(const WsfParseTypePath& aKey) const;
   WsfParseType*       FindNestedSymbol(const WsfParseTypePath& aKey)
   {
      const WsfParseType* thisPtr = this;
      return const_cast<WsfParseType*>(thisPtr->FindNestedSymbol(aKey));
   }

   WsfParseType(const UtStringRef& aName, WsfParseType* aParentPtr = nullptr)
      : mName(aName)
      , mReaderPtr()
      , mSourceTypePtr(nullptr)
      , mContainer(aParentPtr)
   {
   }
   WsfParseType(const UtStringRef& aName, const WsfParseType& aRhs, WsfParseType* aContainerPtr);

   ~WsfParseType();
   void Clear();

   const UtStringRef& GetTypeName() const { return mName; }

   WsfParseType*       GetSourceType() const { return mSourceTypePtr; }
   void                SetSourceType(WsfParseType* aSourceTypePtr) { mSourceTypePtr = aSourceTypePtr; }
   const WsfParseType* GetEffectiveSourceType() const;

   WsfParseType* GetContainer() const { return mContainer; }
   void          SetContainer(WsfParseType* aContainerPtr) { mContainer = aContainerPtr; }
   void          Print(std::ostream& aStream, const std::string& aIndent);


   const WsfParseType* FindType(const WsfParseTypePath& aKey) const;
   WsfParseType*       FindType(const WsfParseTypePath& aKey)
   {
      const WsfParseType* thisPtr = this;
      return const_cast<WsfParseType*>(thisPtr->FindType(aKey));
   }
   const WsfParseType* FindType(const UtStringRef& aKey) const;
   WsfParseType*       FindType(const UtStringRef& aKey)
   {
      const WsfParseType* thisPtr = this;
      return const_cast<WsfParseType*>(thisPtr->FindType(aKey));
   }
   const WsfParseType* FindTypeLocal(const WsfParseTypePath& aKey) const;
   WsfParseType*       FindTypeLocal(const WsfParseTypePath& aKey)
   {
      return const_cast<WsfParseType*>(const_cast<const WsfParseType*>(this)->FindTypeLocal(aKey));
   }
   WsfParseType* FindTypeLocal(const UtStringRef& aKey) const;
   //       bool                 FindTypeLocal(const WsfParseTypePath& aKey,
   //                                          WsfParseType*& aTypePtr) const;

   WsfParseType& AddType(const WsfParseTypePath& aPath, WsfParseType* aTypePtr);
   WsfParseType& AddType(WsfParseType* aTypePtr);
   // WsfParseType& SetType(WsfParseType& aTypePtr);
   bool RemoveType(const UtStringRef& aKey);
   bool RemoveType(const WsfParseTypePath& aKey);

   void Merge(WsfParseType& aRhs);

   void          SetRule(WsfParseRule* aReaderPtr) { mReaderPtr = aReaderPtr; }
   WsfParseRule* GetRule() const { return mReaderPtr; }

   bool HasSymbols() const { return !mSymbols.empty(); }

   const SymbolTable& GetSymbols() const { return mSymbols; }

   SymbolTable::const_iterator begin() const { return mSymbols.begin(); }
   SymbolTable::const_iterator end() const { return mSymbols.end(); }

   int GetDepth() const
   {
      if (mContainer == nullptr)
      {
         return 0;
      }
      return mContainer->GetDepth();
   }

   void GetAncestry(std::vector<WsfParseType*>& aAncestry);
   void GetAncestry(std::vector<const WsfParseType*>& aAncestry) const;
   void GetPath(WsfParseTypePath& aPath) const;

private:
   UtStringRef   mName;
   WsfParseRule* mReaderPtr;

   SymbolTable mSymbols;

   WsfParseType* mSourceTypePtr;
   WsfParseType* mContainer;
};

class WSF_PARSER_EXPORT WsfParseTypeName
{
public:
   WsfParseTypeName()
      : mNestedLookup(false)
      , mOrdinal(ut::npos)
   {
   }
   bool             mNestedLookup;
   WsfParseTypePath mPath;
   // If not npos, identifies the token index from the sequence that is appended to the type path
   // ; as in (type platformType $1)
   size_t mOrdinal;
};
//
// class WSF_PARSER_EXPORT WsfParseSymbolTable
// {
//    public:
//       WsfParseSymbolTable(WsfParseSymbolTable* aParentPtr) : mParentPtr(aParentPtr) {}
//       WsfParseSymbolTable(const WsfParseSymbolTable& aRhs, WsfParseSymbolTable* aParentPtr)
//        : mSymbols(aRhs.mSymbols),
//          mParentPtr(aParentPtr)
//       {
//          for (SymbolTable::iterator i = mSymbols.begin(); i != mSymbols.end(); ++i)
//          {
//             i->second.SetContainer(this);
//          }
//       }
//       ~WsfParseSymbolTable();
//
//
//    private:
//       bool operator==(const WsfParseSymbolTable& aRhs) const;
// };
//


#endif
