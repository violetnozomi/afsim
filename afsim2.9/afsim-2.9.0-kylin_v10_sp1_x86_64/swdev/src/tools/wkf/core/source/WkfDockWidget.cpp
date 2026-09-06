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

#include "WkfDockWidget.hpp"

#include <QApplication>
#include <QContextMenuEvent>
#include <QDesktopWidget>
#include <QMainWindow>

#include "UtQt.hpp"
#include "WkfAction.hpp"

wkf::DockWidget::DockWidget(const QString&  aObjectName,
                            const QString&  aTypeName,
                            QMainWindow*    aParent,
                            Qt::WindowFlags aFlags,
                            bool            aDefaultStartDocked)
   : QDockWidget(aParent, aFlags)
   , mRect(100, 100, 512, 512)
   , mTypeName(aTypeName)
{
   setObjectName(aObjectName);
   setMinimumSize(64, 64);

   mHiddenBar = new QWidget(this);
   setTitleBarWidget(mHiddenBar);
   show();
   PlaceDockWidget(aParent, aTypeName, aDefaultStartDocked);

   connect(this, &QDockWidget::topLevelChanged, this, &DockWidget::FloatingChanged);

   // Create a full screen option
   mFullScreenActionPtr =
      new Action(QIcon::fromTheme("fullscreen"), "Full Screen", this, QKeySequence(QKeySequence::FullScreen));
   mFullScreenActionPtr->setShortcutContext(Qt::WidgetWithChildrenShortcut);
   mFullScreenActionPtr->setCheckable(true);
   connect(mFullScreenActionPtr, &QAction::triggered, this, &DockWidget::FullscreenActionTriggered);
}

wkf::DockWidget::DockWidget(const QString&  aObjectName,
                            QMainWindow*    aParent,
                            Qt::WindowFlags aFlags,
                            bool            aDefaultStartDocked /*= false*/)
   : DockWidget(aObjectName, aObjectName, aParent, aFlags, aDefaultStartDocked)
{
}

void wkf::DockWidget::FloatingChanged(bool aFloating)
{
   PrepareForDockStateChange(aFloating);
   if (aFloating)
   {
      setGeometry(mRect);
      OverrideTitleBar(nullptr);
      show();
   }
   else
   {
      OverrideTitleBar(mHiddenBar);
   }
}

void wkf::DockWidget::FullscreenActionTriggered()
{
   if (isFullScreen())
   {
      showNormal();
      if (!mWasFloating)
      {
         setFloating(false);
      }
      else
      {
         setGeometry(mRect);
      }
   }
   else if (AsGoodAsFullscreen()) // if we some how got in a fullscreenish state that isn't fullscreen
   {
      showNormal();
      mRect = QRect(geometry().x() + 100, 100, 512, 512);
      if (!mWasFloating)
      {
         setFloating(false);
      }
      else
      {
         setGeometry(mRect);
      }
   }
   else
   {
      mWasFloating = isFloating();
      if (isFloating())
      {
         mRect = geometry();
      }
      setFloating(true);
      showFullScreen();
   }
}

void wkf::DockWidget::OverrideTitleBar(QWidget* aWidget)
{
   setTitleBarWidget(aWidget);
}

void wkf::DockWidget::PlaceDockWidget(QMainWindow* aParent, const QString& aTypeName, bool aStartDocked)
{
   if (aParent && !aParent->restoreDockWidget(this))
   {
      QList<wkf::DockWidget*> dockedItems = aParent->findChildren<wkf::DockWidget*>();
      bool                    found       = false;
      if (aStartDocked && !dockedItems.empty())
      {
         for (auto&& it : dockedItems)
         {
            if ((it != this) && (it->mTypeName == aTypeName))
            {
               found = true;
               aParent->tabifyDockWidget(it, this);
               raise();
               break;
            }
         }
         if (!found)
         {
            for (auto&& it : dockedItems)
            {
               if (it != this)
               {
                  found = true;
                  aParent->tabifyDockWidget(it, this);
                  raise();
                  break;
               }
            }
         }
      }
      if (!found)
      {
         aParent->addDockWidget(Qt::TopDockWidgetArea, this);
      }

      if (!aStartDocked)
      {
         setFloating(true);
         OverrideTitleBar(nullptr);
      }
   }
   else if (!isFloating())
   {
      OverrideTitleBar(mHiddenBar);
   }
   else
   {
      OverrideTitleBar(nullptr);
      if (!isFullScreen())
      {
         if (AsGoodAsFullscreen())
         {
            mWasFloating = true;
            mRect        = QRect(geometry().x(), 100, 512, 512);
            setGeometry(mRect);
            setFloating(true);
            showFullScreen();
         }
         else
         {
            mRect        = geometry();
            mWasFloating = true;
         }
      }
   }
}

void wkf::DockWidget::contextMenuEvent(QContextMenuEvent* aEvent)
{
   QMenu menu(this);
   UtQtTranslucentMenu(menu);
   setFocus();
   const QPoint& p = aEvent->pos();
   if (BuildContextMenu(menu, p))
   {
      if (!menu.actions().empty())
      {
         menu.addSeparator();
      }
      BuildContextMenuP(menu, p);
   }

   menu.exec(mapToGlobal(p));
}

// Qt doesn't restore full screen, but it will give you fullscreen-ish geometry.  Lets make sure that when this happens
// we actually return the widget to fullscreen.
bool wkf::DockWidget::AsGoodAsFullscreen() const
{
   if (!isFullScreen())
   {
      int sn = QApplication::desktop()->screenNumber(this);
      if (sn == -1)
      {
         return true; // we better restore some safe geoemtry;
      }
      else
      {
         QRect geom       = geometry();
         QRect screenGeom = QApplication::desktop()->screenGeometry(sn);
         if ((geom.width() >= screenGeom.width()) && geom.height() >= screenGeom.height())
         {
            return true;
         }
      }
   }
   return false;
}

void wkf::DockWidget::BuildContextMenuP(QMenu& aMenu, const QPoint& pos)
{
   // The contents of the Context Menu differ based on whether the central widget is displaying the dock
   // widget or whether it is separate.
   if (!isFloating()) // Dock Widget is displaying the contents widget
   {
      Action* action = new Action(QIcon::fromTheme("move"), "Move to New Window", &aMenu);
      connect(action, &QAction::triggered, this, [this]() { setFloating(true); });
      aMenu.addAction(action);

      auto* hideAction = new QAction(QIcon::fromTheme("hide"), "Hide", &aMenu);
      connect(hideAction, &QAction::triggered, this, &DockWidget::hide);
      aMenu.addAction(hideAction);
   }

   mFullScreenActionPtr->setChecked(isFullScreen());
   aMenu.addAction(mFullScreenActionPtr);
}
