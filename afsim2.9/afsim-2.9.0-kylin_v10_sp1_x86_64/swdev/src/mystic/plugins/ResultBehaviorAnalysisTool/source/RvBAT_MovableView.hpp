// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef RVBAT_MOVABLEVIEW_HPP
#define RVBAT_MOVABLEVIEW_HPP

#include <QGraphicsView>
#include <QMap>
#include <QMenu>
#include <QObject>
#include <QTime>

#include "RvBAT_Scene.hpp"
#include "RvEventPipeClasses.hpp"

namespace rv
{
class ResultData;
}

namespace RvBAT
{
class Blackboard;
class DockWindow;
class GraphicsNode;
class GraphicsState;
class Interface;
class MovableView : public QGraphicsView
{
   Q_OBJECT

public:
   explicit MovableView(Blackboard* aBlackboardPtr, DockWindow* aDockWindowPtr);
   ~MovableView() override = default;

   /** Update based on sim time.
    * @param aData Time used to update
    */
   void Update(const rv::ResultData& aData);

   /** Return a pointer to the interface. */
   const Interface* GetInterfacePtr() const { return mInterfacePtr; }

   /** Set the interface pointer.
    * @param aInterfacePtr interface to point to for data
    */
   void SetInterfacePtr(Interface* aInterfacePtr) { mInterfacePtr = aInterfacePtr; }

   /** Resize the window, using the QDialog window.
    * @param aSize Size to resize window to
    */
   void ResizeWindow(const QSize aSize);

   /** Go to the scene for a platform, given a string.
    * @param aStr string of the scene to swap to
    */
   void GoToComboBox(const QString& aStr);

   /** Returns true if the btt has been loaded. */
   const bool& IsLoaded() const { return mIsLoaded; }

   /** Return the current platform to show btts for. */
   const std::string& GetCurrentPlatform() const { return mCurrentPlatform; }

   /** Set the current platform to show btts for. */
   void SetCurrentPlatform(const std::string& aValue) { mCurrentPlatform = aValue; }

   /** Sets if the btt has been loaded.
    * @param  aValue true/false
    */
   void SetLoaded(const bool aValue) { mIsLoaded = aValue; }

   /** Redraw the scene.  This occurs when the theme changes. */
   void RedrawVisuals();

   /** Center the scene in the window. */
   void CenterSceneInWindow();

   /** Reset the layout of the current behavior tree in the scene. */
   void ResetLayout();

   /** This is called when Plugin::ClearScenario is called. */
   void Reset();

   /** Build the context menu.
    * @param aMenu menu to add to
    * @param aPos position to add the menu
    */
   bool BuildContextMenu(QMenu& aMenu, const QPoint& aPos);
public slots:
   /** Triggered when a different platform is chosen.
    * @param aStr string of the newly selected value in the Combo Box
    */
   void comboBoxChangedIndex(const QString& aStr);

   /** Triggered when a different scene is chosen for a platform.
    * @param aStr string of the newly selected scene
    */
   void SceneComboBoxChangedIndex(const QString& aStr);

   /** Load the scene config for a scene.  This will
    * set up the layout and zoom appropriately.
    */
   void LoadSceneConfig();

   /** Load the current scene for a platform by swapping
    * to the appropriate scene combo box option.
    */
   void LoadSceneComboBox();

protected:
   bool eventFilter(QObject* aWatchedObject, QEvent* aEvent) override;

   /** Key Pressed Event.
    * @param aEvent Key event to handle
    */
   void keyPressEvent(QKeyEvent* aEvent) override;

   /** Key Released Event.
    * @param aEvent Key event to handle
    */
   void keyReleaseEvent(QKeyEvent* aEvent) override;

   /** Mouse Moved Event.
    * @param aEvent Mouse event to handle
    */
   void mouseMoveEvent(QMouseEvent* aEvent) override;

   /** Mouse Button Pressed Event.
    * @param aEvent Mouse event to handle
    */
   void mousePressEvent(QMouseEvent* aEvent) override;

   /** Mouse Button Released Event.
    * @param aEvent Mouse event to handle
    */
   void mouseReleaseEvent(QMouseEvent* aEvent) override;

   /** Mouse Wheel Scrolled Event.
    * @param aEvent Wheel event to handle
    */
   void wheelEvent(QWheelEvent* aEvent) override;

   /** Toggle if a nodes children are visible. */
   void ToggleSelectedNodeChildrenVisible();

private:
   /** Get the current scale. */
   const qreal GetViewScale() const { return mViewScale; }

   /** Zoom into the mouse cursor.
    * @param aScaleFactor Scale Factor value of how much to zoom in or out
    */
   void GentleZoom(const float aScaleFactor);

   /** Pan the view.
    * @param aDelta How far the user panned the window
    */
   void Pan(QPointF aDelta);

   /** Zoom based on a float factor.
    * @param aScaleFactor Scale Factor value of how much to zoom in or out
    */
   void Zoom(const float aScaleFactor);

   /** Zoom in. */
   void ZoomIn();

   /** Zoom out. */
   void ZoomOut();

   /** Create a scene and add it to the map.
    * @param aId id of the scene
    * @param aSceneType scene type of the scene (ABT or FSM)
    * @param aPlatformIndex index of the platform the scene is being added for
    */
   void CreateScene(const size_t aId, const SceneType aSceneType, const size_t aPlatformIndex);

   /** Load a scene, given an id and scene type.
    * @param aId id of the scene
    * @param aSceneType scene type of the scene (ABT or FSM)
    */
   void LoadScene(const size_t aId, const SceneType aSceneType);

   bool                       mIsPanning{false};            ///< Flag to determine if panning should be done.
   bool                       mIsZooming{false};            ///< Flag to determine if zooming should be done.
   QPoint                     mLastMousePos;                ///< Last mouse position
   qreal                      mViewScale{1.0};              ///< Scale of the view
   std::string                mCurrentPlatform;             ///< Currently selected platform name (For combo box)
   QMap<std::string, QString> mCurrentScene;                ///< Platform to a combo box name for a scene
   qreal                      mPanSpeed{1.0};               ///< Panning speed
   qreal                      mZoomDelta{0.1};              ///< Zooming delta
   qreal                      mZoomKey{Qt::Key_Z};          ///< Zooming keybind
   bool                       mIsLoaded{false};             ///< Is the BAT loaded?
   Qt::MouseButton            mPanButton{Qt::MiddleButton}; ///< Panning mouse button
   QPointF                    mTargetScenePos;              ///< Scene coordinates of target zoom point
   QPointF                    mTargetViewportPos;           ///< Viewport coordinates of target zoom point
   int                        mScreenWidth;                 ///< Screen width in pixels
   Scene*                     mActiveScene{nullptr};        ///< Active scene in the view
   std::vector<Scene*>        mScenes;                 ///< List of scenes that have been created so far for the view.
   Interface*                 mInterfacePtr{nullptr};  ///< Pointer to the interface, for getting BAT data
   DockWindow*                mDockWindowPtr{nullptr}; ///< Pointer to the dock window holding the view
   Qt::MouseButton            mMouseButton{Qt::NoButton}; ///< Button registered on mousePressEvent

   // Blackboard vars
   Blackboard* mBlackboardPtr{nullptr}; ///< Pointer to the blackboard for filling in data.
   size_t mPreviousStateMessageIndex{0}; ///< Tracks the previous state message index so we don't update when we don't need to.

   // Selection/Context menu vars
   QPoint mLastMouseMove; ///< The position of the mouse cursor, relative to this widget, when the mouse last moved
   bool   mObjectClicked{false};          ///< True if a node has a mouse click event currently, false otherwise.
   bool   mObjectDragging{false};         ///< True if a node has passed a certain length and is dragging.
   GraphicsNode*  mSelectedNode{nullptr}; ///< Pointer to the currently highlighted node.
   QPoint         mLastObjectPos;         ///< Last Node position when clicked on.
   GraphicsState* mContextState{nullptr}; ///< Pointer to the state with the context menu open.
};
} // namespace RvBAT
#endif // RVBAT_MOVABLEVIEW_HPP
