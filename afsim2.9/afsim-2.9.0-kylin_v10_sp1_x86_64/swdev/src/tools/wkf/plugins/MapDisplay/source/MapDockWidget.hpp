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
#ifndef MAPDISPLAYDOCKWIDGET_HPP
#define MAPDISPLAYDOCKWIDGET_HPP

class QAction;
#include <QList>
class QMenu;
#include <QMutex>

#include "MapCameraMotion.hpp"
#include "MapPrefObject.hpp"
#include "MapRulerPropertiesWidget.hpp"
#include "VaCallbackHolder.hpp"
#include "WkfAction.hpp"
#include "WkfDockWidget.hpp"
#include "WkfPlatform.hpp"
#include "visibility/WkfTrackVisibilityPrefObject.hpp"

namespace vespa
{
class VaAttachment;
class VaAttachmentModel;
struct VaAttachmentSubId;
class VaEntity;
class VaLocationReference;
class VaViewer;
} // namespace vespa

namespace wkf
{
class AttachmentGreatCircleRuler;
class PointOfInterest;
class Track;
class Viewer;
} // namespace wkf

namespace Map
{
class DockWidget : public wkf::DockWidget
{
public:
   DockWidget(QMainWindow* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
   ~DockWidget() override = default;

   wkf::Viewer*        GetViewer() const { return mViewerPtr; }
   QList<wkf::Action*> GetActions() const;

   void MeasureFrom(const vespa::VaLocationReference& aLocRef);

   bool HasActiveRuler() const { return mActiveRulerPtr != nullptr; }
   void SetPreferences(const PrefObject* aPrefObjectPtr);

   void CenterCamera(wkf::Entity* aEntityPtr);
   void Follow(wkf::Entity* aEntityPtr);

protected:
   bool BuildContextMenu(QMenu& aMenu, const QPoint& pos) override;
   void ContextMenuExeced();

   bool event(QEvent* event) override;
   void VisibilityChanged(bool aVisible);
   void dragEnterEvent(QDragEnterEvent* aEvent) override;
   void dropEvent(QDropEvent* aEvent) override;


private:
   void PlatformsAddedHandler(const wkf::PlatformList& aPlatforms);
   void PlatformPropertiesChangedHandler(wkf::Platform* aEntityPtr);

   void PlatformSelectionChanged(const wkf::PlatformList& aSelected, const wkf::PlatformList& aUnselected);
   void PlatformVisibilityChanged();
   void TeamColorChanged();
   void MilStdChanged();
   void ShowVelocityVectorChanged();
   void ShowVelocityVector(wkf::Platform& aPlatform, bool aInitialState);

   void SceneGraphAction();
   void ReloadShaders();
   void ToggleModeAction();
   void ToggleStatsAction();
   void MenuHovered();
   void CopyCursorLocationToClipboard();
   void EndRuler(bool aCancel);

   bool MouseButtonClickCB(vespa::VaViewer* aViewerPtr, int aButton, int aMouseX, int aMouseY, int aState);
   void EndSelectionCB(vespa::VaViewer* aViewerPtr);
   void AttachmentSelectedCB(vespa::VaViewer*                aViewerPtr,
                             int                             aSelect,
                             vespa::VaAttachment*            aAttachmentPtr,
                             const vespa::VaAttachmentSubId& aSubPartId);

   void AttachmentDeletedCB(vespa::VaAttachment* aAttachmentPtr, const vespa::VaAttachmentSubId& aSubPartId);

   vespa::VaAttachmentModel* GetModel(vespa::VaEntity* aEntityPtr);

   wkf::PointOfInterest* AddPointOfInterest(const vespa::VaLocationReference& aLocRef);

   wkf::Action* mCenterOnSelectionPtr;
   wkf::Action* mCenterOnScenarioPtr;
   wkf::Action* mCopyCursorToClipboardPtr;
   wkf::Action* mMeasureFromLocationPtr;
   wkf::Action* mMeasureFromPlatformPtr;
   wkf::Action* mTogglePolygonModePtr;
   wkf::Action* mToggleStatsPtr;

   wkf::Viewer*                      mViewerPtr;
   CameraMotion*                     mCameraMotionPtr;
   std::vector<vespa::VaAttachment*> mTempHighlightAttachments;
   wkf::AttachmentGreatCircleRuler*  mActiveRulerPtr;
   RulerPropertiesWidget*            mRulerPropertiesPtr;
   bool                              mContextMenuActive;

   // When a platform focus is set in the WKF Environment, it sets the platform as selected in the VTK Environments
   // When a platform is selected in the VTK Environments, it sets the platform focus in the wkf::Environment
   // To prevent an infinite loop from occurring, use a mutex to prevent processing when necessary.
   QMutex mEntitySelectionMutex;

   std::string mActiveMap;
};
} // namespace Map
#endif // MAPDISPLAYDOCKWIDGET_HPP
