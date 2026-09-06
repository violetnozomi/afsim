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
#ifndef WKFMAINWINDOWDIALOG_HPP
#define WKFMAINWINDOWDIALOG_HPP

#include "wkf_export.h"

#include <QDialog>

namespace wkf
{
class MainWindow;

class WKF_EXPORT Dialog : public QDialog
{
public:
   Dialog(MainWindow* parent, QAction* aTriggerActionPtr, const QString& aMenuTitle, Qt::WindowFlags f = Qt::WindowFlags());
   ~Dialog() override = default;

protected:
   virtual void Initialize();

   MainWindow* mMainWindowPtr;
   QAction*    mTriggerActionPtr;
   QString     mMenuTitle;
};
} // namespace wkf
#endif
