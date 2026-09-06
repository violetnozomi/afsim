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

#include "VaAttachment.hpp"

#include <sstream>

#include "UtoEntity.hpp"
#include "UtoHitInfo.hpp"
#include "UtoShape.hpp"
#include "UtoViewer.hpp"
//#include "UtoSymbol.hpp"

#include "VaBoundingBox.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"
#include "VaScenario.hpp"
#include "VaViewer.hpp"
#include "VaViewerManager.hpp"

namespace vespa
{
//! Method: VaAttachment
//! Description: Attachment are things that attach to VaEntity.  These may be visuals (models, zones, etc.), sounds, or data.
//!              This constructor will build an attachment, and make the relationship with the passed parent.
//!
//! @param aParent - The parent entity.
//! @param aViewerPtr - The viewer in which this attachment will be visible. If NULL is passed
//!                     then the attachment will be visible in all viewers.

// protected
VaAttachment::VaAttachment(VaEntity& aParent, VaViewer* aViewerPtr, const std::string& aName)
   : VaObject()
   , mInitialized(false)
   , mLoaded(false)
   , mCallbacks()
   , mViewerPtr(aViewerPtr)
   , mShapeMap()
   , mParent(aParent)
   , mStateVisibility(true)
   , mShared(false)
   , mIsProxy(false)
{
   SetName(aName);
   SetType<VaAttachment>();
}

//! Method: ~VaAttachment
//! Description: VaAttachment deconstructor.
//!

// public
VaAttachment::~VaAttachment()
{
   // Clear the callbacks so that none are called during deletion.
   mCallbacks.Clear();

   // Collect the shape names.
   std::vector<std::string> shapeNames;
   for (auto&& it : mShapeMap)
   {
      shapeNames.push_back(it.first);
   }

   // Remove the shapes.
   for (auto&& it : shapeNames)
   {
      RemoveShapeFromParent(it);
   }
}

// virtual
//! Method: Initialize
//! Description: This method will perform any initialization that is dependent on the state of the parent.
//!

// public
bool VaAttachment::Initialize()
{
   // Only initialize if the attachment hasn't already been initialized
   // and the parent entity is initialized
   if (!mInitialized && mParent.IsInitialized())
   {
      bool initialized = PrivateInitialize();
      PrivateInitializeCallbacks();
      mInitialized = initialized; // mInitialized is intentionally set here.
   }
   return mInitialized;
}

//! Load the attachment into the provided viewer. If the viewer is null, then
//! the attachment is loaded into all viewers.
//!
//! @return void
// public
void VaAttachment::Load()
{
   // Only load if the parent entity is initialized.
   if (mParent.IsInitialized())
   {
      PrivateLoad();

      // Apply initial visibility
      UpdateVisibility();

      mLoaded = true;
   }
}

bool VaAttachment::IsProxy() const
{
   return mIsProxy;
}


//! Returns a bool indicating whether the attachment is visible. This is a combination
//! of the attachment's state visibility flag, the parent's visibility, and the attachment's
//! viewer visibility.
//! @return Returns true if the attachment is considered visible.
// public
bool VaAttachment::IsVisible() const
{
   // Visibility is based on state visibility and parent's visibility.
   if (mViewerPtr != nullptr)
   {
      bool visible = mStateVisibility && GetParent().IsVisible(mViewerPtr);
      return visible;
   }
   else
   {
      return mStateVisibility && GetParent().IsVisible(VaEnvironment::Instance().GetFirstViewer());
   }
}

//! Sets the attachment's state visibility flag. This flag helps to determine whether the attachment
//! is considered visible (see VaAttachment::IsVisible for details).
//! @param aStateVisibility The visibility flag.
// public
void VaAttachment::SetStateVisibility(bool aStateVisibility)
{
   if (mStateVisibility != aStateVisibility)
   {
      mStateVisibility = aStateVisibility;
      UpdateVisibility();
   }
}

//! Shows or hides the attachment depending on the attachment's visibility.
// public
void VaAttachment::UpdateVisibility()
{
   bool visible = IsVisible();
   for (auto&& it : mShapeMap)
   {
      it.second->SetAllowUpdates(visible);
      ShowShape(it.first, visible);
   }
}

// virtual
//! Method: GetBoundingBox
//! Description: Returns the bounding box for this attachment.
void VaAttachment::GetBoundingBox(VaBoundingBox& aBoundingBox) const
{
   UtVec3d center(mParent.GetPosition().GetLat(), mParent.GetPosition().GetLon(), mParent.GetPosition().GetAlt());
   aBoundingBox = VaBoundingBox(center, center);
}

void VaAttachment::GetShapes(ShapeList& aShapes) const
{
   // Copy each of the shape names from the shape map.
   for (auto&& it : mShapeMap)
   {
      aShapes.push_back(it.first);
   }
}

//! Called when a subpart of the attachment is to be highlighted
//! Default implementation calls HighlightShapes() on the viewer
//! with all selected shapes.
void VaAttachment::HighlightSubpart(VaViewer*                aViewerPtr,
                                    const VaAttachmentSubId& aSubpartId,
                                    bool                     aHighlight,
                                    const UtColor&           aColor)
{
   VaViewerManager::Viewers viewers;

   if (IsMyViewer(aViewerPtr))
   {
      /*
      if aViewerPtr is non-NULL (ie matches a specific viewer), and
         if aViewerPtr matches mViewerPtr, then
            highlight in just mViewerPtr
      */
      viewers.push_back(aViewerPtr);
   }
   else if (aViewerPtr == nullptr)
   {
      /*
      if aViewerPtr is NULL (ie matches all viewers), then
      highlight the attachment in *all viewers in which it is visible*
         - if mViewerPtr is NULL, then highlight in all viewers
         - if mViewerPtr is non-NULL, then highlight in just mViewerPtr
      */
      if (mViewerPtr)
      {
         viewers.push_back(mViewerPtr);
      }
      else
      {
         viewers = VaEnvironment::Instance().GetViewerManager()->GetViewers();
      }
   }


   ShapeList shapes;
   if (!viewers.empty())
   {
      GetShapesForSubpart(aSubpartId, shapes);
   }
   for (auto&& it : viewers)
   {
      it->HighlightShapes(&GetParent(), shapes, aHighlight, aColor);
   }
}

//! Method: AddShapeToParent
//! Description: This will add a visual UTO shape to a parent entity.
//!
//! @param aShapeName - The name to apply to the shape.
//! @param aShape - A prototype shape to use to build the attached shape.
//! @return - The shape that is attached to the parent.  0 on failure.

// protected
UtoShape* VaAttachment::AddShapeToParent(const std::string& aShapeName, UtoShape& aShape, const std::string& aAdditionalTag)
{
   if (mViewerPtr)
   {
      aShape.SetViewerId(mViewerPtr->GetUniqueId());
   }
   else
   {
      aShape.SetViewerId(0);
   }
   UtoShape*  shapePtr  = nullptr;
   UtoEntity* entityPtr = mParent.GetUtoEntity();
   if ((mParent.IsInitialized()) && (entityPtr != nullptr))
   {
      shapePtr = FindShape(aShapeName);
      if (shapePtr == nullptr)
      {
         shapePtr = entityPtr->Bind(aShapeName, aShape);
         if (shapePtr)
         {
            if (aShape.AuxData().empty())
            {
               std::ostringstream oss;
               if (!aAdditionalTag.empty())
               {
                  oss << mParent.GetUniqueIdString() << " " << GetUniqueIdString() << " " << aAdditionalTag;
               }
               else
               {
                  oss << mParent.GetUniqueIdString() << " " << GetUniqueIdString();
               }
               shapePtr->SetAuxData(oss.str().c_str());
            }
            mShapeMap[aShapeName] = shapePtr;
         }
      }
   }
   return shapePtr;
}

//! Method: RemoveShapeFromParent
//! Description: Removes a shape from the parent entity, and deletes the shape.
//!
//! @param aShapeName - The name of the shape to remove.

// protected
void VaAttachment::RemoveShapeFromParent(const std::string& aShapeName)
{
   UtoEntity* entityPtr = mParent.GetUtoEntity();
   if (mParent.IsInitialized() && (entityPtr != nullptr))
   {
      if (mShapeMap.find(aShapeName) != mShapeMap.end())
      {
         entityPtr->UnbindShape(aShapeName);
         mShapeMap.erase(aShapeName);
      }
   }
}

void VaAttachment::RemoveShapeFromParent(UtoShape* aShapePtr)
{
   RemoveShapeFromParent(aShapePtr->GetName());
}

bool VaAttachment::ShowShape(const std::string& aShapeName, bool aShow)
{
   bool showShape = false;
   if (GetParent().IsInitialized())
   {
      VaViewerManager::Viewers viewers;
      if (mViewerPtr != nullptr)
      {
         viewers.push_back(mViewerPtr);
      }
      else
      {
         viewers = VaEnvironment::Instance().GetViewerManager()->GetViewers();
      }

      for (auto&& it : viewers)
      {
         UtoViewer* viewerPtr = it->GetViewer();
         if (viewerPtr != nullptr)
         {
            GetParent().GetUtoEntity()->Show(aShow, aShapeName, viewerPtr);
            showShape = aShow;
         }
      }
   }
   return showShape;
}

//! Method: FindShape
//! Description: Finds a shape associated with this attachment by name.
//!
//! @param aShapeName - The name of the shape to find.
//! @return - The shape found with the given name.  Returns 0 on failure.

// protected
UtoShape* VaAttachment::FindShape(const std::string& aShapeName)
{
   UtoShape*          shapePtr = nullptr;
   ShapeMap::iterator iter     = mShapeMap.find(aShapeName);
   if (iter != mShapeMap.end())
   {
      shapePtr = iter->second;
   }
   return shapePtr;
}

// virtual
std::string VaAttachment::GetMenuSelectionString(const VaAttachmentSubId& aSubId) const
{
   std::ostringstream oss;
   oss << "(" + GetType() + ")->" + GetParent().GetName() + ":" + GetName();
   return oss.str().c_str();
}

// virtual
void VaAttachment::SetSelected(VaViewer* aViewerPtr, bool aSelected, const VaAttachmentSubId& aSubId, bool aForce)
{
   if ((GetSelected(aViewerPtr) != aSelected) || aForce)
   {
      mSelected[aViewerPtr->GetUniqueId()] = aSelected;
      if (GetParent().GetScenario())
      {
         AttachmentSelect(aViewerPtr, (aSelected ? 1 : 0), aSubId);
      }
   }
}

// virtual
VaAttachment::DeleteAction VaAttachment::DeleteSelection(VaViewer* aViewerPtr)
{
   if (mIsProxy)
   {
      return cDELETE_ENTITY;
   }
   else
   {
      return cDELETE_ATTACHMENT;
   }
}

void VaAttachment::LoadAttachment(VaAttachment& aAttachment)
{
   // Initialize the attachment.
   if (aAttachment.GetParent().IsInitialized())
   {
      aAttachment.Initialize();

      // Load the attachment.
      if (aAttachment.GetParent().IsLoaded())
      {
         aAttachment.Load();
      }
   }
}

// Implement this to explicitly determine the sub part ID's included in the ray hit
// This allows a single shape to contain multiple sub-part ID's
void VaAttachment::HitSubparts(UtoHitInfoList& aHits, vespa::VaHitList& aSubHits)
{
   // default behavior uses AuxData of shape to determine attachment subID
   int entityUID(0), attachmentUID(0), additionalInfo(0);
   if (aHits.mShapePtr)
   {
      std::string        entityUIDStr = aHits.mShapePtr->AuxData();
      std::istringstream iss(entityUIDStr.c_str());
      iss >> entityUID;
      iss >> attachmentUID;
      iss >> additionalInfo;

      aSubHits.push_back(VaHitEntry::FromAttachment(*this, additionalInfo));
   }
}

void VaAttachment::AttachmentSelect(VaViewer* aViewerPtr, int aSelect, const VaAttachmentSubId& aSubId)
{
   if (!IsMyViewer(aViewerPtr))
   {
      return;
   }
   if (aViewerPtr == nullptr)
   {
      VaViewerManager::Viewers v = VaEnvironment::Instance().GetViewerManager()->GetViewers();
      for (auto&& it : v)
      {
         it->AttachmentSelect(aSelect, this, aSubId);
      }
   }
   else
   {
      aViewerPtr->AttachmentSelect(aSelect, this, aSubId);
   }
}

// This method is called by the viewer to determine which parts of the attachment are considered hit
// The default implementation will call HitSubparts for each shape involved, reporting all hits.
// @param aShapesHit  The list of hit information for all shapes bound to this attachment
// @param aSubHits    Hits to be reported should be appended to aSubHits
void VaAttachment::HitMultipleSubparts(std::vector<UtoHitInfoList>& aShapesHit, vespa::VaHitList& aSubHits)
{
   vespa::VaHitList reportedHits;
   for (size_t i = 0; i < aShapesHit.size(); ++i)
   {
      HitSubparts(aShapesHit[i], reportedHits);
   }

   // if more than one subpart is hit, don't report the attachment as a whole
   if (reportedHits.size() > 1)
   {
      for (vespa::VaHitList::iterator i = reportedHits.begin(); i != reportedHits.end();)
      {
         if (i->SubId() == 0)
         {
            reportedHits.erase(i++);
         }
         else
         {
            ++i;
         }
      }
      // if we get through that and have nothing, it indicates that all shapes had a subpart of 0
      // we probably still want a selection, so grab the first one.
      if (reportedHits.empty())
      {
         HitSubparts(aShapesHit[0], reportedHits);
      }
   }

   aSubHits.splice(aSubHits.end(), reportedHits);
}

bool VaAttachment::Pick(VaViewer*                aViewerPtr,
                        int                      aMouseX,
                        int                      aMouseY,
                        bool                     aToggle,
                        bool                     aDoubleClick,
                        const VaAttachmentSubId& aAttachmentSubPartId)
{
   return false;
}

bool VaAttachment::BoxPick(VaViewer* aViewerPtr, const VaSelectionBox& aBox, bool aInside)
{
   return false;
}

bool VaAttachment::IsMyViewer(VaViewer* aViewerPtr)
{
   if ((mViewerPtr == nullptr) || (aViewerPtr == mViewerPtr))
   {
      return true;
   }
   return false;
}

bool VaAttachment::GetSelected(VaViewer* aViewerPtr) const
{
   if (aViewerPtr != nullptr)
   {
      std::map<int, bool>::const_iterator it = mSelected.find(aViewerPtr->GetUniqueId());
      if (it != mSelected.end())
      {
         return it->second;
      }
   }
   return false;
}

void VaAttachment::AddToCallbackHolder(VaCallback* aCallback)
{
   mCallbacks.Add(aCallback);
}

bool VaAttachment::IsModifiable()
{
   return GetParent().IsModifiable();
}

bool VaAttachment::ShowUnmodifiable()
{
   return !(IsModifiable());
}

const vespa::VaPosition& VaAttachment::GetPosition(unsigned int aSubId) const
{
   return GetParent().GetPosition();
}

} // end namespace vespa.
