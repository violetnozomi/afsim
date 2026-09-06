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

#include "DetectedPlatforms.hpp"
#include "ui_DetectedPlatforms.h"

#include <QMessageBox>
#include <QVector>

PostProcessor::DetectedPlatforms::DetectedPlatforms()
   : ReportWidget()
{
   mUI.setupUi(this);
   mUI.platformFilter->SetColumnHeaders(QStringList() << "Name" << "Type" << "Side");
}

void PostProcessor::DetectedPlatforms::WriteData(std::ostream& aStream)
{
   QStringList platforms = mUI.platformFilter->GetCheckedItems(0);

   if (!mUI.platformFilter->AllChecked())
   {
      aStream << "detected_platforms\n";

      for (const auto& p : platforms)
      {
         aStream << "   " << p.toStdString() << "\n";
      }
      aStream << "end_detected_platforms\n";
   }
}

bool PostProcessor::DetectedPlatforms::CheckValidity()
{
   bool isValid = true;
   if (mUI.platformFilter->AllUnchecked())
   {
      QMessageBox::critical(this, "Error", "At least one platform must be selected in Detected Platforms.");
      isValid = false;
   }
   return isValid;
}

void PostProcessor::DetectedPlatforms::AddPlatforms(const QMap<ProxyInterface::Platform, bool>& aPlatforms)
{
   for(auto it = aPlatforms.begin(); it != aPlatforms.end(); ++it) {
      QColor color("white");
      if (QColor::isValidColor(it.key().mSide))
      {
          color = QColor(it.key().mSide);
          color.setAlpha(50);
      }
      mUI.platformFilter->AddItem(QStringList() << it.key().mName << it.key().mType << it.key().mSide, color, it.value());
   }
}

void PostProcessor::DetectedPlatforms::RemovePlatforms(const QVector<ProxyInterface::Platform>& aPlatforms)
{
   for (const auto& p : aPlatforms)
   {
      QVector<QPair<QString, int> > columns;
      columns.push_back(qMakePair(p.mName, 0));
      columns.push_back(qMakePair(p.mType, 1));
      columns.push_back(qMakePair(p.mSide, 2));
      mUI.platformFilter->RemoveItem(columns);
   }
}
