// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "MapRulerPropertiesWidget.hpp"

#include <QSettings>

#include "WkfEnvironment.hpp"

Map::RulerPropertiesWidget::RulerPropertiesWidget(QWidget*        parent /*= nullptr*/,
                                                  Qt::WindowFlags f /*= Qt::WindowFlags()*/)
   : QWidget(parent, f | Qt::Tool | Qt::MSWindowsFixedSizeDialogHint)
   , mLabelMode(Ruler::cHEADING | Ruler::cRANGE | Ruler::cAT_DESTINATION)
   , mRulerMode(Ruler::cGREAT_CIRCLE)
   , mRulerPtr(nullptr)
{
   mUi.setupUi(this);
   setEnabled(false);

   // Assign IDs to buttons in button groups for generic handling of ruler mode and label mode
   mUi.rulerModeButtonGroup->setId(mUi.greatCircleMode, Ruler::cGREAT_CIRCLE);
   mUi.rulerModeButtonGroup->setId(mUi.straightLineMode, Ruler::cSTRAIGHT_LINE);
   mUi.labelModeButtonGroup->setId(mUi.headingButton, Ruler::cHEADING);
   mUi.labelModeButtonGroup->setId(mUi.rangeButton, Ruler::cRANGE);
   mUi.labelModeButtonGroup->setId(mUi.speedButton, Ruler::cSPEED);
   mUi.labelModeButtonGroup->setId(mUi.etaButton, Ruler::cETA);
   mUi.labelModeButtonGroup->setId(mUi.labelSrcButton, Ruler::cAT_SOURCE);
   mUi.labelModeButtonGroup->setId(mUi.labelDestButton, Ruler::cAT_DESTINATION);

   mUi.greatCircleMode->setIcon(QIcon::fromTheme("draw_compass"));
   mUi.straightLineMode->setIcon(QIcon::fromTheme("ruler"));
   mUi.headingButton->setIcon(QIcon::fromTheme("compass"));
   mUi.rangeButton->setIcon(QIcon::fromTheme("distance"));
   mUi.speedButton->setIcon(QIcon::fromTheme("speed"));
   mUi.etaButton->setIcon(QIcon::fromTheme("hourglass"));
   mUi.labelDestButton->setIcon(QIcon::fromTheme("arrow_dest"));
   mUi.labelSrcButton->setIcon(QIcon::fromTheme("arrow_src"));

   connect(mUi.rulerModeButtonGroup,
           QOverload<int, bool>::of(&QButtonGroup::buttonToggled),
           this,
           &RulerPropertiesWidget::RulerModeToggled);
   connect(mUi.labelModeButtonGroup,
           QOverload<int, bool>::of(&QButtonGroup::buttonToggled),
           this,
           &RulerPropertiesWidget::LabelModeToggled);
   connect(mUi.labelSize, QOverload<int>::of(&QSpinBox::valueChanged), [this](int i) { mRulerPtr->SetLabelSize(i); });

   LoadSettings();
}

void Map::RulerPropertiesWidget::SetRuler(wkf::AttachmentGreatCircleRuler* aRulerPtr)
{
   if (mRulerPtr != aRulerPtr)
   {
      mRulerPtr = aRulerPtr;
      UpdateWidget();
   }
}

void Map::RulerPropertiesWidget::UpdateWidget()
{
   if (mRulerPtr)
   {
      setEnabled(true);

      RulerMode rm = mRulerPtr->GetMode();
      mUi.rulerModeButtonGroup->blockSignals(true);
      mUi.rulerModeButtonGroup->button(rm)->setChecked(true);
      mUi.rulerModeButtonGroup->blockSignals(false);

      LabelMode lm = mRulerPtr->GetLabelMode();
      foreach (QAbstractButton* button, mUi.labelModeButtonGroup->buttons())
      {
         button->setChecked(lm & mUi.labelModeButtonGroup->id(button));
      }

      mUi.labelSize->setValue(mRulerPtr->GetLabelSize());
   }
   else
   {
      setEnabled(false);
   }
}

void Map::RulerPropertiesWidget::LabelModeToggled(int aLabelMode, bool aChecked)
{
   if (mRulerPtr)
   {
      LabelMode lm = mRulerPtr->GetLabelMode();
      mRulerPtr->SetLabelMode(aChecked ? lm | aLabelMode : lm & ~aLabelMode);
   }

   mLabelMode = (aChecked ? mLabelMode | aLabelMode : mLabelMode & ~aLabelMode);
   SaveSettings();
}

void Map::RulerPropertiesWidget::RulerModeToggled(int aLabelMode, bool aChecked)
{
   if (aChecked)
   {
      mRulerPtr->ToggleMode();
      mRulerMode = mRulerPtr->GetMode();
   }
   SaveSettings();
}

void Map::RulerPropertiesWidget::LoadSettings()
{
   QSettings& settings = wkfEnv.GetUserSettingsFile();
   settings.beginGroup("MapDisplay");
   settings.beginGroup("RulerProperties");
   mLabelMode = settings.value("LabelMode", (Ruler::cHEADING | Ruler::cRANGE | Ruler::cAT_DESTINATION)).toInt();
   mRulerMode = static_cast<RulerMode>(settings.value("RulerMode", Ruler::cGREAT_CIRCLE).toInt());
   settings.endGroup();
   settings.endGroup();
}

void Map::RulerPropertiesWidget::SaveSettings()
{
   QSettings& settings = wkfEnv.GetUserSettingsFile();
   settings.beginGroup("MapDisplay");
   settings.beginGroup("RulerProperties");
   settings.setValue("LabelMode", mLabelMode);
   settings.setValue("RulerMode", mRulerMode);
   settings.endGroup();
   settings.endGroup();
}
