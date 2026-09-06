// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfBullseyeWidget.hpp"

#include "VaPosition.hpp"
#include "WkfAnnotationDisplayInterface.hpp"
#include "WkfBullseye.hpp"
#include "WkfEnvironment.hpp"
#include "WkfScenario.hpp"
#include "WkfUnitsObject.hpp"
#include "WkfVtkEnvironment.hpp"

Annotation::BullseyeWidget::BullseyeWidget(AnnotationDisplayInterface& aDisplayInterface,
                                           QWidget*                    parent /*= nullptr*/,
                                           Qt::WindowFlags             f /*= Qt::WindowFlags()*/)
   : QWidget(parent, f | Qt::Tool | Qt::MSWindowsFixedSizeDialogHint)
   , mDisplayInterface(aDisplayInterface)
{
   mUI.setupUi(this);
   mUI.tabWidget->setCurrentIndex(0);

   connect(mUI.buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &BullseyeWidget::Apply);
   connect(mUI.buttonBox, &QDialogButtonBox::rejected, this, &BullseyeWidget::hide);

   connect(mUI.createLocationSelector,
           &wkf::LocationSelector::Selected,
           this,
           [this](double aLat, double aLon)
           {
              mUI.createLatitudeLineEdit->SetValue(aLat);
              mUI.createLongitudeLineEdit->SetValue(aLon);
           });
   connect(mUI.moveLocationSelector,
           &wkf::LocationSelector::Selected,
           this,
           [this](double aLat, double aLon)
           {
              mUI.moveLatitudeLineEdit->SetValue(aLat);
              mUI.moveLongitudeLineEdit->SetValue(aLon);
           });
}

void Annotation::BullseyeWidget::Show()
{
   mUI.bullseyeNameComboBox->clear();
   mUI.bullseyeNameLineEdit->setText("Bullseye");

   auto* scenarioPtr = vaEnv.GetStandardScenario();
   if (scenarioPtr)
   {
      vespa::VaPosition p = vaEnv.GetCurrentCursorPosition();
      {
         UtLatPos           latitude(p.GetLat(), wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLatLonFormatting());
         std::ostringstream latStream;
         latStream << latitude;
         QString latString = QString::fromStdString(latStream.str());
         mUI.createLatitudeLineEdit->setText(latString);
         mUI.moveLatitudeLineEdit->setText(latString);
      }
      {
         UtLonPos longitude(p.GetLon(), wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLatLonFormatting());
         std::ostringstream lonStream;
         lonStream << longitude;
         QString lonString = QString::fromStdString(lonStream.str());
         mUI.createLongitudeLineEdit->setText(lonString);
         mUI.moveLongitudeLineEdit->setText(lonString);
      }

      const auto& bullseyes = scenarioPtr->GetBullseyes();
      for (const auto& bullseye : bullseyes)
      {
         mUI.bullseyeNameComboBox->addItem(QString::fromStdString(bullseye->GetName()));
      }

      // Enable the "Move" tab only if the list of bullseye is non-empty.
      mUI.tabWidget->setTabEnabled(1, !bullseyes.empty());
      // If there are existing bullseyes, default to displaying the "Move" tab.
      (!bullseyes.empty() ? mUI.tabWidget->setCurrentIndex(1) : mUI.tabWidget->setCurrentIndex(0));

      show();
   }
}

void Annotation::BullseyeWidget::Apply()
{
   bool closeDialog = true;

   if (mUI.tabWidget->currentIndex() == 0) // First page is the Create Tab
   {
      std::string bullseyeName = mUI.bullseyeNameLineEdit->text().toStdString();

      if (!bullseyeName.empty())
      {
         bool nameTaken = false;

         auto* scenarioPtr = vaEnv.GetStandardScenario();
         if (scenarioPtr)
         {
            const auto& bullseyes = scenarioPtr->GetBullseyes();
            for (const auto& bullseye : bullseyes)
            {
               if (bullseye->GetName() == bullseyeName)
               {
                  nameTaken = true;
                  break;
               }
            }
         }

         if (nameTaken)
         {
            QMessageBox::warning(this,
                                 "Name Exists",
                                 "A bullseye with the name " + QString::fromStdString(bullseyeName) +
                                    " already exist, unable to create new bullseye.");
            closeDialog = false;
         }
         else
         {
            mDisplayInterface.AddPOI<wkf::Bullseye>("bullseye",
                                                    mUI.createLatitudeLineEdit->GetValue(),
                                                    mUI.createLongitudeLineEdit->GetValue(),
                                                    mUI.bullseyeNameLineEdit->text().toStdString());
         }
      }
   }
   else // Move tab
   {
      std::string bullseyeName = mUI.bullseyeNameComboBox->currentText().toStdString();

      auto* scenarioPtr = vaEnv.GetStandardScenario();
      if (scenarioPtr)
      {
         auto& bullseyes = scenarioPtr->GetBullseyes();
         for (auto& bullseye : bullseyes)
         {
            if (bullseye->GetName() == bullseyeName)
            {
               bullseye->SetPosition(
                  vespa::VaPosition(mUI.moveLatitudeLineEdit->GetValue(), mUI.moveLongitudeLineEdit->GetValue(), 1));
               break;
            }
         }
      }
   }

   if (closeDialog)
   {
      hide();
   }
}
