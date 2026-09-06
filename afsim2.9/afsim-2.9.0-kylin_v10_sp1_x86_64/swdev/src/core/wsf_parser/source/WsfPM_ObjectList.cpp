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

#include "WsfPM_ObjectList.hpp"

//! Returns the entry with the given name
//! If no object exists with that name, IsNull() returns true
WsfPProxyNode WsfPM_List::operator[](size_t aIndex)
{
   WsfPProxyList* listPtr = list();
   if (!listPtr)
   {
      return WsfPProxyNode();
   }
   WsfPProxyValue val = listPtr->Get(aIndex);

   return WsfPProxyNode(GetProxy(), GetPath() + aIndex, val);
}

WsfPProxyNode WsfPM_List::back() const
{
   WsfPProxyList* listPtr = list();
   if (!listPtr || listPtr->Size() == 0)
   {
      return WsfPProxyNode();
   }
   size_t ct = listPtr->Size();
   return *this + ct;
}

bool WsfPM_List::append(WsfPProxyValue aValue)
{
   WsfPProxyList* listPtr = list();
   if (listPtr && listPtr->GetDataType() == aValue.GetType())
   {
      size_t        idx         = listPtr->Size();
      WsfPProxyPath newNodePath = GetPath();
      newNodePath += idx;

      GetProxy()->NotifyBeforeProxyModified(WsfPProxy::cPM_CHILD_ADDED, newNodePath, nullptr);
      listPtr->PushBack(aValue);
      GetProxy()->NotifyProxyModified(WsfPProxy::cPM_CHILD_ADDED, newNodePath, nullptr);
      return true;
   }
   return false;
}

bool WsfPM_List::insert(WsfPProxyValue aValue, size_t aIndex)
{
   WsfPProxyList* listPtr = list();
   if (listPtr && listPtr->GetDataType() == aValue.GetType() && listPtr->Size() >= aIndex)
   {
      WsfPProxyPath newNodePath = GetPath();
      newNodePath += aIndex;
      GetProxy()->NotifyBeforeProxyModified(WsfPProxy::cPM_CHILD_ADDED, newNodePath, nullptr);
      listPtr->Insert(aValue, aIndex);
      GetProxy()->NotifyProxyModified(WsfPProxy::cPM_CHILD_ADDED, newNodePath, nullptr);
      return true;
   }
   return false;
}
bool WsfPM_List::remove(size_t aIndex)
{
   WsfPProxyList* listPtr = list();
   if (listPtr && listPtr->Size() > aIndex)
   {
      WsfPProxyPath newNodePath = GetPath();
      newNodePath += aIndex;
      GetProxy()->NotifyBeforeProxyModified(WsfPProxy::cPM_CHILD_REMOVED, newNodePath, nullptr);
      listPtr->Remove(aIndex);
      GetProxy()->NotifyProxyModified(WsfPProxy::cPM_CHILD_REMOVED, newNodePath, nullptr);
      return true;
   }
   return false;
}
