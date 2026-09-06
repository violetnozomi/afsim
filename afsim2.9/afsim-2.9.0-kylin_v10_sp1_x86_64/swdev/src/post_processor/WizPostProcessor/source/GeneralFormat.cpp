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

#include "GeneralFormat.hpp"
#include <ostream>
#include "ui_GeneralFormat.h"

PostProcessor::GeneralFormat::GeneralFormat()
   : ReportWidget()
   , mUIPtr(new Ui::GeneralFormat)
   , mSelectedDelimiter("column")
   , mSelectedLengthUnits("meters")
   , mSelectedAngleUnits("degrees")
   , mSelectedNumFormat("numerical")
{
   mUIPtr->setupUi(this);
   connect(mUIPtr->mButtonColumn,    &QRadioButton::pressed, this, [this]() { mSelectedDelimiter = "column"; });
   connect(mUIPtr->mButtonTab,       &QRadioButton::pressed, this, [this]() { mSelectedDelimiter = "tab"; });
   connect(mUIPtr->mButtonComma,     &QRadioButton::pressed, this, [this]() { mSelectedDelimiter = "comma"; });
   connect(mUIPtr->mButtonSpace,     &QRadioButton::pressed, this, [this]() { mSelectedDelimiter = "space"; });
   connect(mUIPtr->mButtonSemiColon, &QRadioButton::pressed, this, [this]() { mSelectedDelimiter = "semicolon"; });

   connect(mUIPtr->mButtonMeters, &QRadioButton::pressed, this, [this]() { mSelectedLengthUnits = "meters"; });
   connect(mUIPtr->mButtonFeet,   &QRadioButton::pressed, this, [this]() { mSelectedLengthUnits = "feet"; });

   connect(mUIPtr->mButtonDeg, &QRadioButton::pressed, this, [this]() { mSelectedAngleUnits = "degrees"; });
   connect(mUIPtr->mButtonRad, &QRadioButton::pressed, this, [this]() { mSelectedAngleUnits = "radians"; });

   connect(mUIPtr->mButtonNumerical, &QRadioButton::pressed, this, [this]() { mSelectedNumFormat = "numerical"; });
   connect(mUIPtr->mButtonJTIDS,     &QRadioButton::pressed, this, [this]() { mSelectedNumFormat = "jtids"; });
}

PostProcessor::GeneralFormat::~GeneralFormat()
{
   delete mUIPtr;
}

void PostProcessor::GeneralFormat::WriteData(std::ostream& aStream)
{
   aStream << "write_header_information " << (mUIPtr->mCheckBoxHeaderInfo->isChecked() ? "true" : "false")
           << "\ndelimiter " << mSelectedDelimiter.toStdString()
           << "\nlat_lon_format "
           << 'd'
           << (mUIPtr->mCB_LatLonM->isChecked() ? ":m" : "")
           << (mUIPtr->mCB_LatLonS->isChecked() ? ":s" : "")
           << ((mUIPtr->mSB_LatLonDecimal->value() > 0) ? ('.' + QString::number(mUIPtr->mSB_LatLonDecimal->value()).toStdString()) : "")
           << "\nlength_units " << mSelectedLengthUnits.toStdString()
           << "\nangle_units " << mSelectedAngleUnits.toStdString()
           << "\ntrack_number_options " << mSelectedNumFormat.toStdString() << "\n";
}

void PostProcessor::GeneralFormat::SetDelimiter(QString aDelimiter)
{
   if (aDelimiter == "column")
   {
      mUIPtr->mButtonColumn->setChecked(true);
      mSelectedDelimiter = aDelimiter;
   }
   else if (aDelimiter == "tab")
   {
      mUIPtr->mButtonTab->setChecked(true);
      mSelectedDelimiter = aDelimiter;
   }
   else if (aDelimiter == "comma")
   {
      mUIPtr->mButtonComma->setChecked(true);
      mSelectedDelimiter = aDelimiter;
   }
   else if (aDelimiter == "space")
   {
      mUIPtr->mButtonSpace->setChecked(true);
      mSelectedDelimiter = aDelimiter;
   }
   else if (aDelimiter == "semicolon")
   {
      mUIPtr->mButtonSemiColon->setChecked(true);
      mSelectedDelimiter = aDelimiter;
   }
}

void PostProcessor::GeneralFormat::RestoreDefaults()
{
   mUIPtr->mCheckBoxHeaderInfo->setChecked(true);
   mUIPtr->mButtonColumn->setChecked(true);
   mUIPtr->mButtonFeet->setChecked(true);
   mUIPtr->mButtonDeg->setChecked(true);
   mUIPtr->mCB_LatLonM->setChecked(true);
   mUIPtr->mCB_LatLonS->setChecked(true);
   mUIPtr->mSB_LatLonDecimal->setValue(2);
   mUIPtr->mButtonNumerical->setChecked(true);

   mSelectedDelimiter = "column";
   mSelectedLengthUnits = "feet";
   mSelectedAngleUnits = "angle";
   mSelectedNumFormat = "numerical";
}
