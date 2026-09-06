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

#include "VaViewerManager.hpp"

#include <algorithm>

#include "VaDefs.hpp"
#include "VaObserver.hpp"

namespace vespa
{
VaViewerManager::VaViewerManager()
   : mViewers()
   , mCurrentViewIndex(0)
{
}

VaViewerManager::~VaViewerManager()
{
   Viewers::iterator it;
   for (it = mViewers.begin(); it != mViewers.end(); ++it)
   {
      VaViewer* viewer = *it;
      delete viewer;
   }
   mViewers.clear();
   mCallbacks.Clear();
}

//! Method: AddViewer
//! Description: Adds the viewer to the manager
//!
//! @param aViewerPtr The viewer to add.
//! @note ViewerManager does not manage the memory of the viewer.

// public
bool VaViewerManager::AddViewer(VaViewer* aViewerPtr)
{
   bool success = false;
   if (nullptr != aViewerPtr)
   {
      Viewers::const_iterator iter = std::find(mViewers.begin(), mViewers.end(), aViewerPtr);
      if (iter == mViewers.end())
      {
         aViewerPtr->SetSelectionMask(cVESPA_ENTITY_SELECTION_MASK << mViewers.size());
         mViewers.push_back(aViewerPtr);
         success = true;
      }
   }
   return success;
}

//! Method: RemoveViewer
//! Description: Removes a viewer from the manager
//!
//! @param aViewerPtr The viewer to remove.

// public
bool VaViewerManager::RemoveViewer(VaViewer* aViewerPtr)
{
   bool success = false;
   if (nullptr != aViewerPtr)
   {
      Viewers::iterator iter = std::find(mViewers.begin(), mViewers.end(), aViewerPtr);
      if (iter != mViewers.end())
      {
         mViewers.erase(iter);
         success = true;
      }
   }
   return success;
}

//! Method: CurrentViewer
//! Description: Returns the current viewer.

// public
VaViewer* VaViewerManager::CurrentViewer() const
{
   VaViewer* viewerPtr = nullptr;
   if (mCurrentViewIndex < mViewers.size())
   {
      viewerPtr = mViewers[mCurrentViewIndex];
   }
   return viewerPtr;
}

VaViewer* VaViewerManager::GetViewerByIndex(unsigned int aIndex) const
{
   VaViewer* viewerPtr = nullptr;
   if (aIndex < mViewers.size())
   {
      viewerPtr = mViewers[aIndex];
   }
   return viewerPtr;
}

void VaViewerManager::SetCurrentViewer(VaViewer* aViewerPtr)
{
   unsigned int idx = 0;
   for (Viewers::iterator iter = mViewers.begin(); iter != mViewers.end(); ++iter)
   {
      if ((*iter)->GetUniqueId() == aViewerPtr->GetUniqueId())
      {
         break;
      }

      ++idx;
   }
   if (idx < mViewers.size())
   {
      SetCurrentViewIndex(idx);
   }
}

} // namespace vespa
