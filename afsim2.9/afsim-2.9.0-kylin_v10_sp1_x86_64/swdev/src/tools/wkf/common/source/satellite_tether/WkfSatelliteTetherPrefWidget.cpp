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

#include "WkfSatelliteTetherPrefWidget.hpp"

#include <QCheckBox>
#include <QFormLayout>

#include "UtQtMemory.hpp"

wkf::SatelliteTetherPrefWidget::SatelliteTetherPrefWidget()
   : PrefWidgetT<SatelliteTetherPrefObject>()
{
   setWindowTitle("Satellite Tether");

   QFormLayout* lo = ut::qt::make_qt_ptr<QFormLayout>(this);
   setLayout(lo);
   mShowNames        = new QCheckBox(this);
   mShowFutureTracks = new QCheckBox(this);
   mEnableLighting   = new QCheckBox(this);
   lo->addRow("Show Names", mShowNames);
   lo->addRow("Show Future Track", mShowFutureTracks);
   lo->addRow("Enable Lighting", mEnableLighting);
}

void wkf::SatelliteTetherPrefWidget::ReadPreferenceData(const SatelliteTetherPrefData& aPrefData)
{
   mShowFutureTracks->setChecked(aPrefData.mShowFutureTrack);
   mShowNames->setChecked(aPrefData.mShowNames);
   mEnableLighting->setChecked(aPrefData.mEnableLighting);
}

void wkf::SatelliteTetherPrefWidget::WritePreferenceData(SatelliteTetherPrefData& aPrefData)
{
   aPrefData.mShowFutureTrack = mShowFutureTracks->isChecked();
   aPrefData.mShowNames       = mShowNames->isChecked();
   aPrefData.mEnableLighting  = mEnableLighting->isChecked();
}
