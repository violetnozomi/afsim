// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "PositionConverterDialog.hpp"

#include <sstream>

#include <QMessageBox>
#include <QPushButton>

#include "UtEllipsoidalEarth.hpp"
#include "WkfEnvironment.hpp"
#include "WkfUnitsObject.hpp"

PositionConverter::Dialog::Dialog(QWidget* aParent, Qt::WindowFlags aFlags)
   : QDialog(aParent, aFlags)
{
   mUi.setupUi(this);

   // Displays the specified conversion page
   connect(mUi.conversionComboBox,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           [=](int aIndex) { mUi.PositionStackedWidget->setCurrentIndex(aIndex); });

   connect(mUi.copyPushButton,
           &QPushButton::pressed,
           [=]()
           {
              if (mUi.LLToMGRSPage->isVisible())
              {
                 mUi.resultMGRSLineEdit->selectAll();
                 mUi.resultMGRSLineEdit->copy();
                 mUi.resultMGRSLineEdit->deselect();
              }
              else
              {
                 mUi.resultLLLineEdit->selectAll();
                 mUi.resultLLLineEdit->copy();
                 mUi.resultLLLineEdit->deselect();
              }
           });

   // Converts the input to the respective output
   connect(mUi.latitudeLineEdit, &QLineEdit::textChanged, this, &Dialog::Convert);
   connect(mUi.longitudeLineEdit, &QLineEdit::textChanged, this, &Dialog::Convert);
   connect(mUi.MGRSLineEdit, &QLineEdit::textChanged, this, &Dialog::Convert);
   Convert();
   // Sets LL to MGRS as inital page
   mUi.PositionStackedWidget->setCurrentIndex(0);
}

void PositionConverter::Dialog::Convert()
{
   if (mUi.LLToMGRSPage->isVisible()) // convert LL to MGRS
   {
      if (mUi.latitudeLineEdit->IsValid() && mUi.longitudeLineEdit->IsValid())
      {
         auto lat = mUi.latitudeLineEdit->GetValue();
         auto lon = mUi.longitudeLineEdit->GetValue();

         std::string result;
         UtEllipsoidalEarth::ConvertLLToMGRS(lat, lon, result);

         mUi.resultMGRSLineEdit->clear();
         mUi.resultMGRSLineEdit->setText(QString::fromStdString(result));
      }
      else
      {
         mUi.resultMGRSLineEdit->clear();
         if (!mUi.latitudeLineEdit->IsValid() && !mUi.longitudeLineEdit->IsValid())
         {
            mUi.resultMGRSLineEdit->setPlaceholderText("Invalid Inputs");
         }
         else if (!mUi.latitudeLineEdit->IsValid())
         {
            mUi.resultMGRSLineEdit->setPlaceholderText("Invalid Latitude");
         }
         else if (!mUi.longitudeLineEdit->IsValid())
         {
            mUi.resultMGRSLineEdit->setPlaceholderText("Invalid Longitude");
         }
      }
   }
   else if (mUi.MGRSToLLPage->isVisible()) // Convert MGRS to LL
   {
      QString mgrsStr = mUi.MGRSLineEdit->text();
      double  lat;
      double  lon;
      bool    success = UtEllipsoidalEarth::ConvertMGRSToLL(mgrsStr.toStdString(), lat, lon);
      if (success)
      {
         std::ostringstream oss;
         oss << UtLatPos(lat, (wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLatLonFormatting()));
         oss << " ";
         oss << UtLonPos(lon, (wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLatLonFormatting()));

         mUi.resultLLLineEdit->clear();
         mUi.resultLLLineEdit->setText(QString::fromStdString(oss.str()));
      }
      else
      {
         mUi.resultLLLineEdit->clear();
         mUi.resultLLLineEdit->setPlaceholderText("Invalid MGRS Value");
      }
   }
}
