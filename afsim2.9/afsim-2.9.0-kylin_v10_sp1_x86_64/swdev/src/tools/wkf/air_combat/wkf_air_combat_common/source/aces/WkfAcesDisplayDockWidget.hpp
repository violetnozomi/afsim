// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef ACESDISPLAY_DOCKWIDGET_HPP
#define ACESDISPLAY_DOCKWIDGET_HPP

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
#include "aces/WkfAcesDisplayDataContainer.hpp"

namespace vespa
{
class VaEntity;
class VaScenario;
class VaViewer;
} // namespace vespa

namespace wkf
{
namespace AcesDisplay
{
class Overlay;
class WKF_AIR_COMBAT_COMMON_EXPORT DockWidget : public wkf::DockWidget
{
   Q_OBJECT

public:
   /**
    * AcesDisplayDockWidget constructor.
    * @param aObjectName    The name of this DockWidget.
    * @param aPlatformPtr   Pointer to the platform to which this display is tethered.
    * @param aParent        The parent window of this window. Defaults to nullptr.
    * @param aFlags         Any additional flags for the window. Defaults to Qt::WindowFlags().
    */
   DockWidget(const QString&  aObjectName,
              wkf::Platform*  aPlatformPtr,
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

   /** Gets the index of the tethered platform. */
   const size_t GetConnectedPlatformIndex() const { return mConnectedPlatformIndex; }

   /** Get if this window has focus. */
   const bool GetHasFocus();

   /** Set if this window has focus.
    * @param aHasFocus true if the window should have focus, false otherwise.
    */
   void SetHasFocus(const bool aHasFocus);

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
    * Has the connection between the ACES display and the platform been established?
    * @return True iff the connection has been established.
    */
   bool ConnectionEstablished() const { return mConnectionEstablished; }

   /**
    * Called when the connection between this viewer and the tethered platform is destroyed.
    * @param aEntityPtr  A pointer to the entity that was disconnected.
    */
   void ConnectionDestroyed(vespa::VaEntity* aEntityPtr);

   /**
    * Has the ACES display been initialized?
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

   /** Set the ACES Display data.
    * @param aData data to set.
    */
   void SetAcesData(const AcesData& aData);

   /** Set the ACES Display truth data.
    * @param aData entity truth data to set.
    */
   void SetAcesTruthPlatforms(const std::vector<AcesDisplay::EntityTruth>& aData);

   /** Set the ACES Display platform comment data.
    * @param aStr platform comment to set.
    */
   void SetAcesPlatformComment(const std::string& aPlatformCommentStr);

   /** Set the center point for the TSD/SAD on the ACES Display. */
   void SetCenterPoint();

   /** Reset the center point to 0,0 for the TSD/SAD on the ACES Display. */
   void ResetCenterPoint();

   /** Set the target platform list for the ACES Display.
    * @param aPlatforms platforms to set.
    */
   void SetTargetPlatforms(const std::vector<size_t>& aPlatforms);

signals:
   /** Emitted when the display is initialized. */
   void DisplayInitialized();

   /** Emitted when a display gains focus.  Used for target platform selection logic,
    * only change platform selection for the active ACES display.
    * @param aPlatformIndex index of platform gaining focus.
    */
   void DisplayGainedFocus(const size_t aPlatformIndex);

protected:
   /**
    * Build the context menu inside the DockWidget.
    * @param aMenu  The menu to which items are added.
    * @param aPos   The point on the screen where the context menu is openned.
    */
   virtual void BuildContextMenuPrivate(QMenu& aMenu, const QPoint& aPos);

   /**
    * Called when the dock widget is closed. Just deletes itself. Later.
    * @param aEvent The event that prompted the call to this method.
    */
   virtual void closeEvent(QCloseEvent* event);

private:
   /** Handles a change event.  This is used to capture when a
    * display gains focus.
    * @param aEvent aEvent the change event to handle.
    */
   void changeEvent(QEvent* aEvent) override;

   /** Callback for the viewer resizing.
    * @param aViewerPtr pointer to the viewer.
    */
   void ViewerResizedCB(vespa::VaViewer* aViewerPtr);

   /** Clears the callback with the given ID.
    * @param aId id of the callback to clear.
    */
   void ClearCallback(int& aId);

   // Misc variables
   vespa::VaViewer*        mViewerPtr;              ///< Pointer to the viewer.
   wkf::Platform*          mPlatformPtr;            ///< Pointer to the tethered platform.
   vespa::VaCallbackHolder mCallbacks;              ///< Callbacks for this object.
   size_t                  mConnectedPlatformIndex; ///< Name of the connected platform.
   bool mConnectionEstablished{false}; ///< Is the connection between this widget and the platform established?
   bool mInitialized{false};           ///< Is this display initialized?

   // Can't make a smart pointer because VaViewer calls delete on overlays.
   Overlay* mOverlayPtr{nullptr}; ///< Pointer to the ACES Display overlay that will do the drawing.
};
} // namespace AcesDisplay
} // namespace wkf
#endif // ACESDISPLAY_DOCKWIDGET_HPP
