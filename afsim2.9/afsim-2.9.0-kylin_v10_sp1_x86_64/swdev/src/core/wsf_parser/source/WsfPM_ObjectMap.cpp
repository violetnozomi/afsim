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

#include "WsfPM_ObjectMap.hpp"

WsfPM_ObjectMap::WsfPM_ObjectMap() {}

WsfPM_ObjectMap::WsfPM_ObjectMap(const WsfPProxyNode& aNode)
   : WsfPProxyNode(aNode)
{
}

WsfPProxyObjectMap* WsfPM_ObjectMap::GetObjectMap() const
{
   return GetValue().GetObjectMap();
}

std::vector<std::string> WsfPM_ObjectMap::GetObjectNames() const
{
   WsfPProxyObjectMap* mapPtr = GetObjectMap();
   return mapPtr ? GetObjectMap()->GetObjectNames() : std::vector<std::string>();
}

std::vector<std::string> WsfPM_ObjectMap::GetAllObjectNames()
{
   SwitchToUserTypes();
   std::vector<std::string> allNames = GetObjectNames();
   if (SwitchToBasicTypes())
   {
      std::vector<std::string> basicNames = GetObjectNames();
      allNames.insert(allNames.end(), basicNames.begin(), basicNames.end());
      Invalidate();
   }
   return allNames;
}

WsfPM_ObjectMap::iterator WsfPM_ObjectMap::begin() const
{
   return GetObjectMap()->GetValues().begin();
}

WsfPM_ObjectMap::iterator WsfPM_ObjectMap::end() const
{
   return GetObjectMap()->GetValues().end();
}

//! Returns the entry with the given name
//! If no object exists with that name, IsNull() returns true
WsfPProxyNode WsfPM_ObjectMap::Find(const std::string& aName) const
{
   WsfPProxyObjectMap* mapPtr = GetObjectMap();
   if (!mapPtr)
   {
      return WsfPProxyNode();
   }
   WsfPProxyValue* valuePtr = mapPtr->FindAt(aName);
   if (!valuePtr)
   {
      return WsfPProxyNode();
   }
   WsfPProxyNode objectNode(*this);
   objectNode += aName;
   return objectNode;
}

WsfPProxyNode WsfPM_ObjectMap::At(const iterator& i) const
{
   return *this + i->first;
}

bool WsfPM_ObjectMap::Exists(const std::string& aName) const
{
   return GetValue()[aName].IsValid();
}

bool WsfPM_ObjectMap::Remove(const std::string& aName, bool removedByUser)
{
   WsfPProxyObjectMap* mapPtr = GetObjectMap();
   if (mapPtr && mapPtr->FindAt(aName))
   {
      WsfPProxyNode childNode = *this + aName;
      GetProxy()->NotifyBeforeProxyModified(WsfPProxy::cPM_CHILD_REMOVED, childNode.GetPath(), nullptr);
      if (removedByUser)
      {
         mapPtr->UserErase(aName);
      }
      else
      {
         mapPtr->Erase(aName);
      }
      childNode.Invalidate();
      childNode.NotifyValueRemoved();
      return true;
   }
   return false;
}

void WsfPM_ObjectMap::GetValues(std::vector<WsfPProxyNode>& aAll) const
{
   GetValuesT<WsfPProxyNode>(aAll);
}

void WsfPM_ObjectMap::GetAllValues(std::vector<WsfPProxyNode>& aAll)
{
   GetAllValuesT<WsfPProxyNode>(aAll);
}

WsfPProxyNode WsfPM_ObjectMap::Add(const std::string& aName, WsfPProxyValue aEntry)
{
   if (Exists(aName))
   {
      aEntry.Delete();
      return WsfPProxyNode();
   }
   WsfPProxyNode childNode = *this + aName;
   GetProxy()->NotifyBeforeProxyModified(WsfPProxy::cPM_CHILD_ADDED, childNode.GetPath(), nullptr);
   GetObjectMap()->AddNew(aName, aEntry);
   childNode.Invalidate();
   childNode.NotifyValueAdded();
   return childNode;
}

WsfPProxyNode WsfPM_ObjectMap::AssignEntry(const std::string& aName, WsfPProxyValue aEntry)
{
   if (!IsValid())
   {
      return WsfPProxyNode();
   }
   if (Exists(aName))
   {
      WsfPProxyValue* oldValue = GetObjectMap()->FindAt(aName);
      if (oldValue->IsBasicType())
      {
         WsfPProxyNode child = *this + aName;
         child.NotifyBeforeValueChanged();
         GetObjectMap()->AddNew(aName, aEntry, true);
         child.NotifyValueChanged();
         return child;
      }
      else
      {
         Remove(aName);
         return Add(aName, aEntry);
      }
   }
   else
   {
      return Add(aName, aEntry);
   }
   //    WsfPProxyNode childNode = *this + aName;
   //    if (!childNode.assign(aEntry))
   //    {
   //       aEntry.Delete();
   //    }
   //    return childNode;
}

bool WsfPM_ObjectMap::Move(const std::string& aOldName, const std::string& aNewName)
{
   WsfPProxyObjectMap* mapPtr = GetObjectMap();
   if (!mapPtr || mapPtr->FindAt(aNewName))
   {
      return false;
   }
   WsfPProxyValue* oldValue = mapPtr->FindAt(aOldName);
   if (!oldValue)
   {
      return false;
   }
   WsfPProxyPath oldPath = GetPath();
   oldPath += aOldName;
   WsfPProxyPath newPath = GetPath();
   newPath += aNewName;
   GetProxy()->NotifyBeforeProxyModified(WsfPProxy::cPM_CHILD_MOVED, oldPath, &aNewName);

   // move to new name
   mapPtr->AddNew(aNewName, *oldValue);
   mapPtr->EraseNoDelete(aOldName);

   GetProxy()->RecordObjectRename(oldPath, newPath);
   // references to old node need to be invalidated:
   GetProxy()->IncrementSequenceNumber();
   GetProxy()->NotifyProxyModified(WsfPProxy::cPM_CHILD_MOVED, oldPath, &aNewName);
   return true;
}

bool WsfPM_ObjectMap::CanMove(const std::string& aOldName, const std::string& aNewName)
{
   WsfPProxyObjectMap* mapPtr = GetObjectMap();
   if (!mapPtr || mapPtr->FindAt(aNewName))
   {
      return false;
   }
   WsfPProxyValue* oldValue = mapPtr->FindAt(aOldName);
   if (!oldValue)
   {
      return false;
   }
   return true;
}
