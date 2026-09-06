// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef RV_WATERFALLPLOTDIALOG_HPP
#define RV_WATERFALLPLOTDIALOG_HPP

#include <QDialog>

#include "RvResultData.hpp"
#include "WkfDockWidget.hpp"
#include "WkfPlatformGrouping.hpp"
#include "ui_RvWaterfallWidget.h"

namespace RvInteractionPlots
{
class WaterfallPlotDialog : public wkf::DockWidget
{
   Q_OBJECT
public:
   WaterfallPlotDialog(QMainWindow* aParent, rv::ResultDb* aDatabasePtr, rv::ResultPlatform* aPlatformPtr);
   ~WaterfallPlotDialog() override = default;

   void SetTime(double aTime);

private:
   Ui::RvWaterfallWidget mUi;
};
} // namespace RvInteractionPlots

#endif
