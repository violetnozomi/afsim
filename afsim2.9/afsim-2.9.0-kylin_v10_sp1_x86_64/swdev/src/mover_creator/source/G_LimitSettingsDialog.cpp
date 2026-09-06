// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "G_LimitSettingsDialog.hpp"

#include "ui_G_LimitSettingsDialog.h"

#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QDoubleValidator>

#include "G_LimitSettings.hpp"

namespace Designer
{
   G_LimitSettingsDialog::G_LimitSettingsDialog(QWidget* aParentPtr)
      : mUI{}
      , mSettings{QString("")}
   {
      mUI.setupUi(this);

      setWindowTitle(QString("G-Load Limits"));

      setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

      mUI.mMinPitchLineEdit->setValidator(new QDoubleValidator{});
      mUI.mMaxPitchLineEdit->setValidator(new QDoubleValidator{});
      mUI.mMaxYawLineEdit->setValidator(new QDoubleValidator{});

      connect(mUI.buttonBox, &QDialogButtonBox::clicked, this, &G_LimitSettingsDialog::ButtonClicked);
   }

   void G_LimitSettingsDialog::SetSettings(const G_LimitSettings& aSettings)
   {
      mSettings = aSettings;
      setWindowTitle(mSettings.GetName() + QString(" - G-Load Limits"));

      mUI.mMinPitchLineEdit->setText(QString::number(aSettings.GetPitchG_LoadMin()));
      mUI.mMaxPitchLineEdit->setText(QString::number(aSettings.GetPitchG_LoadMax()));
      mUI.mMaxYawLineEdit->setText(QString::number(aSettings.GetYawG_LoadMax()));
   }

   G_LimitSettings G_LimitSettingsDialog::GetSettings() const
   {
      G_LimitSettings results = mSettings;
      results.SetPitchG_LoadMin(mUI.mMinPitchLineEdit->text().toDouble());
      results.SetPitchG_LoadMax(mUI.mMaxPitchLineEdit->text().toDouble());
      results.SetYawG_LoadMax(mUI.mMaxYawLineEdit->text().toDouble());
      return results;
   }

   void G_LimitSettingsDialog::ButtonClicked(QAbstractButton* aButton)
   {
      if (mUI.buttonBox->standardButton(aButton) == QDialogButtonBox::RestoreDefaults)
      {
         mUI.mMinPitchLineEdit->setText(QString::number(mSettings.DefaultMinPitchG_Load()));
         mUI.mMaxPitchLineEdit->setText(QString::number(mSettings.DefaultMaxPitchG_Load()));
         mUI.mMaxYawLineEdit->setText(QString::number(mSettings.DefaultMaxYawG_Load()));
      }
   }
}
