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

#include "CreateWeaponDialog.hpp"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>

#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorTabs.hpp"
#include "MoverCreatorWidget.hpp"
#include "NewTypeDialog.hpp"
#include "ui_CreateWeaponDialog.h"

namespace Designer
{

CreateWeaponDialog::CreateWeaponDialog(QWidget*        aParent,
                                       Qt::WindowFlags aFlags)
   : MoverCreatorDialogBase(aParent, aFlags)
   , mUIPtr(new Ui::CreateWeaponDialog)
{
   mUIPtr->setupUi(this);

   SetBaseDirPath(GetBaseDirPath() + "/Vehicles/Weapons/");

   connect(mUIPtr->mVehicleTypeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CreateWeaponDialog::VehicleTypeChanged);
   connect(mUIPtr->mOkButton, &QPushButton::clicked, this, &CreateWeaponDialog::OkClicked);
   connect(mUIPtr->mCancelButton, &QPushButton::clicked, this, &CreateWeaponDialog::CancelClicked);

   connect(mUIPtr->mP6DofMoverCheckBox, &QCheckBox::clicked, this, &CreateWeaponDialog::MoverCheckBoxChanged);
   connect(mUIPtr->mGuidedMoverCheckBox, &QCheckBox::clicked, this, &CreateWeaponDialog::MoverCheckBoxChanged);
   connect(mUIPtr->mRigidBodyMoverCheckBox, &QCheckBox::clicked, this, &CreateWeaponDialog::MoverCheckBoxChanged);
   connect(mUIPtr->mPointMassMoverCheckBox, &QCheckBox::clicked, this, &CreateWeaponDialog::MoverCheckBoxChanged);

   PopulateTypeComboBox(GetBaseDirPath(), mUIPtr->mVehicleTypeComboBox);

   if (mUIPtr->mVehicleTypeComboBox->count() == 0)
   {
      mUIPtr->mVehicleTypeComboBox->addItem(" ");
   }
   mUIPtr->mVehicleTypeComboBox->addItem("Create New Weapon Type...");
}

CreateWeaponDialog::~CreateWeaponDialog()
{
   delete mUIPtr;
}

void CreateWeaponDialog::VehicleTypeChanged(int aIndex)
{
   QString currentTxt =  mUIPtr->mVehicleTypeComboBox->currentText();
   if (currentTxt == "Create New Vehicle Type...")
   {
      // User selected the create new vehicle type option
      // Open a dialog and prompt for a vehicle name
      NewTypeDialog newTypeDialog(this);
      if (newTypeDialog.exec() == QDialog::Accepted)
      {
         QString newTypeDir = newTypeDialog.GetName();
         QDir baseDir(GetBaseDirPath());
         baseDir.mkdir(newTypeDir);
         PopulateTypeComboBox(GetBaseDirPath(), mUIPtr->mVehicleTypeComboBox);
         mUIPtr->mVehicleTypeComboBox->addItem("Create New Vehicle Type...");
         mUIPtr->mVehicleTypeComboBox->setCurrentText(newTypeDir);
      }
   }

   QString dirName = GetBaseDirPath();
   dirName.append(mUIPtr->mVehicleTypeComboBox->currentText());
   dirName.append("/");
   PopulateListComboBox(dirName, mUIPtr->mVehicleListComboBox);
}

void CreateWeaponDialog::OkClicked(bool aClicked)
{
   if (mUIPtr->mInstanceNameLineEdit->text() == "")
   {
      QMessageBox::warning(this, tr("Mover Creator"),
                           tr("You must give this vehicle a name before proceeding."),
                           QMessageBox::Ok);
      return;
   }

   QString dirName = GetBaseDirPath();
   dirName.append(mUIPtr->mVehicleTypeComboBox->currentText());
   dirName.append("/");
   QString fileName = mUIPtr->mInstanceNameLineEdit->text();
   if (!fileName.endsWith(".amc"))
   {
      fileName.append(".amc");
   }

   if (IsTemplateFile(dirName + fileName))
   {
      QMessageBox::critical(this, tr("Mover Creator"),
                            tr("You are attempting to overwrite a file that has been marked as being a template for this vehicle.\n"
                               "Please enter a different name for this file."),
                            QMessageBox::Ok);
      return;
   }
   QString newFilePath = dirName;
   newFilePath.append(fileName);

   if (QFile::exists(newFilePath))
   {
      QMessageBox::critical(this, tr("Mover Creator"),
                            tr("You are attempting to overwrite a file that already exists.\n"
                               "Please enter a different name for this file."),
                            QMessageBox::Ok);
      return;
   }

   QFile myFile(newFilePath);

   QJsonObject jsonObj;
   if (mUIPtr->mVehicleListComboBox->currentText() == "")
   {
      jsonObj["standardTemplate"] = false;
      jsonObj["aero_file"] = "";
      jsonObj["propulsion_file"] = "";
      jsonObj["controls_file"] = "";
      jsonObj["landing_gear_file"] = "";
      jsonObj["inheritedFileName"] = mUIPtr->mVehicleListComboBox->currentText();
      jsonObj["fileName"] = fileName;
      QJsonDocument myDoc(jsonObj);
      myFile.open(QIODevice::WriteOnly | QIODevice::Text);
      myFile.write(myDoc.toJson());
      myFile.close();
   }
   else
   {
      // Copy the information from the selected file in the list combo box
      QString copyFilePath = dirName + mUIPtr->mVehicleListComboBox->currentText();
      copyFilePath.append(".amc");
      QFile jDoc(copyFilePath);
      if (jDoc.open(QIODevice::ReadOnly))
      {
         QByteArray fileData = jDoc.readAll();
         QJsonDocument copyDoc(QJsonDocument::fromJson(fileData));
         QJsonObject copyObj(copyDoc.object());
         if (copyDoc.isNull() || copyObj.isEmpty())
         {
            QMessageBox::critical(this, tr("Mover Creator"), tr("Error reading template file: malformed data"));
            return;
         }
         copyObj["standardTemplate"] = false;
         copyObj["inheritedFileName"] = mUIPtr->mVehicleListComboBox->currentText();
         copyObj["fileName"] = fileName;
         QJsonDocument myDoc(copyObj);
         myFile.open(QIODevice::WriteOnly | QIODevice::Text);
         myFile.write(myDoc.toJson());
         myFile.close();
      }
   }
   MoverCreatorWidget* mcWidget = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget();
   mcWidget->ResetAllTabs();
   mcWidget->SetCurrentVehicleFile(newFilePath);

   mcWidget->SetInitialTab(Designer::cGEOMETRY_TAB);

   bool generateGuidedMover = mUIPtr->mGuidedMoverCheckBox->isChecked();
   bool generateP6DOFMover = mUIPtr->mP6DofMoverCheckBox->isChecked();
   bool generateRB6DMover = mUIPtr->mRigidBodyMoverCheckBox->isChecked();
   bool generatePM6DMover = mUIPtr->mPointMassMoverCheckBox->isChecked();

   mcWidget->SetGenerateMovers(generateGuidedMover, generateP6DOFMover, generateRB6DMover, generatePM6DMover);

   SaveSettings(generateGuidedMover, generateP6DOFMover, generateRB6DMover, generatePM6DMover);

   close();
}

void CreateWeaponDialog::showEvent(QShowEvent* aEvent)
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
   mUIPtr->mVehicleTypeComboBox->addItem("Create New Vehicle Type...");
   QWidget::showEvent(aEvent);
}

void CreateWeaponDialog::ChangeTheme(QString aThemeName)
{
   QString baseIconStr(QString(":/icons/%1/64x64/").arg(aThemeName));
   mUIPtr->mDialogGraphic->setPixmap(QPixmap(baseIconStr + "weapon.png"));

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

void CreateWeaponDialog::MoverCheckBoxChanged(bool aChecked)
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
