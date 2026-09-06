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

#include "EditAircraftDialog.hpp"

#include <memory>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorTabs.hpp"
#include "MoverCreatorWidget.hpp"
#include "ui_EditAircraftDialog.h"

namespace Designer
{

EditAircraftDialog::EditAircraftDialog(QWidget*        aParent,
                                       Qt::WindowFlags aFlags)
   : MoverCreatorDialogBase(aParent, aFlags)
   , mUIPtr(new Ui::EditAircraftDialog)
{
   mUIPtr->setupUi(this);
   SetBaseDirPath(GetBaseDirPath() + "/Vehicles/Aircraft/");

   connect(mUIPtr->mVehicleTypeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &EditAircraftDialog::VehicleTypeChanged);
   connect(mUIPtr->mOkButton, &QPushButton::clicked, this, &EditAircraftDialog::OkClicked);
   connect(mUIPtr->mCancelButton, &QPushButton::clicked, this, &EditAircraftDialog::CancelClicked);

   connect(mUIPtr->mP6DofMoverCheckBox, &QCheckBox::clicked, this, &EditAircraftDialog::MoverCheckBoxChanged);
   connect(mUIPtr->mRigidBodyMoverCheckBox, &QCheckBox::clicked, this, &EditAircraftDialog::MoverCheckBoxChanged);
   connect(mUIPtr->mPointMassMoverCheckBox, &QCheckBox::clicked, this, &EditAircraftDialog::MoverCheckBoxChanged);
   connect(mUIPtr->mGuidedMoverCheckBox, &QCheckBox::clicked, this, &EditAircraftDialog::MoverCheckBoxChanged);

   PopulateTypeComboBox(GetBaseDirPath(), mUIPtr->mVehicleTypeComboBox);
}

EditAircraftDialog::~EditAircraftDialog()
{
   delete mUIPtr;
}

void EditAircraftDialog::VehicleTypeChanged(int aIndex)
{
   QString dirName = GetBaseDirPath();
   dirName.append(mUIPtr->mVehicleTypeComboBox->currentText());
   dirName.append("/");
   PopulateListComboBox(dirName, mUIPtr->mVehicleListComboBox, false);
}

void EditAircraftDialog::OkClicked(bool aClicked)
{
   std::unique_ptr<QJsonDocument> curDoc(new QJsonDocument);

   QString fileName = GetBaseDirPath() + mUIPtr->mVehicleTypeComboBox->currentText() + "/" + mUIPtr->mVehicleListComboBox->currentText();
   fileName.append(".amc");
   MoverCreatorWidget* mcWidget = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget();
   mcWidget->ResetAllTabs();
   mcWidget->SetCurrentVehicleFile(fileName);

   mcWidget->SetInitialTab(Designer::cGEOMETRY_TAB);

   mcWidget->SetEnableEngineTab(false);

   bool generateGuidedMover = mUIPtr->mGuidedMoverCheckBox->isChecked();
   bool generateP6DOFMover = mUIPtr->mP6DofMoverCheckBox->isChecked();
   bool generateRB6DMover = mUIPtr->mRigidBodyMoverCheckBox->isChecked();
   bool generatePM6DMover = mUIPtr->mPointMassMoverCheckBox->isChecked();

   mcWidget->SetGenerateMovers(generateGuidedMover, generateP6DOFMover, generateRB6DMover, generatePM6DMover);

   SaveSettings(generateGuidedMover, generateP6DOFMover, generateRB6DMover, generatePM6DMover);

   close();
}

void EditAircraftDialog::showEvent(QShowEvent* aEvent)
{
   bool generateRB6D  = true;
   bool generatePM6D  = false;
   bool generateP6DOF = false;
   bool generateGM    = false;
   LoadSettings(generateGM, generateP6DOF, generateRB6D, generatePM6D);

   mUIPtr->mRigidBodyMoverCheckBox->setChecked(generateRB6D);
   mUIPtr->mPointMassMoverCheckBox->setChecked(generatePM6D);
   mUIPtr->mP6DofMoverCheckBox->setChecked(generateP6DOF);
   mUIPtr->mGuidedMoverCheckBox->setChecked(generateGM);

   if (!(mUIPtr->mRigidBodyMoverCheckBox->isChecked() ||
         mUIPtr->mPointMassMoverCheckBox->isChecked() ||
         mUIPtr->mP6DofMoverCheckBox->isChecked() ||
         mUIPtr->mGuidedMoverCheckBox->isChecked()))
   {
      mUIPtr->mOkButton->setEnabled(false);
   }

   PopulateTypeComboBox(GetBaseDirPath(), mUIPtr->mVehicleTypeComboBox);
   QWidget::showEvent(aEvent);
}

void EditAircraftDialog::ChangeTheme(QString aThemeName)
{
   QString baseIconStr(QString(":/icons/%1/64x64/").arg(aThemeName));
   mUIPtr->mDialogGraphic->setPixmap(QPixmap(baseIconStr + "aircraft.png"));

   // This fixes the QT bug where combo boxes do not correctly change when the palette is changed
   QList<QComboBox*> comboBoxList = this->findChildren<QComboBox*>();

   for (auto& curCB : comboBoxList)
   {
      if (aThemeName == "Dark")

      {
         curCB->setStyleSheet("color: white; background - color: gray");
      }
      else
      {
         curCB->setStyleSheet("color: black; background - color: white");
      }
   }
}

void EditAircraftDialog::MoverCheckBoxChanged(bool aChecked)
{
   if (aChecked)
   {
      // at least one mover is selected - enable the OK button
      mUIPtr->mOkButton->setEnabled(true);
   }
   else if (mUIPtr->mGuidedMoverCheckBox->isChecked() ||
            mUIPtr->mP6DofMoverCheckBox->isChecked() ||
            mUIPtr->mRigidBodyMoverCheckBox->isChecked() ||
            mUIPtr->mPointMassMoverCheckBox->isChecked())
   {
      mUIPtr->mOkButton->setEnabled(true);
   }
   else
   {
      mUIPtr->mOkButton->setEnabled(false);
   }
}

} // namespace Designer
