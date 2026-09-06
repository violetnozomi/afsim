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

#include "ConvertBrawlerAircraftDialog.hpp"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>

#include "AutopilotWidget.hpp"
#include "BrawlerConverter.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorTabs.hpp"
#include "MoverCreatorWidget.hpp"
#include "NewTypeDialog.hpp"
#include "ui_ConvertBrawlerAircraftDialog.h"

namespace Designer
{

ConvertBrawlerAircraftDialog::ConvertBrawlerAircraftDialog(QWidget*        aParent,
                                                           Qt::WindowFlags aFlags)
   : MoverCreatorDialogBase(aParent, aFlags)
   , mUIPtr(new Ui::ConvertBrawlerAircraftDialog)
{
   mUIPtr->setupUi(this);

   SetBaseDirPath(GetBaseDirPath() + "/Vehicles/Brawler Conversions/");

   QDir directory;
   if (!directory.exists(GetBaseDirPath()))
   {
      directory.mkpath(GetBaseDirPath());
   }

   connect(mUIPtr->mFileBrowseButton, &QPushButton::clicked, this, &ConvertBrawlerAircraftDialog::BrowseClicked);

   connect(mUIPtr->mOkButton, &QPushButton::clicked, this, &ConvertBrawlerAircraftDialog::OkClicked);
   connect(mUIPtr->mCancelButton, &QPushButton::clicked, this, &ConvertBrawlerAircraftDialog::CancelClicked);

}

ConvertBrawlerAircraftDialog::~ConvertBrawlerAircraftDialog()
{
   delete mUIPtr;
}

void ConvertBrawlerAircraftDialog::BrowseClicked(bool aClicked)
{
   QFileDialog dialog(this);
   dialog.setFileMode(QFileDialog::FileMode::ExistingFile);
   dialog.setNameFilter(tr("Brawler data (*.fxw)"));

   connect(&dialog, &QFileDialog::fileSelected, mUIPtr->mInputFileLineEdit, &QLineEdit::setText);
   connect(mUIPtr->mInputFileLineEdit, &QLineEdit::textChanged, mUIPtr->mInstanceNameLineEdit, [this]()
   {
      QFileInfo inputFileInfo(mUIPtr->mInputFileLineEdit->text());
      mUIPtr->mInstanceNameLineEdit->setText(inputFileInfo.baseName());
   });

   QString sourceFileName;
   if (dialog.exec())
   {
      dialog.selectFile(sourceFileName);
   }
}

void ConvertBrawlerAircraftDialog::OkClicked(bool aClicked)
{
   if (mUIPtr->mInstanceNameLineEdit->text() == "")
   {
      QMessageBox::warning(this, tr("Mover Creator"),
                           tr("You must give this vehicle a name before proceeding."),
                           QMessageBox::Ok);
      return;
   }

   QString dirName = GetBaseDirPath();
   dirName.append("/");
   QString fileName = mUIPtr->mInstanceNameLineEdit->text();
   if (!fileName.endsWith(".amc"))
   {
      fileName.append(".amc");
   }

   QString newFilePath = dirName;
   newFilePath.append(fileName);

   if (QFile::exists(newFilePath))
   {
      auto overwrite = QMessageBox::warning(this, tr("Mover Creator"),
                                            tr("You are attempting to overwrite a vehicle that already exists.\n"
                                               "Overwrite the existing files?"),
                                            QMessageBox::Yes | QMessageBox::No);

      // If 'No', return early early, otherwise keep going
      if (overwrite == QMessageBox::No) { return; }
   }

   auto mc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget();
   auto vehicle = mc->GetVehicle();
   vehicle->SetVehicleName(mUIPtr->mInstanceNameLineEdit->text().toStdString());

   // Set properties that need to be defined prior to the construction of
   // ScriptGeneratorSixDOF and BrawlerConverter
   std::string vehicleTypeName("Aircraft");
   vehicle->SetVehicleType(vehicleTypeName);
   vehicle->SetBankToTurnControl();
   vehicle->SetVehicleControlConfiguration(Vehicle::VehicleControlConfig::cCONFIG_STABS_ONE_TAIL);

   QJsonObject jsonObj;
   jsonObj["fxwFileName"] = mUIPtr->mInputFileLineEdit->text();
   jsonObj["fileName"] = fileName;
   jsonObj["VehicleType"] = QString(vehicle->GetVehicleType().c_str());
   jsonObj["VehicleControlConfiguration"] = vehicle->GetVehicleControlConfigurationString();
   jsonObj["VehicleTurnControl"] = QString("Bank-to-Turn");

   QJsonObject geometryObject;
   QJsonObject vehicleMassObject;
   vehicleMassObject["objectName"] = QString("Vehicle");
   vehicleMassObject["GeometryObjectType"] = QString("GeometryMassProperties");
   geometryObject[0] = vehicleMassObject;
   jsonObj["geometry"] = geometryObject;

   QJsonDocument myDoc(jsonObj);

   QFile myFile(newFilePath);
   myFile.open(QIODevice::WriteOnly | QIODevice::Text);
   myFile.write(myDoc.toJson());
   myFile.close();

   mc->SetCurrentVehicleFile(newFilePath);

   BrawlerConverter converter(mUIPtr->mInputFileLineEdit->text());
   if (converter.OutputSixDOF_Vehicle())
   {
      std::string dialogText = "A PointMassSixDOF vehicle has been generated and saved to ";
      dialogText += converter.GetVehicleDir().absolutePath().toStdString();
      dialogText += ".\n\nThis vehicle is ready for use as-is, but you may wish to tune its controllers to your use case.\n\n";
      dialogText += "Additional adjustments, such as aerodynamic response, can be made according to the documentation for PointMassSixDOF.\n\n";
      dialogText += "Future versions of Mover Creator will improve the match between Brawler and PM6DOF movers out of the box.";

      QMessageBox conclusionBox;
      conclusionBox.setWindowTitle("Vehicle Exported");
      conclusionBox.setText(dialogText.c_str());
      conclusionBox.addButton(QMessageBox::Ok);
      QPushButton* openTunerButton = conclusionBox.addButton("Open Tuner", QMessageBox::ActionRole);

      conclusionBox.exec();
      if (conclusionBox.clickedButton() == openTunerButton)
      {
         // Less-than-ideal method of using the autopilot widget class to launch the SixDOF Tuner
         auto autopilotWidget = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetAutoPilotWidget();

         autopilotWidget->showEvent(nullptr);
         autopilotWidget->HandleTuneControlsClickedSixDOF(&converter);
      }
   }

   close();

}

void ConvertBrawlerAircraftDialog::showEvent(QShowEvent* aEvent)
{
   QWidget::showEvent(aEvent);
}

void ConvertBrawlerAircraftDialog::ChangeTheme(QString aThemeName)
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

} // namespace Designer
