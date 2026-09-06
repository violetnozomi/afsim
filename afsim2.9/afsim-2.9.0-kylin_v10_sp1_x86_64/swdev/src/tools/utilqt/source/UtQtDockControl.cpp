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

#include "UtQtDockControl.hpp"

#include <QApplication>
#include <QPushButton>
#include <QScrollBar>
#include <QStyleOption>
#include <QStyleOptionTitleBar>
#include <QStylePainter>
#include <QVBoxLayout>
#include <QtCore/QSignalMapper>
#include <QtEvents>

#include "UtQt.hpp"
#include "UtQtDockArea.hpp"

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#endif

// **** Note ****
// Getting docking widgets to work requires using some very gray areas of Qt only found by trial and error.
// Currently the solution which works for windows and Linux is as follows:
// A docked widget must draw the title bar explicitly.  In the docked state, handling the user's click&drag
// is done explicitly using mousePressEvent, mouseReleaseEvent, and mouseMoveEvent.
// If the widget is floating, it must be a full window with a title bar (or Gnome may mis-handle the widget).
// So while dragging, a widget is given a real title bar, and the fake title bar is hidden.
// When a user drags the real title bar, we must handle drag & drop a bit differently using
// platform-specific methods (event() and x11event())
// **** **** ****

//! The window title for a control widget.
class UtQtDockTitleWidget : public QWidget
{
public:
   UtQtDockTitleWidget(UtQtDockControl* parent, int w, int h);

   void paintEvent(QPaintEvent*) override;
   void resizeEvent(QResizeEvent*) override;

   void UpdateButtons();
   void CloseClick();
   void RestoreClick();

   UtQtDockControl* mControlPtr;

   QPushButton* mCloseButton;
   QPushButton* mRestoreButton;
};


UtQtDockTitleWidget::UtQtDockTitleWidget(UtQtDockControl* aControlPtr, int w, int h)
   : QWidget(aControlPtr, nullptr)
   , mControlPtr(aControlPtr)
{
   resize(w, h);
   setFixedHeight(h);
   setMinimumWidth(80);
   int     cBUTTON_SIZE = 16;
   QPixmap closeIcon    = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
   mCloseButton         = new QPushButton(closeIcon, "", this);
   mCloseButton->setGeometry(width() - 18, 2, cBUTTON_SIZE, cBUTTON_SIZE);
   QIcon restoreIcon = style()->standardIcon(QStyle::SP_TitleBarNormalButton);
   QIcon minIcon     = style()->standardIcon(QStyle::SP_TitleBarMinButton);
   mRestoreButton    = new QPushButton(minIcon, "", this);
   mRestoreButton->setGeometry(mCloseButton->x() - cBUTTON_SIZE - 2, 2, cBUTTON_SIZE, cBUTTON_SIZE);

   QObject::connect(mCloseButton, &QPushButton::clicked, this, &UtQtDockTitleWidget::CloseClick);
   QObject::connect(mRestoreButton, &QPushButton::clicked, this, &UtQtDockTitleWidget::RestoreClick);
}
void UtQtDockTitleWidget::UpdateButtons()
{
   if (!mControlPtr->mIsMinimized)
   {
      QIcon minIcon = style()->standardIcon(QStyle::SP_TitleBarMinButton);
      mRestoreButton->setIcon(minIcon);
   }
   else
   {
      QIcon restoreIcon = style()->standardIcon(QStyle::SP_TitleBarNormalButton);
      mRestoreButton->setIcon(restoreIcon);
   }
}
void UtQtDockTitleWidget::CloseClick()
{
   mControlPtr->SetVisible(false);
}

void UtQtDockTitleWidget::RestoreClick()
{
   mControlPtr->Restore();
}

void UtQtDockTitleWidget::resizeEvent(QResizeEvent* e)
{
   mCloseButton->move(width() - 18, 2);
   mRestoreButton->move(mCloseButton->x() - mCloseButton->width() - 2, 2);
   QWidget::resizeEvent(e);
}

void UtQtDockTitleWidget::paintEvent(QPaintEvent* e)
{
   QStyleOptionTitleBar compOp;
   compOp.initFrom(this);
   compOp.rect          = geometry();
   compOp.text          = mControlPtr->mWidgetPtr->windowTitle();
   compOp.titleBarFlags = Qt::Tool;
   compOp.subControls |= QStyle::SC_TitleBarCloseButton | QStyle::SC_TitleBarSysMenu;
   compOp.activeSubControls |= QStyle::SC_TitleBarCloseButton | QStyle::SC_TitleBarSysMenu;
   compOp.titleBarFlags = Qt::Tool | Qt::WindowTitleHint | Qt::CustomizeWindowHint;
   compOp.titleBarState |= QStyle::State_Active;
   QStylePainter p(this);
   p.drawComplexControl(QStyle::CC_TitleBar, compOp);

   QWidget::paintEvent(e);
}

UtQtDockControl::UtQtDockControl(UtQtDockArea* aControlListPtr, QWidget* aWidgetPtr, bool aFixedSize)
   : QWidget(aControlListPtr->GetScrollArea())
   , mWidgetPtr(aWidgetPtr)
   ,
   // mIsFloating(false),
   mIsHovering(false)
   , mIsMinimized(false)
   , mHidden(false)
   , mDockable(true)
   , mFixedSize(aFixedSize)
   , mStartDrag(false)
   , mWindowState(cDOCKED)
{
   mTitleMove = false;
   setWindowTitle(aWidgetPtr->windowTitle());
}

//***** HACK *****
// event() and x11Event() are used to determine when the user has stopped dragging
// the window.  moveEvent() is called as the user drags the window, but there is no standard
// event fired when the user lets to of the mouse button.  The following are two workarounds
// for win32 and X11:
//***** HACK *****
#ifdef _WIN32
bool UtQtDockControl::event(QEvent* e)
{
   // This is a hack because we do not get the events we need.
   // NonClientAreaMouseMove will be fired after the user lets go of the left mouse button,
   // so we are using this to trigger docking the window if necessary.
   if (e->type() == QEvent::NonClientAreaMouseMove)
   {
      if (mWindowState == cDRAGGING)
      {
         if (mIsHovering)
         {
            ToControl();
            mWindowState = cDOCKED;
         }
         else
         {
            mWindowState = cWINDOW;
         }
      }
   }
   return QWidget::event(e);
}
#endif

#ifdef Q_WS_X11
bool UtQtDockControl::x11Event(XEvent* e)
{
   if (mWindowState == cDRAGGING && mTitleMove)
   {
      if (e->type == FocusIn)
      {
         if (mWindowState == cDRAGGING)
         {
            if (mIsHovering)
            {
               ToControl();
               mWindowState = cDOCKED;
            }
            else
            {
               mWindowState = cWINDOW;
            }
         }
      }
      mTitleMove = false;
   }
   return QWidget::x11Event(e);
}
#endif

//! This event executes when the window is moved by the user using the real title-bar
void UtQtDockControl::moveEvent(QMoveEvent* e)
{
   //! spontaneous() is true when the move is caused by the user
   if (e->spontaneous())
   {
      mTitleMove   = true;
      mWindowState = cDRAGGING;
      // mMoving = true;
      mParent->WindowMove(QCursor::pos(), this);
   }
   QWidget::moveEvent(e);
}

//! Moving the docked widget is done manually using this event.  moveEvent() is used when the control is in window mode.
void UtQtDockControl::mouseMoveEvent(QMouseEvent* e)
{
   if (mWindowState == cDRAGGING || mStartDrag)
   {
      grabMouse();
      ToWindow();
      mWindowState = cDRAGGING;
      mStartDrag   = false;
      move(pos() + (e->globalPos() - mAnchor));
      mAnchor = e->globalPos();
      mParent->WindowMove(e->globalPos(), this);
   }
   QWidget::mouseMoveEvent(e);
}

void UtQtDockControl::mousePressEvent(QMouseEvent* e)
{
   if (mWindowState == cDOCKED && e->y() < 20)
   {
      mAnchor    = e->globalPos();
      mStartDrag = true;
   }
   QWidget::mousePressEvent(e);
}

void UtQtDockControl::mouseReleaseEvent(QMouseEvent* e)
{
   if (mWindowState == cDRAGGING)
   {
      releaseMouse();
      if (mIsHovering)
      {
         ToControl();
         mWindowState = cDOCKED;
      }
      else
      {
         mWindowState = cWINDOW;
      }
   }
   QWidget::mouseReleaseEvent(e);
}
void UtQtDockControl::closeEvent(QCloseEvent* e)
{
   if (e->spontaneous())
   {
      SetVisible(false);
      e->ignore();
   }
   else
   {
      QWidget::closeEvent(e);
   }
}
//! Transform to a floating window.
void UtQtDockControl::ToWindow(bool aKeepPosition, bool aKeepHidden)
{
   if (parent() == nullptr)
   {
      mWindowState = cWINDOW;
      return;
   }
   Maximize();

   // For controls that start floating, we would like the window manager to choose a position.
   // Unfortunately this doesn't seem to work and all the windows pop up at the same location
   if (aKeepPosition)
   {
      QPoint globalPos = mapToGlobal(QPoint());
      setParent(nullptr);
      move(globalPos);
   }
   else
   {
      setParent(nullptr);
   }
   // Windows will let the control go above the screen (due to the title bar being removed?)
   if (pos().y() < 20)
   {
      move(pos().x(), 20);
   }

   // Compute the size of the wrapping window.  Set max and min sizes to allow resizing.
   UpdateSize(true);

   mTitlePtr->hide();
   setWindowFlags(windowFlags() /*| Qt::WindowStaysOnTopHint*/);
   if (!aKeepHidden)
   {
      show();
   }
   mWindowState = cWINDOW;
}
void UtQtDockControl::UpdateSize(bool aWindow)
{
   QSize titleHeight(0, 0);
   if (!aWindow)
   {
      titleHeight = mTitlePtr->minimumSize();
      titleHeight.setWidth(0);
   }

   QSize minWidgetSize = mWidgetPtr->minimumSizeHint().expandedTo(mWidgetPtr->minimumSize());
   // Qt complains if a size is larger than this, so bound to this number
   QSize absoluteMaximumSize(16777215, 16777215);
   setMaximumSize((mWidgetPtr->maximumSize() + titleHeight).boundedTo(absoluteMaximumSize));
   if (!aWindow)
   {
      setMinimumSize(titleHeight); // allow to be minimized
      QSize newSize(minWidgetSize + titleHeight);
      newSize.setWidth(mParent->GetScrollArea()->width());
      resize(newSize);
   }
   else
   {
      if (!mMinWindowSize.isValid())
      {
         setMinimumSize((minWidgetSize + titleHeight).boundedTo(absoluteMaximumSize));
      }
      else
      {
         setMinimumSize(mMinWindowSize);
      }
   }
}

//! Transform to a docked control
void UtQtDockControl::ToControl(bool aKeepHidden)
{
   mTitlePtr->show();
   setParent(mParent->GetScrollArea());
   mIsHovering  = false;
   mWindowState = cDOCKED;
   // resize(mParent->GetScrollArea()->width(), height());
   UpdateSize(false);
   mParent->UpdateLayout();
   if (!aKeepHidden)
   {
      show();
   }
}

void UtQtDockControl::Initialize()
{
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->setMargin(0);
   layout->setSpacing(0);
   setLayout(layout);
   int w     = mParent->GetScrollArea()->width();
   mTitlePtr = new UtQtDockTitleWidget(this, w, 20);
   mWidgetPtr->move(0, 20);
   mWidgetPtr->setParent(this);
   layout->addWidget(mTitlePtr);
   layout->addWidget(mWidgetPtr);
   resize(w, mWidgetPtr->height() + 20);
   setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void UtQtDockControl::Restore()
{
   if (!mIsMinimized)
   {
      mRestoreSize = size();
      layout()->removeWidget(mWidgetPtr);
      mWidgetPtr->hide();
      resize(width(), 20);
      // Yes, this is needed due to a bug in Qt.  If there is a min-height on the widget,
      // the layout system doesn't realize the widget is gone until the second call to resize.
      resize(width(), 20);
   }
   else
   {
      layout()->addWidget(mWidgetPtr);
      mWidgetPtr->show();
      resize(mRestoreSize);
   }
   mIsMinimized = !mIsMinimized;
   mTitlePtr->UpdateButtons();
   if (mWindowState == cDOCKED || mIsHovering)
   {
      mParent->UpdateLayout();
   }
}

void UtQtDockControl::SetVisible(bool aIsVisible)
{
   mHidden = !aIsVisible;
   setVisible(aIsVisible);
   mParent->UpdateLayout();
   mParent->ControlVisibleChange();
}

void UtQtDockControl::SetAlwaysOnTop()
{
   Qt::WindowFlags f = windowFlags();
   setWindowFlags(f | Qt::WindowStaysOnTopHint);
}
