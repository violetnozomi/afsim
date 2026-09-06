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

#ifndef VaEntity_hpp
#define VaEntity_hpp

#include "vespatk_export.h"

#include <string>
#include <vector>

#include <osg/Matrix>

#include "UtEntity.hpp"
#include "UtMemory.hpp"
#include "UtVec3.hpp"
#include "VaAttachment.hpp"
#include "VaCallbackHolder.hpp"
#include "VaDefs.hpp"
#include "VaObject.hpp"
#include "VaPosition.hpp"
#include "VaScenarioKey.hpp"
#include "VaUniqueId.hpp"

namespace vespa
{
class VaBoundingBox;
class VaEnvironment;
class VaSelectionBox;
class VaViewer;

//! Base class for all entity types in the application (i.e. players, zones, etc.)
class VESPATK_EXPORT VaEntity : public VaObject, protected VaPosition
{
   friend class VaAttachment;
   friend class VaEnvironment;
   friend class VaScenario;
   friend class VaViewer;

public:
   using Attachments = std::vector<std::unique_ptr<VaAttachment>>;

   VaEntity(const std::string& aName);
   VaEntity(const VaEntity&) = delete; // disallow.
   VaEntity(VaEntity&&)      = delete;
   VaEntity& operator=(const VaEntity&) = delete;
   VaEntity& operator=(VaEntity&&) = delete;
   ~VaEntity() override;

   bool IsInitialized() const { return mInitialized; }
   void Initialize();

   bool IsLoaded() const;
   void Load();

   virtual void Update(double aTime);
   virtual void UpdateParallel(double aSimTime, double aRealTime) {}
   virtual void UpdatePostParallel(double aSimTime) {}
   virtual void UpdateFrame(double aTime);

   void         SetName(const std::string& aName) override;
   virtual void NameChanged();

   virtual bool IsActive(double aTime) const { return true; }

   virtual void Pick(VaViewer* aViewerPtr, int aMouseX, int aMouseY, bool aToggle, bool aDoubleClick = false);
   virtual bool BoxPick(VaViewer* aViewerPtr, const VaSelectionBox& aBox, bool aInside);

   virtual void ClearSelection(); // unselect this entity and all attachments
   virtual void SetSelected(VaViewer* aViewerPtr, bool aSelected);
   bool         GetSelected(VaViewer* aViewerPtr) const;
   void         SelectAllAttachments(VaViewer* aViewerPtr);

   virtual std::string GetMenuSelectionString() const;

   virtual bool IsInside(const VaSelectionBox& aBox) const;

   virtual bool IsVisible(VaViewer* aViewerPtr) const;
   void         SetVisibilityContribution(bool aVisibility, const unsigned short aMask, VaViewer* aViewerPtr);
   virtual void UpdateVisibility(); // Only the proxy attachment should call this.

   void          SetProxy(VaAttachment* aProxyPtr);
   VaAttachment* GetProxy() const { return mProxyPtr; }

   void                 SetScenario(VaScenario* aScenarioPtr);
   const VaScenarioKey& GetScenarioKey() const { return mScenarioKey; }
   VaScenario*          GetScenario() const { return mScenarioPtr; }

   void SetSupportsUndo(bool aSupportsUndo) { mSupportsUndo = aSupportsUndo; }
   bool GetSupportsUndo() const { return mSupportsUndo; }

   void SetInUndoStack(bool aInUndoStack) { mInUndoStack = aInUndoStack; }
   bool GetInUndoStack() const { return mInUndoStack; }

   // this property sets whether an entity is draggable with a ctrl-left-click-drag, and is somewhat a legacy capability.
   virtual void SetDraggable(bool aDraggable) { mDraggable = aDraggable; }
   virtual bool GetDraggable() const { return mDraggable; }

   // this property sets whether an entity is draggable with a left-click-drag.
   virtual void SetSimpleDraggable(bool aDraggable) { mSimpleDraggable = aDraggable; }
   virtual bool IsSimpleDraggable() { return mSimpleDraggable; }

   virtual void GetBoundingBox(VaBoundingBox& aBoundingBox) const;

   // Attachment management
   bool                    AddAttachment(std::unique_ptr<VaAttachment> aAttachmentPtr);
   VESPATK_DEPRECATED bool AddAttachment(VaAttachment* aAttachmentPtr);
   bool                    RemoveAttachment(int aUniqueId, int aSubpartId = 0);
   VaAttachment*           FindAttachment(int aUniqueId) const;
   VaAttachment*           FindAttachment(const std::string& aUniqueId) const;
   template<class T>
   void FindAttachments(std::vector<VaAttachment*>& aAttachments) const;
   template<class T>
   T* FindFirstAttachmentOfType() const;
   template<class T>
   std::vector<T*> FindAttachmentsOfType() const;

   const Attachments& GetAttachments() const { return mAttachments; }
   Attachments&       GetAttachments() { return mAttachments; }
   void               UpdateAttachmentHighlights(VaViewer* aViewerPtr);

   // State management
   void MoveRelative(UtEntity&        aRefBefore,
                     UtEntity&        aRefAfter,
                     int              aFeedbackFormat,
                     double           aFeedbackPrecision,
                     MoveRelativeMask aMoveRelativeMask,
                     VaViewer*        aViewerPtr);


   const VaPosition& GetPosition() const;

   virtual void SetPosition(const VaPosition& aPosition, const bool aMotionInTime = false);

   //! A user move action is complete.  Called after drag-move operations
   virtual void MoveComplete() {}
   virtual void RotateComplete() {}

   void         GetOrientation(double& aHeading, double& aPitch, double& aRoll) const;
   virtual bool SetOrientation(const double aHeading, const double aPitch, const double aRoll, const bool aInMotion = false);

   double       GetHeading() const { return mHeading; }
   virtual bool SetHeading(const double aHeading) { return SetOrientation(aHeading, mPitch, mRoll); }

   double       GetPitch() const { return mPitch; }
   virtual bool SetPitch(const double aPitch) { return SetOrientation(mHeading, aPitch, mRoll); }

   double       GetRoll() const { return mRoll; }
   virtual bool SetRoll(const double aRoll) { return SetOrientation(mHeading, mPitch, aRoll); }

   virtual bool SetPositionOrientation(const VaPosition& aPosition,
                                       const double      aHeading,
                                       const double      aPitch,
                                       const double      aRoll,
                                       const bool        aMotionInTime = false);

   void SetPositionOrientation(const double aLocationWCS[3], double aPsi, double aTheta, double aPhi);

   UtVec3d GetVelocityWCS() const { return mVelocityWCS; }
   void    SetVelocityWCS(const UtVec3d& aVelocityWCS);

   void Translate(double aDeltaLat, double aDeltaLon, VaViewer* aViewerPtr = nullptr);

   void Rotate(const UtLLAPos& aCenterLLA, double aAngle) override;

   void GetScreenSpaceCoords(int& aPositionX, int& aPositionY, VaViewer* aViewerPtr) const;

   void ModelToWorld(const double aModelXYZ[3], double aWorldXYZ[3], bool aVector = false) const;

   void WorldToModelPlane(const double aWorldXYZ[3], double aModelXY[2]) const;

   osg::Matrix ModelToWorldMatrix() const;

   UtoEntity* GetUtoEntity() { return mEntityPtr; }

   const UtoEntity* GetUtoEntity() const { return mEntityPtr; }

   virtual void DirtyTimeRange();
   virtual void GetTimeRange(double& aStartTime, double& aEndTime);

   VaPosition ConvertToXY(const VaPosition& aPosition) const;

   virtual bool IsUserDeletable() const { return true; }

   virtual VaPosition GetFocus(int aFocus = -1) const { return GetPosition(); }

   enum HiddenEntityContribution // applications should use the most significant byte
   {
      ScenarioStateHidden = 0x0001, // the entity is hidden for scenario reasons (ie. it is destroyed) (trans-viewer effect)
      UserOptionHidden          = 0x0002, // the entity is hidden due to user options
      ScenarioHidden            = 0x0004, // the entity is hidden because the scenario is hidden
      CameraHidden              = 0x0008, // the entity is hidden because the camera has hidden it
      Reserved                  = 0x0010, // for future use
      FirstApplicationHiddenBit = 0x0100  // The application is allowed to use bits at or greater than this
   };

   // derived classes should implement these & makes them useful
   virtual UtVec3d GetECEF_AtTime(double aTime)
   {
      UtVec3d ecef;
      GetECEF(ecef.GetData());
      return ecef;
   }
   virtual std::map<double, UtVec3d> GetECEF_BetweenTimes(double aTime1, double aTime2)
   {
      std::map<double, UtVec3d> temp;
      temp[aTime1] = GetECEF_AtTime(aTime1);
      return temp;
   }

   UtEntity ToUtEntity() const;

   bool IsModifiable() const;

   virtual std::string GetDescription() { return GetName(); }

   void AddToCallbackHolder(VaCallback* aCallback);

   void DeleteLater(vespa::VaAttachment* aAttachmentPtr);

protected:
   virtual bool PrivateInitialize() { return false; }
   virtual void PrivateLoad() {}
   virtual void PrivateInitializeCallbacks();
   virtual void PrivateBeforeDelete();
   virtual void ForcePositionUpdate() { mForceChange = true; }

   bool                mCallbacksLoaded;
   VaCallbackHolder    mCallbacks;
   double              mHeading;
   double              mPitch;
   double              mRoll;
   UtVec3d             mVelocityWCS;
   VaAttachment*       mProxyPtr;
   std::map<int, bool> mSelected;

private:
   bool GetVisibilityContribution(const unsigned short aMask, VaViewer* aViewerPtr) const;

   using VaObject::SetName;

   void InitializeAttachments();

   using UniqueIdToAttachment = std::map<int, VaAttachment*>;
   using TypeAttachment       = std::pair<std::string, VaAttachment*>;
   using TypeAttachmentSet    = std::set<TypeAttachment>;

   void ViewerDestroyedCB(VaViewer* aViewer);

   std::string CreateName();

   static unsigned int sId;

   unsigned int  mId;
   VaScenario*   mScenarioPtr;
   VaScenarioKey mScenarioKey;

   UtoEntity* mEntityPtr;

   bool mInitialized;

   bool mLoaded;

   // individual bits of this value represent contributions to visibility.
   // 0 is visible see enum HiddenEntityContributions
   // this is keyed off of VaViewers' unique ids.
   std::map<unsigned int, unsigned short> mHiddenMask;
   unsigned short                         mGlobalHiddenMask; // for cross-viewer options
   // the following can be modified in your application to make more options cross viewer
   unsigned short mGlobalVisibilityContributions;

   Attachments          mAttachments;
   UniqueIdToAttachment mNameToAttachmentMap;
   TypeAttachmentSet    mTypeAttachmentSet;

   bool mForceChange;

   bool mSupportsUndo;
   bool mInUndoStack;
   bool mDraggable;
   bool mSimpleDraggable;
   bool mDeleting;

   double mStartTime;
   double mEndTime;
   bool   mTimeRangeDirty;

   std::vector<VaAttachment*> mAttachmentToBeRemoved;
};


//! Method: FindAttachments
//! Description: Returns a list of attachments with the specified type.
//!
//! @param aType The type of the desired attachments.
//! @param aAttachments The list of attachments that are returned by reference.
template<class T>
void VaEntity::FindAttachments(std::vector<VaAttachment*>& aAttachments) const
{
   std::string typeStr  = cVAOBJECT_TYPE<T>();
   auto        typeIter = mTypeAttachmentSet.lower_bound(TypeAttachment(typeStr, nullptr));
   while ((typeIter != mTypeAttachmentSet.end()) && (typeIter->first == typeStr))
   {
      aAttachments.push_back(typeIter->second);
      ++typeIter;
   }
}

template<class T>
T* VaEntity::FindFirstAttachmentOfType() const
{
   auto attachs = FindAttachmentsOfType<T>();
   if (!attachs.empty())
   {
      return attachs.front();
   }
   return nullptr;
}

template<class T>
std::vector<T*> VaEntity::FindAttachmentsOfType() const
{
   std::vector<T*>            typedAttachs;
   std::vector<VaAttachment*> attachs;
   FindAttachments<T>(attachs);
   for (auto attach : attachs)
   {
      auto typedAttach = dynamic_cast<T*>(attach);
      if (typedAttach)
      {
         typedAttachs.push_back(typedAttach);
      }
   }
   return typedAttachs;
}

} // end namespace vespa.

VA_DECLARE_OBJECT_TYPE(vespa::VaEntity)
#endif
