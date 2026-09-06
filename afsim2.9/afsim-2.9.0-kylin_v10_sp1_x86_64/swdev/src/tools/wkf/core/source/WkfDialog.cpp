// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WkfDialog.hpp"

#include <QAction>
#include <QMenu>

#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"

wkf::Dialog::Dialog(MainWindow*     parent,
                    QAction*        aTriggerActionPtr,
                    const QString&  aMenuTitle,
                    Qt::WindowFlags f /*= Qt::WindowFlags()*/)
   : QDialog(parent, f)
   , mMainWindowPtr(parent)
   , mTriggerActionPtr(aTriggerActionPtr)
   , mMenuTitle(aMenuTitle)
{
   mTriggerActionPtr->setParent(this);
   connect(&wkfEnv, &Environment::Initialize, this, &Dialog::Initialize);
   connect(mTriggerActionPtr, &QAction::triggered, this, &QDialog::show);
}

void wkf::Dialog::Initialize()
{
   QMenu* menuPtr = mMainWindowPtr->GetMenuByName(mMenuTitle);
   if (menuPtr)
   {
      menuPtr->addAction(mTriggerActionPtr);
   }
}
