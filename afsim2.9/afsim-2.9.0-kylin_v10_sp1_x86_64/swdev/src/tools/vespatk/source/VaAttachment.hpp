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

#ifndef VaAttachment_hpp
#define VaAttachment_hpp

#include "vespatk_export.h"

#include <map>
#include <string>
#include <vector>

#include "UtColor.hpp"
#include "UtLLAPos.hpp"
#include "VaCallbackHolder.hpp"
#include "VaDefs.hpp"
#include "VaObject.hpp"
#include "VaUniqueId.hpp"

class UtEntity;
class UtoShape;

namespace vespa
{
class VaBoundingBox;
class VaEntity;
class VaPosition;
class VaSelectionBox;
class VaViewer;

//! Base class for all the things that can be attached to entities (models, rings, zones, etc...)
class VESPATK_EXPORT VaAttachment : public VaObject
{
public:
   friend class VaEntity;

   using ShapeList = std::vector<std::string>;

   VaAttachment(VaEntity& aParent, VaViewer* aViewerPtr, const std::string& aName);
   ~VaAttachment() override;

   static void LoadAttachment(VaAttachment& aAttachment);

   bool IsInitialized() const { return mInitialized; }
   bool Initialize();

   void Load();

   virtual void Update(double aTime) {}
   virtual void UpdateFrame(double aTime) {}

   VaEntity&       GetParent() { return mParent; }
   const VaEntity& GetParent() const { return mParent; }

   bool IsProxy() const;

   virtual VaObject* FindSubPart(const VaAttachmentSubId& aUniqueId) const { return nullptr; }

   //! Indicates that the parent's name has changed.
   virtual void NameChanged() {}

   virtual bool IsVisible() const;

   virtual void UpdateVisibility();

   void SetStateVisibility(bool aStateVisibility);
   bool GetStateVisibility() const { return mStateVisibility; }

   enum
   {
      cPICK_NOTHING    = 0x0000,
      cPICK_ATTACHMENT = 0x0001,
      cPICK_ENTITY     = 0x0002,
      cPICK_MODE_MASK  = 0x000B, // should this not include auto?
      cPICK_AUTO       = 0x0004,
      cPICK_SUBPARTS   = 0x0008
   };
   virtual unsigned int GetPickMode() const { return cPICK_NOTHING; }

   enum
   {
      cHOVER_NOTHING    = 0x0000,
      cHOVER_ATTACHMENT = 0x0001,
      cHOVER_ENTITY     = 0x0002,
      cHOVER_MODE_MASK  = 0x000B, // should this not include auto?
      cHOVER_AUTO       = 0x0004,
      cHOVER_SUBPARTS   = 0x0008
   };
   virtual unsigned int GetHoverMode() const { return GetPickMode(); }
   virtual bool         Pick(VaViewer*                aViewerPtr,
                             int                      aMouseX,
                             int                      aMouseY,
                             bool                     aToggle,
                             bool                     aDoubleClick         = false,
                             const VaAttachmentSubId& aAttachmentSubPartId = 0);
   virtual bool         BoxPick(VaViewer* aViewerPtr, const VaSelectionBox& aBox, bool aInside);

   virtual void HitSubparts(UtoHitInfoList& aHits, vespa::VaHitList& aSubHits);

   virtual void HitMultipleSubparts(std::vector<UtoHitInfoList>& aShapesHit, vespa::VaHitList& aSubHits);

   //! Indicates that the parent's position has been translated.
   virtual void Translated(double aDeltaLat, double aDeltaLon, VaViewer* aViewerPtr = nullptr) {}

   //! Indicates that the parent has been rotated.
   virtual void Rotated(const UtLLAPos& aCenterLLA, double aAngle) {}

   // viewer is added for sub-part selection state
   virtual void MoveRelative(UtEntity&        aRefBefore,
                             UtEntity&        aRefAfter,
                             int              aFeedbackFormat,
                             double           aFeedbackPrecision,
                             MoveRelativeMask aMoveRelativeMask,
                             VaViewer*        aViewerPtr)
   {
   }

   //! A user move action is complete.  Called after drag-move operations
   virtual void MoveComplete() {}

   virtual void GetBoundingBox(VaBoundingBox& aBoundingBox) const;

   virtual void GetShapes(ShapeList& aShapes) const;
   virtual void GetHighlightableShapes(ShapeList& aShapes) const { GetShapes(aShapes); }
   virtual void HighlightSubpart(VaViewer*                aViewerPtr,
                                 const VaAttachmentSubId& aSubpartId,
                                 bool                     aHighlight,
                                 const UtColor&           aColor = UtColor(1.f, 1.f, 1.f));

   enum DeleteAction
   {
      cDELETE_NOTHING,
      cDELETE_ATTACHMENT,
      cDELETE_ENTITY
   };
   virtual DeleteAction DeleteSelection(VaViewer* aViewerPtr);
   virtual void         ClearSelection(VaViewer* aViewerPtr) { SetSelected(aViewerPtr, false, 0); }
   virtual void SetSelected(VaViewer* aViewerPtr, bool aSelected, const VaAttachmentSubId& aSubId, bool aForce = false);
   virtual bool GetSelected(VaViewer* aViewerPtr) const;

   UtoShape* AddShapeToParent(const std::string& aShapeName, UtoShape& aShape, const std::string& aAdditionalTag = "");
   void      RemoveShapeFromParent(const std::string& aShapeName);
   void      RemoveShapeFromParent(UtoShape* aShapePtr);
   bool      ShowShape(const std::string& aShapeName, bool aShow);

   virtual std::string GetMenuSelectionString(const VaAttachmentSubId& aSubId) const;

   virtual void GetTimeRange(double& aStartTime, double& aEndTime)
   {
      aStartTime = 0.0;
      aEndTime   = 0.0;
   }

   bool         IsShared() { return mShared; }
   virtual void SetShared(bool aShared) { mShared = aShared; }
   UtoShape*    FindShape(const std::string& aShapeName);

   enum VisibilityContribution // applications should use the most significant byte
   {
      ScenarioStateHidden       = 0x0001, // the entity is hidden for scenario reasons (ie. it is destroyed)
      UserOptionHidden          = 0x0002, // the entity is hidden due to user options
      ScenarioHidden            = 0x0004, // the entity is hidden because the scenario is hidden
      Reserved                  = 0x0008, // for future use
      CommanedHiddenDIS         = 0x0010, // commanded hidden by DIS Other pdu
      ZeroTetherHidden          = 0x0020, // hidden because the camera doesn't want it shown
      FirstApplicationHiddenBit = 0x0100  // The application is allowed to use bits at or greater than this
   };

   virtual bool IsHighlightable() { return true; }
   // returns true if the attachment will handle the highlighting on its own, otherwise the responsibility falls on the viewer.
   virtual bool Highlight(VaViewer* aViewerPtr, const VaAttachmentSubId& aSubpartId, bool aHighlight, const UtColor& aColor)
   {
      return false;
   }

   // Signal the attachment or sub part selection
   void AttachmentSelect(VaViewer* aViewerPtr, int aSelect, const VaAttachmentSubId& aSubId);

   bool IsMyViewer(VaViewer* aViewerPtr);

   VaViewer* GetViewer() { return mViewerPtr; }

   void AddToCallbackHolder(VaCallback* aCallback);

   virtual bool IsModifiable();     // is the attachment modifiable
   virtual bool ShowUnmodifiable(); // should the attachment show as unmodifiable

   virtual void SetProxy(bool aState) { mIsProxy = aState; }

   virtual std::string GetDescription(unsigned int aSubId) { return GetName(); }

   virtual void PreApplyOptions(bool aForce) {}

   virtual const VaPosition& GetPosition(unsigned int aSubId) const;

protected:
   virtual void ChangedProxy(VaAttachment* aProxyPtr) {}

   virtual void GetShapesForSubpart(const VaAttachmentSubId& aSubpartId, ShapeList& aShapes) const
   {
      return GetShapes(aShapes);
   }

   bool             mInitialized;
   bool             mLoaded;
   VaCallbackHolder mCallbacks;
   VaViewer*        mViewerPtr;

private:
   using ShapeMap = std::map<std::string, UtoShape*>;

   virtual bool PrivateInitialize() { return true; }

   virtual void PrivateLoad() {}

   virtual void PrivateInitializeCallbacks() {}

   ShapeMap            mShapeMap;
   VaEntity&           mParent;
   bool                mStateVisibility;
   std::map<int, bool> mSelected;
   bool                mShared;

   // if an attachment is a proxy, it will delete the parent entity on delete
   // some attachments (models) pass selection state to the parent entity if they are proxy
   // entities pass their selection state to proxy attachments
   bool mIsProxy;
};
} // end namespace vespa.

VA_DECLARE_OBJECT_TYPE(vespa::VaAttachment)

#endif
