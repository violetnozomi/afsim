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

#include "VaEntity.hpp"

#include <algorithm>
#include <iterator>

#include <osg/Matrixd>
#include <osg/Vec4>

#include "UtEntity.hpp"
#include "UtMath.hpp"
#include "UtQuaternion.hpp"
#include "UtSphericalEarth.hpp"
#include "UtoEntity.hpp"
#include "UtoEntityDB.hpp"
#include "UtoRenderBinManager.hpp"
#include "UtoUtility.hpp"
#include "VaAttachment.hpp"
#include "VaAttachmentZoneCircle.hpp"
#include "VaAttachmentZoneEllipse.hpp"
#include "VaAttachmentZonePolygon.hpp"
#include "VaBoundingBox.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"
#include "VaScenario.hpp"
#include "VaSelectionBox.hpp"
#include "VaUtils.hpp"
#include "VaViewer.hpp"
#include "VaViewerManager.hpp"


namespace vespa
{
unsigned int VaEntity::sId = 0;

//! Method: VaEntity
//! Description: The constructor for VaEntity. VaEntity is the base class for all
//!              geographically positioned objects in the VESPA API
//!

// protected
VaEntity::VaEntity(const std::string& aName)
   : VaObject()
   , VaPosition()
   , mCallbacksLoaded(false)
   , mCallbacks()
   , mHeading(0.0)
   , mPitch(0.0)
   , mRoll(0.0)
   , mVelocityWCS()
   , mSelected()
   , mScenarioPtr(nullptr)
   , mScenarioKey()
   , mEntityPtr(nullptr)
   , mInitialized(false)
   , mLoaded(false)
   , mGlobalHiddenMask(0x0000)
   , mGlobalVisibilityContributions(ScenarioStateHidden)
   , mAttachments()
   , mNameToAttachmentMap()
   , mTypeAttachmentSet()
   , mForceChange(false)
   , mSupportsUndo(true)
   , mInUndoStack(false)
   , mDraggable(true)
   , mSimpleDraggable(false)
   , mDeleting(false)
   , mStartTime(0.0)
   , mEndTime(0.0)
   , mTimeRangeDirty(true)
{
   UtoEntityDB::iterator iter = UtoEntityDB::Instance("").Add();
   mEntityPtr                 = iter->second;

   SetType<VaEntity>();

   std::string name = aName;
   if (name.empty())
   {
      name = CreateName();
   }
   SetName(name);
}

//! ~VaEntity
//! Description: The VaEntity destructor.

// public
VaEntity::~VaEntity()
{
   // Clear the callbacks, so that none are called while the entity is being deleted.
   mCallbacks.Clear();

   // Delete the attachments.
   Attachments tempAttachments = std::move(mAttachments);
   mAttachments                = Attachments();
   mNameToAttachmentMap.clear();
   mTypeAttachmentSet.clear();
   for (auto& iter : tempAttachments)
   {
      VaObserver::AttachmentDeleted(iter.get(), 0);
      iter = nullptr;
   }

   // Remove the entity from uto.
   if (mEntityPtr != nullptr)
   {
      UtoEntityDB::Instance("").Remove(*mEntityPtr);
   }
}

void VaEntity::SetScenario(VaScenario* aScenarioPtr)
{
   mScenarioPtr = aScenarioPtr;
   if (mScenarioPtr)
   {
      mScenarioKey = mScenarioPtr->GetKey();
   }
   else
   {
      mScenarioKey = VaScenarioKey();
   }
}

//! Initialize
//! Description: The method loads the entity into the uto entity database.  It also calls load on all attachments.
//!

// public
void VaEntity::Initialize()
{
   if (!mInitialized)
   {
      mInitialized = true;

      mForceChange = true;
      SetPositionOrientation(*this, mHeading, mPitch, mRoll);

      PrivateInitialize();

      PrivateInitializeCallbacks();

      InitializeAttachments();
   }
}

void VaEntity::InitializeAttachments()
{
   // Temporarily move the list because some attachments may create
   // additional attachments that will modify the size of 'mAttachments'.
   // Then re-add the moved attachments back to the original list.
   auto tempAttachments = std::move(mAttachments);
   mAttachments         = Attachments();
   for (const auto& attachment : tempAttachments)
   {
      attachment->Initialize();
   }
   std::move(std::begin(tempAttachments), std::end(tempAttachments), std::back_inserter(mAttachments));
}

//! Indicates whether the entity has been loaded.
//!
//! @return bool Returns true if the entity is loaded
// public
bool VaEntity::IsLoaded() const
{
   return mLoaded;
}

void VaEntity::Load()
{
   PrivateLoad();

   // Mark the entity as loaded.
   mLoaded = true;

   // Temporarily move the list because some attachments may create
   // additional attachments that will modify the size of 'mAttachments'.
   // Then re-add the moved attachments back to the original list.
   auto tempAttachments = std::move(mAttachments);
   mAttachments         = Attachments();
   for (const auto& attachment : tempAttachments)
   {
      attachment->Load();
   }
   std::move(std::begin(tempAttachments), std::end(tempAttachments), std::back_inserter(mAttachments));
}

void VaEntity::Update(double aTime)
{
   for (auto& attachment : mAttachments)
   {
      attachment->Update(aTime);
   }
}

void VaEntity::UpdateFrame(double aTime)
{
   for (auto& it : mAttachmentToBeRemoved)
   {
      RemoveAttachment(it->GetUniqueId());
   }
   mAttachmentToBeRemoved.clear();
   for (auto& attachment : mAttachments)
   {
      attachment->UpdateFrame(aTime);
   }
}

void VaEntity::SetName(const std::string& aName)
{
   if (GetName() != aName)
   {
      std::string oldName(GetName());
      GetUtoEntity()->SetName(aName);
      VaObject::SetName(aName);
      VaObserver::EntityNameChanged(oldName, aName, this);
   }
}

void VaEntity::NameChanged()
{
   for (auto& attachment : mAttachments)
   {
      attachment->NameChanged();
   }
}

void VaEntity::Pick(VaViewer* aViewerPtr, int aMouseX, int aMouseY, bool aToggle, bool aDoubleClick)
{
   bool selected = aToggle ? !GetSelected(aViewerPtr) : true;
   SetSelected(aViewerPtr, selected);
}

bool VaEntity::BoxPick(VaViewer* aViewerPtr, const VaSelectionBox& aBox, bool aInside)
{
   bool picked = false;

   const VaPosition& pos(GetPosition());
   if (!aBox.Contains(pos.GetLat(), pos.GetLon(), pos.GetAlt()) ^ aInside)
   {
      SetSelected(aViewerPtr, true);
      picked = true;
   }
   return picked;
}

void VaEntity::ClearSelection()
{
   SetSelected(nullptr, false);

   for (auto& attachment : mAttachments)
   {
      const auto& viewers = VaEnvironment::Instance().GetViewerManager()->GetViewers();
      for (auto& viewer : viewers)
      {
         attachment->ClearSelection(viewer);
      }
   }
}

bool VaEntity::GetSelected(VaViewer* aViewerPtr) const
{
   if (aViewerPtr)
   {
      auto it = mSelected.find(aViewerPtr->GetUniqueId());
      if (it != mSelected.end())
      {
         return it->second;
      }
   }
   return false;
}

void VaEntity::SetSelected(VaViewer* aViewerPtr, bool aSelected)
{
   if (aViewerPtr == nullptr)
   {
      const auto& viewers = VaEnvironment::Instance().GetViewerManager()->GetViewers();
      for (auto& viewer : viewers)
      {
         SetSelected(viewer, aSelected);
      }
   }
   else if (mSelected[aViewerPtr->GetUniqueId()] != aSelected)
   {
      // Prevent invisible entities and their attachments from being selected
      bool visible                         = IsVisible(aViewerPtr);
      mSelected[aViewerPtr->GetUniqueId()] = aSelected && visible;

      for (auto& attachment : mAttachments)
      {
         if (attachment->IsProxy())
         {
            attachment->SetSelected(aViewerPtr, aSelected && visible, 0);
         }
      }
      VaObserver::EntitySelect[(int)GetScenarioKey()](aViewerPtr, (aSelected ? 1 : 0), this);
   }
}

bool VaEntity::IsInside(const VaSelectionBox& aBox) const
{
   const VaPosition& pos(GetPosition());
   return aBox.Contains(pos.GetLat(), pos.GetLon(), pos.GetAlt());
}

//! Returns a bool indicating whether the entity is visible.
//! @param  aViewerPtr The viewer to test visibility against. If NULL is passed it will return true if the
//!                    entity is visible in any viewer.
//! @return Returns true if the entity is considered visible.
// public
bool VaEntity::IsVisible(VaViewer* aViewerPtr) const
{
   if (aViewerPtr == nullptr)
   {
      return !mGlobalHiddenMask;
   }
   auto it = mHiddenMask.find(aViewerPtr->GetUniqueId());

   unsigned short localMask = 0x0000;
   if (it != mHiddenMask.end())
   {
      localMask = it->second;
   }

   bool visible = !localMask && !mGlobalHiddenMask;

   return visible;
}

//! Sets the entity's visibility flag for the given mask. This flag helps to determine whether the entity
//! is considered visible (see VaEntity::IsVisible for details).
//! @param aVisibility The visibility flag.
//! @param aMask indicates the type of visibility contribution
// public
void VaEntity::SetVisibilityContribution(bool aVisibility, const unsigned short aMask, VaViewer* aViewerPtr)
{
   bool visibilityChanged = (aVisibility != IsVisible(aViewerPtr));
   if (nullptr == aViewerPtr)
   {
      const auto& viewers = VaEnvironment::Instance().GetViewerManager()->GetViewers();
      for (auto& viewer : viewers)
      {
         SetVisibilityContribution(aVisibility, aMask, viewer);
      }
   }
   else
   {
      unsigned short global     = aMask & mGlobalVisibilityContributions;
      unsigned short viewerSpec = aMask & ~mGlobalVisibilityContributions;
      unsigned short maskBefore = mHiddenMask[aViewerPtr->GetUniqueId()] | mGlobalHiddenMask;
      unsigned short maskResult = 0x0000;
      if (0 != global)
      {
         if (aVisibility != GetVisibilityContribution(global, aViewerPtr))
         {
            if (aVisibility)
            {
               mGlobalHiddenMask &= ~global;
            }
            else
            {
               mGlobalHiddenMask |= global;
            }
         }
         maskResult |= mGlobalHiddenMask;
      }
      if (0 != viewerSpec)
      {
         if (mHiddenMask.find(aViewerPtr->GetUniqueId()) == mHiddenMask.end())
         {
            mHiddenMask[aViewerPtr->GetUniqueId()] = 0x0000;
         }
         // TODO_NOT_USED unsigned short maskBefore = mHiddenMask[aViewerPtr->GetUniqueId()];
         if (aVisibility != GetVisibilityContribution(viewerSpec, aViewerPtr))
         {
            if (aVisibility)
            {
               mHiddenMask[aViewerPtr->GetUniqueId()] &= ~viewerSpec;
            }
            else
            {
               mHiddenMask[aViewerPtr->GetUniqueId()] |= viewerSpec;
            }
         }
         maskResult |= mHiddenMask[aViewerPtr->GetUniqueId()];
      }
      if ((maskBefore == 0) != (maskResult == 0))
      {
         UpdateVisibility();
      }
   }

   if (GetSelected(aViewerPtr) && visibilityChanged)
   {
      UpdateAttachmentHighlights(aViewerPtr);
   }
}

bool VaEntity::GetVisibilityContribution(const unsigned short aMask, VaViewer* aViewerPtr) const
{
   assert(nullptr != aViewerPtr);

   unsigned short global      = aMask & mGlobalVisibilityContributions;
   unsigned short viewerSpec  = aMask & ~mGlobalVisibilityContributions;
   bool           globalValue = true;
   bool           viewerValue = true;
   if (0 != global)
   {
      globalValue = ((mGlobalHiddenMask & aMask) == 0);
   }
   if (0 != viewerSpec)
   {
      auto           it         = mHiddenMask.find(aViewerPtr->GetUniqueId());
      unsigned short viewerMask = 0x0000;
      if (it != mHiddenMask.end())
      {
         viewerMask = it->second;
      }
      viewerValue = ((viewerMask & aMask) == 0);
   }
   return (viewerValue && globalValue);
}

//! Called when the entity's visibility has changed
void VaEntity::UpdateVisibility()
{
   for (auto& attachment : mAttachments)
   {
      attachment->UpdateVisibility();
   }
}

void VaEntity::PrivateInitializeCallbacks()
{
   if (!mCallbacksLoaded)
   {
      mCallbacks.Add(VaObserver::ViewerDestroyed.Connect(&VaEntity::ViewerDestroyedCB, this));
      mCallbacksLoaded = true;
   }
}

void VaEntity::GetBoundingBox(VaBoundingBox& aBoundingBox) const
{
   if (mAttachments.empty())
   {
      UtVec3d center(GetPosition().GetLat(), GetPosition().GetLon(), GetPosition().GetAlt());
      aBoundingBox = VaBoundingBox(center, center);
   }
   else
   {
      for (const auto& attachmentPtr : mAttachments)
      {
         if (attachmentPtr != nullptr)
         {
            VaBoundingBox bb;
            attachmentPtr->GetBoundingBox(bb);
            aBoundingBox.Grow(bb);
         }
      }
   }
}

//! Method: AddAttachment
//! Description: Adds the given attachment to the entity.  Attachments are visual, audio, or data objects to add to the entity.
//!
//! @param aAttachmentPtr The attachment to add to the entity.
//! @return Returns true if the attachment was successfully added.

// public
bool VaEntity::AddAttachment(std::unique_ptr<VaAttachment> aAttachmentPtr)
{
   // Make sure the name isn't already taken.
   const int uniqueId = aAttachmentPtr->GetUniqueId();
   if (mNameToAttachmentMap.find(uniqueId) == mNameToAttachmentMap.end())
   {
      // Add the attachment to the attachment list.
      auto attachmentPtr = aAttachmentPtr.get();
      mAttachments.push_back(std::move(aAttachmentPtr));

      // Create an entry in the name map.
      mNameToAttachmentMap[uniqueId] = attachmentPtr;

      // Create an entry in the type map.
      mTypeAttachmentSet.emplace(attachmentPtr->GetType(), attachmentPtr);

      // Add the attachment to the scenario.
      if (mScenarioPtr)
      {
         mScenarioPtr->AttachmentAdded(*attachmentPtr);
      }
      return true;
   }
   return false;
}

bool VaEntity::AddAttachment(VaAttachment* aAttachmentPtr)
{
   return AddAttachment(std::unique_ptr<VaAttachment>(aAttachmentPtr));
}

//! Method: RemoveAttachment
//! Description: This method removes an attachment from an entity.
//!
//! @param aUniqueId - The attachment's name to remove.
//! @return Returns true if the attachment was successfully removed.

// public
bool VaEntity::RemoveAttachment(int aUniqueId, int aSubpartId)
{
   auto nameIter = mNameToAttachmentMap.find(aUniqueId);
   if (nameIter != mNameToAttachmentMap.end())
   {
      VaAttachment* attachmentPtr = nameIter->second;
      attachmentPtr->AttachmentSelect(nullptr, 0, aSubpartId);

      if (aSubpartId != 0)
      {
         VaObserver::AttachmentRemoved(attachmentPtr, aSubpartId);
         VaObserver::AttachmentDeleted(attachmentPtr, aSubpartId);
         // remove the subpart from the selection list
      }
      else
      {
         // Remove the entry from the name map.
         mNameToAttachmentMap.erase(nameIter);

         // Remove the entry from the type map.
         for (const auto& e : attachmentPtr->GetTypeList())
         {
            mTypeAttachmentSet.erase(TypeAttachment(e, attachmentPtr));
         }

         // Remove the entry from the attachment list.
         auto attachIter = find_if(mAttachments.begin(),
                                   mAttachments.end(),
                                   [&](const std::unique_ptr<VaAttachment>& ptr) { return attachmentPtr == ptr.get(); });

         std::unique_ptr<VaAttachment> attachment;
         if (attachIter != mAttachments.end())
         {
            attachment = std::move(*attachIter);
            mAttachments.erase(attachIter);
         }

         // Remove the attachment from the scenario.
         if (mScenarioPtr)
         {
            mScenarioPtr->AttachmentRemoved(*attachmentPtr);
         }

         VaObserver::AttachmentDeleted(attachmentPtr, 0);
         // Attachment gets deleted as the unique_ptr goes out of scope.
      }

      return true;
   }
   return false;
}

//! Method: FindAttachment
//! Description: Returns the attachment with the specified name.
//!
//! @param aUniqueId The unique id of the desired attachment.
//! @return Returns the attachment if successful, otherwise NULL.

// public
VaAttachment* VaEntity::FindAttachment(int aUniqueId) const
{
   VaAttachment* attachmentPtr = nullptr;

   auto nameIter = mNameToAttachmentMap.find(aUniqueId);
   if (nameIter != mNameToAttachmentMap.end())
   {
      attachmentPtr = nameIter->second;
   }
   return attachmentPtr;
}

//! Method: FindAttachment
//! Description: Returns the first attachment with the specified name.  In most cases
//!              the uniqueId version should be used, as it will be faster, and does
//!              not even have to consider name conflicts.
//!
//! @param aName The name of the desired attachment.
//! @return Returns the attachment if successful, otherwise NULL.

// public
VaAttachment* VaEntity::FindAttachment(const std::string& aName) const
{
   for (const auto& attachment : mAttachments)
   {
      if (aName == attachment->GetName())
      {
         return attachment.get();
      }
   }
   return nullptr;
}

void VaEntity::UpdateAttachmentHighlights(VaViewer* aViewerPtr)
{
   // Reselecting the attachment triggers a callback/slot that will handle highlighting
   for (auto& attachment : mAttachments)
   {
      if (attachment->IsProxy())
      {
         attachment->AttachmentSelect(aViewerPtr, true, 0);
      }
   }
}

void VaEntity::MoveRelative(UtEntity&        aRefBefore,
                            UtEntity&        aRefAfter,
                            int              aFeedbackFormat,
                            double           aFeedbackPrecision,
                            MoveRelativeMask aMoveRelativeMask,
                            VaViewer*        aViewerPtr)
{
   if (aMoveRelativeMask & cMR_ENTITY)
   {
      VaPosition::MoveRelative(aRefBefore, aRefAfter, aFeedbackFormat, aFeedbackPrecision);
      double xyz[3];
      GetECEF(xyz);
      mEntityPtr->SetLocationWCS(xyz, false);

      VaObserver::EntityMoved[GetUniqueId()](this, false);
   }

   if (aMoveRelativeMask & cMR_ATTACHMENTS)
   {
      for (auto&& attachment : mAttachments)
      {
         if (attachment->IsA_TypeOf<VaAttachmentZonePolygon>() || attachment->IsA_TypeOf<VaAttachmentZoneEllipse>() ||
             attachment->IsA_TypeOf<VaAttachmentZoneCircle>())
         {
            attachment->MoveRelative(aRefBefore, aRefAfter, aFeedbackFormat, aFeedbackPrecision, aMoveRelativeMask, aViewerPtr);
         }
      }
   }
}


//! Method: GetPosition
//! Description: Returns the position of the entity
//!
//! @return the position of the entity.

// public
const VaPosition& VaEntity::GetPosition() const
{
   return *this;
}

// virtual
//  private
//! Method: SetPosition
//! Description: This method changes the position of the entity.
//!
//! @param aPosition The position to move the entity to.

// private  virtual  virtual
void VaEntity::SetPosition(const VaPosition& aPosition, const bool aMotionInTime /* = false*/)
{
   SetPositionOrientation(aPosition, mHeading, mPitch, mRoll, aMotionInTime);
}

//! Method: GetOrientation
//! Description: This method gets the orientation of the entity from body centered euler angles (radians).
//!
//! @param aHeading The heading (rotation about z, out the bottom) in radians
//! @param aPitch The pitch (rotation about y, out the right) in radians
//! @param aRoll The roll (rotation about x, out the front) in radians

// public
void VaEntity::GetOrientation(double& aHeading, double& aPitch,
                              double& aRoll) const // in radians
{
   aHeading = mHeading;
   aPitch   = mPitch;
   aRoll    = mRoll;
}

// virtual
//! Method: SetOrientation
//! Description: Sets the orientation of the entity using body centered euler angles (radians).
//!
//! @param aHeading The heading (rotation about z, out the bottom) in radians
//! @param aPitch The pitch (rotation about y, out the right) in radians
//! @param aRoll The roll (rotation about x, out the front) in radians
//! @return true if the orientation has changed, false if it is the same as before the call

// public
bool VaEntity::SetOrientation(const double aHeading,
                              const double aPitch,
                              const double aRoll,
                              const bool   aInMotion) // in radians
{
   bool changed = false;
   // Only update if the orientation has changed.
   if ((aHeading != mHeading) || (aPitch != mPitch) || (aRoll != mRoll))
   {
      changed = true;
   }

   if (changed)
   {
      mHeading = aHeading;
      mPitch   = aPitch;
      mRoll    = aRoll;

      mEntityPtr->SetOrientationNED(aHeading, aPitch, aRoll);

      VaObserver::EntityMoved[GetUniqueId()](this, aInMotion);
   }
   return changed;
}

// virtual
//! Method: SetPositionOrientation
//! Description: Sets the position and orientation (body centered euler angles in radians) of the object.  This will
//! update the
//!              position and orientation of the uto (visual) entity.
//!
//! @param aPosition The position to move the entity to.
//! @param aHeading The heading (rotation about z, out the bottom) in radians
//! @param aPitch The pitch (rotation about y, out the right) in radians
//! @param aRoll The roll (rotation about x, out the front) in radians
//! @return true if the orientation or position has changed, false if it is the same as before the call

// public
bool VaEntity::SetPositionOrientation(const VaPosition& aPosition,
                                      const double      aHeading,
                                      const double      aPitch,
                                      const double      aRoll,
                                      const bool        aMotionInTime /*= false*/)
{
   bool changed = false;
   if (mForceChange || (aHeading != mHeading) || (aPitch != mPitch) || (aRoll != mRoll) || ((*this) != aPosition))
   {
      changed      = true;
      mForceChange = false;
   }
   else
   {
      // Some data (such as Ut Formats) may be reflected in a position update that do not
      // involve or require actual movement or callback reference. These still need to be
      // updated
      *((VaPosition*)this) = aPosition;
   }
   if (changed)
   {
      *((VaPosition*)this) = aPosition;
      mHeading             = aHeading;
      mPitch               = aPitch;
      mRoll                = aRoll;

      double xyz[3];
      GetECEF(xyz);
      mEntityPtr->SetLocationWCS(xyz, false);
      mEntityPtr->SetOrientationNED(aHeading, aPitch, aRoll);

      VaObserver::EntityMoved[GetUniqueId()](this, aMotionInTime);
   }

   return changed;
}

void VaEntity::SetPositionOrientation(const double aLocationWCS[3], double aPsi, double aTheta, double aPhi)
{
   mEntityPtr->SetLocationWCS(aLocationWCS);
   mEntityPtr->SetOrientationWCS(aPsi, aTheta, aPhi);

   double lat, lon, alt;
   mEntityPtr->GetLocationLLA(lat, lon, alt);
   VaPosition::SetLLA(lat, lon, alt);

   mEntityPtr->GetOrientationNED(mHeading, mPitch, mRoll);

   VaObserver::EntityMoved[GetUniqueId()](this, true);
}

void VaEntity::SetVelocityWCS(const UtVec3d& aVelocityWCS)
{
   mVelocityWCS = aVelocityWCS;
}

//! Translate the entity by the specified latitude / longitude increment.
//! @note This is intended only for moving the entire scenario and NOT routine movement of entities
//! @param  double aDeltaLat The latitude increment to move the entity.
//! @param  double aDeltaLon The longitude increment to move the entity.
//! @param  bool   aMoveAtt  denotes whether or not to move the entity's attachments
// public
void VaEntity::Translate(double aDeltaLat, double aDeltaLon, VaViewer* aViewerPtr /* = NULL*/)
{
   VaPosition pos(GetPosition());
   pos.Translate(aDeltaLat, aDeltaLon);
   SetPosition(pos);

   // Notify each attachment that their parent entity has been translated.
   for (auto& attachment : mAttachments)
   {
      attachment->Translated(aDeltaLat, aDeltaLon, aViewerPtr);
   }
}

// Rotate the entity around the specified position by the specified angle.
//! @param aCenterPosition The position to rotate around.
//! @param aAngle The angle to rotate by in degrees.
void VaEntity::Rotate(const UtLLAPos& aCenterLLA, double aAngle)
{
   VaPosition pos(GetPosition());

   // Rotate the position.
   pos.Rotate(aCenterLLA, aAngle);


   // Set the entity's new heading.
   double heading = UtMath::NormalizeAngle0_TwoPi(GetHeading() + aAngle * UtMath::cRAD_PER_DEG);
   SetHeading(heading);

   // Set the entity's new position.
   SetPosition(pos);


   // Notify each attachment that their parent entity has been rotated.
   for (auto& attachment : mAttachments)
   {
      attachment->Rotated(aCenterLLA, aAngle);
   }
}

//! Method: GetScreenSpaceCoords
//! Description: Returns the screen space coordinate of the entity.
//!
//! @param aPositionX return position in x direction
//! @param aPositionY return position in y direction
//! @param aViewerPtr the viewer we want the position for

// public
void VaEntity::GetScreenSpaceCoords(int& aPositionX, int& aPositionY, VaViewer* aViewerPtr) const
{
   if (nullptr == aViewerPtr)
   {
      VaViewerManager::Viewers viewerList = VaEnvironment::Instance().GetViewerManager()->GetViewers();
      if (!viewerList.empty())
      {
         return GetScreenSpaceCoords(aPositionX, aPositionY, *(viewerList.begin()));
      }
      else
      {
         aPositionX = -1000;
         aPositionY = -1000;
      }
   }
   else
   {
      double x;
      double y;
      double z;
      if (aViewerPtr->WorldToScreen(*this, x, y, z))
      {
         aPositionX = (int)x;
         aPositionY = (int)y;
      }
      else
      {
         // if the platform is behind the camera we will put the label off screen
         aPositionX = -1000;
         aPositionY = -1000;
      }
   }
}

void VaEntity::ModelToWorld(const double aModelXYZ[3], double aWorldXYZ[3], bool aVector) const
{
   if (aVector)
   {
      mEntityPtr->BodyCF().ConvertECSVectorToWCS(aWorldXYZ, aModelXYZ);
   }
   else
   {
      mEntityPtr->BodyCF().ConvertECSToWCS(aModelXYZ, aWorldXYZ);
   }
}

void VaEntity::WorldToModelPlane(const double aWorldXYZ[3], double aModelXY[2]) const
{
   double heading, foo;
   GetOrientation(heading, foo, foo);
   osg::Vec3 u(GetECEF_X(), GetECEF_Y(), GetECEF_Z());
   u.normalize(); // this is a vector representing 'up' relative to the entity position
   osg::Vec3 e = osg::Vec3(0, 0, 1) ^ u;
   e.normalize();             // this is a vector representing 'east' relative to the entity position
   osg::Vec3 n       = u ^ e; // this is a vector representing 'north' relative to the entity position
   double    unori_x = aWorldXYZ[0] * n.x() + aWorldXYZ[1] * n.y() + aWorldXYZ[2] * n.z();
   double    unori_y = aWorldXYZ[0] * e.x() + aWorldXYZ[1] * e.y() +
                    aWorldXYZ[2] * e.z(); // this is the world position in locale coordinates
   // TODO: verify this change to plus    v
   aModelXY[0] = -unori_x * cos(heading) + -unori_y * sin(heading);
   aModelXY[1] = unori_y * cos(heading) +
                 -unori_x * sin(heading); // this is the world position in heading rotated local coordinates
}


osg::Matrix VaEntity::ModelToWorldMatrix() const
{
   auto pos = mEntityPtr->BodyCF().GetLocationWCS();

   double ptp[3];
   mEntityPtr->BodyCF().GetOrientationWCS(ptp[0], ptp[1], ptp[2]);
   osg::Matrix mat;
   mat.makeTranslate(osg::Vec3(pos.Get(0), pos.Get(1), pos.Get(2)));
   osg::Quat quat(ptp[2], osg::Vec3d(1.0, 0.0, 0.0), ptp[1], osg::Vec3d(0.0, 1.0, 0.0), ptp[0], osg::Vec3d(0.0, 0.0, 1.0));
   mat.preMultRotate(quat);

   return mat;
}

void VaEntity::ViewerDestroyedCB(VaViewer* aViewer)
{
   // VaAttachments contain pointers to VaViewers.  Those pointers go stale
   // when a viewer is deleted.  Here, we watch for viewer removal, and
   // delete the attachments that are affected.

   // Special case must be handled where VaAttachments use nullptr as
   // their VaViewer, to make them visible in all views. these special
   // cases will not be removed

   std::list<VaAttachment*> toRemove;

   for (auto& attachment : mAttachments)
   {
      if (attachment->IsMyViewer(aViewer) && attachment->GetViewer() != nullptr)
      {
         toRemove.push_back(attachment.get());
      }
   }

   for (auto& removeIter : toRemove)
   {
      RemoveAttachment(removeIter->GetUniqueId());
   }
}

// virtual
void VaEntity::DirtyTimeRange()
{
   mTimeRangeDirty      = true;
   VaScenario* scenario = VaEnvironment::Instance().FindScenario(GetScenarioKey());
   if (scenario != nullptr)
   {
      scenario->DirtyTimeRange();
   }
}

// virtual
void VaEntity::GetTimeRange(double& aStartTime, double& aEndTime)
{
   if (mTimeRangeDirty)
   {
      bool first = true;
      for (auto& attachment : mAttachments)
      {
         double startTime = 0.0;
         double endTime   = 0.0;

         attachment->GetTimeRange(startTime, endTime);
         if (first)
         {
            mStartTime = startTime;
            mEndTime   = endTime;
            first      = false;
         }
         else
         {
            if (startTime < mStartTime)
            {
               mStartTime = startTime;
            }
            if (endTime > mEndTime)
            {
               mEndTime = endTime;
            }
         }
      }
      mTimeRangeDirty = false;
   }
   aStartTime = mStartTime;
   aEndTime   = mEndTime;
}

VaPosition VaEntity::ConvertToXY(const VaPosition& aPosition) const
{
   if (!aPosition.PositionIsXY())
   {
      VaPosition temp;
      double     xy[2];
      double     xyz[3];
      aPosition.GetSceneXYZ(xyz, 0);
      WorldToModelPlane(xyz, xy);
      temp.SetX(xy[0]);
      temp.SetY(xy[1]);
      return temp;
   }
   return aPosition;
}

void VaEntity::SelectAllAttachments(VaViewer* aViewerPtr)
{
   for (auto& attachment : mAttachments)
   {
      attachment->SetSelected(aViewerPtr, true, 0);
   }
}

std::string VaEntity::CreateName()
{
   std::string name = "entity-" + VaUtils::ToString<unsigned int>(++sId);
   return name;
}

UtEntity VaEntity::ToUtEntity() const
{
   UtEntity ent;
   ent.SetLocationLLA(GetLat(), GetLon(), GetAlt());
   ent.SetOrientationNED(mHeading, mPitch, mRoll);
   ent.SetVelocityWCS(GetVelocityWCS().GetData());
   return ent;
}

void VaEntity::PrivateBeforeDelete() {}

std::string VaEntity::GetMenuSelectionString() const
{
   return GetName();
}

bool VaEntity::IsModifiable() const
{
   if (GetScenario() != nullptr)
   {
      return GetScenario()->IsModifiable();
   }
   else
   {
      return true;
   }
}

void VaEntity::AddToCallbackHolder(VaCallback* aCallback)
{
   mCallbacks.Add(aCallback);
}


void VaEntity::DeleteLater(vespa::VaAttachment* aAttachmentPtr)
{
   mAttachmentToBeRemoved.emplace_back(aAttachmentPtr);
}

} // end namespace vespa.
