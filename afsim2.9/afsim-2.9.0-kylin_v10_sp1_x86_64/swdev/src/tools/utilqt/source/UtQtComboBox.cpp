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

#include "UtQtComboBox.hpp"

#include <vector>

#include <QAbstractItemView>
#include <QWheelEvent>

#include "UtQt.hpp"

UtQtComboBox::UtQtComboBox(QWidget* parent)
   : QComboBox(parent)
{
   mAutoPopup         = false;
   mDisableMouseWheel = false;
   SetAutoPopup(true);
}

void UtQtComboBox::mouseMoveEvent(QMouseEvent* e)
{
   if (mAutoPopup && !view()->isVisible())
   {
      BeginPopup();
      mCheckMouseTimer.start(100);
      showPopup();
   }
   else
   {
      QComboBox::mouseMoveEvent(e);
   }
}

void UtQtComboBox::CheckMouseProximity()
{
   if (view()->isVisible())
   {
      QRect editBox(mapToGlobal(rect().topLeft()), mapToGlobal(rect().bottomRight()));
      QRect viewBox(view()->mapToGlobal(view()->rect().topLeft()), view()->mapToGlobal(view()->rect().bottomRight()));
      if (!viewBox.contains(QCursor::pos()) && !editBox.contains(QCursor::pos()))
      {
         hidePopup();
         mCheckMouseTimer.stop();
      }
   }
   else
   {
      mCheckMouseTimer.stop();
   }
}

void UtQtComboBox::SetAutoPopup(bool aAutoPopup)
{
   if (mAutoPopup == aAutoPopup)
      return;
   mAutoPopup = aAutoPopup;
   if (mAutoPopup)
   {
      setMouseTracking(true);
      QObject::connect(&mCheckMouseTimer, &QTimer::timeout, this, &UtQtComboBox::CheckMouseProximity);
   }
   else
   {
      setMouseTracking(false);
   }
}

void UtQtComboBox::DisableMouseWheel()
{
   mDisableMouseWheel = true;
   setFocusPolicy(Qt::StrongFocus);
}

void UtQtComboBox::wheelEvent(QWheelEvent* event)
{
   if (mDisableMouseWheel && !hasFocus())
   {
      event->ignore();
   }
   else
   {
      QComboBox::wheelEvent(event);
   }
}
