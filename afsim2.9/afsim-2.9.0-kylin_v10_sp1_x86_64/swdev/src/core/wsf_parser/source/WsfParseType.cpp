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

#include "WsfParseType.hpp"

#include <sstream>

#include "UtStringUtil.hpp"
#include "WsfParseRule.hpp"

WsfParseTypePath WsfParseTypePathParse(const std::string& aDotSepPath)
{
   WsfParseTypePath out;
   if (!aDotSepPath.empty())
   {
      std::vector<std::string> words;
      UtStringUtil::Parse(aDotSepPath, words, ".");
      for (size_t i = 0; i < words.size(); ++i)
      {
         out.push_back(UtStringRef(words[i]));
      }
   }
   return out;
}

std::string WsfParseTypePathString(const WsfParseTypePath& aPaths)
{
   std::stringstream ss;
   for (size_t i = 0; i < aPaths.size(); ++i)
   {
      if (i != 0)
      {
         ss << '.';
      }
      ss << aPaths[i].Get();
   }
   return ss.str();
}

WsfParseType& WsfParseType::AddType(WsfParseType* aTypePtr)
{
   WsfParseType*& type = mSymbols[aTypePtr->mName];
   if (!type)
   {
      type = aTypePtr;
      type->SetContainer(this);
   }
   else
   {
      // A stub type may have already been added for nested types
      // Leave the existing nested types.
      type->Merge(*aTypePtr);
      delete aTypePtr;
   }

   //
   //    if (type.Table() && aType.Table())
   //    {
   //       type.Table()->Merge(*aType.Table());
   //    }
   //    else if (aType.Table())
   //    {
   //       std::swap(type.mNestedTypesPtr, aType.mNestedTypesPtr);
   //    }
   return *type;
}

WsfParseType& WsfParseType::AddType(const WsfParseTypePath& aPath, WsfParseType* aType)
{
   WsfParseType* tablePtr = this;
   for (size_t i = 0; i + 1 < aPath.size(); ++i)
   {
      SymbolTable::iterator iter = tablePtr->mSymbols.find(aPath[i]);
      if (iter != tablePtr->mSymbols.end())
      {
         tablePtr = iter->second;
      }
      else
      {
         tablePtr = tablePtr->mSymbols[aPath[i]] = new WsfParseType(aPath[i], tablePtr);
      }
   }
   return tablePtr->AddType(aType);
}

bool WsfParseType::RemoveType(const WsfParseTypePath& aKey)
{
   bool removed = false;
   if (!aKey.empty())
   {
      WsfParseType* tbl = this;
      for (size_t i = 0; i + 1 < aKey.size() && tbl; ++i)
      {
         tbl = tbl->FindType(aKey[i]);
      }
      if (tbl)
      {
         removed = tbl->RemoveType(aKey.back());
      }
   }
   return removed;
}
bool WsfParseType::RemoveType(const UtStringRef& aName)
{
   SymbolTable::iterator iter = mSymbols.find(aName);
   if (iter != mSymbols.end())
   {
      // TODO: Verify this pointer really isn't referenced anymore?
      delete iter->second;

      mSymbols.erase(iter);
      return true;
   }
   else
   {
      return false;
   }
}

WsfParseType::~WsfParseType()
{
   Clear();
}

const WsfParseType* WsfParseType::FindNestedSymbol(const WsfParseTypePath& aKey) const
{
   const WsfParseType* top = this;
   while (top)
   {
      const WsfParseType* foundTypePtr = top->FindTypeLocal(aKey);
      if (foundTypePtr)
      {
         return foundTypePtr;
      }
      top = top->GetContainer();
   }
   return nullptr;
}

WsfParseType::WsfParseType(const UtStringRef& aName, const WsfParseType& aRhs, WsfParseType* aContainerPtr)
   : mName(aName)
   , mReaderPtr(aRhs.mReaderPtr)
   , mSymbols(aRhs.mSymbols)
   , mSourceTypePtr(nullptr)
   , mContainer(aContainerPtr)
{
   for (SymbolTable::iterator i = mSymbols.begin(); i != mSymbols.end(); ++i)
   {
      i->second = new WsfParseType(i->first, *i->second, this);
   }
}

void WsfParseType::Clear()
{
   for (SymbolTable::iterator i = mSymbols.begin(); i != mSymbols.end(); ++i)
   {
      delete i->second;
   }
   mSymbols.clear();
   mReaderPtr = nullptr;
}

void WsfParseType::Merge(WsfParseType& aRhs)
{
   for (SymbolTable::iterator i = aRhs.mSymbols.begin(); i != aRhs.mSymbols.end(); ++i)
   {
      if (mSymbols.find(i->first) == mSymbols.end())
      {
         WsfParseType*& type = mSymbols[i->first] = i->second;
         i->second                                = nullptr; // take ownership ...
         type->SetContainer(this);
      }
   }
   if (!mReaderPtr)
   {
      mReaderPtr = aRhs.mReaderPtr;
   }
   aRhs.Clear();
}

void WsfParseType::Print(std::ostream& aStream, const std::string& aIndent)
{
   aStream << aIndent << mName.Get();
   if (mReaderPtr)
   {
      WsfParseNamedRule* ctx = mReaderPtr->GetBestContext();
      aStream << " ( " << ctx->GetFullName() << " )";
   }
   aStream << '\n';
   if (!mSymbols.empty())
   {
      std::string nextIndent = aIndent + ' ';
      for (SymbolTable::iterator i = mSymbols.begin(); i != mSymbols.end(); ++i)
      {
         i->second->Print(aStream, nextIndent);
      }
   }
}

void WsfParseType::GetAncestry(std::vector<WsfParseType*>& aAncestry)
{
   if (mContainer)
   {
      mContainer->GetAncestry(aAncestry);
   }
   aAncestry.push_back(this);
}

void WsfParseType::GetAncestry(std::vector<const WsfParseType*>& aAncestry) const
{
   if (mContainer)
   {
      mContainer->GetAncestry(aAncestry);
   }
   aAncestry.push_back(this);
}

WsfParseType* WsfParseType::FindTypeLocal(const UtStringRef& aKey) const
{
   SymbolTable::const_iterator i = mSymbols.find(aKey);
   if (i != mSymbols.end())
   {
      return i->second;
   }
   return nullptr;
}

const WsfParseType* WsfParseType::FindTypeLocal(const WsfParseTypePath& aKey) const
{
   WsfParseType*       typePtr  = nullptr;
   const WsfParseType* symTable = this;
   for (size_t i = 0; i < aKey.size() && symTable; ++i)
   {
      typePtr  = symTable->FindTypeLocal(aKey[i]);
      symTable = typePtr;
   }
   return typePtr;
}

const WsfParseType* WsfParseType::FindType(const UtStringRef& aKey) const
{
   WsfParseType* typePtr = FindTypeLocal(aKey);
   if (!typePtr && mContainer)
   {
      typePtr = mContainer->FindType(aKey);
   }
   return typePtr;
}

const WsfParseType* WsfParseType::FindType(const WsfParseTypePath& aKey) const
{
   const WsfParseType* localTypePtr = FindTypeLocal(aKey);
   if (!localTypePtr && mContainer)
   {
      return mContainer->FindType(aKey);
   }
   return localTypePtr;
}

void WsfParseType::GetPath(WsfParseTypePath& aPath) const
{
   if (mContainer)
   {
      mContainer->GetPath(aPath);
      // Don't include the top-level type's name
      aPath.push_back(mName);
   }
}

/*
platform_type x WSF_PLATFORM
   processor proc WSF_SCRIPT_PROCESSOR
   end_processor
end_platform_type
platform_type y x
   processor proc
   end_processor
end_platform_type

A view showing container links (mContainer)
   platforms <-- x <- processors <- proc
              \- y <- processors <- proc
View with 'source' links
   WSF_PLATFORM <- x <- y
   WSF_SCRIPT_PROCESSOR <- proc
   (y's proc has no source ptr)

y.processors.proc.GetEffectiveSourceType() == x.processors.proc
*/
const WsfParseType* WsfParseType::GetEffectiveSourceType() const
{
   if (mSourceTypePtr)
   {
      return mSourceTypePtr;
   }
   const WsfParseType* current = mContainer;
   WsfParseTypePath    relPath;
   relPath.push_back(mName);
   while (current)
   {
      if (current->GetSourceType())
      {
         const WsfParseType* typePtr = current->GetSourceType()->FindTypeLocal(relPath);
         if (typePtr)
         {
            return typePtr;
         }
      }
      relPath.insert(relPath.begin(), current->GetTypeName());
      current = current->GetContainer();
   }
   return nullptr;
}
