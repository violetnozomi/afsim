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
#ifndef UTQTDOCKCONTROL_HPP
#define UTQTDOCKCONTROL_HPP

#include "utilqt_export.h"

#include <QWidget>

class UtQtDockTitleWidget;
class UtQtDockArea;

//! Wraps a dockable control window in a parent widget
class UTILQT_EXPORT UtQtDockControl : public QWidget
{
public:
   friend class UtQtDockArea;
   friend class UtQtDockTitleWidget;

   enum WindowState
   {
      cDOCKED,
      cDRAGGING,
      cWINDOW
   };

   UtQtDockControl(UtQtDockArea* aControlListPtr, QWidget* aWidgetPtr, bool aFixedSize = false);
   QWidget* widget() { return mWidgetPtr; }

   void ToWindow(bool aKeepPosition = true, bool aKeepHidden = false);
   void ToControl(bool aKeepHidden = false);
   void Initialize();
   void SetVisible(bool mIsVisible);
   void Minimize()
   {
      if (!mIsMinimized)
         Restore();
   }
   void Maximize()
   {
      if (mIsMinimized)
         Restore();
   }
   bool Dockable() const { return mDockable; }
   //! Sets if the control can be docked
   void SetDockable(bool aIsDockable) { mDockable = aIsDockable; }

   void SetAlwaysOnTop();

   void SetMinimumWindowSize(QSize aSize) { mMinWindowSize = aSize; }
   void mouseMoveEvent(QMouseEvent*) override;
   void mousePressEvent(QMouseEvent*) override;
   void mouseReleaseEvent(QMouseEvent*) override;
   void moveEvent(QMoveEvent*) override;
   void closeEvent(QCloseEvent*) override;
#ifdef _WIN32
   virtual bool event(QEvent*);
#endif
#if defined(Q_WS_X11)
   virtual bool x11Event(XEvent*);
#endif
   WindowState GetWindowState() const { return mWindowState; }

   bool Hidden() const { return mHidden; }
   bool IsMinimized() const { return mIsMinimized; }

protected:
   void Restore();
   void UpdateSize(bool aWindow);

   UtQtDockArea*        mParent;
   QWidget*             mWidgetPtr;
   UtQtDockTitleWidget* mTitlePtr;
   //! True if the control is being dragged over the docking area.
   bool mIsHovering;
   //! True if the control is minimized.
   bool mIsMinimized;
   bool mHidden;
   bool mDockable;
   bool mFixedSize;
   bool mStartDrag;
   //! for an X11 workaround
   bool        mTitleMove;
   QSize       mRestoreSize;
   WindowState mWindowState;
   QPoint      mAnchor;

   QSize mMinWindowSize;
};
#endif
