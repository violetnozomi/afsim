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

#include "WsfPProxyPath.hpp"

#include <sstream>

#include "UtLexicalCast.hpp"
#include "UtStringUtil.hpp"
#include "WsfPProxyStructValue.hpp"
#include "WsfPProxyType.hpp"

std::string WsfPProxyPath::ToString(const WsfPProxyValue& aRootProxy, size_t aEntryIndex) const
{
   WsfPProxyValue val = aRootProxy;
   for (size_t i = 0; i < mEntries.size(); ++i)
   {
      std::string         entryName;
      const WsfPProxyKey& e = mEntries[i];
      if (e.IsIndex())
      {
         if (val && val.IsStruct())
         {
            entryName = WsfPProxyStructValue(val).GetMemberName(e.GetIndex());
         }
         else
         {
            entryName = std::to_string(e.GetIndex());
         }
         if (val)
         {
            val = val.GetAtIndex(e.GetIndex());
         }
      }
      else
      {
         entryName = e.GetMapKey();
         if (val)
         {
            val = val.GetAttr(e.GetMapKey());
         }
      }
      if (i == aEntryIndex)
      {
         return entryName;
      }
   }
   return "";
}

// Return a string representation of this address
std::string WsfPProxyPath::ToString(const WsfPProxyValue& aRootProxy) const
{
   std::string    rep;
   WsfPProxyValue val = aRootProxy;
   for (size_t i = 0; i < mEntries.size(); ++i)
   {
      std::string         entryName;
      const WsfPProxyKey& e = mEntries[i];
      if (e.IsIndex())
      {
         if (val && val.IsStruct())
         {
            entryName = WsfPProxyStructValue(val).GetMemberName(e.GetIndex());
            if (entryName.empty())
            {
               entryName = "_NO_MEMBER_";
            }
         }
         else
         {
            entryName = std::to_string(e.GetIndex());
         }
         if (val)
         {
            val = val.GetAtIndex(e.GetIndex());
         }
      }
      else
      {
         entryName = e.GetMapKey();
         if (val)
         {
            val = val.GetAttr(e.GetMapKey());
         }
      }
      if (!rep.empty())
      {
         rep += '.';
      }
      rep += entryName;
   }
   return rep;
}

std::vector<std::string> WsfPProxyPath::ToStringList(const WsfPProxyValue& aRootProxy) const
{
   std::vector<std::string> stringList;
   WsfPProxyValue           val = aRootProxy;
   for (const auto& e : mEntries)
   {
      std::string entryName;
      if (e.IsIndex())
      {
         if (val && val.IsStruct())
         {
            entryName = WsfPProxyStructValue(val).GetMemberName(e.GetIndex());
         }
         else
         {
            entryName = std::to_string(e.GetIndex());
         }
         if (val)
         {
            val = val.GetAtIndex(e.GetIndex());
         }
      }
      else
      {
         entryName = e.GetMapKey();
         if (val)
         {
            val = val.GetAttr(e.GetMapKey());
         }
      }
      stringList.push_back(entryName);
   }
   return stringList;
}

WsfPProxyValue WsfPProxyPath::Lookup(WsfPProxyValue aRoot)
{
   WsfPProxyValue val = aRoot;
   for (const auto& e : mEntries)
   {
      if (e.IsIndex())
      {
         val = val.GetAtIndex(e.GetIndex());
      }
      else
      {
         val = val.GetAttr(e.GetMapKey());
      }
   }
   return val;
}
//! Convert a string representing a proxy path to an actual proxy path object.
//! The string should be in the form:  A.B.C
WsfPProxyPath WsfPProxyPath::FromString(WsfPProxyValue aRoot, const std::string& aStr)
{
   WsfPProxyValue currentValue = aRoot;
   WsfPProxyPath  path;
   size_t         p = 0;
   size_t         i = 0;
   for (; i < aStr.size(); ++i)
   {
      if (aStr[i] == '.')
      {
         if (i > p)
         {
            std::string part = aStr.substr(p, i - p);
            if (isdigit(part[0]) && currentValue.GetObjectMap() == nullptr)
            {
               size_t intVal = ut::lexical_cast<size_t>(part);
               path += intVal;
               currentValue = currentValue.GetAtIndex(intVal);
            }
            else
            {
               if (currentValue.IsStruct())
               {
                  size_t idx = WsfPProxyStructValue(currentValue).GetMemberIndex(part);
                  path += idx;
                  currentValue = currentValue.GetAtIndex(idx);
               }
               else
               {
                  path += part;
                  currentValue = currentValue.GetAttr(part);
               }
            }
         }
         p = i + 1;
      }
   }
   if (i > p)
   {
      std::string part = aStr.substr(p, i - p);
      if (isdigit(part[0]) && currentValue.GetObjectMap() == nullptr)
      {
         size_t intVal = ut::lexical_cast<size_t>(part);
         path += intVal;
      }
      else
      {
         if (currentValue.IsStruct())
         {
            size_t idx = WsfPProxyStructValue(currentValue).GetMemberIndex(part);
            path += idx;
         }
         else
         {
            path += part;
         }
      }
   }
   return path;
}

WsfPProxyPath WsfPProxyPath::FromStringList(WsfPProxyValue aRoot, const std::vector<std::string>& aStrList)
{
   WsfPProxyValue currentValue = aRoot;
   WsfPProxyPath  path;
   for (const auto& part : aStrList)
   {
      if (part.empty())
      {
         continue;
      }
      if (isdigit(part[0]) && currentValue.GetObjectMap() == nullptr)
      {
         size_t intVal = ut::lexical_cast<size_t>(part);
         path += intVal;
         currentValue = currentValue.GetAtIndex(intVal);
      }
      else
      {
         if (currentValue.IsStruct())
         {
            size_t idx = WsfPProxyStructValue(currentValue).GetMemberIndex(part);
            path += idx;
            currentValue = currentValue.GetAtIndex(idx);
         }
         else
         {
            path += part;
            currentValue = currentValue.GetAttr(part);
         }
      }
   }
   return path;
}

WsfPProxyPath WsfPProxyPath::FromTypePath(WsfPProxyValue aRoot, const WsfParseTypePath& aTypePath)
{
   std::vector<std::string> stringList(aTypePath.size());
   for (size_t i = 0; i < aTypePath.size(); ++i)
   {
      stringList[i] = aTypePath[i].Get();
   }
   return FromStringList(aRoot, stringList);
}


bool WsfPProxyPath::Push(const WsfPProxyValue& aBaseValue, const std::string& aAttrName)
{
   const WsfPProxyType* typePtr = aBaseValue.GetType();
   if (typePtr)
   {
      switch (aBaseValue.GetType()->mTypeKind)
      {
      case WsfProxy::cOBJECT_MAP:
         (*this) += aAttrName;
         return true;
      case WsfProxy::cLIST:
      {
         size_t index;
         if (ut::conversion::try_lexical_convert(aAttrName, index))
         {
            (*this) += index;
            return true;
         }
         return false;
      }
      case WsfProxy::cSTRUCT:
      {
         size_t index = typePtr->GetMemberIndex(aAttrName);
         if (index != ut::npos)
         {
            (*this) += index;
            return true;
         }
         else
         {
            return false;
         }
      }
      default:
         return false;
      }
   }
   else
   {
      return false;
   }
}

size_t WsfPProxyPath::MemoryUsage() const
{
   size_t usage = mEntries.size() * sizeof(WsfPProxyKey);
   for (size_t i = 0; i < mEntries.size(); ++i)
   {
      usage += mEntries[i].GetMapKey().size();
   }
   return usage;
}
