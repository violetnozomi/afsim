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

#include "EclipsePlatforms.hpp"
#include "ui_EclipsePlatforms.h"

#include <QMessageBox>
#include <QVector>

//Constructor for Eclipse platform filter widget
PostProcessor::EclipsePlatforms::EclipsePlatforms()
   : ReportWidget()
   , mUIPtr(new Ui::EclipsePlatforms)
{
   mUIPtr->setupUi(this);
   mUIPtr->platformFilter->SetColumnHeaders(QStringList() << "Name" << "Type" << "Side");
}

PostProcessor::EclipsePlatforms::~EclipsePlatforms()
{
   delete mUIPtr;
}

//Writes data to configuration file. Outputs desired platforms for the report.
void PostProcessor::EclipsePlatforms::WriteData(std::ostream& aStream)
{
   QStringList platforms = mUIPtr->platformFilter->GetCheckedItems(0);

   if (!mUIPtr->platformFilter->AllChecked())
   {
      aStream << "platforms\n";

      for (const auto& p : platforms)
      {
         aStream << "   " << p.toStdString() << "\n";
      }
      aStream << "end_platforms\n";
   }
}

//Valid if at least one platform is selected.
bool PostProcessor::EclipsePlatforms::CheckValidity()
{
   bool isValid = true;
   if (mUIPtr->platformFilter->AllUnchecked())
   {
      QMessageBox::critical(this, "Error", "At least one platform must be selected in Detected Platforms.");
      isValid = false;
   }
   return isValid;
}

//Only adds a platform to filter if it has a WSF_SPACE_MOVER. It does not make sense for platforms not in space
//to have eclipse reports.
void PostProcessor::EclipsePlatforms::AddPlatforms(const QMap<ProxyInterface::Platform, bool>& aPlatforms)
{
   for (auto p : aPlatforms.keys())
   {
      if (p.mMover == "WSF_SPACE_MOVER")
      {
         QColor color("white");
         if (QColor::isValidColor(p.mSide))
         {
            color = QColor(p.mSide);
            color.setAlpha(50);
         }
         mUIPtr->platformFilter->AddItem(QStringList() << p.mName << p.mType << p.mSide, color, aPlatforms.value(p));
      }
   }
}

//Remove platforms from the filter.
void PostProcessor::EclipsePlatforms::RemovePlatforms(const QVector<ProxyInterface::Platform>& aPlatforms)
{
   for (auto p : aPlatforms)
   {
      QVector<QPair<QString, int> > columns;
      columns.push_back(qMakePair(p.mName, 0));
      columns.push_back(qMakePair(p.mType, 1));
      columns.push_back(qMakePair(p.mSide, 2));
      mUIPtr->platformFilter->RemoveItem(columns);
   }
}
