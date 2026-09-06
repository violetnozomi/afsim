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



#include "GeometryOptionsDialog.hpp"

#include "ui_GeometryOptionsDialog.h"

namespace Designer
{

GeometryOptionsDialog::GeometryOptionsDialog(QWidget* aParent,
                                             Qt::WindowFlags aFlags)
   : QDialog(aParent, aFlags)
   , mUIPtr(new Ui::GeometryOptionsDialog)
{
   mUIPtr->setupUi(this);
   connect(mUIPtr->mOptionsListWidget, &QListWidget::currentItemChanged, this, &GeometryOptionsDialog::PageChanged);
   connect(mUIPtr->mOkButton, &QPushButton::clicked, this, &GeometryOptionsDialog::OkClicked);
   connect(mUIPtr->mCancelButton, &QPushButton::clicked, this, &GeometryOptionsDialog::close);
   connect(mUIPtr->mUseDefaultsCB, &QCheckBox::clicked, this, &GeometryOptionsDialog::SetUseDefaults);

   connect(mUIPtr->mRevertOptionsButton, &QPushButton::clicked, this, [this]() {emit RevertOptions(); });
   connect(mUIPtr->mMoveFPRightButton, &QPushButton::clicked, this, [this]() {emit MoveFocusRight(); });
   connect(mUIPtr->mMoveFPLeftButton, &QPushButton::clicked, this, [this]() {emit MoveFocusLeft(); });
   connect(mUIPtr->mMoveFPUpButton, &QPushButton::clicked, this, [this]() {emit MoveFocusForward(); });
   connect(mUIPtr->mMoveFPDownButton, &QPushButton::clicked, this, [this]() {emit MoveFocusAft(); });
   connect(mUIPtr->mMoveFPResetButton, &QPushButton::clicked, this, [this]() {emit ResetFocusPoint(); });
   connect(mUIPtr->mShowPrimaryAxesCB, &QCheckBox::clicked, this, [this]() {emit TogglePrimaryAxes(); });
   connect(mUIPtr->mShowSelectedCGCB, &QCheckBox::clicked, this, [this]() {emit ToggleShowSelectedCG(); });
   connect(mUIPtr->mShowEnginesCB, &QCheckBox::clicked, this, [this]() {emit ToggleShowEngines(); });
   connect(mUIPtr->mShowThrustVectorsCB, &QCheckBox::clicked, this, [this]() {emit ToggleShowThrustVectors(); });
   connect(mUIPtr->mShowPointMassesCB, &QCheckBox::clicked, this, [this]() {emit ToggleShowPointMasses(); });
   connect(mUIPtr->mShowFuelTanksCB, &QCheckBox::clicked, this, [this]() {emit ToggleShowFuelTanks(); });
   connect(mUIPtr->mShowVehicleCGCB, &QCheckBox::clicked, this, [this]() {emit ToggleShowVehicleCG(); });
   connect(mUIPtr->mShowLandingGearCB, &QCheckBox::clicked, this, [this]() {emit ToggleShowLandingGear(); });
   connect(mUIPtr->mShowSpeedBrakesCB, &QCheckBox::clicked, this, [this]() {emit ToggleShowSpeedBrakes(); });
   connect(mUIPtr->mNoAeroVectorsButton, &QRadioButton::clicked, this, [this]() {emit HideAeroVectors(); });
   connect(mUIPtr->mSelectedObjectOnlyButton, &QRadioButton::clicked, this, [this]() {emit ShowSelectedAeroVectors(); });
   connect(mUIPtr->mEntireVehicleButton, &QRadioButton::clicked, this, [this]() {emit ShowAllAeroVectors(); });
   connect(mUIPtr->mShowWireframeCB, &QCheckBox::clicked, this, [this]() {emit ToggleShowWireframe(); });
   connect(mUIPtr->mShowDebugCB, &QCheckBox::clicked, this, [this]() {emit ToggleShowDebugData(); });

   mUIPtr->mOptionsListWidget->setCurrentItem(mUIPtr->mOptionsListWidget->item(0));
}

GeometryOptionsDialog::~GeometryOptionsDialog()
{

}

void GeometryOptionsDialog::showEvent(QShowEvent* aEvent)
{

}

void GeometryOptionsDialog::OkClicked()
{
   close();
}

void GeometryOptionsDialog::SetUseDefaults()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   QSettings* settings = mw.GetSettings();

   settings->setValue("UseDefaults", mUIPtr->mUseDefaultsCB->isChecked());
}

void GeometryOptionsDialog::PageChanged(QListWidgetItem* aCurrent, QListWidgetItem* aPrevious)
{
   if (aCurrent == mUIPtr->mOptionsListWidget->item(0))
   {
      //Focus Point Item
      mUIPtr->mStackedWidget->setCurrentIndex(0);
   }
   else if (aCurrent == mUIPtr->mOptionsListWidget->item(1))
   {
      //Show/Hide Item
      mUIPtr->mStackedWidget->setCurrentIndex(1);
   }
}

void GeometryOptionsDialog::SetShowPrimaryAxes(bool aVal)
{
   mUIPtr->mShowPrimaryAxesCB->setChecked(aVal);
}

void GeometryOptionsDialog::SetShowSelectedCG(bool aVal)
{
   mUIPtr->mShowSelectedCGCB->setChecked(aVal);
}

void GeometryOptionsDialog::SetShowEngines(bool aVal)
{
   mUIPtr->mShowEnginesCB->setChecked(aVal);
}

void GeometryOptionsDialog::SetShowThrustVectors(bool aVal)
{
   mUIPtr->mShowThrustVectorsCB->setChecked(aVal);
}

void GeometryOptionsDialog::SetShowPointMasses(bool aVal)
{
   mUIPtr->mShowPointMassesCB->setChecked(aVal);
}

void GeometryOptionsDialog::SetShowFuelTanks(bool aVal)
{
   mUIPtr->mShowFuelTanksCB->setChecked(aVal);
}

void GeometryOptionsDialog::SetShowVehicleCG(bool aVal)
{
   mUIPtr->mShowVehicleCGCB->setChecked(aVal);
}

void GeometryOptionsDialog::SetShowLandingGear(bool aVal)
{
   mUIPtr->mShowLandingGearCB->setChecked(aVal);
}

void GeometryOptionsDialog::SetShowSpeedBrakes(bool aVal)
{
   mUIPtr->mShowSpeedBrakesCB->setChecked(aVal);
}

void GeometryOptionsDialog::SetShowWireframe(bool aVal)
{
   mUIPtr->mShowWireframeCB->setChecked(aVal);
}

void GeometryOptionsDialog::SetShowDebugData(bool aVal)
{
   mUIPtr->mShowDebugCB->setChecked(aVal);
}

void GeometryOptionsDialog::SetShowUseDefaults(bool aVal)
{
   mUIPtr->mUseDefaultsCB->setChecked(aVal);
}

void GeometryOptionsDialog::SetTogglePrimaryAxes()
{
   mUIPtr->mShowPrimaryAxesCB->setChecked(!mUIPtr->mShowPrimaryAxesCB->isChecked());
}

void GeometryOptionsDialog::SetToggleShowSelectedCG()
{
   mUIPtr->mShowSelectedCGCB->setChecked(!mUIPtr->mShowSelectedCGCB->isChecked());
}

void GeometryOptionsDialog::SetToggleShowEngines()
{
   mUIPtr->mShowEnginesCB->setChecked(!mUIPtr->mShowEnginesCB->isChecked());
}

void GeometryOptionsDialog::SetToggleShowThrustVectors()
{
   mUIPtr->mShowThrustVectorsCB->setChecked(!mUIPtr->mShowThrustVectorsCB->isChecked());
}

void GeometryOptionsDialog::SetToggleShowPointMasses()
{
   mUIPtr->mShowPointMassesCB->setChecked(!mUIPtr->mShowPointMassesCB->isChecked());
}

void GeometryOptionsDialog::SetToggleShowFuelTanks()
{
   mUIPtr->mShowFuelTanksCB->setChecked(!mUIPtr->mShowFuelTanksCB->isChecked());
}

void GeometryOptionsDialog::SetToggleShowVehicleCG()
{
   mUIPtr->mShowVehicleCGCB->setChecked(!mUIPtr->mShowVehicleCGCB->isChecked());
}

void GeometryOptionsDialog::SetToggleShowLandingGear()
{
   mUIPtr->mShowLandingGearCB->setChecked(!mUIPtr->mShowLandingGearCB->isChecked());
}

void GeometryOptionsDialog::SetToggleShowSpeedBrakes()
{
   mUIPtr->mShowSpeedBrakesCB->setChecked(!mUIPtr->mShowSpeedBrakesCB->isChecked());
}

void GeometryOptionsDialog::SetHideAeroVectors()
{
   mUIPtr->mNoAeroVectorsButton->setChecked(true);
}

void GeometryOptionsDialog::SetShowAllAeroVectors()
{
   mUIPtr->mEntireVehicleButton->setChecked(true);
}

void GeometryOptionsDialog::SetShowSelectedAeroVectors()
{
   mUIPtr->mSelectedObjectOnlyButton->setChecked(true);
}

void GeometryOptionsDialog::SetToggleShowWireframe()
{
   mUIPtr->mShowWireframeCB->setChecked(!mUIPtr->mShowWireframeCB->isChecked());
}

void GeometryOptionsDialog::SetToggleShowDebugData()
{
   mUIPtr->mShowDebugCB->setChecked(!mUIPtr->mShowDebugCB->isChecked());
}

} // namespace Designer
