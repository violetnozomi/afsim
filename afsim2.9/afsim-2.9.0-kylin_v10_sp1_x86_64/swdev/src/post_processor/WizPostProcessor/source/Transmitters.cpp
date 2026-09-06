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

#include "Transmitters.hpp"
#include "ui_Transmitters.h"

#include <QMessageBox>

PostProcessor::Transmitters::Transmitters()
   : ReportWidget()
   , mUIPtr(new Ui::Transmitters)
{
   mUIPtr->setupUi(this);
   mUIPtr->transmitterFilter->SetColumnHeaders(QStringList() << "Name" << "Type" << "Platform Name" << "Platform Type" << "Side");
}

PostProcessor::Transmitters::~Transmitters()
{
   delete mUIPtr;
}

void PostProcessor::Transmitters::WriteData(std::ostream& aStream)
{
   QStringList transmitters = mUIPtr->transmitterFilter->GetCheckedItems(0);

   if (!mUIPtr->transmitterFilter->AllChecked())
   {
      aStream << "transmitters\n";

      for (const auto& t : transmitters)
      {
         aStream << "\t" << t.toStdString() << "\n";
      }
      aStream << "end_transmitters\n";
   }
}

bool PostProcessor::Transmitters::CheckValidity()
{
   bool isValid = true;
   if (mUIPtr->transmitterFilter->AllUnchecked())
   {
      QMessageBox::critical(this, "Error", "At least one transmitter must be selected.");
      isValid = false;
   }
   return isValid;
}

void PostProcessor::Transmitters::AddTransmitters(const QVector<ProxyInterface::PlatformPart>& aTransmitters)
{
   // TODO: uncomment once post processor is equiped to handle this filter
   //for (auto t : aTransmitters)
   //{
   //   QColor color("white");
   //   if (QColor::isValidColor(t.mPlatform.mSide))
   //   {
   //      color = QColor(t.mPlatform.mSide);
   //      color.setAlpha(50);
   //   }
   //   mUIPtr->transmitterFilter->AddItem(QStringList() << t.mName << t.mType << t.mPlatform.mName << t.mPlatform.mType << t.mPlatform.mSide, color);
   //}
}

void PostProcessor::Transmitters::RemoveTransmitters(const QVector<ProxyInterface::PlatformPart>& aTransmitters)
{
   // TODO: uncomment once post processor is equiped to handle this filter
   //for (auto t : aTransmitters)
   //{
   //   QVector<QPair<QString, int> > columns;
   //   columns.push_back(qMakePair(t.mName, 0));
   //   columns.push_back(qMakePair(t.mType, 1));
   //   columns.push_back(qMakePair(t.mPlatform.mName, 2));
   //   columns.push_back(qMakePair(t.mPlatform.mType, 3));
   //   columns.push_back(qMakePair(t.mPlatform.mSide, 4));
   //   mUIPtr->transmitterFilter->RemoveItem(columns);
   //}
}

void PostProcessor::Transmitters::RestoreDefaults()
{
   // TODO
}