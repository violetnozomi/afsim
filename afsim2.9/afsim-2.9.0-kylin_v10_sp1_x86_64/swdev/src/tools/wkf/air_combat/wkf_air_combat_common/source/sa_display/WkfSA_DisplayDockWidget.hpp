// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef SA_DISPLAY_DOCKWIDGET_HPP
#define SA_DISPLAY_DOCKWIDGET_HPP

#include "wkf_air_combat_common_export.h"

#include <unordered_map>

#include <QCheckBox>
#include <QFrame>
#include <QPushButton>
#include <QRadioButton>
#include <QString>
#include <QTreeWidgetItem>

#include "VaAttachmentModel.hpp"
#include "VaCallbackHolder.hpp"
#include "VaOverlayTexture.hpp"
#include "WkfDockWidget.hpp"
#include "WkfEnvironment.hpp"
#include "sa_display/WkfSA_DisplayDataContainer.hpp"

namespace vespa
{
class VaEntity;
class VaScenario;
class VaViewer;
} // namespace vespa

namespace wkf
{
class AttachmentPerceptionVectors;
namespace SA_Display
{
class Overlay;
class WKF_AIR_COMBAT_COMMON_EXPORT DockWidget : public wkf::DockWidget
{
   Q_OBJECT

public:
   /**
    * PerceptionDisplayDockWidget constructor.
    * @param aObjectName    The name of this DockWidget.
    * @param aPlatformName  The name of the platform to which this display is tethered.
    * @param aParent        The parent window of this window. Defaults to nullptr.
    * @param aFlags         Any additional flags for the window. Defaults to Qt::WindowFlags().
    */
   DockWidget(const QString&  aObjectName,
              const QString&  aPlatformName,
              QMainWindow*    aParent = nullptr,
              Qt::WindowFlags aFlags  = Qt::WindowFlags());

   /**
    * The destructor removes the attachments from the viewer, removes the viewer,
    * then clears the vector manager.
    */
   ~DockWidget() override;

   /**
    * This function accepts QDragEnterEvents, meaning that this widget will not
    * pass the event on to its parent, and instead, allow itself to be dragged.
    * @param aEvent  The drag enter event that prompted this function call.
    */
   void dragEnterEvent(QDragEnterEvent* aEvent);

   /**
    * Handles when the widget is 'dropped'.
    * @param aEvent  The drop event that prompted this function call.
    */
   void dropEvent(QDropEvent* aEvent);

   /**
    * Called if a whole scenario is removed. It clears the vectors and nulls the platform pointer.
    * @param aScenarioPtr  A pointer to the scenario that was removed.
    */
   void ScenarioRemovedCB(vespa::VaScenario* aScenarioPtr);

   /**
    * Called if something other than this widget shuts down the viewer. That
    * shouldn't happen, but because it is possible to 'get' the viewer pointer,
    * we have to guard against it.
    * @param aViewerPtr  The pointer to the viewer that was shut down.
    */
   void ViewerShutdownCB(vespa::VaViewer* aViewerPtr);

   /**
    * Called after the viewer has been initialized. It adds callbacks to the
    * viewer, calls PlatformAdded for the platform to which it's tethered, and
    * applies preferences to the viewer.
    */
   void ViewerInitialized();

   /**
    * Compares a given viewer to this viewer to see if they're the same one.
    * @param aViewerPtr  A pointer to the viewer being compared.
    * @return            True iff aViewerPtr is the same as mViewerPtr.
    */
   bool IsMyViewer(vespa::VaViewer* aViewerPtr);

   /** Gets the name of the tethered platform. */
   QString GetConnectedPlatformName() const { return mConnectedPlatformName; }

   /**
    * Adds an overlay to the viewer.
    * @param aOverlayPtr  A pointer to the overlay to be added.
    */
   void AddOverlay(vespa::VaOverlay* aOverlayPtr);

   /** Opens the screen graph dialog for the viewer. */
   void SceneGraphAction();

   /**
    * Builds the context menu for inside the perception display.
    * @param aMenu  The menu to which items are added.
    * @param aPos   The point on the screen where the context menu is openned.
    * @return       True.
    */
   bool BuildContextMenu(QMenu& aMenu, const QPoint& aPos) override;

   /**
    * Activates the 'kill frame.'
    * @param aState  Determines whether the frame has a red border (true) or not (false).
    */
   void ActivateKillFrame(bool aState);

   /**
    * Has the connection between the SA display and the platform been established?
    * @return True iff the connection has been established.
    */
   bool ConnectionEstablished() const { return mConnectionEstablished; }

   /**
    * Called when the connection between this viewer and the tethered platform is destroyed.
    * @param aEntityPtr  A pointer to the entity that was disconnected.
    */
   void ConnectionDestroyed(vespa::VaEntity* aEntityPtr);

   /**
    * Has the SA display been initialized?
    * @return true if this display is initialized.
    */
   bool IsInitialized() const { return mInitialized; }

   /**
    * Returns a pointer to the viewer.
    * @return A pointer to the viewer.
    */
   vespa::VaViewer* GetViewer() { return mViewerPtr; }

   /**
    * Returns a pointer to the platform it's following.
    * @return a pointer to the tethered platform.
    */
   wkf::Platform* GetPlatform() { return mPlatformPtr; }

   void SetSA_Data(const SA_Data& aData);
   void SetSA_TruthPlatforms(const std::vector<SA_Display::EntityTruth>& aData);
   void SetCenterPoint();
   void ResetCenterPoint();

signals:
   void DisplayInitialized();

protected slots:
   // These slots all match the buttons/checkboxes on the left side of the display.
   void ShowPerception(bool aShow);
   void ShowTruth(bool aShow);
   void ShowTruthGround(bool aShow);
   void ShowTruthAir(bool aShow);
   void ShowFriends(bool aShow);
   void ShowFlight(bool aShow);
   void ShowPriorityThreats(bool aShow);
   void ShowPriorityTargets(bool aShow);
   void ShowAltitude(bool aShow);
   void ShowESM_RWR(bool aShow);
   void ShowAngleOnly(bool aShow);
   void ShowOwnEngagements(bool aShow);
   void ShowFlightEngagements(bool aShow);
   void ShowThreatEngagements(bool aShow);
   void ShowSAMs(bool aShow);
   void ShowRoute(bool aShow);
   void ShowBullseye(bool aShow);
   void ShowFOV(bool aShow);
   void ShowGroups(bool aShow);
   void HoverMode(bool aState);
   void CenterPosition(bool aState);
   void OffsetPosition(bool aState);
   void BottomPosition(bool aState);

protected:
   /**
    * Build the context menu inside the DockWidget.
    * @param aMenu  The menu to which items are added.
    * @param aPos   The point on the screen where the context menu is openned.
    */
   virtual void BuildContextMenuPrivate(QMenu& aMenu, const QPoint& aPos);

   /**
    * Called when the dock widget is closed. Just deletes itself. Later.
    * @param aEvent  The event that prompted the call to this method.
    */
   virtual void closeEvent(QCloseEvent* event);

private:
   /** Callback for the viewer resizing.
    * @param aViewerPtr pointer to the viewer.
    */
   void ViewerResizedCB(vespa::VaViewer* aViewerPtr);

   /** Create all of the widgets for this DockWidget. */
   void CreateWidgets();

   /** Clears the callback with the given ID.
    * @param aId id of the callback to clear.
    */
   void ClearCallback(int& aId);

   // Misc variables
   vespa::VaViewer*        mViewerPtr;   ///< Pointer to the viewer.
   wkf::Platform*          mPlatformPtr; ///< Pointer to the tethered platform.
   vespa::VaCallbackHolder mCallbacks;   ///< Callbacks for this object.
   QPointer<QFrame>        mFrame; ///< Pointer to the frame that contains all of the widgets inside of this DockWidget
   QString                 mConnectedPlatformName; ///< Name of the connected platform.
   QLabel*                 mRangeLabel{nullptr};   ///< Label displaying the range in nm.

   // Left and right widgets
   QWidget* mLeftWidget{nullptr};
   QWidget* mRightWidget{nullptr};

   // Kinematic state widget items
   QTreeWidgetItem* mAltitude{nullptr};
   QTreeWidgetItem* mKCAS{nullptr};
   QTreeWidgetItem* mKTAS{nullptr};
   QTreeWidgetItem* mMach{nullptr};
   QTreeWidgetItem* mGs{nullptr};

   // Weapon state widget items
   std::unique_ptr<QTreeWidget>  mWeaponStateTree;
   std::vector<QTreeWidgetItem*> mWeapons;

   // Fuel state widget items
   QTreeWidgetItem* mInternalFuel{nullptr};
   QTreeWidgetItem* mExternalFuel{nullptr};
   QTreeWidgetItem* mTotalFuel{nullptr};
   QTreeWidgetItem* mJoker{nullptr};
   QTreeWidgetItem* mBingo{nullptr};

   // Entity details widget items
   QTreeWidgetItem* mEntityTruthData{nullptr};
   QTreeWidgetItem* mEntityTruthName{nullptr};
   QTreeWidgetItem* mEntityTruthType{nullptr};
   QTreeWidgetItem* mEntityTruthRange{nullptr};

   QTreeWidgetItem* mEntityPerceivedData{nullptr};
   QTreeWidgetItem* mEntityPerceivedType{nullptr};
   QTreeWidgetItem* mEntityPerceivedRange{nullptr};

   QPointer<QRadioButton> mCenterPositionPtr;

   // Widget specific booleans
   bool mConnectionEstablished{nullptr}; ///< Is the connection between this widget and the platform established?
   bool mInitialized{nullptr};           ///< Is this display initialized?

   // Overlay
   Overlay* mOverlayPtr{nullptr}; ///< Pointer to the SA Display overlay that will do the drawing.
};
} // namespace SA_Display
} // namespace wkf
#endif // SA_DISPLAY_DOCKWIDGET_HPP
