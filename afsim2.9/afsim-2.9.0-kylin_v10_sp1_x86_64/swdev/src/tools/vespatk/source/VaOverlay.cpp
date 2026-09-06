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

#include "VaOverlay.hpp"

#include "Uto.hpp"
#include "UtoScreenOverlayManager.hpp"
#include "VaViewer.hpp"

namespace vespa
{
VaOverlay::VaOverlay(const std::string& aOverlayName, OverlayType aOverlayType, bool aCreateUniqueName)
   : VaObject()
   , mViewerPtr(nullptr)
   , mName(aOverlayName)
   , mOverlayType(aOverlayType)
   , mOverlayScope(cOVERLAY_GLOBAL)
   , mSelectable(false)
   , mUpdateVisibilityHACK(false)
   , mNewVisibilityHACK(false)
{
   if (aCreateUniqueName)
   {
      mName = aOverlayName + "_" + std::to_string(GetUniqueId());
   }
   SetType<VaOverlay>();
}

VaOverlay::VaOverlay(const VaOverlay& aSrc)
   : VaObject(aSrc)
   , mViewerPtr(aSrc.mViewerPtr)
   , mName(aSrc.mName)
   , mOverlayType(aSrc.mOverlayType)
   , mOverlayScope(aSrc.mOverlayScope)
   , mSelectable(aSrc.mSelectable)
   , mUpdateVisibilityHACK(aSrc.mUpdateVisibilityHACK)
   , mNewVisibilityHACK(aSrc.mNewVisibilityHACK)
{
}

VaOverlay::~VaOverlay()
{
   // Remove the overlay from either the window or world, but
   // make sure the viewer is valid first.
   if ((!mName.empty()) && (mViewerPtr != nullptr) && (mViewerPtr->GetViewer() != nullptr))
   {
      if (mOverlayType == cOVERLAY_WINDOW)
      {
         UtoScreenOverlayManager*          som         = mViewerPtr->GetViewer()->GetScreenOverlayManager();
         UtoScreenOverlayManager::iterator overlayIter = som->FindOverlay(mName);
         if (overlayIter != som->Overlays().end)
         {
            som->RemoveOverlay(overlayIter);
         }
      }
      else
      {
         UtoWorld&                  world       = mViewerPtr->GetViewer()->World();
         UtoWorld::overlay_iterator overlayIter = world.FindOverlay(mName);
         if (overlayIter != world.Overlays().end())
         {
            world.RemoveOverlay(overlayIter);
         }
      }
   }
}

void VaOverlay::Update()
{
   if (mUpdateVisibilityHACK)
   {
      SetVisible(mNewVisibilityHACK);
      mUpdateVisibilityHACK = false;
   }
}

bool VaOverlay::AttachViewer(VaViewer* aViewerPtr)
{
   mViewerPtr = aViewerPtr;

   if (mViewerPtr != nullptr)
   {
      UtoViewer* viewerPtr = mViewerPtr->GetViewer();
      if (!viewerPtr)
      {
         return false;
      }
      else
      {
         if (mOverlayType == cOVERLAY_WINDOW)
         {
            if (viewerPtr->GetScreenOverlayManager()->FindOverlay(mName) ==
                viewerPtr->GetScreenOverlayManager()->Overlays().end)
            {
               viewerPtr->GetScreenOverlayManager()->AddOverlay(mName);
            }
         }
         else
         {
            if (viewerPtr->World().FindOverlay(mName) == viewerPtr->World().Overlays().end())
            {
               viewerPtr->World().AddOverlay(mName);
            }
         }
      }
   }
   return Initialize();
}

bool VaOverlay::IsVisible() const
{
   bool visible = false;
   if (mOverlayType == cOVERLAY_WINDOW)
   {
      visible = IsVisibleWindow();
   }
   else
   {
      visible = IsVisibleWorld();
   }
   return visible;
}

void VaOverlay::SetVisible(bool aVisible)
{
   if (mOverlayType == cOVERLAY_WINDOW)
   {
      SetVisibleWindow(aVisible);
   }
   else
   {
      SetVisibleWorld(aVisible);
   }
}

bool VaOverlay::IsVisibleWindow() const
{
   bool visible = false;
   if ((mViewerPtr != nullptr) && (mViewerPtr->GetViewer() != nullptr))
   {
      UtoViewer*                        viewerPtr   = mViewerPtr->GetViewer();
      UtoScreenOverlayManager::iterator overlayIter = viewerPtr->GetScreenOverlayManager()->FindOverlay(mName);
      if (overlayIter != viewerPtr->GetScreenOverlayManager()->Overlays().end)
      {
         visible = overlayIter->IsVisible();
      }
   }
   return visible;
}

void VaOverlay::SetVisibleWindow(bool aVisible)
{
   if ((mViewerPtr != nullptr) && (mViewerPtr->GetViewer() != nullptr))
   {
      UtoViewer*                        viewerPtr   = mViewerPtr->GetViewer();
      UtoScreenOverlayManager::iterator overlayIter = viewerPtr->GetScreenOverlayManager()->FindOverlay(mName);
      if (overlayIter != viewerPtr->GetScreenOverlayManager()->Overlays().end)
      {
         // TODO - LMM - This is a hack. For some reason when calling this method
         // from a window's handle method (not including the VaViewer's handle method)
         // the visibility isn't maintained. These two 'HACK' variables are accessed
         // in the VaOverlay's 'Update' method, which resets the visibility.
         mUpdateVisibilityHACK = true;
         mNewVisibilityHACK    = aVisible;
         overlayIter->Show(aVisible);
      }
   }
}

bool VaOverlay::IsVisibleWorld() const
{
   bool visible = false;
   if ((mViewerPtr != nullptr) && (mViewerPtr->GetViewer() != nullptr))
   {
      UtoViewer*                 viewerPtr   = mViewerPtr->GetViewer();
      UtoWorld::overlay_iterator overlayIter = viewerPtr->World().FindOverlay(mName);
      if (overlayIter != viewerPtr->World().Overlays().end())
      {
         visible = overlayIter->overlay->IsVisible();
      }
   }
   return visible;
}

void VaOverlay::SetVisibleWorld(bool aVisible)
{
   if ((mViewerPtr != nullptr) && (mViewerPtr->GetViewer() != nullptr))
   {
      UtoViewer*                 viewerPtr   = mViewerPtr->GetViewer();
      UtoWorld::overlay_iterator overlayIter = viewerPtr->World().FindOverlay(mName);
      if (overlayIter != viewerPtr->World().Overlays().end())
      {
         // TODO - LMM - This is a hack. For some reason when calling this method
         // from a window's handle method (not including the VaViewer's handle method)
         // the visibility isn't maintained. These two 'HACK' variables are accessed
         // in the VaOverlay's 'Update' method, which resets the visibility.
         mUpdateVisibilityHACK = true;
         mNewVisibilityHACK    = aVisible;
         overlayIter->overlay->Show(aVisible);
      }
   }
}

UtoShape* VaOverlay::FindShape(const std::string& aName) const
{
   UtoShape* shapePtr = nullptr;
   if (mOverlayType == cOVERLAY_WINDOW)
   {
      shapePtr = FindShapeWindow(aName);
   }
   else
   {
      shapePtr = FindShapeWorld(aName);
   }
   return shapePtr;
}

UtoShape* VaOverlay::BindShape(const std::string& aName, const UtoShape& aShape)
{
   UtoShape* shapePtr = nullptr;
   if (mOverlayType == cOVERLAY_WINDOW)
   {
      shapePtr = BindShapeWindow(aName, aShape);
   }
   else
   {
      shapePtr = BindShapeWorld(aName, aShape);
   }
   return shapePtr;
}

void VaOverlay::UnbindShape(const std::string& aName)
{
   if (mOverlayType == cOVERLAY_WINDOW)
   {
      UnbindShapeWindow(aName);
   }
   else
   {
      UnbindShapeWorld(aName);
   }
}

UtoShape* VaOverlay::FindShapeWindow(const std::string& aName) const
{
   if ((mViewerPtr != nullptr) && (mViewerPtr->GetViewer() != nullptr))
   {
      UtoViewer*                        viewerPtr   = mViewerPtr->GetViewer();
      UtoScreenOverlayManager::iterator overlayIter = viewerPtr->GetScreenOverlayManager()->FindOverlay(mName);
      if (overlayIter != viewerPtr->GetScreenOverlayManager()->Overlays().end)
      {
         return overlayIter->Lookup(aName);
      }
   }
   return nullptr;
}

UtoShape* VaOverlay::BindShapeWindow(const std::string& aName, const UtoShape& aShape)
{
   if ((mViewerPtr != nullptr) && (mViewerPtr->GetViewer() != nullptr))
   {
      UtoViewer*                        viewerPtr   = mViewerPtr->GetViewer();
      UtoScreenOverlayManager::iterator overlayIter = viewerPtr->GetScreenOverlayManager()->FindOverlay(mName);

      if (overlayIter != viewerPtr->GetScreenOverlayManager()->Overlays().end)
      {
         return overlayIter->Bind(aName, aShape);
      }
   }
   return nullptr;
}

void VaOverlay::UnbindShapeWindow(const std::string& aName)
{
   if ((mViewerPtr != nullptr) && (mViewerPtr->GetViewer() != nullptr))
   {
      UtoViewer*                        viewerPtr   = mViewerPtr->GetViewer();
      UtoScreenOverlayManager::iterator overlayIter = viewerPtr->GetScreenOverlayManager()->FindOverlay(mName);
      if (overlayIter != viewerPtr->GetScreenOverlayManager()->Overlays().end)
      {
         overlayIter->Unbind(aName);
      }
   }
}

UtoShape* VaOverlay::FindShapeWorld(const std::string& aName) const
{
   if ((mViewerPtr != nullptr) && (mViewerPtr->GetViewer() != nullptr))
   {
      UtoViewer*                 viewerPtr   = mViewerPtr->GetViewer();
      UtoWorld::overlay_iterator overlayIter = viewerPtr->World().FindOverlay(mName);
      if (overlayIter != viewerPtr->World().Overlays().end())
      {
         return overlayIter->overlay->Lookup(aName);
      }
   }
   return nullptr;
}

UtoShape* VaOverlay::BindShapeWorld(const std::string& aName, const UtoShape& aShape)
{
   if ((mViewerPtr != nullptr) && (mViewerPtr->GetViewer() != nullptr))
   {
      UtoViewer*                 viewerPtr   = mViewerPtr->GetViewer();
      UtoWorld::overlay_iterator overlayIter = viewerPtr->World().FindOverlay(mName);

      if (overlayIter != viewerPtr->World().Overlays().end())
      {
         return overlayIter->overlay->Bind(aName, aShape);
      }
   }
   return nullptr;
}

void VaOverlay::UnbindShapeWorld(const std::string& aName)
{
   if ((mViewerPtr != nullptr) && (mViewerPtr->GetViewer() != nullptr))
   {
      UtoViewer*                 viewerPtr   = mViewerPtr->GetViewer();
      UtoWorld::overlay_iterator overlayIter = viewerPtr->World().FindOverlay(mName);
      if (overlayIter != viewerPtr->World().Overlays().end())
      {
         overlayIter->overlay->Unbind(aName);
      }
   }
}
} // end namespace vespa
