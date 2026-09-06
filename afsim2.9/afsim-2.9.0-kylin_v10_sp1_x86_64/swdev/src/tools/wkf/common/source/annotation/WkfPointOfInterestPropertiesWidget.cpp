// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WkfPointOfInterestPropertiesWidget.hpp"

#include <array>

#include "VaAttachmentModel.hpp"
#include "VaEntity.hpp"
#include "VaUtils.hpp"
#include "WkfAttachmentDecorator.hpp"
#include "WkfBullseye.hpp"
#include "WkfEnvironment.hpp"
#include "WkfPointOfInterest.hpp"
#include "WkfUnitsObject.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"
#include "ui_WkfPointOfInterestProperties.h"

Annotation::PointOfInterestPropertiesWidget::PointOfInterestPropertiesWidget(QWidget*        parent /*= nullptr*/,
                                                                             Qt::WindowFlags f /*= Qt::WindowFlags()*/)
   : QWidget(parent, f | Qt::Tool | Qt::MSWindowsFixedSizeDialogHint)
   , mUi()
   , mCreatingNewPoi(false)
   , mEntityPtr(nullptr)
{
   mUi.setupUi(this);
   setWindowTitle("POI Properties");
   setEnabled(false);

   connect(mUi.mLatitude, &UtQtLatPosLineEdit::textChanged, this, &PointOfInterestPropertiesWidget::PositionUpdated);
   connect(mUi.mLongitude, &UtQtLatPosLineEdit::textChanged, this, &PointOfInterestPropertiesWidget::PositionUpdated);
   connect(mUi.mAltitude, &UtQtLengthValueEdit::ValueChanged, this, &PointOfInterestPropertiesWidget::PositionUpdated);
   connect(mUi.mName, &QLineEdit::editingFinished, this, &PointOfInterestPropertiesWidget::NameChanged);
   connect(mUi.mBullseyeCheckbox, &QCheckBox::stateChanged, this, &PointOfInterestPropertiesWidget::BullseyeUpdated);
   connect(mUi.mLocationSelector,
           &wkf::LocationSelector::Selected,
           this,
           [this](double aLat, double aLon)
           {
              mUi.mLatitude->SetValue(aLat);
              mUi.mLongitude->SetValue(aLon);
           });
   connect(mUi.mButtonBox->button(QDialogButtonBox::Ok),
           &QPushButton::clicked,
           this,
           [&]()
           {
              if (mUi.mLatitude->IsValid() && mUi.mLongitude->IsValid() && mUi.mAltitude->IsValid())
              {
                 mAcceptChanges = true;
                 close();
              }
              else
              {
                 QMessageBox::warning(this,
                                      "Invalid Input",
                                      "Please make sure the POI inputs are valid before finalizing any changes.");
              }
           });
   connect(mUi.mButtonBox->button(QDialogButtonBox::Cancel),
           &QPushButton::clicked,
           this,
           [&]()
           {
              mAcceptChanges = false;
              close();
           });

   QRegExp validInput("([^/#\\s]|/[^*#\\s])*");
   mUi.mName->setValidator(new QRegExpValidator(validInput, mUi.mName));
}

void Annotation::PointOfInterestPropertiesWidget::SetEntity(vespa::VaEntity* aEntityPtr, bool aCreate)
{
   if (!mAcceptChanges)
   {
      CancelChanges();
   }
   mAcceptChanges  = false;
   mCreatingNewPoi = aCreate;
   mEntityPtr      = aEntityPtr;
   if (mEntityPtr)
   {
      bool isBullseye = mEntityPtr->IsA_TypeOf<wkf::Bullseye>();
      if (isBullseye)
      {
         mUi.mAltitude->SetValue(0);
      }
      mUi.mBullseyeCheckbox->setChecked(isBullseye);
      // Gray out if the widget is a bullseye. Do not set the value to 0 until the change is confirmed
      mUi.mAltitude->setEnabled(!isBullseye);

      PoiInfo info;
      mUi.mName->setText(QString::fromStdString(mEntityPtr->GetName()));
      info.mName = mEntityPtr->GetName();

      BlockPositionSignals(true);
      UtLatPos lat = mEntityPtr->GetPosition().GetLat();
      UtLonPos lon = mEntityPtr->GetPosition().GetLon();
      mUi.mLatitude->SetValue(lat);
      mUi.mLongitude->SetValue(lon);

      // Convert from the old UtUnitLength to UtLengthValue for use with UtQtLengthValueEdit
      // Altitude will be in meters here
      double                 altitude_m = mEntityPtr->GetPosition().GetAlt();
      UtUnitLength::BaseUnit unit       = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAltitudeUnit(altitude_m);
      mUi.mAltitude->SetValue(UtLengthValue(UtUnitLength::ConvertFromStandard(altitude_m, unit), unit));

      info.mLat      = lat;
      info.mLon      = lon;
      info.mAlt      = altitude_m;
      info.mBullseye = isBullseye;

      BlockPositionSignals(false);
      mOriginalValues = info;
   }
   UpdateWidget();
}

void Annotation::PointOfInterestPropertiesWidget::closeEvent(QCloseEvent* aEvent)
{
   if (mAcceptChanges)
   {
      AcceptChanges();
   }
   else
   {
      CancelChanges();
   }
   QWidget::closeEvent(aEvent);
}

void Annotation::PointOfInterestPropertiesWidget::hideEvent(QHideEvent* aEvent)
{
   if (!mAcceptChanges)
   {
      CancelChanges();
   }
   QWidget::hideEvent(aEvent);
}

void Annotation::PointOfInterestPropertiesWidget::SetExportable()
{
   wkf::PointOfInterest* poi = dynamic_cast<wkf::PointOfInterest*>(mEntityPtr);
   if (poi)
   {
      poi->SetExportable(true);
   }
}

void Annotation::PointOfInterestPropertiesWidget::UpdateWidget()
{
   if (mEntityPtr)
   {
      setEnabled(true);
   }
   else
   {
      setEnabled(false);
   }
}


void Annotation::PointOfInterestPropertiesWidget::NameChanged()
{
   if (mEntityPtr)
   {
      std::string name = mUi.mName->text().toStdString();
      // If the name is different than our current name
      if (mEntityPtr->GetName() != name)
      {
         bool nameTaken   = false;
         bool nameInvalid = false;

         QString uiText = mUi.mName->text();
         if (mUi.mName->text().isEmpty() || uiText.toStdString().find_first_of("\t\n ") != std::string::npos)
         {
            nameInvalid = true;
         }


         vespa::VaEntityList list;

         vaEnv.GetEntityList(list);

         for (auto&& it : list)
         {
            if (name == it->GetName())
            {
               nameTaken = true;
               break;
            }
         }

         if (nameInvalid)
         {
            QMessageBox::warning(nullptr,
                                 "Name Invalid",
                                 "A POI name cannot be empty or contain whitespace, unable to change name.");
         }
         else if (nameTaken)
         {
            QMessageBox::warning(nullptr,
                                 "Name Exists",
                                 "A POI/platform with the name " + QString::fromStdString(name) +
                                    " already exist, unable to change name. Reverting to original name.");
            mUi.mName->setText(QString::fromStdString(mOriginalValues.mName));
         }
         else
         {
            NameOperation();
         }
      }
   }
}


void Annotation::PointOfInterestPropertiesWidget::BlockPositionSignals(bool aState)
{
   mUi.mLatitude->blockSignals(aState);
   mUi.mLongitude->blockSignals(aState);
   mUi.mAltitude->blockSignals(aState);
}

void Annotation::PointOfInterestPropertiesWidget::PositionUpdated()
{
   std::array<double, 3> lla = {mEntityPtr->GetPosition().GetLat(),
                                mEntityPtr->GetPosition().GetLon(),
                                mEntityPtr->GetPosition().GetAlt()};
   if (mUi.mLatitude->IsValid())
   {
      lla[0] = mUi.mLatitude->GetValue();
   }
   if (mUi.mLongitude->IsValid())
   {
      lla[1] = mUi.mLongitude->GetValue();
   }
   if (mUi.mAltitude->IsValid())
   {
      lla[2] = mUi.mAltitude->GetValue();
   }
   PositionOperation(vespa::VaPosition(lla[0], lla[1], lla[2]));
}

void Annotation::PointOfInterestPropertiesWidget::BullseyeUpdated(int aState)
{
   bool isBullseye = (aState == Qt::CheckState::Checked) ? true : false;
   mUi.mAltitude->setEnabled(!isBullseye);
   // Modifying only the entity instead of what is in the altitude line-edit allows for reverting altitude to whatever
   // was previously inputted when toggling the bullseye (instead of just what was inputted when the dialog was initially opened)
   vespa::VaPosition altPos = mEntityPtr->GetPosition();
   if (!isBullseye && mUi.mAltitude->IsValid())
   {
      altPos.SetAlt(mUi.mAltitude->GetValue());
   }
   else
   {
      altPos.SetAlt(0);
   }
   mEntityPtr->SetPosition(altPos);
   BullseyeOperation(isBullseye);
}

void Annotation::PointOfInterestPropertiesWidget::CancelChanges()
{
   if (mEntityPtr)
   {
      if (mCreatingNewPoi)
      {
         vaEnv.DeleteEntity(mEntityPtr);
         mEntityPtr = nullptr;
      }
      else
      {
         mEntityPtr->SetName(mOriginalValues.mName);
         mEntityPtr->SetPosition(vespa::VaPosition(mOriginalValues.mLat, mOriginalValues.mLon, mOriginalValues.mAlt));
         PointOfInterestPropertiesWidget::BullseyeOperation(
            mOriginalValues.mBullseye); // don't use the overridden functions here
      }
   }
}

void Annotation::PointOfInterestPropertiesWidget::AcceptChanges()
{
   SetExportable();
   if (mOriginalValues.mBullseye != mUi.mBullseyeCheckbox->isChecked())
   {
      emit PoiBullseyeToggled(mEntityPtr->GetUniqueId(), mUi.mBullseyeCheckbox->isChecked());
   }
}

void Annotation::PointOfInterestPropertiesWidget::NameOperation()
{
   mEntityPtr->SetName(mUi.mName->text().toStdString());
}

void Annotation::PointOfInterestPropertiesWidget::PositionOperation(const vespa::VaPosition& aPosition)
{
   mEntityPtr->SetPosition(aPosition);
}

void Annotation::PointOfInterestPropertiesWidget::BullseyeOperation(bool aBullseye)
{
   auto* modelPtr = mEntityPtr->FindFirstAttachmentOfType<vespa::VaAttachmentModel>();
   if (modelPtr)
   {
      if (aBullseye)
      {
         modelPtr->SetIcon("bullseye");
      }
      else
      {
         modelPtr->SetIcon("poi");
         modelPtr->SetColor(UtColor(1.0f, 1.0f, 0.5f));
      }
   }
}
