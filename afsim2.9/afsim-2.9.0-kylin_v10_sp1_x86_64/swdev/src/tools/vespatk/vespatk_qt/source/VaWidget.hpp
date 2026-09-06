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

#ifndef VAWIDGET_HPP
#define VAWIDGET_HPP

#include "vespatk_qt_export.h"

#include "UtQtOpenGLWidget.hpp"
class QInputEvent;
#include <QTime>
#include <osgViewer/Viewer>

#include "VaChooser.hpp"
#include "VaViewer.hpp"

namespace vespa
{
class VaWidgetChooser : public VaChooser
{
public:
   VaWidgetChooser(QWidget* aWidgetPtr)
      : VaChooser()
      , mWidgetPtr(aWidgetPtr)
   {
   }
   virtual ~VaWidgetChooser() = default;
   virtual VaHitEntry Choose(const std::map<VaHitEntry, std::string>& aChoiceList,
                             int                                      aX,
                             int                                      aY,
                             const std::function<void(VaHitEntry)>*   aHoverFunction =
                                nullptr) const override; // allow a user to choose from a list
private:
   QWidget* mWidgetPtr;
};

class VESPATK_QT_EXPORT VaWidget : public QOpenGLWidget
{
   Q_OBJECT
public:
   // if this is the main view, and it may be hidden to start, you may want to prime the context to get vespa to work
   // properly, otherwise don't worry about it
   VaWidget(unsigned int aViewerType, unsigned int aViewerModes, QWidget* aParentPtr, bool aPrimeContext = false);
   // VTK will take ownership of the VaViewer
   VaWidget(VaViewer* aViewerPtr, QWidget* aParentPtr, bool aPrimeContext = false);

   ~VaWidget() override;

   VaViewer* GetViewer();

   void BlockRightClick(bool aState) { mBlockRightClick = aState; }

   /** When enabled, checks that the mouse must move past a certain threshold
    * (default 10 pixels) before clicks are disabled in mouseMoveEvent. This
    * will help catch clicks with spurious mouse movement, but will also delay
    * the start of dragging by the threshold.
    * @param aState state to set: true to check for the threshold, false otherwise.
    */
   void CheckDragThreshold(bool aState) { mCheckDragThreshold = aState; }

   VaChooser* GetChooser();

signals:
   void Initialized();
   void DragEnter(QDragEnterEvent* aEvent);
   void DragDrop(QDropEvent* aEvent);
   void DragMove(QDragMoveEvent* aEvent);
   void DragLeave(QDragLeaveEvent* aEvent);

protected:
   void paintGL() override;
   void resizeGL(int aWidth, int aHeight) override;
   void initializeGL() override;

   bool event(QEvent* aEvent) override;

   void               wheelEvent(QWheelEvent* aEvent) override;
   void               mousePressEvent(QMouseEvent* aEvent) override;
   void               mouseReleaseEvent(QMouseEvent* aEvent) override;
   void               mouseMoveEvent(QMouseEvent* aEvent) override;
   void               mouseDoubleClickEvent(QMouseEvent* aEvent) override;
   void               keyPressEvent(QKeyEvent* aEvent) override;
   void               keyReleaseEvent(QKeyEvent* aEvent) override;
   void               enterEvent(QEvent* aEvent) override;
   void               leaveEvent(QEvent* aEvent) override;
   void               dragEnterEvent(QDragEnterEvent* aEvent) override;
   void               dropEvent(QDropEvent* aEvent) override;
   void               dragMoveEvent(QDragMoveEvent* aEvent) override;
   void               dragLeaveEvent(QDragLeaveEvent* aEvent) override;
   void               MouseMove(int aX, int aY, int aMods, int aButtons);
   void               SetViewer(VaViewer* aViewerPtr);
   osgViewer::Viewer* GetOsgViewer() { return mOsgViewer; }

private:
   VaWidget(QWidget* aParentPtr, bool aPrimeContext = false);
   void ViewerDestroyedCB(VaViewer* aViewerPtr);

   osgViewer::GraphicsWindowEmbedded* mGraphicsWindow;
   osgViewer::Viewer*                 mOsgViewer;
   VaViewer*                          mVaViewer{nullptr};
   bool                               mInitialized{false};

   int mHighDPI_ScalingFactor;

   QTime mButtonDownTime[3];
   // The position of the mouse cursor, relative to this widget, when the mouse last moved
   QPoint                  mLastMouseMove;
   bool                    mMiddleDown{false};
   bool                    mRightDown{false};
   bool                    mLeftDown{false};
   bool                    mClickPossible[3];
   vespa::VaCallbackHolder mCallbacks;
   bool                    mBlockRightClick{false};
   VaWidgetChooser         mChooser;
   bool                    mCheckDragThreshold{false};
};
} // namespace vespa

#endif
