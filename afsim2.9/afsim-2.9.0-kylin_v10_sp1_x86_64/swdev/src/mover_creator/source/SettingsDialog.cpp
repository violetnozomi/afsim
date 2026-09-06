// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "SettingsDialog.hpp"

#include "ui_SettingsDialog.h"

namespace Designer
{

SettingsDialog::SettingsDialog(QWidget* aParent,
                               Qt::WindowFlags aFlags)
   : QDialog(aParent, aFlags)
   , mUIPtr(new Ui::SettingsDialog)
{
   mUIPtr->setupUi(this);
   //mProgressTimer = new QTimer(this);
   //connect(mProgressTimer, &QTimer::timeout, this, &CalculateAeroDialog::UpdateProgressBar);
   connect(mUIPtr->mResetSettingsButton, &QPushButton::clicked, this, [this]() {emit ResetSettings(); });
   connect(mUIPtr->mBrowseSiteDirButton, &QPushButton::clicked, this, [this]() {emit SetSiteDirectory(); });
   connect(mUIPtr->mBrowseOsgConvButton, &QPushButton::clicked, this, [this]() {emit SetOsgConvPath(); });
   connect(mUIPtr->mIVEButton, &QRadioButton::clicked, this, [this]() {emit SetOsgConvFileTypeIVE(); });
   connect(mUIPtr->mOSGBButton, &QRadioButton::clicked, this, [this]() {emit SetOsgConvFileTypeOSGB(); });
   connect(mUIPtr->mDarkThemeButton, &QRadioButton::clicked, this, [this]() {emit ChangeThemeDark(); });
   connect(mUIPtr->mLightThemeButton, &QRadioButton::clicked, this, [this]() {emit ChangeThemeLight(); });

   connect(mUIPtr->mOkButton, &QPushButton::clicked, this, &SettingsDialog::close);
}


SettingsDialog::~SettingsDialog()
{
   delete mUIPtr;
}

void SettingsDialog::showEvent(QShowEvent* aEvent)
{

}

} // namespace Designer
