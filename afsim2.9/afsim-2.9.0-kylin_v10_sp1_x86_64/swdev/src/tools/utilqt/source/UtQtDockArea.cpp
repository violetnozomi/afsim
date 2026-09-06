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
#include "UtQtDockArea.hpp"

#include <algorithm>

#include "UtQtDockControl.hpp"


UtQtDockArea::UtQtDockArea(QWidget* p)
   : QScrollArea(p)
{
   setGeometry(p->width() - 275, 20, 270, p->height() - 20);

   mScrolledArea = new QWidget();
   this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   // There seems to be no way to get the scrollbar width here.  Qt returns
   // 100 pix, so for now just pick a standard width and hope it's close enough.
   // mScrolledArea->setFixedWidth(width()-15);
   mScrolledArea->setFixedWidth(260);
   setWidget(mScrolledArea);
}

UtQtDockArea::~UtQtDockArea()
{
   for (size_t i = 0; i < mControls.size(); ++i)
   {
      delete mControls[i];
   }
}

void UtQtDockArea::AddControl(UtQtDockControl* aDockControlPtr)
{
   aDockControlPtr->mParent = this;
   mControls.push_back(aDockControlPtr);
   aDockControlPtr->Initialize();
   UpdateLayout();
}

void UtQtDockArea::AddControlAfter(UtQtDockControl* aWhereDockControlPtr, UtQtDockControl* aDockControlPtr)
{
   aDockControlPtr->mParent = this;

   // Find the 'where' control.
   std::vector<UtQtDockControl*>::iterator it = std::find(mControls.begin(), mControls.end(), aWhereDockControlPtr);
   if (it != mControls.end())
   {
      ++it; // Move ahead one, since 'insert' inserts before the iterator position.
   }
   mControls.insert(it, aDockControlPtr);

   aDockControlPtr->Initialize();
   UpdateLayout();
}

void UtQtDockArea::AddControlBefore(UtQtDockControl* aWhereDockControlPtr, UtQtDockControl* aDockControlPtr)
{
   aDockControlPtr->mParent = this;

   // Find the 'where' control.
   std::vector<UtQtDockControl*>::iterator it = std::find(mControls.begin(), mControls.end(), aWhereDockControlPtr);
   mControls.insert(it, aDockControlPtr);

   aDockControlPtr->Initialize();
   UpdateLayout();
}

UtQtDockControl* UtQtDockArea::First() const
{
   UtQtDockControl* firstControlPtr = nullptr;
   if (!mControls.empty())
   {
      firstControlPtr = mControls[0];
   }
   return firstControlPtr;
}

UtQtDockControl* UtQtDockArea::Find(QString aTitle) const
{
   for (size_t i = 0; i < mControls.size(); ++i)
   {
      UtQtDockControl* cw = mControls[i];
      if (cw->widget()->windowTitle() == aTitle)
      {
         return cw;
      }
   }
   return nullptr;
}
UtQtDockControl* UtQtDockArea::FindByName(const std::string& aName) const
{
   QString nameStr = QString::fromStdString(aName);
   for (size_t i = 0; i < mControls.size(); ++i)
   {
      UtQtDockControl* cw = mControls[i];
      if (cw->objectName() == nameStr)
      {
         return cw;
      }
   }
   return nullptr;
}
//! Update the position of docked controls
void UtQtDockArea::UpdateLayout()
{
   int y = 0;
   for (size_t i = 0; i < mControls.size(); ++i)
   {
      UtQtDockControl* cw = mControls[i];
      if (cw->GetWindowState() != UtQtDockControl::cWINDOW && !cw->mHidden)
      {
         if (cw->GetWindowState() != UtQtDockControl::cDRAGGING)
         {
            if (cw->y() != y)
            {
               cw->move(0, y);
            }
            y += cw->height();
         }
         else if (cw->mIsHovering)
         {
            y += cw->height();
         }
      }
      else
      {
         if (cw->mIsHovering)
         {
            y += cw->height();
         }
      }
   }
   mScrolledArea->resize(mScrolledArea->width(), y);
}
void UtQtDockArea::WindowMove(QPoint point, UtQtDockControl* aControlPtr)
{
   QPoint local = mapFromGlobal(point);
   bool   hover = false;
   if (local.x() > 0 && local.y() > 0 && local.x() < width() && local.y() < height() && aControlPtr->Dockable() &&
       isVisible())
   {
      hover = true;
   }
   ReorderControl(aControlPtr, local.y() - mScrolledArea->y());
   aControlPtr->mIsHovering = hover;
   UpdateLayout();
}

void UtQtDockArea::ReorderControls(const std::vector<UtQtDockControl*>& aControlOrder)
{
   std::vector<UtQtDockControl*> newList = aControlOrder;
   for (size_t i = 0; i < mControls.size(); ++i)
   {
      UtQtDockControl* controlPtr = mControls[i];
      if (std::find(newList.begin(), newList.end(), controlPtr) == newList.end())
      {
         newList.push_back(controlPtr);
      }
   }
   mControls = newList;
   UpdateLayout();
}

void UtQtDockArea::ReorderControl(UtQtDockControl* aControl, int posY)
{
   size_t i = 0;
   int    y = 0;
   for (; i < mControls.size(); ++i)
   {
      UtQtDockControl* cw = mControls[i];
      if (cw->GetWindowState() != UtQtDockControl::cWINDOW && cw != aControl && !cw->mHidden)
      {
         int h = cw->height();
         if (posY < y + h / 2)
            break;
         y += h;
      }
   }
   mControls.insert(mControls.begin() + i, aControl);
   for (size_t j = 0; j < mControls.size(); ++j)
   {
      if (j != i && mControls[j] == aControl)
      {
         mControls.erase(mControls.begin() + j);
      }
   }
}
