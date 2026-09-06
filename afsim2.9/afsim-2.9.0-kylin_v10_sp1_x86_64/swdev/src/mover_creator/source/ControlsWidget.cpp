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

#include "ControlsWidget.hpp"

#include <QMessageBox>

#include "G_LimitSettings.hpp"
#include "G_LimitSettingsDialog.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorWidget.hpp"
#include "ui_ControlsWidget.h"
#include "Vehicle.hpp"
#include "VehicleControls.hpp"

namespace Designer
{

ControlsWidget::ControlsWidget(QWidget*        aParent,
                               Qt::WindowFlags aFlags)
   : QWidget(aParent, aFlags)
   , mUIPtr(new Ui::ControlsWidget)
   , mG_LimitDialogPtr{new G_LimitSettingsDialog{}}
{
   mUIPtr->setupUi(this);

   ToggleManualPilotSettings();
   ToggleHardwareAutoPilot();

   connect(mUIPtr->mSyntheticPilotCheckBox, &QCheckBox::stateChanged, this, &ControlsWidget::PilotsControllersChanged);
   connect(mUIPtr->mSyntheticPilotCheckBox, &QCheckBox::stateChanged, this, &ControlsWidget::ValueChanged);
   connect(mUIPtr->mSyntheticPilotCheckBox, &QCheckBox::stateChanged, this, &ControlsWidget::PilotSelectionsChanged);

   connect(mUIPtr->mManualPilotCheckBox, &QCheckBox::stateChanged, this, &ControlsWidget::PilotsControllersChanged);
   connect(mUIPtr->mManualPilotCheckBox, &QCheckBox::stateChanged, this, &ControlsWidget::ToggleManualPilotSettings);
   connect(mUIPtr->mManualPilotCheckBox, &QCheckBox::stateChanged, this, &ControlsWidget::ValueChanged);
   connect(mUIPtr->mManualPilotCheckBox, &QCheckBox::stateChanged, this, &ControlsWidget::PilotSelectionsChanged);

   connect(mUIPtr->mGuidanceSystemCheckBox, &QCheckBox::stateChanged, this, &ControlsWidget::PilotsControllersChanged);
   connect(mUIPtr->mGuidanceSystemCheckBox, &QCheckBox::stateChanged, this, &ControlsWidget::ValueChanged);
   connect(mUIPtr->mGuidanceSystemCheckBox, &QCheckBox::stateChanged, this, &ControlsWidget::PilotSelectionsChanged);

   connect(mUIPtr->mHardwareAutopilotCheckBox, &QCheckBox::stateChanged, this, &ControlsWidget::PilotsControllersChanged);
   connect(mUIPtr->mHardwareAutopilotCheckBox, &QCheckBox::stateChanged, this, &ControlsWidget::ToggleHardwareAutoPilot);
   connect(mUIPtr->mHardwareAutopilotCheckBox, &QCheckBox::stateChanged, this, &ControlsWidget::ValueChanged);
   connect(mUIPtr->mHardwareAutopilotCheckBox, &QCheckBox::toggled, mUIPtr->mHardwareG_LimitsButton, &QPushButton::setEnabled);

   connect(mUIPtr->mActivePilotComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged), this, &ControlsWidget::ValueChanged);

   connect(mUIPtr->mControlSystemTypeComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged), this, &ControlsWidget::ControlSystemTypeChanged);

   connect(mUIPtr->mYawControlComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged), this, &ControlsWidget::ValueChanged);
   connect(mUIPtr->mWeaponYawControlComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged), this, &ControlsWidget::ValueChanged);
   connect(mUIPtr->mWeaponRollControlComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged), this, &ControlsWidget::ValueChanged);

   connect(mUIPtr->mRollControlSurfacesComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged), this, &ControlsWidget::ValueChanged);
   connect(mUIPtr->mWeaponRollControlSurfacesComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged), this, &ControlsWidget::ValueChanged);

   connect(mUIPtr->mAlphaControlGainCheckBox, &QCheckBox::stateChanged, this, &ControlsWidget::ValueChanged);
   connect(mUIPtr->mAlphaBetaControlsGainCheckBox, &QCheckBox::stateChanged, this, &ControlsWidget::ValueChanged);
   connect(mUIPtr->mControlSystemTypeComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged), this, &ControlsWidget::ValueChanged);
   connect(mUIPtr->mYawControlMappingComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged), this, &ControlsWidget::ValueChanged);
   connect(mUIPtr->mPitchControlMappingComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged), this, &ControlsWidget::ValueChanged);
   connect(mUIPtr->mRollControlMappingComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged), this, &ControlsWidget::ValueChanged);
   connect(mUIPtr->mYawTrimFactorLineEdit, &QLineEdit::editingFinished, this, &ControlsWidget::ValueChanged);
   connect(mUIPtr->mPitchTrimFactorLineEdit, &QLineEdit::editingFinished, this, &ControlsWidget::ValueChanged);
   connect(mUIPtr->mRollTrimFactorLineEdit, &QLineEdit::editingFinished, this, &ControlsWidget::ValueChanged);
   connect(mUIPtr->mPitchAugmentationLineEdit, &QLineEdit::editingFinished, this, &ControlsWidget::ValueChanged);
   connect(mUIPtr->mRollAugmentationLineEdit, &QLineEdit::editingFinished, this, &ControlsWidget::ValueChanged);

   connect(mUIPtr->mPilotsG_LimitsButton, &QPushButton::clicked, this, &ControlsWidget::LaunchPilotsG_LimitDialog);
   connect(mUIPtr->mHardwareG_LimitsButton, &QPushButton::clicked, this, &ControlsWidget::LaunchHardwareG_LimitDialog);
   connect(mG_LimitDialogPtr.get(), &G_LimitSettingsDialog::accepted, this, &ControlsWidget::HandleG_LimitResults);

   connect(mUIPtr->mNextButton, &QPushButton::clicked, this, &ControlsWidget::HandleNextButtonPressed);
   connect(mUIPtr->mSaveControlsButton, &QPushButton::clicked, this, &ControlsWidget::SaveControls);

   mCASValidator = new QDoubleValidator();
   mCASValidator->setBottom(0.0);
   mUIPtr->mPitchAugmentationLineEdit->setValidator(mCASValidator);
   mUIPtr->mRollAugmentationLineEdit->setValidator(mCASValidator);

   mUIPtr->mWeaponRollControlLabel->hide();
   mUIPtr->mWeaponRollControlComboBox->hide();
   mUIPtr->mWeaponYawControlLabel->hide();
   mUIPtr->mWeaponYawControlComboBox->hide();
   mUIPtr->mYawControlLabel->hide();
   mUIPtr->mYawControlComboBox->hide();
}

ControlsWidget::~ControlsWidget()
{
   delete mUIPtr;
}

void ControlsWidget::Initialize()
{
   // No need to do anything
}

void ControlsWidget::LoadControls()
{
   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentVehicleFile();
   if (!curDoc)
   {
      return;
   }

   QJsonObject mainObj = curDoc->object();
   if (!mainObj["controls"].isUndefined())
   {
      QJsonObject controlsObj = mainObj["controls"].toObject();

      QList<QWidget*> widgetList = findChildren<QWidget*>();
      for (auto& curWidget : widgetList)
      {
         auto isCheckBox = qobject_cast<QCheckBox*>(curWidget);
         if (isCheckBox)
         {
            isCheckBox->setChecked(controlsObj[isCheckBox->objectName()].toBool());
         }
         else
         {
            auto isComboBox = qobject_cast<QComboBox*>(curWidget);
            if (isComboBox)
            {
               isComboBox->setCurrentText(controlsObj[isComboBox->objectName()].toString());
            }
            else
            {
               auto isLineEdit = qobject_cast<QLineEdit*>(curWidget);
               if (isLineEdit)
               {
                  isLineEdit->setText(controlsObj[isLineEdit->objectName()].toString());
               }
            }
         }
      }
      mControlsJsonObject = controlsObj;
   }
   mUIPtr->mAnimatedGLWidget->AdjustViewingDistance(true);
   mUIPtr->mAnimatedGLWidget->StartAnimationTimer();
}

void ControlsWidget::SaveControls()
{
   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentVehicleFile();
   if (curDoc == nullptr)
   {
      return;
   }

   QJsonObject mainObj = curDoc->object();
   UpdateControlsObject();
   UpdateVehicleControls();
   mainObj["controls"] = mControlsJsonObject;
   curDoc->setObject(mainObj);

   MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->SaveCurrentVehicleFile();
}

void ControlsWidget::PilotsControllersChanged()
{
   // Populate the combo box for active pilot
   QStringList activePilotList;

   // Synthetic Pilot CheckBox ...
   if (IsSyntheticPilotAvailable())
   {
      activePilotList.append("Synthetic Pilot");
   }

   // Manual Pilot CheckBox ...
   if (IsManualPilotAvailable())
   {
      activePilotList.append("Manual Pilot");
   }

   // Hardware Autopilot CheckBox ...
   if (IsHardwareAutoPilotAvailable())
   {
      activePilotList.append("Hardware Autopilot");
   }

   // Guidance System CheckBox ...
   if (IsGuidanceSystemAvailable())
   {
      activePilotList.append("Guidance System");
   }

   if (activePilotList.size() == 0)
   {
      activePilotList.append("None");
   }

   // Populate the Active Pilot/Controller CheckBox
   mUIPtr->mActivePilotComboBox->clear();
   mUIPtr->mActivePilotComboBox->addItems(activePilotList);

   // Hide or enable/disable certain group boxes depending on CheckBox state...
   // mUIPtr->mManualPilotSettingGroupBox->setHidden(!mUIPtr->mManualPilotCheckBox->isChecked());
   // mUIPtr->mHardwarePilotGroupBox->setEnabled(mUIPtr->mHardwareAutopilotCheckBox->isChecked());
}

void ControlsWidget::ControlSystemTypeChanged(const QString& aText)
{
   // Enable/disable CAS elements
   if (aText == "CAS")
   {
      QPalette palette = MoverCreatorMainWindow::GetInstance().GetPalette();
      mUIPtr->mCASSettingsGroupBox->setEnabled(true);
      mUIPtr->mCASSettingsGroupBox->setPalette(palette);

      QList<QLabel*> labelList = mUIPtr->mCASSettingsGroupBox->findChildren<QLabel*>();
      for (auto& curLabel : labelList)
      {
         curLabel->setPalette(palette);
      }
   }
   else
   {
      mUIPtr->mCASSettingsGroupBox->setEnabled(false);
      QPalette CASpalette = mUIPtr->mCASSettingsGroupBox->palette();
      CASpalette.setColor(QPalette::WindowText, Qt::gray);
      mUIPtr->mCASSettingsGroupBox->setPalette(CASpalette);
      QList<QLabel*> casLabelList = mUIPtr->mCASSettingsGroupBox->findChildren<QLabel*>();
      for (auto& curLabel : casLabelList)
      {
         curLabel->setPalette(CASpalette);
      }
   }

   // Enable/disable SAS elements
   if (aText == "SAS")
   {
      QPalette palette = MoverCreatorMainWindow::GetInstance().GetPalette();
      mUIPtr->mSASSettingsGroupBox->setEnabled(true);
      mUIPtr->mSASSettingsGroupBox->setPalette(palette);

      QList<QLabel*> labelList = mUIPtr->mSASSettingsGroupBox->findChildren<QLabel*>();
      for (auto& curLabel : labelList)
      {
         curLabel->setPalette(palette);
      }
   }
   else
   {
      mUIPtr->mSASSettingsGroupBox->setEnabled(false);
      QPalette SASpalette = mUIPtr->mSASSettingsGroupBox->palette();
      SASpalette.setColor(QPalette::WindowText, Qt::gray);
      mUIPtr->mSASSettingsGroupBox->setPalette(SASpalette);
      QList<QLabel*> sasLabelList = mUIPtr->mSASSettingsGroupBox->findChildren<QLabel*>();
      for (auto& curLabel : sasLabelList)
      {
         curLabel->setPalette(SASpalette);
      }
   }
}

void ControlsWidget::HandleTurnControlChanged(bool aUsesBankToTurnControl)
{
   if (aUsesBankToTurnControl)
   {
      MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
      Designer::GeometryWing* hasWing = mw.GetMoverCreatorWidget()->GetVehicle()->GetGeometry().GetWing();
      if (hasWing != nullptr)
      {
         mUIPtr->mWeaponRollControlSurfacesLabel->show();
         mUIPtr->mWeaponRollControlSurfacesComboBox->show();

         mUIPtr->mAlphaControlGainCheckBox->show();
         mUIPtr->mAlphaBetaControlsGainCheckBox->hide();
      }
      else
      {
         mUIPtr->mWeaponRollControlSurfacesLabel->hide();
         mUIPtr->mWeaponRollControlSurfacesComboBox->hide();

         mUIPtr->mAlphaControlGainCheckBox->hide();
         mUIPtr->mAlphaBetaControlsGainCheckBox->show();
      }

      mUIPtr->mWeaponYawControlLabel->show();
      mUIPtr->mWeaponYawControlComboBox->show();
      mUIPtr->mWeaponRollControlLabel->hide();
      mUIPtr->mWeaponRollControlComboBox->hide();
   }

   else
   {
      mUIPtr->mWeaponYawControlLabel->hide();
      mUIPtr->mWeaponYawControlComboBox->hide();
      mUIPtr->mWeaponRollControlLabel->show();
      mUIPtr->mWeaponRollControlComboBox->show();

      mUIPtr->mWeaponRollControlSurfacesLabel->hide();
      mUIPtr->mWeaponRollControlSurfacesComboBox->hide();

      mUIPtr->mAlphaControlGainCheckBox->hide();
      mUIPtr->mAlphaBetaControlsGainCheckBox->show();
   }
}

void ControlsWidget::showEvent(QShowEvent* aEvent)
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Vehicle* vehicle = mw.GetMoverCreatorWidget()->GetVehicle();
   bool isAircraft = vehicle->IsAircraft();
   bool usesBankToTurn = vehicle->UsesBankToTurnControl();

   if (isAircraft)
   {
      mUIPtr->mControlOptionsStackedWidget->setCurrentIndex(1);
      mUIPtr->mAlphaControlGainCheckBox->show();
      mUIPtr->mAlphaBetaControlsGainCheckBox->hide();

      mUIPtr->mYawControlLabel->show();
      mUIPtr->mYawControlComboBox->show();
   }
   else
   {
      HandleTurnControlChanged(usesBankToTurn);
      mUIPtr->mControlOptionsStackedWidget->setCurrentIndex(0);
      mUIPtr->mAlphaControlGainCheckBox->hide();
      mUIPtr->mAlphaBetaControlsGainCheckBox->show();
   }

   mUIPtr->mAnimatedGLWidget->StartAnimationTimer();
}

void ControlsWidget::ToggleManualPilotSettings()
{
   bool disableControls = !mUIPtr->mManualPilotCheckBox->isChecked();
   mUIPtr->mManualPilotSettingGroupBox->setDisabled(disableControls);


   QList<QLabel*> manualPilotLabelList = mUIPtr->mManualPilotSettingGroupBox->findChildren<QLabel*>();

   if (disableControls)
   {
      QPalette palette = mUIPtr->mManualPilotSettingGroupBox->palette();
      palette.setColor(QPalette::WindowText, Qt::gray);
      mUIPtr->mManualPilotSettingGroupBox->setPalette(palette);
      mUIPtr->mCASSettingsGroupBox->setPalette(palette);
      mUIPtr->mSASSettingsGroupBox->setPalette(palette);

      for (auto& curLabel : manualPilotLabelList)
      {
         curLabel->setPalette(palette);
      }
   }
   else
   {
      QPalette curPalette = MoverCreatorMainWindow::GetInstance().GetPalette();
      mUIPtr->mManualPilotSettingGroupBox->setPalette(curPalette);
      for (auto& curLabel : manualPilotLabelList)
      {
         curLabel->setPalette(curPalette);
      }

      if (mUIPtr->mControlSystemTypeComboBox->currentText() == "CAS")
      {
         mUIPtr->mCASSettingsGroupBox->setPalette(curPalette);
      }
      else
      {
         QPalette palette = mUIPtr->mManualPilotSettingGroupBox->palette();
         palette.setColor(QPalette::WindowText, Qt::gray);

         mUIPtr->mCASSettingsGroupBox->setPalette(palette);
         for (auto& curLabel : mUIPtr->mCASSettingsGroupBox->findChildren<QLabel*>())
         {
            curLabel->setPalette(palette);
         }
      }

      if (mUIPtr->mControlSystemTypeComboBox->currentText() == "SAS")
      {
         mUIPtr->mSASSettingsGroupBox->setPalette(curPalette);
      }
      else
      {
         QPalette palette = mUIPtr->mManualPilotSettingGroupBox->palette();
         palette.setColor(QPalette::WindowText, Qt::gray);

         mUIPtr->mSASSettingsGroupBox->setPalette(palette);
         for (auto& curLabel : mUIPtr->mSASSettingsGroupBox->findChildren<QLabel*>())
         {
            curLabel->setPalette(palette);
         }
      }
   }
}

void ControlsWidget::ToggleHardwareAutoPilot()
{
   //bool disableHWA = !mUIPtr->mHardwareAutopilotCheckBox->isChecked();
   //mUIPtr->mHardwarePilotGroupBox->setDisabled(disableHWA);

   //if (disableHWA)
   //{
   //   QPalette palette = mUIPtr->mHardwarePilotGroupBox->palette();
   //   palette.setColor(QPalette::WindowText, Qt::gray);
   //   mUIPtr->mHardwarePilotGroupBox->setPalette(palette);

   //   QList<QLabel*> labelList = mUIPtr->mHardwarePilotGroupBox->findChildren<QLabel*>();
   //   for (auto& curLabel : labelList)
   //   {
   //      curLabel->setPalette(palette);
   //   }
   //}
   //else
   //{
   //   QPalette palette = MoverCreatorMainWindow::GetInstance().GetPalette();
   //   mUIPtr->mHardwarePilotGroupBox->setPalette(palette);

   //   QList<QLabel*> labelList = mUIPtr->mHardwarePilotGroupBox->findChildren<QLabel*>();
   //   for (auto& curLabel : labelList)
   //   {
   //      curLabel->setPalette(palette);
   //   }
   //}
}

void ControlsWidget::HandleNextButtonPressed()
{
   MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->MoveToNextTab();
}

void ControlsWidget::UpdateControlsObject()
{
   QJsonObject updatedObj;
   QList<QWidget*> widgetList = findChildren<QWidget*>();
   for (auto& curWidget : widgetList)
   {
      auto isCheckBox = qobject_cast<QCheckBox*>(curWidget);
      if (isCheckBox)
      {
         updatedObj[isCheckBox->objectName()] = isCheckBox->isChecked();
      }
      else
      {
         auto isComboBox = qobject_cast<QComboBox*>(curWidget);
         if (isComboBox)
         {
            updatedObj[isComboBox->objectName()] = isComboBox->currentText();
         }
         else
         {
            auto isLineEdit = qobject_cast<QLineEdit*>(curWidget);
            if (isLineEdit)
            {
               updatedObj[isLineEdit->objectName()] = isLineEdit->text();
            }
         }
      }
   }
   mControlsJsonObject = updatedObj;
}

void ControlsWidget::UpdateVehicleControls()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Vehicle* vehicle = mw.GetMoverCreatorWidget()->GetVehicle();
   if (vehicle == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   VehicleControls& controls = vehicle->GetControls();

   // First, determine if we have control -- we need at least one "pilot" to have control
   if (!mUIPtr->mSyntheticPilotCheckBox->isChecked() &&
       !mUIPtr->mManualPilotCheckBox->isChecked() &&
       !mUIPtr->mHardwareAutopilotCheckBox->isChecked() &&
       !mUIPtr->mGuidanceSystemCheckBox->isChecked())
   {
      // We lack any pilots, so we lack control
      controls.ClearAllControlsAndPilots();
      return;
   }

   // Check for an aircraft
   bool isAircraft = vehicle->IsAircraft();
   controls.SetIsAircraft(isAircraft);

   // Determine active pilot type
   VehicleControls::VehicleControlPilotType activePilot = VehicleControls::cNO_CONTROL;

   QString textStr = mUIPtr->mActivePilotComboBox->currentText();

   if (textStr == "Synthetic Pilot")
   {
      activePilot = VehicleControls::cSYNTHETIC_PILOT;
   }
   else if (textStr == "Manual Pilot")
   {
      activePilot = VehicleControls::cMANUAL_PILOT;
   }
   else if (textStr == "Hardware Autopilot")
   {
      activePilot = VehicleControls::cHARDWARE_AUTOPILOT;
   }
   else if (textStr == "Guidance System")
   {
      activePilot = VehicleControls::cGUIDANCE_SYSTEM;
   }

   controls.SetPilotTypes(mUIPtr->mSyntheticPilotCheckBox->isChecked(),
                          mUIPtr->mManualPilotCheckBox->isChecked(),
                          mUIPtr->mHardwareAutopilotCheckBox->isChecked(),
                          mUIPtr->mGuidanceSystemCheckBox->isChecked(),
                          activePilot);

   // Turn control type can be bank-to-turn or skid-to-turn
   if (isAircraft)
   {
      bool alphaControlGain = mUIPtr->mAlphaControlGainCheckBox->isChecked();

      VehicleControls::VehicleControlBankToTurnRollControl bankControls = VehicleControls::cAILERONS_ONLY;
      QString bankControlsTextString = mUIPtr->mRollControlSurfacesComboBox->currentText();

      if (bankControlsTextString == "Stabilizers Only")
      {
         bankControls = VehicleControls::cSTABILIZERS_ONLY;
      }
      else if (bankControlsTextString == "Ailerons and Stabilizers")
      {
         bankControls = VehicleControls::cAILERONS_AND_STABILIZERS;
      }

      VehicleControls::VehicleControlBankToTurnYawControlType yawControls = VehicleControls::cNO_YAW_CONTROL;
      QString yawControlTypeTextString = mUIPtr->mYawControlComboBox->currentText();

      if (yawControlTypeTextString == "Yaw Damper")
      {
         yawControls = VehicleControls::cYAW_DAMPER;
      }

      controls.SetBankToTurnControlSettings(bankControls, yawControls, alphaControlGain);
   }
   else
   {
      if (vehicle->UsesBankToTurnControl())
      {
         // Using bank-to-turn control
         bool alphaControlGain = mUIPtr->mAlphaControlGainCheckBox->isChecked();

         VehicleControls::VehicleControlBankToTurnRollControl bankControls = VehicleControls::cAILERONS_ONLY;
         QString bankControlsTextString = mUIPtr->mWeaponRollControlSurfacesComboBox->currentText();

         if (bankControlsTextString == "Stabilizers Only")
         {
            bankControls = VehicleControls::cSTABILIZERS_ONLY;
         }
         else if (bankControlsTextString == "Ailerons and Stabilizers")
         {
            bankControls = VehicleControls::cAILERONS_AND_STABILIZERS;
         }

         VehicleControls::VehicleControlBankToTurnYawControlType yawControls = VehicleControls::cNO_YAW_CONTROL;
         QString yawControlTypeTextString = mUIPtr->mWeaponYawControlComboBox->currentText();

         if (yawControlTypeTextString == "Yaw Damper")
         {
            yawControls = VehicleControls::cYAW_DAMPER;
         }

         controls.SetBankToTurnControlSettings(bankControls, yawControls, alphaControlGain);
      }
      else
      {
         // Using skid-to-turn control
         bool alphaBetaControlsGain = mUIPtr->mAlphaBetaControlsGainCheckBox->isChecked();

         VehicleControls::VehicleControlSkidToTurnRollControlType rollControlType = VehicleControls::cNO_ROLL_CONTROL;
         QString textString = mUIPtr->mWeaponRollControlComboBox->currentText();

         if (textString == "Roll Damper")
         {
            rollControlType = VehicleControls::cROLL_DAMPER;
         }
         else if (textString == "Full Control")
         {
            rollControlType = VehicleControls::cFULL_ROLL_CONTROL;
         }

         controls.SetSkidToTurnControlSettings(rollControlType, alphaBetaControlsGain);
      }
   }

   // Manual pilot settings, if appropriate
   if (mUIPtr->mManualPilotCheckBox->isChecked())
   {
      bool useExponentialYawControlMapping = false;
      if (mUIPtr->mYawControlMappingComboBox->currentText() == "Exponential")
      {
         useExponentialYawControlMapping = true;
      }

      bool useExponentialPitchControlMapping = false;
      if (mUIPtr->mPitchControlMappingComboBox->currentText() == "Exponential")
      {
         useExponentialPitchControlMapping = true;
      }

      bool useExponentialRollControlMapping = false;
      if (mUIPtr->mRollControlMappingComboBox->currentText() == "Exponential")
      {
         useExponentialRollControlMapping = true;
      }

      double yawTrimFactor   = mUIPtr->mYawTrimFactorLineEdit->text().toDouble();
      double pitchTrimFactor = mUIPtr->mPitchTrimFactorLineEdit->text().toDouble();
      double rollTrimFactor  = mUIPtr->mRollTrimFactorLineEdit->text().toDouble();

      VehicleControls::VehicleControlAugmentationType manualPilotAugmentationType = VehicleControls::cDIRECT_CONTROL;
      if (mUIPtr->mControlSystemTypeComboBox->currentText() == "SAS")
      {
         manualPilotAugmentationType = VehicleControls::cSTABILITY_AUGMENTATION;
      }
      else if (mUIPtr->mControlSystemTypeComboBox->currentText() == "CAS")
      {
         manualPilotAugmentationType = VehicleControls::cCONTROL_AUGMENTATION;
      }

      controls.SetManualPilotDirectControlSettings(useExponentialYawControlMapping,
                                                   useExponentialPitchControlMapping,
                                                   useExponentialRollControlMapping,
                                                   yawTrimFactor,
                                                   pitchTrimFactor,
                                                   rollTrimFactor,
                                                   manualPilotAugmentationType);

      // Retain the data entered in the combo boxes and checkboxes regardless of selected
      // augmentation type in case a user wants to change the pilot augmentation type after the fact

      double CAS_PitchG = mUIPtr->mPitchAugmentationLineEdit->text().toDouble();
      double CAS_RollRate_dps = mUIPtr->mRollAugmentationLineEdit->text().toDouble();
      bool SAS_AugmentPitchAxis = mUIPtr->mPitchSASCheckBox->isChecked();
      bool SAS_AugmentYawAxis = mUIPtr->mYawSASCheckBox->isChecked();
      bool SAS_AugmentRollAxis = mUIPtr->mRollSASCheckBox->isChecked();

      controls.SetManualPilotSAS_Settings(SAS_AugmentPitchAxis,
                                          SAS_AugmentYawAxis,
                                          SAS_AugmentRollAxis);

      controls.SetManualPilotCAS_Settings(CAS_PitchG,
                                          CAS_RollRate_dps);
   }
}

void ControlsWidget::ValueChanged()
{
   UpdateControlsObject();
   UpdateVehicleControls();

   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentVehicleFile();

   QJsonObject mainObj = curDoc->object();
   mainObj["controls"] = mControlsJsonObject;
   curDoc->setObject(mainObj);

   MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->SaveTempVehicleFile();
}

void ControlsWidget::PilotSelectionsChanged(int aState)
{
   bool enable = mUIPtr->mSyntheticPilotCheckBox->isChecked()
                 || mUIPtr->mManualPilotCheckBox->isChecked()
                 || mUIPtr->mGuidanceSystemCheckBox->isChecked();
   mUIPtr->mPilotsG_LimitsButton->setEnabled(enable);
}

void ControlsWidget::LaunchPilotsG_LimitDialog()
{
   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentVehicleFile();
   QJsonObject mainObject = curDoc->object();
   G_LimitSettings settings{G_LimitSettings::PilotsGuidanceName()};
   settings.LoadFromFile(mainObject);

   mG_LimitDialogPtr->SetSettings(settings);
   mG_LimitDialogPtr->show();
}

void ControlsWidget::LaunchHardwareG_LimitDialog()
{
   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentVehicleFile();
   QJsonObject mainObject = curDoc->object();
   G_LimitSettings settings{G_LimitSettings::HardwareAutopilotName()};
   settings.LoadFromFile(mainObject);

   mG_LimitDialogPtr->SetSettings(settings);
   mG_LimitDialogPtr->show();
}

void ControlsWidget::HandleG_LimitResults()
{
   G_LimitSettings settings = mG_LimitDialogPtr->GetSettings();
   if (settings.LimitsAreValid())
   {
      std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentVehicleFile();
      QJsonObject mainObject = curDoc->object();
      settings.SaveToFile(mainObject);
      curDoc->setObject(mainObject);
      MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->SaveCurrentVehicleFile();
   }
   else
   {
      QMessageBox::warning(nullptr,
                           QString("Invalid Limits"),
                           QString("The maximum pitch g-load must be greater than the minimum, and the maximum yaw g-load should be positive."),
                           QMessageBox::Ok);
      if (settings.GetName() == G_LimitSettings::PilotsGuidanceName())
      {
         LaunchPilotsG_LimitDialog();
      }
      else if (settings.GetName() == G_LimitSettings::HardwareAutopilotName())
      {
         LaunchHardwareG_LimitDialog();
      }
   }
}

void ControlsWidget::PauseGLAnimation()
{
   mUIPtr->mAnimatedGLWidget->PauseAnimationTimer();
}

bool ControlsWidget::IsHardwareAutoPilotAvailable()
{
   return mUIPtr->mHardwareAutopilotCheckBox->isChecked();
}

bool ControlsWidget::IsSyntheticPilotAvailable()
{
   return mUIPtr->mSyntheticPilotCheckBox->isChecked();
}

bool ControlsWidget::IsManualPilotAvailable()
{
   return mUIPtr->mManualPilotCheckBox->isChecked();
}

bool ControlsWidget::IsGuidanceSystemAvailable()
{
   return mUIPtr->mGuidanceSystemCheckBox->isChecked();
}

void ControlsWidget::Reset()
{
   mUIPtr->mAnimatedGLWidget->SetResettingView(true);
}

} // namespace Designer
