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

#include "MoverCreatorWidget.hpp"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFileInfo>
#include <QKeyEvent>
#include <QMessageBox>
#include <QProcess>
#include <QWidgetAction>

#include "AuditErrorDialog.hpp"
#include "AutopilotWidget.hpp"
#include "CalculateAeroDialog.hpp"
#include "FlightTestWidget.hpp"
#include "GeometryGLWidget.hpp"
#include "GeometryWidget.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "PerformanceWidget.hpp"
#include "UtLog.hpp"
#include "ui_MoverCreatorWidget.h"

namespace Designer
{

MoverCreatorWidget::MoverCreatorWidget(QWidget*        aParent,
                                       Qt::WindowFlags aFlags /*= Qt::WindowFlags()*/)
   : QWidget(aParent, aFlags)
   , mUIPtr(new Ui::MoverCreatorWidget)
{
   mUIPtr->setupUi(this);
}

MoverCreatorWidget::~MoverCreatorWidget()
{
   delete mUIPtr;
   delete mCalculateAeroDialog;
}

void MoverCreatorWidget::Initialize()
{
   // Setup a new vehicle
   if (mVehicle != nullptr)
   {
      delete mVehicle;
   }
   mVehicle = new Vehicle();

   mUIPtr->mGeometryWidget->Initialize();
   mUIPtr->mControlsWidget->Initialize();
   mUIPtr->mAerodynamicsWidget->Initialize();
   mUIPtr->mPerformanceWidget->Initialize();

   mUIPtr->mTabWidget->setCurrentIndex(0);
   for (int i = 1; i < mUIPtr->mTabWidget->count(); ++i)
   {
      mUIPtr->mTabWidget->setTabEnabled(i, false);
   }

   // Output the version number
   auto outStream = ut::log::info() << "AFSIM Mover Creator";
   outStream.AddNote() << "Version: " << VersionInfo::GetVersionNumberString();

   connect(mUIPtr->mTabWidget, &QTabWidget::currentChanged, this, &MoverCreatorWidget::CurrentTabChanged);
}

Vehicle* MoverCreatorWidget::LoadNewVehicle()
{
   if (mVehicle != nullptr)
   {
      delete mVehicle;
   }
   mVehicle = new Vehicle();
   return mVehicle;
}

void MoverCreatorWidget::ChangeTheme(QString aThemeName)
{
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

   mUIPtr->mStartDialog->ChangeTheme(aThemeName);
}

StartDialog* MoverCreatorWidget::GetStartDialog()
{
   return mUIPtr->mStartDialog;
}

EngineDesignerWidget* MoverCreatorWidget::GetEngineDesignerWidget()
{
   return mUIPtr->mEngineDesignerWidget;
}

ControlsWidget* MoverCreatorWidget::GetControlsWidget()
{
   return mUIPtr->mControlsWidget;
}

GeometryWidget* MoverCreatorWidget::GetGeometryWidget()
{
   return mUIPtr->mGeometryWidget;
}

AerodynamicsWidget* MoverCreatorWidget::GetAerodynamicsWidget()
{
   return mUIPtr->mAerodynamicsWidget;
}

PerformanceWidget* MoverCreatorWidget::GetPerformanceWidget()
{
   return mUIPtr->mPerformanceWidget;
}

AutopilotWidget* MoverCreatorWidget::GetAutoPilotWidget()
{
   return mUIPtr->mAutopilotWidget;
}

FlightTestWidget* MoverCreatorWidget::GetFlightTestWidget()
{
   return mUIPtr->mFlightTestWidget;
}

void MoverCreatorWidget::ResetAllTabs()
{
   GetStartDialog()->Reset();
   GetEngineDesignerWidget()->Reset();
   GetControlsWidget()->Reset();
   GetGeometryWidget()->Reset();
   GetAerodynamicsWidget()->Reset();
   GetPerformanceWidget()->Reset();
   GetAutoPilotWidget()->Reset();
   GetFlightTestWidget()->Reset();
}

void MoverCreatorWidget::SetCurrentVehicleFile(QString aFileName)
{
   QFile jFile(aFileName);
   if (jFile.open(QIODevice::ReadOnly))
   {
      QByteArray fileData = jFile.readAll();
      mCurrentVehicleFile = std::make_shared<QJsonDocument>(QJsonDocument::fromJson(fileData));
      mOriginalVehicleFile = std::make_shared<QJsonDocument>(QJsonDocument::fromJson(fileData));

      mCurrentVehicleFilePath = aFileName;
      mOriginalVehicleFilePath = aFileName;

      QFileInfo jFileInfo(jFile);
      MoverCreatorMainWindow::GetInstance().AppendToTitle(" -- " + jFileInfo.baseName());
      MoverCreatorMainWindow::GetInstance().SetCurrentBaseName(jFileInfo.baseName());

      QJsonObject mainObj = mCurrentVehicleFile->object();
      QJsonObject geomObj = mainObj["geometry"].toObject();
      QJsonObject engineObj = geomObj["Engine"].toObject();
      QString engineTypeStr = engineObj["EngineType"].toString();
      QString engineModelStr = engineObj["EngineModel"].toString();

      QString dirPath = MoverCreatorMainWindow::GetDataPath() + "/Engines/";
      dirPath.append(engineTypeStr);
      dirPath.append("/" + engineModelStr + ".amc");
      SetCurrentEngineFile(dirPath);

      GetEngineDesignerWidget()->SetTypeAndModel(engineTypeStr, engineModelStr);
      GetGeometryWidget()->LoadVehicleGeometry();
      GetControlsWidget()->LoadControls();
      GetAutoPilotWidget()->LoadPidPrefs();
   }
}

void MoverCreatorWidget::SaveCurrentVehicleFile()
{
   QFile jFile(mCurrentVehicleFilePath);
   jFile.open(QIODevice::WriteOnly | QIODevice::Text);
   QJsonObject curObj = mCurrentVehicleFile->object();
   QJsonDocument myDoc(curObj);
   jFile.write(myDoc.toJson());
   jFile.close();

   QString curFile = curObj["fileName"].toString();
   MoverCreatorMainWindow::GetInstance().AppendToTitle(" -- " + curFile);
   MoverCreatorMainWindow::GetInstance().SetCurrentBaseName(curFile);
}

void MoverCreatorWidget::SaveTempVehicleFile()
{
   QJsonObject curObj = mCurrentVehicleFile->object();
   QString curFile    = curObj["fileName"].toString();
   QString tempFile   = "~" + curObj["fileName"].toString();
   QString curLoc     = mCurrentVehicleFilePath;
   QFileInfo curFileInfo(curLoc);
   QString pathStr    = curFileInfo.path();

   pathStr.append("/" + tempFile);

   QFile jFile(pathStr);
   jFile.open(QIODevice::WriteOnly | QIODevice::Text);
   QJsonDocument myDoc(curObj);
   jFile.write(myDoc.toJson());
   jFile.close();

   MoverCreatorMainWindow::GetInstance().SetCurrentBaseName(curFile); // Save base name prior to prepending the "*"
   curFile.prepend("*");
   MoverCreatorMainWindow::GetInstance().AppendToTitle(" -- " + curFile);
}

void MoverCreatorWidget::SetCurrentEngineFile(QString aFileName)
{
   QFile jFile(aFileName);
   if (jFile.open(QIODevice::ReadOnly))
   {
      QByteArray fileData = jFile.readAll();
      mCurrentEngineFile = std::make_shared<QJsonDocument>(QJsonDocument::fromJson(fileData));

      mCurrentEngineFilePath = aFileName;

      QFileInfo jFileInfo(jFile);
      if (mUIPtr->mTabWidget->currentIndex() == 1)
      {
         MoverCreatorMainWindow::GetInstance().AppendToTitle(" -- " + jFileInfo.baseName());
         MoverCreatorMainWindow::GetInstance().SetCurrentBaseName(jFileInfo.baseName());
      }
   }
}

void MoverCreatorWidget::SaveCurrentEngineFile()
{
   QFile jFile(mCurrentEngineFilePath);
   jFile.open(QIODevice::WriteOnly | QIODevice::Text);
   QJsonObject curObj = mCurrentEngineFile->object();
   QJsonDocument myDoc(curObj);
   jFile.write(myDoc.toJson());
   jFile.close();

   QString curFile = curObj["fileName"].toString();
   MoverCreatorMainWindow::GetInstance().AppendToTitle(" -- " + curFile);
   MoverCreatorMainWindow::GetInstance().SetCurrentBaseName(curFile);
}

void MoverCreatorWidget::SetGenerateMovers(bool aGenerateGM, bool aGenerateP6DOF, bool aGenerateRB6D, bool aGeneratePM6D)
{
   mGenerateGuidedMover = aGenerateGM;
   mGenerateP6DOFMover  = aGenerateP6DOF;
   mGenerateRB6DOFMover = aGenerateRB6D;
   mGeneratePM6DOFMover = aGeneratePM6D;

   mMoverCount = 0;

   if (mGenerateGuidedMover)
   {
      ++mMoverCount;
   }
   if (mGenerateP6DOFMover)
   {
      ++mMoverCount;
   }
   if (mGenerateRB6DOFMover)
   {
      ++mMoverCount;
   }
   if (mGeneratePM6DOFMover)
   {
       ++mMoverCount;
   }
}

void MoverCreatorWidget::SetInitialTab(int aTabIndex)
{
   // Be sure to clear any selections when moving to a new tab
   ClearSelections();

   if (mVehicle != nullptr)
   {
      // Reset vehicle attitude when moving to a new tab
      mVehicle->SetVehicleAlphaBeta(0.0, 0.0);

      // Reset vehicle rates when moving to a new tab
      mVehicle->SetVehicleYawPitchRollRates(0.0, 0.0, 0.0);

      // Set default speed conditions when moving to a new tab
      SetDefaultSpeedConditions();

      // Update the aero vectors
      mVehicle->CalculateAeroAttitudeVectors();
   }

   mUIPtr->mTabWidget->setCurrentIndex(aTabIndex);

   if (aTabIndex == cENGINE_TAB && !mCurrentVehicleFile.get())  // Editing or creating an engine without a current vehicle set, only enable the engine tab
   {
      mUIPtr->mTabWidget->setTabEnabled(cENGINE_TAB, true);
   }
   else
   {
      if (mCurrentEngineFile == nullptr)
      {
         mUIPtr->mTabWidget->setTabEnabled(cENGINE_TAB, false);
      }
      else
      {
         mUIPtr->mTabWidget->setTabEnabled(cENGINE_TAB, true);
      }

      // If there is currently no loaded vehicle, disable all vehicle related tabs
      if (mCurrentVehicleFile == nullptr)
      {
         for (int i = cCONTROLS_TAB; i <= mUIPtr->mTabWidget->count(); i++)
         {
            mUIPtr->mTabWidget->setTabEnabled(i, false);
         }
      }
      else
      {
         // A vehicle is loaded
         for (int i = cCONTROLS_TAB; i <= aTabIndex + 1; i++)
         {
            mUIPtr->mTabWidget->setTabEnabled(i, true);
         }
         if (GetVehicle()->GetVehicleControlConfiguration() == Vehicle::cNO_CONTROL)
         {
            mUIPtr->mTabWidget->setTabEnabled(Designer::cCONTROLS_TAB, false);
         }
      }
   }
}

void MoverCreatorWidget::MoveBetweenTabs()
{
   // Be sure to clear any selections when moving to a new tab
   ClearSelections();

   if (mVehicle != nullptr)
   {
      // Reset vehicle attitude when moving to a new tab
      mVehicle->SetVehicleAlphaBeta(0.0, 0.0);

      // Reset vehicle rates when moving to a new tab
      mVehicle->SetVehicleYawPitchRollRates(0.0, 0.0, 0.0);

      // Set default speed conditions when moving to a new tab
      SetDefaultSpeedConditions();

      // Update the aero vectors
      mVehicle->CalculateAeroAttitudeVectors();
   }

   SaveCurrentVehicleFile();
}

void MoverCreatorWidget::MoveToNextTab()
{
   if (mUIPtr->mTabWidget->currentIndex() == Designer::cPERFORMANCE_TAB
      && GetVehicle()->GetVehicleControlConfiguration() == Vehicle::cNO_CONTROL)
   {
      MoveTabs(2);
   }
   else
   {
      MoveTabs(1);
   }
}

void MoverCreatorWidget::MoveToPreviousTab()
{
   if (mUIPtr->mTabWidget->currentIndex() == Designer::cFLIGHT_TEST_TAB
      && GetVehicle()->GetVehicleControlConfiguration() == Vehicle::cNO_CONTROL)
   {
      MoveTabs(-2);
   }
   else
   {
      MoveTabs(-1);
   }
}

void MoverCreatorWidget::MoveTabs(int aDeltaIndex)
{
   MoveBetweenTabs();

   int newTab = mUIPtr->mTabWidget->currentIndex() + aDeltaIndex;
   newTab = std::min(std::max(0, newTab), mUIPtr->mTabWidget->count() - 1);

   mUIPtr->mTabWidget->setCurrentIndex(newTab);
}

void MoverCreatorWidget::SetEnableEngineTab(bool aEnable)
{
   mUIPtr->mTabWidget->setTabEnabled(cENGINE_TAB, aEnable);
}

void MoverCreatorWidget::DisableTabsAfterCurrent()
{
   for (int i = mUIPtr->mTabWidget->currentIndex() + 1; i < mUIPtr->mTabWidget->count(); ++i)
   {
      mUIPtr->mTabWidget->setTabEnabled(i, false);
   }
}

void MoverCreatorWidget::SetEngineTypeAndModel(QString aType, QString aModel)
{
   mUIPtr->mEngineDesignerWidget->SetTypeAndModel(aType, aModel);
}

void MoverCreatorWidget::ClearSelections()
{
   Designer::GeometryWidget* geometry = GetGeometryWidget();
   geometry->DeselectTreeWidget();
}

void MoverCreatorWidget::CurrentTabChanged(int aIndex)
{
   // Be sure to clear any selections when moving to a new tab
   ClearSelections();

   //reset vehicle and vehicle file if moving to the Start tab
   if (aIndex == cSETUP_TAB)
   {
      LoadNewVehicle();
      if (mCurrentVehicleFile != nullptr)
      {
         mCurrentVehicleFile.reset();
      }
   }

   if (mVehicle)
   {
      // Reset vehicle attitude when moving to a new tab
      mVehicle->SetVehicleAlphaBeta(0.0, 0.0);

      // Reset vehicle rates when moving to a new tab
      mVehicle->SetVehicleYawPitchRollRates(0.0, 0.0, 0.0);

      // Set default speed conditions when moving to a new tab
      SetDefaultSpeedConditions();

      // Update the aero vectors
      mVehicle->CalculateAeroAttitudeVectors();
   }

   // Save off the JSON for each tab
   GetGeometryWidget()->SaveAeroGeometry();
   GetControlsWidget()->SaveControls();
   GetAutoPilotWidget()->SaveAutoPilot();

   // Enable/disable tabs
   EnableDisableTabs(aIndex);
   if (aIndex == cGEOMETRY_TAB)
   {
      GetGeometryWidget()->ReloadGeometryEngines();
   }
   // Pause any animated OGL widgets if necessary
   if (aIndex == cCONTROLS_TAB)
   {
      GetAutoPilotWidget()->PauseGLAnimation();
      GetFlightTestWidget()->PauseGLAnimation();
   }
   else if (aIndex == cAUTOPILOT_TAB)
   {
      GetControlsWidget()->PauseGLAnimation();
      GetFlightTestWidget()->PauseGLAnimation();
   }
   else if (aIndex == cFLIGHT_TEST_TAB)
   {
      GetControlsWidget()->PauseGLAnimation();
      GetAutoPilotWidget()->PauseGLAnimation();
   }
   else
   {
      GetControlsWidget()->PauseGLAnimation();
      GetAutoPilotWidget()->PauseGLAnimation();
      GetFlightTestWidget()->PauseGLAnimation();
   }
}

void MoverCreatorWidget::EnableDisableTabs(int aIndex)
{
   // Special cases for a selected/active active Engine tab:
   if (aIndex == cENGINE_TAB)
   {
      // Ensure the engine tab is enabled
      mUIPtr->mTabWidget->setTabEnabled(cENGINE_TAB, true);

      // Check for special case if editing an engine while a vehicle is loaded
      if (mCurrentVehicleFile.get())
      {
         for (int i = cENGINE_TAB; i < cAERODYNAMICS_TAB; i++)
         {
            mUIPtr->mTabWidget->setTabEnabled(i, true);
         }

         for (int j = cAERODYNAMICS_TAB; j <= mUIPtr->mTabWidget->count(); j++)
         {
            mUIPtr->mTabWidget->setTabEnabled(j, false);
         }
      }

      return;
   }

   // We only want the user to be able to transition to the 
   // tab immediately after the current tab and all tabs before. 
   for (int i = cGEOMETRY_TAB; i <= aIndex + 1; i++)
   {
      mUIPtr->mTabWidget->setTabEnabled(i, true);
   } 
   for (int j = aIndex + 2; j <= mUIPtr->mTabWidget->count(); j++)
   {
      mUIPtr->mTabWidget->setTabEnabled(j, false);
   }

   // Engine tab is typically not enabled, except for special conditions above
   mUIPtr->mTabWidget->setTabEnabled(cENGINE_TAB, false);

   // If the vehicle does not have a controllable configuration, 
   // (or if they are only generating a guided mover)
   // disable the Controls and Autopilot tabs and  
   // allow the user to transition from the Performance tab to the
   // Flight Test tab (skipping the Autopilot and Controls tab).

   // Enabling the Flight Test tab from the Performance tab requires:
   // A SixDOF or P6DOF Mover being generated
   // TODO: This should be enabled for Guided Mover once we develop 
   // a Guided Mover scenario to run in Warlock
   bool enableFlightTest = mGenerateP6DOFMover | mGenerateRB6DOFMover | mGeneratePM6DOFMover;

   // Enabling the Controls and Autopilot tabs requires:
   // A SixDOF or P6DOF Mover being generated (same as for Flight Test)
   // An active vehicle control configuration
   bool enableControlTabs = enableFlightTest;
   enableControlTabs &= (mVehicle->GetVehicleControlConfiguration() != Vehicle::cNO_CONTROL);

   if (!enableControlTabs)
   {
      mUIPtr->mTabWidget->setTabEnabled(cCONTROLS_TAB, false);
      mUIPtr->mTabWidget->setTabToolTip(cCONTROLS_TAB, "Controls Tab is disabled for uncontrollable configurations");
      mUIPtr->mTabWidget->setTabEnabled(cAUTOPILOT_TAB, false);
      mUIPtr->mTabWidget->setTabToolTip(cAUTOPILOT_TAB, "Autopilot Tab is disabled for uncontrollable configurations");
   }
   else
   {
      mUIPtr->mTabWidget->setTabToolTip(cCONTROLS_TAB, "");
      mUIPtr->mTabWidget->setTabToolTip(cAUTOPILOT_TAB, "");
   }

   if (enableFlightTest)
   {
      if (mUIPtr->mTabWidget->currentIndex() == Designer::cPERFORMANCE_TAB)
      {
         mUIPtr->mTabWidget->setTabEnabled(cFLIGHT_TEST_TAB, true);
      }
   }
}

void MoverCreatorWidget::keyPressEvent(QKeyEvent* aEvent)
{
   QWidget::keyPressEvent(aEvent);
}

void MoverCreatorWidget::RevertChanges()
{
   if (QMessageBox::warning(this, "Revert File Warning",
                            "Warning: this will undo all changes made to the currently loaded vehicle!",
                            QDialogButtonBox::Ok, QDialogButtonBox::Cancel) == QDialogButtonBox::Ok)
   {
      // User clicked ok, revert to the original file
      mCurrentVehicleFile = mOriginalVehicleFile;
      GetControlsWidget()->LoadControls();
      GetGeometryWidget()->LoadVehicleGeometry();
      GetAutoPilotWidget()->LoadPidPrefs();
   }
}

void MoverCreatorWidget::SetDefaultSpeedConditions()
{
   double mach        = mVehicleMach;
   double altitude_ft = 20000.0;
   double altitude_m  = altitude_ft * UtMath::cM_PER_FT;

   UtAtmosphere atm   = mVehicle->GetAtmosphere();

   // Calculate speed, in feet per second
   double speed_fps = atm.CalcFpsFromMach(altitude_m, mach);

   // Calculate speed, in knots (knots true airspeed)
   double ktas = UtMath::cNMPH_PER_FPS * speed_fps;

   // Calculate knots calibrated airspeed
   double kcas = atm.CalcKcasFromFps(altitude_m, speed_fps);

   // Calculate dynamic pressure, in lbf per square foot
   double dynamicPressure_psf = atm.CalcDynamicPressure_psf(altitude_m, speed_fps);

   // Calculate density, in slugs per cubic foot
   double density_kgm3 = atm.Density(altitude_m);
   double density_slugs_ft3 = UtMath::cM_PER_FT * UtMath::cM_PER_FT * UtMath::cM_PER_FT / UtMath::cKG_PER_SLUG * density_kgm3;

   mVehicle->SetVehicleSpeedParameters(speed_fps, mach, ktas, kcas, dynamicPressure_psf, density_slugs_ft3);
}

void MoverCreatorWidget::ShowCalculatingAeroDialog()
{
   if (!mCalculateAeroDialog)
   {
      mCalculateAeroDialog = new CalculateAeroDialog(this);
   }
   mCalculateAeroDialog->SetLabelText("Calculating Vehicle Aerodynamics");
   mCalculateAeroDialog->SetWindowTitle("Calculating Aerodynamics...");
   mCalculateAeroDialog->setModal(false);
   mCalculateAeroDialog->exec();
}

// Hijack the Calculating Aero Dialog, change the text on the labels
void MoverCreatorWidget::ShowCalculatingPerformanceDialog()
{
   if (!mCalculateAeroDialog)
   {
      mCalculateAeroDialog = new CalculateAeroDialog(this);
   }

   mCalculateAeroDialog->SetLabelText("Calculating Vehicle Performance");
   mCalculateAeroDialog->SetWindowTitle("Calculating Performance...");
   mCalculateAeroDialog->setModal(false);
   mCalculateAeroDialog->show();
}

void MoverCreatorWidget::EndCalculatingPerformanceDialog(int aExitCode, QProcess::ExitStatus aStatus)
{
   if (!mCalculateAeroDialog)
   {
      return;
   }
   else
   {
      mCalculateAeroDialog->EndDialog();
   }
}

const int MoverCreatorWidget::GetCurrentTab()
{
   return mUIPtr->mTabWidget->currentIndex();
}

} // namespace Designer
