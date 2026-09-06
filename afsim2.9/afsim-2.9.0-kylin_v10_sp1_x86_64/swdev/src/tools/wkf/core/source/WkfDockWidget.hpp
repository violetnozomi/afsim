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

#ifndef WKFDOCKWIDGET_HPP
#define WKFDOCKWIDGET_HPP

#include "wkf_export.h"

#include <QDockWidget>
#include <QMenu>

#include "VaCallbackHolder.hpp"

namespace wkf
{
class Action;

class WKF_EXPORT DockWidget : public QDockWidget
{
   Q_OBJECT
public:
   DockWidget(const QString& aObjectName, QMainWindow* aParent, Qt::WindowFlags aFlags, bool aDefaultStartDocked = false);
   DockWidget(const QString&  aObjectName,
              const QString&  aTypeName,
              QMainWindow*    aParent,
              Qt::WindowFlags aFlags,
              bool            aDefaultStartDocked = false);
   ~DockWidget() override = default;

   void FloatingChanged(bool aFloating);

   //! Convenience function to convert a DockWidget* to a quintptr
   //! @note This is useful when comparing DockWidget* to QVariants
   //! @return an unsigned integer representing the pointer's value
   quintptr Key() const noexcept { return reinterpret_cast<quintptr>(this); }

protected:
   //! Builds the context menu
   //! @param aMenu is the context menu to build
   //! @param aPos is the point where to create the context menu
   //! @return true if the subclass would like the standard dock widget menu
   //! @note override this if you want to add to the context menu
   virtual bool BuildContextMenu(QMenu& aMenu, const QPoint& aPos) { return true; }
   void         contextMenuEvent(QContextMenuEvent* event) override;

   void FullscreenActionTriggered();

   virtual void OverrideTitleBar(QWidget* aWidget);
   void         PlaceDockWidget(QMainWindow* aParent, const QString& aTypeName, bool aStartDocked);

   vespa::VaCallbackHolder mCallbacks;
   Action*                 mFullScreenActionPtr;

private:
   virtual void PrepareForDockStateChange(bool aFloating) {}
   void         BuildContextMenuP(QMenu& aMenu, const QPoint& pos);

   bool AsGoodAsFullscreen() const;

   QWidget* mHiddenBar;

   QRect mRect;
   bool  mWasFloating;

   QString mTypeName;
};
} // namespace wkf

#endif
