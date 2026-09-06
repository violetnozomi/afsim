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

#include "AutopilotWidget.hpp"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>

#include "G_LimitSettingsDialog.hpp"
#include "ControlsWidget.hpp"
#include "MoverCreatorEnvironment.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorWidget.hpp"
#include "ScriptGeneratorP6DOF.hpp"
#include "ScriptGeneratorSixDOF.hpp"
#include "ui_AutopilotWidget.h"

namespace Designer
{

AutopilotWidget::AutopilotWidget(QWidget*        aParent,
                                 Qt::WindowFlags aFlags)
   : QWidget(aParent, aFlags)
   , mUIPtr(new Ui::AutopilotWidget)
   , mG_LimitDialogPtr{new G_LimitSettingsDialog{}}
{
   mUIPtr->setupUi(this);

   connect(mUIPtr->mSetupTuneControlsButton, &QPushButton::clicked, this, &AutopilotWidget::HandleTuneControlsClickedP6DOF);
   connect(mUIPtr->mSetupRB6DofTuneControlsButton, &QPushButton::clicked, this, [this] { AutopilotWidget::HandleTuneControlsClickedRB6DOF(); });
   connect(mUIPtr->mSetupPM6DofTuneControlsButton, &QPushButton::clicked, this, [this] { AutopilotWidget::HandleTuneControlsClickedPM6DOF(); });

   connect(mUIPtr->mHardwareSetupTuneControlsButton, &QPushButton::clicked, this, &AutopilotWidget::HandleHardwareTuneControlsClicked);
   connect(mUIPtr->mPidComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &AutopilotWidget::LoadPidPrefs);
   connect(mUIPtr->mPidTunerFileBrowseButton, &QPushButton::clicked, this, &AutopilotWidget::HandleBrowseButtonClicked);
   connect(mUIPtr->mCopyDataButton, &QPushButton::clicked, this, &AutopilotWidget::HandleCopyDataButtonClicked);
   connect(mUIPtr->mHardwareCopyDataButton, &QPushButton::clicked, this, &AutopilotWidget::HandleHardwareCopyDataButtonClicked);
   connect(mUIPtr->mSaveButton, &QPushButton::clicked, this, &AutopilotWidget::SaveAutoPilot);
   connect(mUIPtr->mPreviousButton, &QPushButton::clicked, this, &AutopilotWidget::PreviousButtonClicked);
   connect(mUIPtr->mNextButton, &QPushButton::clicked, this, &AutopilotWidget::NextButtonClicked);

   //connect all of the line edits to value changed
   QList<QWidget*> widgetList = findChildren<QWidget*>();
   for (auto& curWidget : widgetList)
   {
      auto isLineEdit = qobject_cast<QLineEdit*>(curWidget);
      if (isLineEdit)
      {
         connect(isLineEdit, &QLineEdit::editingFinished, this, &AutopilotWidget::SaveAutoPilot);
         connect(isLineEdit, &QLineEdit::textEdited, this, [this]() {mDirty = true; });
      }
   }

   connect(mUIPtr->mPilotsG_LimitButton, &QPushButton::clicked, this, &AutopilotWidget::LaunchPilotsG_LimitsDialog);
   connect(mUIPtr->mHardwareG_LimitsButton, &QPushButton::clicked, this, &AutopilotWidget::LaunchHardwareG_LimitsDialog);
   connect(mG_LimitDialogPtr.get(), &G_LimitSettingsDialog::accepted, this, &AutopilotWidget::HandleG_LimitResults);

   // Ability to specify tuner file is OBE
   mUIPtr->mPidTunerFileLabel->hide();
   mUIPtr->mPidTunerFileLineEdit->hide();
   mUIPtr->mPidTunerFileBrowseButton->hide();
}

AutopilotWidget::~AutopilotWidget()
{
   delete mUIPtr;
}

void AutopilotWidget::Initialize()
{

}

void AutopilotWidget::showEvent(QShowEvent* aEvent)
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   mUIPtr->mAnimatedGLWidget->AdjustViewingDistance(true);
   mUIPtr->mAnimatedGLWidget->StartAnimationTimer();

   ToggleHardwareAutoPilot();
   ToggleG_LimitButtons();

   bool enableP6DOF = mw.GetMoverCreatorWidget()->GenerateP6DOFMover();
   mUIPtr->mHardwarePilotGroupBox->setHidden(!enableP6DOF);
   mUIPtr->mPilotsGuidanceGroupBox->setHidden(!enableP6DOF);

   bool enableRB6DOF = mw.GetMoverCreatorWidget()->GenerateRB6DOFMover();
   bool enablePM6DOF = mw.GetMoverCreatorWidget()->GeneratePM6DOFMover();

   mUIPtr->mSixDofGuidanceGroupBox->setHidden(!enableRB6DOF && !enablePM6DOF);
   mUIPtr->mSetupRB6DofTuneControlsButton->setDisabled(!enableRB6DOF);
   mUIPtr->mSetupPM6DofTuneControlsButton->setDisabled(!enablePM6DOF);

   // Setup the warlock settings info
   if (mCustomIniFile == "")
   {
      Vehicle* curVehicle = mw.GetMoverCreatorWidget()->GetVehicle();
      QString vehicleName = QString::fromStdString(curVehicle->GetVehicleName());

      QString configFile = MoverCreatorMainWindow::GetDataPath() + "/APConfig.ini";
      mCustomIniFile = mw.GetDataPath() + "/AFSIM_Scripts/" + vehicleName + "/APConfig.ini";
      QFile::copy(configFile, mCustomIniFile);

      if (QFileInfo::exists(mCustomIniFile))
      {
         mWarlockSettings.reset(new QSettings(mCustomIniFile, QSettings::IniFormat, this));
      }
   }

   // Update the list shown under Pilots/Guidance
   ControlsWidget* controlsPtr = mw.GetMoverCreatorWidget()->GetControlsWidget();
   QString listTxt = "";

   // Synthetic Pilot check box...
   if (controlsPtr->IsSyntheticPilotAvailable())
   {
      listTxt.append("Synthetic Pilot");
   }

   // Manual Pilot check box...
   if (controlsPtr->IsManualPilotAvailable())
   {
      if (listTxt != "")
      {
         listTxt.append("\n");
      }
      listTxt.append("Manual Pilot");
   }

   // Guidance System check box...
   if (controlsPtr->IsGuidanceSystemAvailable())
   {
      if (listTxt != "")
      {
         listTxt.append("\n");
      }
      listTxt.append("Guidance System");
   }

   mUIPtr->mPilotsGuidanceLabel->setText(listTxt);

   // Get the settings for the selected pid
   LoadPidPrefs();
}

void AutopilotWidget::ToggleHardwareAutoPilot()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   bool disableHWA = !mw.GetMoverCreatorWidget()->GetControlsWidget()->IsHardwareAutoPilotAvailable();
   mUIPtr->mHardwarePilotGroupBox->setDisabled(disableHWA);

   if (disableHWA)
   {
      QPalette palette = mUIPtr->mHardwarePilotGroupBox->palette();
      palette.setColor(QPalette::WindowText, Qt::gray);
      mUIPtr->mHardwarePilotGroupBox->setPalette(palette);

      QList<QLabel*> labelList = mUIPtr->mHardwarePilotGroupBox->findChildren<QLabel*>();
      for (auto& curLabel : labelList)
      {
         curLabel->setPalette(palette);
      }
   }
   else
   {
      QPalette palette = MoverCreatorMainWindow::GetInstance().GetPalette();
      mUIPtr->mHardwarePilotGroupBox->setPalette(palette);

      QList<QLabel*> labelList = mUIPtr->mHardwarePilotGroupBox->findChildren<QLabel*>();
      for (auto& curLabel : labelList)
      {
         curLabel->setPalette(palette);
      }
   }
}

void AutopilotWidget::SaveAutoPilot()
{
   if (mDirty)
   {
      // Write out the JSON settings
      MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
      std::shared_ptr<QJsonDocument> curDoc = mw.GetMoverCreatorWidget()->GetCurrentVehicleFile();

      if (curDoc == nullptr)
      {
         return;
      }

      QJsonObject mainObj = curDoc->object();
      UpdatePidPrefsObject();
      mainObj.insert("pid_prefs", mPidPrefsObject);
      curDoc->setObject(mainObj);

      MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->SaveCurrentVehicleFile();

      mDirty = false;
   }
}

void AutopilotWidget::HandleTuneControlsClickedP6DOF()
{
   // Kill any ongoing Tuner task
   if (mWarlockProcess)
   {
      mWarlockProcess->terminate();
      mWarlockProcess->waitForFinished();
   }

   MoverCreatorWidget* mw = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget();
   Vehicle* vehicle = mw->GetVehicle();

   if (vehicle == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   UtAtmosphere atm = vehicle->GetAtmosphere();

   double altitude_m = vehicle->GetVehicleAltitude_ft() * UtMath::cM_PER_FT;
   double mach       = vehicle->GetTestingMach();
   double latitude   = 21.1888;
   double longitude  = -157.9534;
   double heading    = 0.0;
   double speed_mps  = UtMath::cM_PER_FT * atm.CalcFpsFromMach(altitude_m, mach);

   ScriptGeneratorP6DOF scriptGenerator;
   mTunerFileName = scriptGenerator.CreateAutopilotTestFile(latitude, longitude, altitude_m, heading, speed_mps);

   if (!mWarlockSettings)
   {
      // Could not find the warlock settings file, warn the user and prompt them to install warlock
      QMessageBox::warning(this, "Warlock Not Found", "Notice: Warlock needs to be installed in order to Setup/Tune Controls!", QMessageBox::Ok);
      return;
   }

   if (mTunerFileName == "")
   {
      QMessageBox::warning(this, "Warning: No PID Tuner File Specified", "Please specify a PID Tuning file before proceeding");
      return;
   }

   SaveAutoPilot();

   // Write out the warlock settings
   if (mWarlockSettings)
   {
      mWarlockSettings->beginGroup("P6DOFTuner");
      mWarlockSettings->beginGroup(mUIPtr->mPidComboBox->currentText());
      mWarlockSettings->setValue("minKp", mUIPtr->mMinKpLineEdit->text().toDouble());
      mWarlockSettings->setValue("maxKp", mUIPtr->mMaxKpLineEdit->text().toDouble());
      mWarlockSettings->setValue("minKi", mUIPtr->mMinKiLineEdit->text().toDouble());
      mWarlockSettings->setValue("maxKi", mUIPtr->mMaxKiLineEdit->text().toDouble());
      mWarlockSettings->setValue("minKd", mUIPtr->mMinKdLineEdit->text().toDouble());
      mWarlockSettings->setValue("maxKd", mUIPtr->mMaxKdLineEdit->text().toDouble());
      mWarlockSettings->setValue("runTime", mUIPtr->mRuntimeLineEdit->text().toDouble());
      mWarlockSettings->setValue("errorWeight", mUIPtr->mErrorWeightLineEdit->text().toDouble());
      mWarlockSettings->setValue("overshootWeight", mUIPtr->mOvershootWeightLineEdit->text().toDouble());

      // Begin write out of genetic algorithm values
      mWarlockSettings->beginGroup("GeneticAlgorithm");
      mWarlockSettings->setValue("populationSize", mUIPtr->mPopulationSizeLineEdit->text().toDouble());
      mWarlockSettings->setValue("maxGenerations", mUIPtr->mMaxGenerationsLineEdit->text().toDouble());
      mWarlockSettings->setValue("carryoverFraction", mUIPtr->mCarryoverFracLineEdit->text().toDouble());
      mWarlockSettings->setValue("crossoverProbability", mUIPtr->mCrossOverProbLineEdit->text().toDouble());
      mWarlockSettings->setValue("mutationProbability", mUIPtr->mMutationProbLineEdit->text().toDouble());
      mWarlockSettings->endGroup(); // End GeneticAlorithm group
      mWarlockSettings->endGroup(); // End currentPID group

      MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
      Vehicle* curVehicle = mw.GetMoverCreatorWidget()->GetVehicle();
      QString vehicleName = QString::fromStdString(curVehicle->GetVehicleName());
      QString vehicleType = "";
      if (curVehicle->IsAircraft())
      {
         vehicleType = "aircraft";
      }
      else if (curVehicle->IsWeapon())
      {
         vehicleType = "weapons";
      }

      QString autoPilotControlFile = mw.GetDataPath() + "/AFSIM_Scripts/" + vehicleName + "/p6dof_types/" + vehicleType + "/" + vehicleName.toLower() + "/controls/autopilot_config.txt";
      mWarlockSettings->setValue("moverCreatorFile", autoPilotControlFile);
      mWarlockSettings->endGroup(); // End P6DOFTuner group
      mWarlockSettings->sync();     // Call this to permanently write the settings before launching warlock

      // Launch warlock

      mWarlockProcess.reset(new QProcess());
      connect(mWarlockProcess.get(), static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &AutopilotWidget::HandleProcessFinished);
      QStringList warlockArgs;
      // Use the -cf option (not -icf) for Warlock because we do not want the User's default settings to be overwritten
      // with the settings file provided by MoverCreator.
      QString cfStr = "-cf";
      // QString configFile = MoverCreatorMainWindow::GetDataPath() + "/APConfig.ini";
      warlockArgs.push_back(cfStr);
      warlockArgs.push_back(mCustomIniFile);
      warlockArgs.push_back(mTunerFileName);

      mWarlockProcess->start(MoverCreatorEnvironment::GetEnvironment().GetWarlockDir(), warlockArgs);
   }
}

void AutopilotWidget::HandleTuneControlsClickedRB6DOF()
{
   ScriptGeneratorRigidBodySixDOF scriptGenerator;
   HandleTuneControlsClickedSixDOF(&scriptGenerator);
}

void AutopilotWidget::HandleTuneControlsClickedPM6DOF()
{
   ScriptGeneratorPointMassSixDOF scriptGenerator;
   HandleTuneControlsClickedSixDOF(&scriptGenerator);
}

void AutopilotWidget::HandleTuneControlsClickedSixDOF(ScriptGeneratorSixDOF* aScriptGenerator)
{
   // Kill any ongoing Tuner task
   if (mWarlockProcess)
   {
      mWarlockProcess->terminate();
      mWarlockProcess->waitForFinished();
   }

   if (!aScriptGenerator) { return; }

   MoverCreatorMainWindow& mainWindow = MoverCreatorMainWindow::GetInstance();
   Vehicle* vehicle = mainWindow.GetMoverCreatorWidget()->GetVehicle();

   if (vehicle == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   UtAtmosphere atm = vehicle->GetAtmosphere();

   double altitude_m = vehicle->GetVehicleAltitude_ft() * UtMath::cM_PER_FT;
   double mach       = vehicle->GetTestingMach();
   double latitude   = 21.1888;
   double longitude  = -157.9534;
   double heading    = 0.0;
   double speed_mps  = UtMath::cM_PER_FT * atm.CalcFpsFromMach(altitude_m, mach);

   mTunerFileName = aScriptGenerator->CreateAutopilotTestFile(latitude, longitude, altitude_m, heading, speed_mps);

   if (!mWarlockSettings)
   {
      // Could not find the warlock settings file, warn the user and prompt them to install warlock
      QMessageBox::warning(this, "Warlock Not Found", "Notice: Warlock needs to be installed in order to Setup/Tune Controls!", QMessageBox::Ok);
      return;
   }

   if (mTunerFileName == "")
   {
      QMessageBox::warning(this, "Warning: No PID Tuner File Specified", "Please specify a PID Tuning file before proceeding");
      return;
   }

   SaveAutoPilot();

   // Write out the warlock settings
   if (mWarlockSettings)
   {
      mWarlockSettings->beginGroup("SixDOF Tuner");
      mWarlockSettings->beginGroup(mUIPtr->mPidComboBox->currentText());
      mWarlockSettings->setValue("minKp", mUIPtr->mMinKpLineEdit->text().toDouble());
      mWarlockSettings->setValue("maxKp", mUIPtr->mMaxKpLineEdit->text().toDouble());
      mWarlockSettings->setValue("minKi", mUIPtr->mMinKiLineEdit->text().toDouble());
      mWarlockSettings->setValue("maxKi", mUIPtr->mMaxKiLineEdit->text().toDouble());
      mWarlockSettings->setValue("minKd", mUIPtr->mMinKdLineEdit->text().toDouble());
      mWarlockSettings->setValue("maxKd", mUIPtr->mMaxKdLineEdit->text().toDouble());
      mWarlockSettings->setValue("runTime", mUIPtr->mRuntimeLineEdit->text().toDouble());
      mWarlockSettings->setValue("errorWeight", mUIPtr->mErrorWeightLineEdit->text().toDouble());
      mWarlockSettings->setValue("overshootWeight", mUIPtr->mOvershootWeightLineEdit->text().toDouble());

      // Begin write out of genetic algorithm values
      mWarlockSettings->beginGroup("GeneticAlgorithm");
      mWarlockSettings->setValue("populationSize", mUIPtr->mPopulationSizeLineEdit->text().toDouble());
      mWarlockSettings->setValue("maxGenerations", mUIPtr->mMaxGenerationsLineEdit->text().toDouble());
      mWarlockSettings->setValue("carryoverFraction", mUIPtr->mCarryoverFracLineEdit->text().toDouble());
      mWarlockSettings->setValue("crossoverProbability", mUIPtr->mCrossOverProbLineEdit->text().toDouble());
      mWarlockSettings->setValue("mutationProbability", mUIPtr->mMutationProbLineEdit->text().toDouble());
      mWarlockSettings->endGroup(); // End GeneticAlorithm group
      mWarlockSettings->endGroup(); // End currentPID group

      MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
      Vehicle* curVehicle = mw.GetMoverCreatorWidget()->GetVehicle();
      QString vehicleName = QString::fromStdString(curVehicle->GetVehicleName());
      QString vehicleType = "";
      if (curVehicle->IsAircraft())
      {
         vehicleType = "aircraft";
      }
      else if (curVehicle->IsWeapon())
      {
         vehicleType = "weapons";
      }

      QString autoPilotControlFile = mw.GetDataPath() + "/AFSIM_Scripts/" + vehicleName + "/six_dof_types/" + vehicleType + "/" + vehicleName.toLower() + "/controls/autopilot_config" + aScriptGenerator->GetFilenameSuffix() + ".txt";
      mWarlockSettings->setValue("moverCreatorFile", autoPilotControlFile);
      mWarlockSettings->endGroup(); // End P6DOFTuner group
      mWarlockSettings->sync();     // Call this to permanently write the settings before launching warlock

      // Launch warlock

      mWarlockProcess.reset(new QProcess());
      connect(mWarlockProcess.get(), static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &AutopilotWidget::HandleProcessFinished);
      QStringList warlockArgs;
      // Use the -cf option (not -icf) for Warlock because we do not want the User's default settings to be overwritten
      // with the settings file provided by MoverCreator.
      QString cfStr = "-cf";
      // QString configFile = MoverCreatorMainWindow::GetDataPath() + "/APConfig.ini";
      warlockArgs.push_back(cfStr);
      warlockArgs.push_back(mCustomIniFile);
      warlockArgs.push_back(mTunerFileName);

      mWarlockProcess->start(MoverCreatorEnvironment::GetEnvironment().GetWarlockDir(), warlockArgs);

      if (!mWarlockProcess->waitForStarted())
      {
         MoverCreatorMainWindow().CreateErrorBox("The Warlock executable could not be launched. Ensure Warlock is installed before attempting to tune the vehicle.");
      }
   }
}

void AutopilotWidget::HandleHardwareTuneControlsClicked()
{
   // Kill any ongoing Tuner task
   if (mWarlockProcess)
   {
      mWarlockProcess->terminate();
      mWarlockProcess->waitForFinished();
   }

   MoverCreatorMainWindow& mainWindow = MoverCreatorMainWindow::GetInstance();
   Vehicle* vehicle = mainWindow.GetMoverCreatorWidget()->GetVehicle();

   if (vehicle == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   UtAtmosphere atm = vehicle->GetAtmosphere();

   double altitude_m = vehicle->GetVehicleAltitude_ft() * UtMath::cM_PER_FT;
   double mach       = vehicle->GetTestingMach();
   double latitude   = 21.1888;
   double longitude  = -157.9534;
   double heading    = 0.0;
   double speed_mps  = UtMath::cM_PER_FT * atm.CalcFpsFromMach(altitude_m, mach);

   ScriptGeneratorP6DOF scriptGenerator;
   mTunerFileName = scriptGenerator.CreateAutopilotTestFile(latitude, longitude, altitude_m, heading, speed_mps);

   if (!mWarlockSettings)
   {
      // Could not find the warlock settings file, warn the user and prompt them to install warlock
      QMessageBox::warning(this, "Warlock Not Found", "Notice: Warlock needs to be installed in order to Setup/Tune Controls!", QMessageBox::Ok);
      return;
   }
   if (mTunerFileName == "")
   {
      QMessageBox::warning(this, "Warning: No PID Tuner File Specified", "Please specify a PID Tuning file before proceeding");
      return;
   }

   SaveAutoPilot();

   // Write out the warlock settings
   if (mWarlockSettings)
   {
      mWarlockSettings->beginGroup("P6DOFTuner");
      mWarlockSettings->beginGroup(mUIPtr->mPidComboBox->currentText());
      mWarlockSettings->setValue("minKp", mUIPtr->mMinKpLineEdit->text().toDouble());
      mWarlockSettings->setValue("maxKp", mUIPtr->mMaxKpLineEdit->text().toDouble());
      mWarlockSettings->setValue("minKi", mUIPtr->mMinKiLineEdit->text().toDouble());
      mWarlockSettings->setValue("maxKi", mUIPtr->mMaxKiLineEdit->text().toDouble());
      mWarlockSettings->setValue("minKd", mUIPtr->mMinKdLineEdit->text().toDouble());
      mWarlockSettings->setValue("maxKd", mUIPtr->mMaxKdLineEdit->text().toDouble());
      mWarlockSettings->setValue("runTime", mUIPtr->mRuntimeLineEdit->text().toDouble());
      mWarlockSettings->setValue("errorWeight", mUIPtr->mErrorWeightLineEdit->text().toDouble());
      mWarlockSettings->setValue("overshootWeight", mUIPtr->mOvershootWeightLineEdit->text().toDouble());

      // Begin write out of genetic algorithm values
      mWarlockSettings->beginGroup("GeneticAlgorithm");
      mWarlockSettings->setValue("populationSize", mUIPtr->mPopulationSizeLineEdit->text().toDouble());
      mWarlockSettings->setValue("maxGenerations", mUIPtr->mMaxGenerationsLineEdit->text().toDouble());
      mWarlockSettings->setValue("carryoverFraction", mUIPtr->mCarryoverFracLineEdit->text().toDouble());
      mWarlockSettings->setValue("crossoverProbability", mUIPtr->mCrossOverProbLineEdit->text().toDouble());
      mWarlockSettings->setValue("mutationProbability", mUIPtr->mMutationProbLineEdit->text().toDouble());
      mWarlockSettings->endGroup();
      mWarlockSettings->endGroup();

      MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
      Vehicle* curVehicle = mw.GetMoverCreatorWidget()->GetVehicle();
      QString vehicleName = QString::fromStdString(curVehicle->GetVehicleName());
      QString vehicleType = "";
      if (curVehicle->IsAircraft())
      {
         vehicleType = "aircraft";
      }
      else if (curVehicle->IsWeapon())
      {
         vehicleType = "weapons";
      }

      QString autoPilotControlFile = mw.GetDataPath() + "/AFSIM_Scripts/" + vehicleName + "/p6dof_types/" +
                                     vehicleType + "/" + vehicleName.toLower() + "/controls/hw_autopilot_config.txt";

      mWarlockSettings->setValue("moverCreatorFile", autoPilotControlFile);
      mWarlockSettings->endGroup(); //end P6DOFTuner group
      mWarlockSettings->sync(); //call this to permanently write the settings before launching warlock

      // Launch warlock

      mWarlockProcess.reset(new QProcess());
      connect(mWarlockProcess.get(), static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
              this, &AutopilotWidget::HandleProcessFinished);
      QStringList warlockArgs;

      // Use the -cf option (not -icf) for Warlock because we do not want the User's default settings to be overwritten
      // with the settings file provided by MoverCreator.
      QString cfStr = "-cf";
      // QString configFile = MoverCreatorMainWindow::GetDataPath() + "/APConfig.ini";
      warlockArgs.push_back(cfStr);
      warlockArgs.push_back(mCustomIniFile);
      warlockArgs.push_back(mTunerFileName);
      mWarlockProcess->start(MoverCreatorEnvironment::GetEnvironment().GetWarlockDir(), warlockArgs);

      if (!mWarlockProcess->waitForStarted())
      {
         MoverCreatorMainWindow().CreateErrorBox("The Warlock executable could not be launched. Ensure Warlock is installed before attempting to tune the vehicle.");
      }
   }
}

void AutopilotWidget::HandleProcessFinished()
{
   // The intent of this function is to reset the mWarlockSettings "moverCreatorFile" back to "".  Since Warlock
   // has no way of allowing plugins to handle commandline arguments, we need to work around by playing with the
   // settings.  Mover creator sets this key/value and the P6DOF tuner plugin looks for it.  If it is set then
   // the plugin knows that the call to warlock came from Mover Creator and will auto save the tuner file to
   // where Mover Creator tells it to. Otherwise, the P6DOF Tuner will ask the user where to save the file.
   mWarlockSettings->beginGroup("P6DOFTuner");
   mWarlockSettings->setValue("moverCreatorFile", "");
   mWarlockSettings->endGroup(); // End P6DOFTuner group

   mWarlockSettings->beginGroup("SixDOF Tuner");
   mWarlockSettings->setValue("moverCreatorFile", "");
   mWarlockSettings->endGroup(); // End SixDOF Tuner group

   mWarlockSettings->sync();     // Call this to permanently write the settings before launching warlock
}


void AutopilotWidget::PauseGLAnimation()
{
   mUIPtr->mAnimatedGLWidget->PauseAnimationTimer();
}

void AutopilotWidget::LoadPrefsFromWarlockSettings()
{
   // Get the settings for the selected pid
   QString curPID = mUIPtr->mPidComboBox->currentText();
   if (mWarlockSettings)
   {
      mWarlockSettings->beginGroup("P6DOFTuner");
      mWarlockSettings->beginGroup(curPID);
      double minKp           = mWarlockSettings->value("minKp").toDouble();
      double maxKp           = mWarlockSettings->value("maxKp").toDouble();
      double minKi           = mWarlockSettings->value("minKi").toDouble();
      double maxKi           = mWarlockSettings->value("maxKi").toDouble();
      double minKd           = mWarlockSettings->value("minKd").toDouble();
      double maxKd           = mWarlockSettings->value("maxKd").toDouble();
      double runTime         = mWarlockSettings->value("runTime").toDouble();
      double errorWeight     = mWarlockSettings->value("errorWeight").toDouble();
      double overshootWeight = mWarlockSettings->value("overshootWeight").toDouble();

      mUIPtr->mMinKpLineEdit->setText(QString::number(minKp));
      mUIPtr->mMaxKpLineEdit->setText(QString::number(maxKp));
      mUIPtr->mMinKiLineEdit->setText(QString::number(minKi));
      mUIPtr->mMaxKiLineEdit->setText(QString::number(maxKi));
      mUIPtr->mMinKdLineEdit->setText(QString::number(minKd));
      mUIPtr->mMaxKdLineEdit->setText(QString::number(maxKd));
      mUIPtr->mRuntimeLineEdit->setText(QString::number(runTime));
      mUIPtr->mErrorWeightLineEdit->setText(QString::number(errorWeight));
      mUIPtr->mOvershootWeightLineEdit->setText(QString::number(overshootWeight));

      // Load up the genetic algorithm values
      mWarlockSettings->beginGroup("GeneticAlgorithm");
      double popSize   = mWarlockSettings->value("populationSize").toDouble();
      double maxGen    = mWarlockSettings->value("maxGenerations").toDouble();
      double carryover = mWarlockSettings->value("carryoverFraction").toDouble();
      double crossover = mWarlockSettings->value("crossoverProbability").toDouble();
      double mutation  = mWarlockSettings->value("mutationProbability").toDouble();

      mUIPtr->mPopulationSizeLineEdit->setText(QString::number(popSize));
      mUIPtr->mMaxGenerationsLineEdit->setText(QString::number(maxGen));
      mUIPtr->mCarryoverFracLineEdit->setText(QString::number(carryover));
      mUIPtr->mCrossOverProbLineEdit->setText(QString::number(crossover));
      mUIPtr->mMutationProbLineEdit->setText(QString::number(mutation));

      mWarlockSettings->endGroup();
      mWarlockSettings->endGroup();
      mWarlockSettings->endGroup();
   }

   // Now go ahead and load up the pidprefs object
   SaveAutoPilot();
}

void AutopilotWidget::LoadPidPrefs()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   std::shared_ptr<QJsonDocument> curDoc = mw.GetMoverCreatorWidget()->GetCurrentVehicleFile();

   // Look for the pid preferences object for this vehicle, if it doesn't exist in
   // the JSON document then load from Warlock's QSettings...
   QJsonObject mainObj = curDoc->object();
   if (!mainObj.contains("pid_prefs"))
   {
      LoadPrefsFromWarlockSettings();
   }
   else
   {
      mPidPrefsObject    = mainObj["pid_prefs"].toObject();
      QString currentPid = mUIPtr->mPidComboBox->currentText();
      if (mPidPrefsObject.contains(currentPid))
      {
         QJsonObject curPidObj = mPidPrefsObject[currentPid].toObject();

         double minKp           = curPidObj["minKp"].toDouble();
         double maxKp           = curPidObj["maxKp"].toDouble();
         double minKi           = curPidObj["minKi"].toDouble();
         double maxKi           = curPidObj["maxKi"].toDouble();
         double minKd           = curPidObj["minKd"].toDouble();
         double maxKd           = curPidObj["maxKd"].toDouble();
         double runTime         = curPidObj["runTime"].toDouble();
         double errorWeight     = curPidObj["errorWeight"].toDouble();
         double overshootWeight = curPidObj["overshootWeight"].toDouble();

         double popSize   = curPidObj["populationSize"].toDouble();
         double maxGen    = curPidObj["maxGenerations"].toDouble();
         double carryover = curPidObj["carryoverFraction"].toDouble();
         double crossover = curPidObj["crossoverProbability"].toDouble();
         double mutation  = curPidObj["mutationProbability"].toDouble();

         mUIPtr->mMinKpLineEdit->setText(QString::number(minKp));
         mUIPtr->mMaxKpLineEdit->setText(QString::number(maxKp));
         mUIPtr->mMinKiLineEdit->setText(QString::number(minKi));
         mUIPtr->mMaxKiLineEdit->setText(QString::number(maxKi));
         mUIPtr->mMinKdLineEdit->setText(QString::number(minKd));
         mUIPtr->mMaxKdLineEdit->setText(QString::number(maxKd));
         mUIPtr->mRuntimeLineEdit->setText(QString::number(runTime));
         mUIPtr->mErrorWeightLineEdit->setText(QString::number(errorWeight));
         mUIPtr->mOvershootWeightLineEdit->setText(QString::number(overshootWeight));

         mUIPtr->mPopulationSizeLineEdit->setText(QString::number(popSize));
         mUIPtr->mMaxGenerationsLineEdit->setText(QString::number(maxGen));
         mUIPtr->mCarryoverFracLineEdit->setText(QString::number(carryover));
         mUIPtr->mCrossOverProbLineEdit->setText(QString::number(crossover));
         mUIPtr->mMutationProbLineEdit->setText(QString::number(mutation));
      }
      else
      {
         LoadPrefsFromWarlockSettings();
      }

      mUIPtr->mPidTunerFileLineEdit->setText(mTunerFileName);
   }
}

void AutopilotWidget::UpdatePidPrefsObject()
{
   QJsonObject curPidObj;
   curPidObj["minKp"]           = mUIPtr->mMinKpLineEdit->text().toDouble();
   curPidObj["maxKp"]           = mUIPtr->mMaxKpLineEdit->text().toDouble();
   curPidObj["minKi"]           = mUIPtr->mMinKiLineEdit->text().toDouble();
   curPidObj["maxKi"]           = mUIPtr->mMaxKiLineEdit->text().toDouble();
   curPidObj["minKd"]           = mUIPtr->mMinKdLineEdit->text().toDouble();
   curPidObj["maxKd"]           = mUIPtr->mMaxKdLineEdit->text().toDouble();
   curPidObj["runTime"]         = mUIPtr->mRuntimeLineEdit->text().toDouble();
   curPidObj["errorWeight"]     = mUIPtr->mErrorWeightLineEdit->text().toDouble();
   curPidObj["overshootWeight"] = mUIPtr->mOvershootWeightLineEdit->text().toDouble();

   curPidObj["populationSize"]       = mUIPtr->mPopulationSizeLineEdit->text().toDouble();
   curPidObj["maxGenerations"]       = mUIPtr->mMaxGenerationsLineEdit->text().toDouble();
   curPidObj["carryoverFraction"]    = mUIPtr->mCarryoverFracLineEdit->text().toDouble();
   curPidObj["crossoverProbability"] = mUIPtr->mCrossOverProbLineEdit->text().toDouble();
   curPidObj["mutationProbability"]  = mUIPtr->mMutationProbLineEdit->text().toDouble();

   mPidPrefsObject.insert(mUIPtr->mPidComboBox->currentText(), curPidObj);
}

void AutopilotWidget::HandleBrowseButtonClicked()
{
   QString fileName = QFileDialog::getOpenFileName(this, "Choose PID Tuning File", ".", "Text Files (*.txt)");
   mUIPtr->mPidTunerFileLineEdit->setText(fileName);
   mTunerFileName = fileName;
   SaveAutoPilot();
}

void AutopilotWidget::PreviousButtonClicked()
{
   MoverCreatorWidget* mcWidget = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget();
   mcWidget->MoveToPreviousTab();
}

void AutopilotWidget::NextButtonClicked()
{
   MoverCreatorWidget* mcWidget = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget();
   mcWidget->MoveToNextTab();
}

void AutopilotWidget::HandleCopyDataButtonClicked()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Vehicle* curVehicle = mw.GetMoverCreatorWidget()->GetVehicle();
   QString vehicleName = QString::fromStdString(curVehicle->GetVehicleName());
   QString vehicleType = "";
   if (curVehicle->IsAircraft())
   {
      vehicleType = "aircraft";
   }
   else if (curVehicle->IsWeapon())
   {
      vehicleType = "weapons";
   }

   QString autoPilotControlFile = mw.GetDataPath() + "/AFSIM_Scripts/" + vehicleName + "/p6dof_types/" +
                                  vehicleType + "/" + vehicleName.toLower() + "/controls/autopilot_config.txt";
   QString hw_autoPilotControlFile = mw.GetDataPath() + "/AFSIM_Scripts/" + vehicleName + "/p6dof_types/" +
                                     vehicleType + "/" + vehicleName.toLower() + "/controls/hw_autopilot_config.txt";

   //copy will not work if the file already exists, delete it first then copy
   QFile::remove(hw_autoPilotControlFile);
   QFile::copy(autoPilotControlFile, hw_autoPilotControlFile);
}

void AutopilotWidget::HandleHardwareCopyDataButtonClicked()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Vehicle* curVehicle = mw.GetMoverCreatorWidget()->GetVehicle();
   QString vehicleName = QString::fromStdString(curVehicle->GetVehicleName());
   QString vehicleType = "";
   if (curVehicle->IsAircraft())
   {
      vehicleType = "aircraft";
   }
   else if (curVehicle->IsWeapon())
   {
      vehicleType = "weapons";
   }

   QString autoPilotControlFile = mw.GetDataPath() + "/AFSIM_Scripts/" + vehicleName + "/p6dof_types/" +
                                  vehicleType + "/" + vehicleName.toLower() + "/controls/autopilot_config.txt";
   QString hw_autoPilotControlFile = mw.GetDataPath() + "/AFSIM_Scripts/" + vehicleName + "/p6dof_types/" +
                                     vehicleType + "/" + vehicleName.toLower() + "/controls/hw_autopilot_config.txt";

   // Copy will not work if the file already exists, delete it first then copy
   QFile::remove(autoPilotControlFile);
   QFile::copy(hw_autoPilotControlFile, autoPilotControlFile);
}

void AutopilotWidget::LaunchPilotsG_LimitsDialog()
{
   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentVehicleFile();
   QJsonObject mainObject = curDoc->object();
   G_LimitSettings settings{G_LimitSettings::PilotsGuidanceName()};
   settings.LoadFromFile(mainObject);

   mG_LimitDialogPtr->SetSettings(settings);
   mG_LimitDialogPtr->show();
}

void AutopilotWidget::LaunchHardwareG_LimitsDialog()
{
   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentVehicleFile();
   QJsonObject mainObject = curDoc->object();
   G_LimitSettings settings{G_LimitSettings::HardwareAutopilotName()};
   settings.LoadFromFile(mainObject);

   mG_LimitDialogPtr->SetSettings(settings);
   mG_LimitDialogPtr->show();
}

void AutopilotWidget::HandleG_LimitResults()
{
   G_LimitSettings settings = mG_LimitDialogPtr->GetSettings();
   if (settings.LimitsAreValid())
   {
      std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentVehicleFile();
      QJsonObject mainObject = curDoc->object();
      settings.SaveToFile(mainObject);
      curDoc->setObject(mainObject);
      MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->SaveCurrentVehicleFile();

      ScriptGeneratorP6DOF generator{};
      QString controlsFolder = generator.ControlsFolderPath();
      if (settings.GetName() == G_LimitSettings::PilotsGuidanceName())
      {
         generator.CreateAutopilotG_LimitConfigFile(controlsFolder);
      }
      else if (settings.GetName() == G_LimitSettings::HardwareAutopilotName())
      {
         generator.CreateHardwareG_LimitsConfigFile(controlsFolder);
      }
   }
   else
   {
      QMessageBox::warning(nullptr,
                           QString("Invalid Limits"),
                           QString("The maximum pitch g-load must be greater than the minimum, and the maximum yaw g-load should be positive."),
                           QMessageBox::Ok);
      if (settings.GetName() == G_LimitSettings::PilotsGuidanceName())
      {
         LaunchPilotsG_LimitsDialog();
      }
      else if (settings.GetName() == G_LimitSettings::HardwareAutopilotName())
      {
         LaunchHardwareG_LimitsDialog();
      }
   }
}

void AutopilotWidget::ToggleG_LimitButtons()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   ControlsWidget* controlsPtr = mw.GetMoverCreatorWidget()->GetControlsWidget();
   bool hwOn = controlsPtr->IsHardwareAutoPilotAvailable();
   bool pilotOn = controlsPtr->IsSyntheticPilotAvailable()
                  || controlsPtr->IsManualPilotAvailable()
                  || controlsPtr->IsGuidanceSystemAvailable();
   mUIPtr->mHardwareG_LimitsButton->setEnabled(hwOn);
   mUIPtr->mPilotsG_LimitButton->setEnabled(pilotOn);
}

} // namespace Designer
