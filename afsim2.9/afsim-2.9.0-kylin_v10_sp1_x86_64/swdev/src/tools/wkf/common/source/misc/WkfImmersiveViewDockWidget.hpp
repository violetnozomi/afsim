// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WKFIMMERSIVEVIEWDOCKWIDGET_HPP
#define WKFIMMERSIVEVIEWDOCKWIDGET_HPP

#include "wkf_common_export.h"

#include <memory>

#include <QFrame>

#include "VaAttachmentAngles.hpp"
#include "WkfDockWidget.hpp"
#include "WkfOverlayUpdater.hpp"
#include "WkfPlatform.hpp"
#include "WkfPlatformGrouping.hpp"
#include "WkfScenario.hpp"

class QFrame;

namespace vespa
{
class VaAttachment;
class VaAttachmentVectors;
class VaOverlayTexture;
class VaViewer;
} // namespace vespa

namespace wkf
{
class WKF_COMMON_EXPORT ImmersiveViewDockWidget : public DockWidget
{
   Q_OBJECT
public:
   ImmersiveViewDockWidget(const QString& aObjectName, const QString& aPlatformName, QMainWindow* parent, Qt::WindowFlags flags);
   ~ImmersiveViewDockWidget() override;

   void LookAt(const QString& aEntityName);

   void ViewerInitialized();

   void           PlatformToBeDeletedCB(wkf::Platform* aPlatformPtr);
   void           ShowBoresight(const std::string& aLabel, double aAzimuth, double aElevation);
   void           UpdateBoresight(const std::string& aLabel, double aAzimuth, double aElevation);
   void           HideBoresight(const std::string& aLabel);
   void           SubpartAddedCB(wkf::Platform* aPlatformPtr, const std::string& aName);
   void           SubpartRemovedCB(wkf::Platform* aPlatformPtr, const std::string& aName);
   void           SubpartUpdatedCB(wkf::Platform* aPlatformPtr, const std::string& aName);
   void           ScenarioRemovedCB(vespa::VaScenario* aScenarioPtr);
   void           ViewerShutdownCB(vespa::VaViewer* aViewerPtr);
   void           PlatformDecoratedCB(wkf::Entity* aPlatformPtr, const QIcon& aIcon, const std::string& aDescription);
   const QString& GetConnectedPlatformName() const { return mConnectedPlatformName; }
   void           ResetCamera();

   vespa::VaAttachment* AddAttachment(const std::string& aType);
   void                 AddOverlay(vespa::VaOverlay* aOverlayPtr);

   void SceneGraphAction();
   bool BuildContextMenu(QMenu& aMenu, const QPoint& pos) override;
   void ActivateKillFrame(bool aState);
   bool IsMyViewer(vespa::VaViewer* aViewerPtr);

   void SetFirstPerson(bool aState);
   void GetAzElDist(double& aAz, double& aEl, double& aDist);
   void SetAzElDist(double aAz, double aEl, double aDist);

   void closeEvent(QCloseEvent* aEvent) override { emit ImmersiveViewClosed(); }
   bool ConnectionEstablished() const { return mConnectionEstablished; }
   void ConnectionDestroyed(vespa::VaEntity* aEntityPtr);
   bool IsInitialized() const { return mInitialized; }
   bool GetBoresightShown(const std::string& aSensorName) const { return mShownBoresights.count(aSensorName); }
   bool SaveSettingsEnabled() const
   {
      return mLookAtName.isEmpty();
   } // lets not save these off if the view is a look-at

   virtual void SetLockToHeading(bool aEnabled);
   virtual void SetLightingEnabled(bool aEnabled);
   bool         GetLockToHeading() const;

   virtual bool SupportsVectors() { return true; }

signals:
   void ImmersiveViewInitialized();
   void ImmersiveViewClosed();

protected:
   void         CompleteConstruction();
   void         LookAt(vespa::VaEntity* aPlatform);
   virtual void PlatformAddedPrivate(wkf::Platform* aPlatformPtr) {}
   virtual void PlatformToBeDeletedPrivate(wkf::Platform* aPlatformPtr) {}
   virtual void ConnectionEstablishedPrivate() {}
   virtual void ConnectionDestroyedPrivate() {}
   virtual void ClearDataPrivate() {}
   virtual void BuildContextMenuPrivate(QMenu& aMenu, const QPoint& pos) {}
   virtual void SetFirstPersonPrivate(bool aState) {}
   virtual void ViewerInitializedPrivate() {}
   void         dragEnterEvent(QDragEnterEvent* aEvent) override;
   void         dropEvent(QDropEvent* event) override;

   vespa::VaViewer* mViewerPtr;
   wkf::Platform*   mPlatformPtr;

private:
   class VectorManager : public QObject
   {
   public:
      //! AngleInfo for sharing intent to visualize an angle between to vector.
      //! <VectorIdOne, VectorIdTwo, ColorOfAngleBeingVisualized>
      using AngleInfo = std::tuple<int, int, QColor>;
      VectorManager(ImmersiveViewDockWidget* aParent);
      virtual ~VectorManager() = default;
      void Clear();
      void PopulateMenu(QMenu& aMenu);
      void VectorActionTriggered();
      //! Handles when the context menu button is clicked to
      //! form a new angle for visualization
      void VisualizeAnglesClickHandler();
      //! Toggles visibility when context menu is clicked.
      void HandleAngleVisibilityClick();
      //! Toggles wire frame rendering when context menu is clicked.
      void HandleAngleFillClick();
      //! Toggles the visibility of the angles overlay.
      void HandleOverlayVisibilityClick();
      //! Synchronizes the overlay with the scene graph.
      //! This allows use of the existing angles context menu
      //! to also manage the overlay with ease.
      //! It works by deleting all entries and resetting them.
      //! This had to be that way because OverlayUpdate uses vectors for Updator storage
      //! and doesn't allow per Updater change. I considered changing this but the implications
      //! were there for older code, instead im working around it.
      void HandleOverlaySync();
      void ShowVector(int aVector, bool aState);
      void AddPlatform(wkf::Platform* aPlatformPtr);
      void ShowBoresight(const std::string& aLabel, double aAzimuth, double aElevation);
      void UpdateBoresight(const std::string& aLabel, double aAzimuth, double aElevation);
      void HideBoresight(const std::string& aLabel);
      void ConnectionEstablished();

      void EntityRemovedCB(vespa::VaEntity* aEntityPtr);
      void PlatformToBeDeleted(wkf::Platform* aPlatformPtr);
      //! Retrieves the ECEF position of a vector for use in
      //! calculation and visualization of angle.
      UtVec3f GetVectorPosition(int aVectorId) const;
      //! Retrieves the scale of the vector for use in
      //! rendering, this makes the angle look good for all vectors on models
      float GetVectorScale(int aVectorId) const;
      //! Callback for frame update driven updates to position.
      //! This is called by VaAttachmentVectors when
      //! their vectors have changed.
      void PositionUpdated(int aVectorId, UtVec3f aNewPosition);
      //! Unfortunately We don't support drawing an angle for grouped vectors.
      //! The VaAttachmentVector allows groupings of vectors on a scene graph node.
      //! Given its impossible to know which vector you wanted to draw an angle
      //! for, we simply don't allow it. Single platforms only for custom vectors.
      //! This method checks that the vector you are asking for doesn't contain
      //! more than one candidate.
      bool IsMultiVectorEntry(int aVectorId) const;

   private:
      //! Check the angle attachment, if null, instantiate it and adds it to the parent.
      void InstantiateAngleAttachment();
      void RemovePlatform(wkf::Platform* aPlatformPtr);
      void AddVectorSet(const wkf::PlatformGrouping& aGrouping, const std::set<int>& aPlatformSet, const QColor& aColor);
      //! Dynamically builds the dialogue that allows user to select two vectors
      //! to calculate an angle between.
      AngleInfo RunVisualizerDialogue();
      //! Checks for the angles overlay existence on the Viewer, since we don't own
      //! the overlay, the viewer does, it allows user to close it at anytime.
      wkf::OverlayUpdater*        GetAnglesOverlay() const;
      vespa::VaCallbackHolder     mCallbacks;
      ImmersiveViewDockWidget*    mParentWidget;
      std::map<int, bool>         mVectorState;
      vespa::VaAttachmentVectors* mVectorAttachmentPtr{nullptr};
      // Angle visualization support for visualized vectors facilitated by VaAttachment angles.
      vespa::VaAttachmentAngles*           mAnglesAttachmentPtr{nullptr};
      std::map<wkf::PlatformGrouping, int> mGroupingVectors;
   };

   void PlatformsAdded(const wkf::PlatformList& aPlatforms);
   void ClearData();
   void LookAtPlatformMovedCB(vespa::VaEntity* aEntityPtr, bool aUiDriven);
   void LookAtPlatformDeletedCB(vespa::VaEntity* aEntityPtr);
   void ClearCallback(int& aId);

   QFrame*             mFrame;
   QString             mConnectedPlatformName;
   QString             mLookAtName;
   vespa::VaEntity*    mLookAtEntity;
   std::pair<int, int> mLookAtCallbackId;
   std::pair<int, int> mDeleteCallbackId;
   double              mLastDist;
   bool                mConnectionEstablished;

   std::map<std::pair<int, std::string>, unsigned int> mSubpartLookup;
   vespa::VaOverlayTexture*                            mDecoratorOverlay;
   bool                                                mInitialized;
   std::set<std::string>                               mShownBoresights;
   std::unique_ptr<VectorManager>                      mVectorManager;
};
} // namespace wkf

#endif
