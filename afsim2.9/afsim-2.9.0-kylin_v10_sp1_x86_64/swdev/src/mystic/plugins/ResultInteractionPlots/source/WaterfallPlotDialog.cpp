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

#include "WaterfallPlotDialog.hpp"

RvInteractionPlots::WaterfallPlotDialog::WaterfallPlotDialog(QMainWindow*        aParent,
                                                             rv::ResultDb*       aDatabasePtr,
                                                             rv::ResultPlatform* aPlatformPtr)
   : wkf::DockWidget("WaterfallPlot", aParent, Qt::WindowFlags())
{
   QWidget* widget = new QWidget(this);
   setWidget(widget);
   mUi.setupUi(widget);

   mUi.mTypeBox->addItems(
      QStringList{"Detected", "Detecting", "Sensor Tracked", "Sensor Tracking", "Local Tracked", "Local Tracking", "Jammed", "Jamming"});
   mUi.mWaterfallPlot->Setup(aDatabasePtr, aPlatformPtr, mUi.mTypeBox->currentText(), mUi.mGroupChooser->GetGrouping());
   connect(mUi.mTypeBox, &QComboBox::currentTextChanged, [this](QString aValue) { mUi.mWaterfallPlot->SetType(aValue); });
   connect(mUi.mGroupChooser,
           &wkf::GroupingSelector::GroupingSelected,
           [this](const wkf::PlatformGrouping aGrouping) { mUi.mWaterfallPlot->SetFilter(aGrouping); });
   mUi.mGroupChooser->setText("Platform Type: WSF_PLATFORM");
}

void RvInteractionPlots::WaterfallPlotDialog::SetTime(double aTime)
{
   mUi.mWaterfallPlot->SetTime(aTime);
}