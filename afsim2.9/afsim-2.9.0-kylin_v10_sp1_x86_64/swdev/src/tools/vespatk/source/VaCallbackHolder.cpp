// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "VaCallbackHolder.hpp"

#include "VaCallback.hpp"

namespace vespa
{
int VaCallbackHolder::Add(VaCallback* aCallbackPtr)
{
   int callbackUniqueId = aCallbackPtr->GetUniqueId();
   mCallbacks.push_back(aCallbackPtr);
   return callbackUniqueId;
}

VaCallback* VaCallbackHolder::Remove(int aCallbackUID)
{
   VaCallback* callbackPtr = nullptr;
   for (std::list<VaCallback*>::iterator it = mCallbacks.begin(); it != mCallbacks.end(); ++it)
   {
      if ((*it)->GetUniqueId() == (unsigned int)aCallbackUID)
      {
         callbackPtr = *it;
         mCallbacks.erase(it);
         break;
      }
   }
   return callbackPtr;
}

bool VaCallbackHolder::Disconnect(int aCallbackUID)
{
   bool        disconnected = false;
   VaCallback* callbackPtr  = Remove(aCallbackUID);
   if (callbackPtr != nullptr)
   {
      callbackPtr->SetDeleteFlag(true);
      callbackPtr->Disconnect();
      disconnected = true;
   }
   return disconnected;
}

//! Delete all registered callbacks.
//! This method disconnects all callbacks from their respective callback lists
//! and deletes the callback object.
void VaCallbackHolder::Clear()
{
   for (std::list<VaCallback*>::iterator i = mCallbacks.begin(); i != mCallbacks.end(); ++i)
   {
      (*i)->SetDeleteFlag(true);
      (*i)->Disconnect();
   }
   mCallbacks.clear();
}

bool VaCallbackHolder::Exists(int aCallbackUID) const
{
   bool exists = false;
   for (std::list<VaCallback*>::const_iterator it = mCallbacks.begin(); it != mCallbacks.end(); ++it)
   {
      if ((*it)->GetUniqueId() == (unsigned int)aCallbackUID)
      {
         exists = true;
         break;
      }
   }
   return exists;
}

bool VaCallbackHolder::Exists(int aCallbackUID, const VaCallbackGroupId& aCallbackGroupId) const
{
   bool exists = false;
   for (std::list<VaCallback*>::const_iterator it = mCallbacks.begin(); it != mCallbacks.end(); ++it)
   {
      if (((*it)->GetUniqueId() == (unsigned int)aCallbackUID) && ((*it)->GetGroupId() == aCallbackGroupId))
      {
         exists = true;
         break;
      }
   }
   return exists;
}
} // end namespace vespa
