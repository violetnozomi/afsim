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

#include "WsfPProxyDiff.hpp"

#include "UtAlgorithm.hpp"
#include "WsfPProxyList.hpp"
#include "WsfPProxyObjectMap.hpp"
#include "WsfPProxyStructValue.hpp"
#include "WsfPProxyType.hpp"

void WsfPProxyDiff::AddDiff(const WsfPProxyPath& aPath, DiffType aType)
{
   DiffEntry e;
   e.mPath = aPath;
   e.mType = aType;
   mDiffs.push_back(e);
}

void WsfPProxyDiff::Diff(WsfPProxyValue aOld, WsfPProxyValue aNew, const WsfPProxyPath& aPath)
{
   if (aOld && !aNew)
   {
      AddDiff(aPath, cREMOVED);
      return;
   }
   if (!aOld && aNew)
   {
      AddDiff(aPath, cADDED);
      return;
   }

   if (aOld.GetType() != aNew.GetType())
   {
      AddDiff(aPath, cCHANGED);
      return;
   }

   const WsfPProxyType* typePtr = aOld.GetType();
   if (!typePtr)
   {
      return;
   }

   if (typePtr->IsBasicType())
   {
      const WsfPProxyBasicType* bt = (const WsfPProxyBasicType*)typePtr;
      if (bt->Equal(aOld.GetDataPtr(), aNew.GetDataPtr()))
      {
         return;
      }
      AddDiff(aPath, cCHANGED);
      return;
   }
   if (typePtr->IsStruct())
   {
      WsfPProxyStructValue iold  = aOld;
      WsfPProxyStructValue inew  = aNew;
      WsfPProxyPath        path  = aPath;
      size_t               count = iold.GetMemberCount();
      for (size_t i = 0; i < count; ++i)
      {
         path += i;
         Diff(iold[i], inew[i], path);
         path.Pop();
      }
   }
   if (typePtr->IsObjectMap())
   {
      WsfPProxyPath       path   = aPath;
      WsfPProxyObjectMap* mold   = aOld.GetObjectMap();
      WsfPProxyObjectMap* mnew   = aNew.GetObjectMap();
      auto                iold   = mold->GetValues().begin();
      auto                endOld = mold->GetValues().end();
      auto                inew   = mnew->GetValues().begin();
      auto                endNew = mnew->GetValues().end();
      while (iold != endOld || inew != endNew)
      {
         bool removed = (inew == endNew);
         bool added   = (iold == endOld);
         if (!(removed || added))
         {
            if (iold->first < inew->first)
            {
               removed = true;
            }
            else if (inew->first < iold->first)
            {
               added = true;
            }
         }
         if (removed)
         {
            path += iold->first;
            AddDiff(path, cREMOVED);
            path.Pop();
            ++iold;
         }
         else if (added)
         {
            path += inew->first;
            AddDiff(path, cADDED);
            path.Pop();
            ++inew;
         }
         else
         {
            path += inew->first;
            Diff(iold->second, inew->second, path);
            path.Pop();
            ++inew;
            ++iold;
         }
      }
   }
   if (typePtr->IsList())
   {
      WsfPProxyPath  path = aPath;
      WsfPProxyList* lold = aOld.GetList();
      WsfPProxyList* lnew = aNew.GetList();
      if (lold->Size() != lnew->Size())
      {
         // assert(lold->Size() == 0);//todo: Not sure if we should get to this condition...
         size_t size = lnew->Size();
         for (size_t i = 0; i < size; ++i)
         {
            path += i;
            AddDiff(path, cADDED);
            path.Pop();
         }
         return;
      }
      size_t size = lold->Size();
      for (size_t i = 0; i < size; ++i)
      {
         path += i;
         Diff(lold->Get(i), lnew->Get(i), path);
         path.Pop();
      }
   }
}

bool WsfPProxyDiff::IsEqual(WsfPProxyValue aOld, WsfPProxyValue aNew)
{
   if (aOld && !aNew)
   {
      return false;
   }
   if (!aOld && aNew)
   {
      return false;
   }

   if (aOld.GetType() != aNew.GetType())
   {
      return false;
   }

   const WsfPProxyType* typePtr = aOld.GetType();
   if (!typePtr)
   {
      return true;
   }

   if (typePtr->IsBasicType())
   {
      const WsfPProxyBasicType* bt = (const WsfPProxyBasicType*)typePtr;
      return bt->Equal(aOld.GetDataPtr(), aNew.GetDataPtr());
   }
   if (typePtr->IsStruct())
   {
      WsfPProxyStructValue iold  = aOld;
      WsfPProxyStructValue inew  = aNew;
      size_t               count = iold.GetMemberCount();
      for (size_t i = 0; i < count; ++i)
      {
         if (!IsEqual(iold[i], inew[i]))
         {
            return false;
         }
      }
      return true;
   }
   if (typePtr->IsObjectMap())
   {
      WsfPProxyObjectMap* mold   = aOld.GetObjectMap();
      WsfPProxyObjectMap* mnew   = aNew.GetObjectMap();
      auto                iold   = mold->GetValues().begin();
      auto                endOld = mold->GetValues().end();
      auto                inew   = mnew->GetValues().begin();
      auto                endNew = mnew->GetValues().end();
      while (iold != endOld || inew != endNew)
      {
         bool removed = (inew == endNew);
         bool added   = (iold == endOld);
         if (!(removed || added))
         {
            if (iold->first < inew->first)
            {
               removed = true;
            }
            else if (inew->first < iold->first)
            {
               added = true;
            }
         }
         if (removed)
         {
            return false;
         }
         else if (added)
         {
            return false;
         }
         else
         {
            if (!IsEqual(iold->second, inew->second))
            {
               return false;
            }
            ++inew;
            ++iold;
         }
      }
      return true;
   }
   if (typePtr->IsList())
   {
      WsfPProxyList* lold = aOld.GetList();
      WsfPProxyList* lnew = aNew.GetList();
      if (lold->Size() != lnew->Size())
      {
         return false;
      }
      size_t size = lold->Size();
      for (size_t i = 0; i < size; ++i)
      {
         IsEqual(lold->Get(i), lnew->Get(i));
      }
   }
   return true; // TODO:
}
