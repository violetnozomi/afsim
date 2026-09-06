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

#include "WkfZoneBrowserPrefWidget.hpp"

#include <QLabel>

#include "UtQtMemory.hpp"
#include "WkfZoneBrowserPrefObject.hpp"

wkf::ZoneBrowserPrefWidget::ZoneBrowserPrefWidget()
   : wkf::PrefWidgetT<ZoneBrowserPrefObject>()
{
   mUi.setupUi(this);

   QLabel* lineLabelPtr = ut::qt::make_qt_ptr<QLabel>("Line Color:");
   QLabel* fillLabelPtr = ut::qt::make_qt_ptr<QLabel>("Fill Color:");
   mLineColorButtonPtr  = ut::qt::make_qt_ptr<UtQtColorButton>(this);
   mLineColorButtonPtr->setFixedWidth(30);
   mLineColorButtonPtr->SetColorWindowTitle("Choose Line Color");
   mFillColorButtonPtr = ut::qt::make_qt_ptr<UtQtColorButton>(this);
   mFillColorButtonPtr->setFixedWidth(30);
   mFillColorButtonPtr->SetColorWindowTitle("Choose Fill Color");
   mFillColorButtonPtr->ShowAlpha(true);

   QHBoxLayout* horizontalBoxPtr = ut::qt::make_qt_ptr<QHBoxLayout>();
   horizontalBoxPtr->addSpacerItem(
      new QSpacerItem(mUi.mRandomButton->iconSize().width() * 2, 1, QSizePolicy::Fixed, QSizePolicy::Fixed));
   horizontalBoxPtr->addWidget(lineLabelPtr);
   horizontalBoxPtr->addWidget(mLineColorButtonPtr);
   horizontalBoxPtr->addWidget(fillLabelPtr);
   horizontalBoxPtr->addWidget(mFillColorButtonPtr);
   horizontalBoxPtr->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed));
   mUi.verticalLayout->addLayout(horizontalBoxPtr);
   mUi.verticalLayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding));

   connect(mUi.mSingleColorButton,
           &QRadioButton::toggled,
           this,
           [&]()
           {
              bool showDefault = mUi.mSingleColorButton->isChecked();
              mLineColorButtonPtr->setEnabled(showDefault);
              mFillColorButtonPtr->setEnabled(showDefault);
           });
}

void wkf::ZoneBrowserPrefWidget::EnableColorButtons(bool aEnable)
{
   if (aEnable)
   {
      mLineColorButtonPtr->setEnabled(true);
      mFillColorButtonPtr->setEnabled(true);
   }
   else
   {
      mLineColorButtonPtr->setEnabled(false);
      mFillColorButtonPtr->setEnabled(false);
   }
}

void wkf::ZoneBrowserPrefWidget::ReadPreferenceData(const wkf::ZoneBrowserPrefData& aPrefData)
{
   mUi.mRandomButton->setChecked(aPrefData.mColorChoice == ColorOption::cRANDOM ? true : false);
   mUi.mSingleColorButton->setChecked(aPrefData.mColorChoice == ColorOption::cSINGLESELECT ? true : false);
   mLineColorButtonPtr->SetColor(aPrefData.mDefaultLineColor);
   mFillColorButtonPtr->SetColor(aPrefData.mDefaultFillColor);
}

void wkf::ZoneBrowserPrefWidget::WritePreferenceData(wkf::ZoneBrowserPrefData& aPrefData)
{
   if (mUi.mRandomButton->isChecked())
   {
      aPrefData.mColorChoice = ColorOption::cRANDOM;
   }
   else if (mUi.mSingleColorButton->isChecked())
   {
      aPrefData.mColorChoice = ColorOption::cSINGLESELECT;
   }
   aPrefData.mDefaultLineColor = mLineColorButtonPtr->GetColor();
   aPrefData.mDefaultFillColor = mFillColorButtonPtr->GetColor();
}
