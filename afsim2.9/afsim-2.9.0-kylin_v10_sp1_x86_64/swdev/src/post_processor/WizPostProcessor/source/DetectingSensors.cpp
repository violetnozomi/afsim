// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "DetectingSensors.hpp"
#include "ui_DetectingSensors.h"

#include <QMessageBox>
#include <QVector>

PostProcessor::DetectingSensors::DetectingSensors()
   : ReportWidget()
   , mUIPtr(new Ui::DetectingSensors)
{
   mUIPtr->setupUi(this);
   mUIPtr->sensorFilter->SetColumnHeaders(QStringList() << "Name" << "Type" << "Platform Name" << "Platform Type" << "Side");
}

PostProcessor::DetectingSensors::~DetectingSensors()
{
   delete mUIPtr;
}

void PostProcessor::DetectingSensors::WriteData(std::ostream& aStream)
{
   QStringList sensorNames = mUIPtr->sensorFilter->GetCheckedItems(0);
   QStringList platformNames = mUIPtr->sensorFilter->GetCheckedItems(2);

   if (!mUIPtr->sensorFilter->AllChecked())
   {
      aStream << "detecting_sensors\n";
      for (int i = 0; i < sensorNames.size(); ++i)
      {
         aStream << "   sensor " << sensorNames[i].toStdString()
                 << " on_platform " << platformNames[i].toStdString() << "\n";
         if (i != sensorNames.size() - 1)
         {
            aStream << "\n";
         }
      }
      aStream << "end_detecting_sensors\n";
   }
}

bool PostProcessor::DetectingSensors::CheckValidity()
{
   bool isValid = true;
   if (mUIPtr->sensorFilter->AllUnchecked())
   {
      QMessageBox::critical(this, "Error", "At least one sensor must be selected in Detecting Sensor.");
      isValid = false;
   }
   return isValid;
}

void PostProcessor::DetectingSensors::AddSensors(const QVector<ProxyInterface::PlatformPart>& aSensors)
{
   for (auto s : aSensors)
   {
      QColor color("white");
      if (QColor::isValidColor(s.mPlatform.mSide))
      {
         color = QColor(s.mPlatform.mSide);
         color.setAlpha(50);
      }
      mUIPtr->sensorFilter->AddItem(QStringList() << s.mName << s.mType << s.mPlatform.mName << s.mPlatform.mType << s.mPlatform.mSide, color);
   }
}

void PostProcessor::DetectingSensors::RemoveSensors(const QVector<ProxyInterface::PlatformPart>& aSensors)
{
   for (auto s : aSensors)
   {
      QVector<QPair<QString, int> > columns;
      columns.push_back(qMakePair(s.mName, 0));
      columns.push_back(qMakePair(s.mType, 1));
      columns.push_back(qMakePair(s.mPlatform.mName, 2));
      columns.push_back(qMakePair(s.mPlatform.mType, 3));
      columns.push_back(qMakePair(s.mPlatform.mSide, 4));
      mUIPtr->sensorFilter->RemoveItem(columns);
   }
}
