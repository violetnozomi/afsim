// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtoScreenOverlayManager.hpp"

#include <climits>

#include <UtoOverlay.hpp>
#include <UtoViewer.hpp>
#include <osg/Projection>
#include <osg/Switch>


/////////////////////////////////////////////////////////////////////////////

// {secret}
struct UtoScreenOverlayManagerIter_i
{
   typedef UtoOverlayManager::OverlayList::iterator iterator;
   iterator                                         m_Iter;
   UtoScreenOverlayManagerIter_i() {}
   UtoScreenOverlayManagerIter_i(iterator i)
      : m_Iter(i)
   {
   }
};

/////////////////////////////////////////////////////////////////////////////

UtoScreenOverlayManagerIteratorPolicies::UtoScreenOverlayManagerIteratorPolicies()
   : m_pImpl(new UtoScreenOverlayManagerIter_i)
{
}

UtoScreenOverlayManagerIteratorPolicies::UtoScreenOverlayManagerIteratorPolicies(impl_ptr imp)
   : m_pImpl(imp)
{
}

UtoScreenOverlayManagerIteratorPolicies::UtoScreenOverlayManagerIteratorPolicies(
   const UtoScreenOverlayManagerIteratorPolicies& rhs)
   : m_pImpl(new UtoScreenOverlayManagerIter_i(*rhs.m_pImpl))
{
}

UtoScreenOverlayManagerIteratorPolicies::~UtoScreenOverlayManagerIteratorPolicies()
{
   delete m_pImpl;
}

UtoScreenOverlayManagerIteratorPolicies::value_type* UtoScreenOverlayManagerIteratorPolicies::ptr()
{
   return m_pImpl->m_Iter->overlay;
}

void UtoScreenOverlayManagerIteratorPolicies::forward()
{
   ++m_pImpl->m_Iter;
}

bool UtoScreenOverlayManagerIteratorPolicies::is_equal(const UtoScreenOverlayManagerIteratorPolicies& rhs) const
{
   return m_pImpl->m_Iter == rhs.m_pImpl->m_Iter;
}

void UtoScreenOverlayManagerIteratorPolicies::swap(UtoScreenOverlayManagerIteratorPolicies& rhs)
{
   UtoScreenOverlayManagerIter_i* temp = rhs.m_pImpl;
   rhs.m_pImpl                         = m_pImpl;
   m_pImpl                             = temp;
}

UtoScreenOverlayManagerIteratorPolicies::impl_ptr UtoScreenOverlayManagerIteratorPolicies::GetImpl()
{
   return m_pImpl;
}


UtoScreenOverlayManager::UtoScreenOverlayManager(const UtoSize& aSize, UtoViewer* aViewerPtr)
   : mViewerPtr(aViewerPtr)
   , mTransform(new osg::MatrixTransform)
   , mProjection(new osg::Projection)
   , mSwitch(new osg::Switch)
{
   SetSize(aSize);
   mTransform->addChild(mProjection.get());
   mTransform->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
   mTransform->setName("Window Overlay");
   mProjection->addChild(mSwitch.get());

   // Turn off lighting and fog for window overlays
   osg::StateSet* stateset = mSwitch->getOrCreateStateSet();
   stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   stateset->setMode(GL_FOG, osg::StateAttribute::OFF);
   // Window overlays are rendered with the depth buffer off allowing the
   // user to pick the render order.
   stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

   // Window overlays need to be on top and the last rendering bin
   // is as close to that as we get.
   stateset->setRenderBinDetails(INT_MAX - 1, "RenderBin", osg::StateSet::OVERRIDE_RENDERBIN_DETAILS);
}

void UtoScreenOverlayManager::SetSize(const UtoSize& size)
{
   mProjection->setMatrix(osg::Matrix::ortho(0, size.cx, 0, size.cy, -10, 10));
}

/* UTO Function Block **********************************************************

Function: AddOverlay

Description:
Adds an overlay to the overlay scene
- an overlay is created using the OverlayManager
- the root of that overlay is returned using the GetTree function on
the overlay
- the root node is added to the overlay switch

*******************************************************************************/

UtoScreenOverlayManager::iterator UtoScreenOverlayManager::AddOverlay(const std::string& name)
{
   UtoOverlayManager::OverlayList::iterator OverlayIt;
   UtoOverlayManager::IteratorPair          OverlayItPair = mOverlays.Overlays();

   // Call the base class and store the iterator
   OverlayIt = mOverlays.AddOverlay(name, mViewerPtr->GetWorld());

   if (OverlayIt != OverlayItPair.second)
   {
      UtoOverlay* NewOverlay = dynamic_cast<UtoOverlay*>(OverlayIt->overlay);

      // Add the overlay to the scene
      mSwitch->addChild(NewOverlay->GetTree());
   }

   return iterator(new UtoScreenOverlayManagerIter_i(OverlayIt));
}

/* UTO Function Block **********************************************************

Function: InsertOverlay

Description:
Inserts the overlay at the given position

*******************************************************************************/

UtoScreenOverlayManager::iterator UtoScreenOverlayManager::InsertOverlay(const std::string& name, iterator it)
{
   UtoOverlayManager::OverlayList::iterator OverlayIt;
   UtoOverlayManager::IteratorPair          OverlayItPair = mOverlays.Overlays();

   // Call the base class and store the iterator
   UtoScreenOverlayManagerIter_i::iterator beforeIterator = it.GetImpl()->m_Iter;

   // determine if the overlay was the last iterator
   bool bEnd = mOverlays.Overlays().second->overlay == beforeIterator->overlay;

   // insert the overlay in the list of overlays
   OverlayIt = mOverlays.InsertOverlay(name, beforeIterator, mViewerPtr->GetWorld());

   if (OverlayIt != OverlayItPair.second)
   {
      // obtain the new overlay
      UtoOverlay* NewOverlay = dynamic_cast<UtoOverlay*>(OverlayIt->overlay);

      // if the overlay was the end iterator, we cannot get the tree from an invalid pointer
      if (!bEnd)
      {
         // valid iterator
         // obtain the root node for the iterator
         osg::Group* pRoot = dynamic_cast<UtoOverlay*>(beforeIterator->overlay)->GetTree();
         // insert the root into the tree
         mSwitch->insertChild(mSwitch->getChildIndex(pRoot), NewOverlay->GetTree());
      }
      else
      {
         // invalid iterator
         mSwitch->addChild(NewOverlay->GetTree());
      }
   }

   return iterator(new UtoScreenOverlayManagerIter_i(OverlayIt));
}

/* UTO Function Block **********************************************************

Function: RemoveOverlay

Description:
Removes an overlay

*******************************************************************************/

void UtoScreenOverlayManager::RemoveOverlay(iterator remove)
{
   UtoOverlay* Overlay = dynamic_cast<UtoOverlay*>(remove.GetImpl()->m_Iter->overlay);
   mSwitch->removeChild(Overlay->GetTree());

   // Call the base class
   mOverlays.RemoveOverlay(remove.GetImpl()->m_Iter);
}

/* UTO Function Block **********************************************************

Function: RemoveAllOverlays

Description:
Removes all the overalys from the scene

*******************************************************************************/

void UtoScreenOverlayManager::RemoveAllOverlays()
{
   UtoOverlay* Overlay = nullptr;

   // Get the list of overlays
   UtoOverlayManager::OverlayList::iterator it;
   UtoOverlayManager::IteratorPair          OverlayItPair = mOverlays.Overlays();

   for (it = OverlayItPair.first; it != OverlayItPair.second; ++it)
   {
      Overlay = dynamic_cast<UtoOverlay*>(it->overlay);
      mSwitch->removeChild(Overlay->GetTree());
   }

   // Remove the overlay from the overlay manager
   mOverlays.RemoveAllOverlays();
}

/* UTO Function Block **********************************************************

Function: FindOverlay

Description:
Returns the given overlay

*******************************************************************************/

UtoScreenOverlayManager::iterator UtoScreenOverlayManager::FindOverlay(const std::string& name)
{
   return iterator(new UtoScreenOverlayManagerIter_i(mOverlays.FindOverlay(name)));
}

/* UTO Function Block **********************************************************

Function: Overlays

Description:
Returns the beginning and the end of the overlay list

*******************************************************************************/

UtoScreenOverlayManager::iterator_pair UtoScreenOverlayManager::Overlays()
{
   UtoOverlayManager::IteratorPair overlays = mOverlays.Overlays();
   return iterator_pair(new UtoScreenOverlayManagerIter_i(overlays.first),
                        new UtoScreenOverlayManagerIter_i(overlays.second));
}

/* UTO Function Block **********************************************************

Function: GetOverlayName

Description:
Returns the name of an overlay from an iterator

*******************************************************************************/

std::string UtoScreenOverlayManager::FindName(iterator it)
{
   return mOverlays.FindName(it.GetImpl()->m_Iter);
}

/* UTO Function Block **********************************************************

Function: BringOverlayToFront

Description:
Resort overlays so that this one is first

*******************************************************************************/

void UtoScreenOverlayManager::BringOverlayToFront(iterator it)
{
   UtoOverlay* Overlay = dynamic_cast<UtoOverlay*>(it.GetImpl()->m_Iter->overlay);
   mSwitch->removeChild(Overlay->GetTree());
   mSwitch->insertChild(mSwitch->getNumChildren(), Overlay->GetTree()); // make it the last drawn
   mOverlays.BringOverlayToFront(it.GetImpl()->m_Iter);
   // overlays remove it, push to back
}
