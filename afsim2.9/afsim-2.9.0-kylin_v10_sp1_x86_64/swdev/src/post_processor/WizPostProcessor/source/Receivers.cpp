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

#include "Receivers.hpp"
#include "ui_Receivers.h"

#include <QMessageBox>

PostProcessor::Receivers::Receivers()
   : ReportWidget()
   , mUIPtr(new Ui::Receivers)
{
   mUIPtr->setupUi(this);
   mUIPtr->receiverFilter->SetColumnHeaders(QStringList() << "Name" << "Type" << "Platform Name" << "Platform Type" << "Side");
}

PostProcessor::Receivers::~Receivers()
{
   delete mUIPtr;
}

void PostProcessor::Receivers::WriteData(std::ostream& aStream)
{
   QStringList receivers = mUIPtr->receiverFilter->GetCheckedItems(0);

   if (!mUIPtr->receiverFilter->AllChecked())
   {
      aStream << "receivers\n";

      for (const auto& r : receivers)
      {
         aStream << "\t" << r.toStdString() << "\n";
      }
      aStream << "end_receivers\n";
   }
}

bool PostProcessor::Receivers::CheckValidity()
{
   bool isValid = true;
   if (mUIPtr->receiverFilter->AllUnchecked())
   {
      QMessageBox::critical(this, "Error", "At least one transmitter must be selected.");
      isValid = false;
   }
   return isValid;
}

void PostProcessor::Receivers::AddReceivers(const QVector<ProxyInterface::PlatformPart>& aReceivers)
{
   // TODO: uncomment once post processor is equiped to handle this filter
   //for (auto r : aReceivers)
   //{
   //   QColor color("white");
   //   if (QColor::isValidColor(r.mPlatform.mSide))
   //   {
   //      color = QColor(r.mPlatform.mSide);
   //      color.setAlpha(50);
   //   }
   //   mUIPtr->receiverFilter->AddItem(QStringList() << r.mName << r.mType << r.mPlatform.mName << r.mPlatform.mType << r.mPlatform.mSide, color);
   //}
}

void PostProcessor::Receivers::RemoveReceivers(const QVector<ProxyInterface::PlatformPart>& aReceivers)
{
   // TODO: uncomment once post processor is equipped to handle this filter
   //for (auto r : aReceivers)
   //{
   //   QVector<QPair<QString, int> > columns;
   //   columns.push_back(qMakePair(r.mName, 0));
   //   columns.push_back(qMakePair(r.mType, 1));
   //   columns.push_back(qMakePair(r.mPlatform.mName, 2));
   //   columns.push_back(qMakePair(r.mPlatform.mType, 3));
   //   columns.push_back(qMakePair(r.mPlatform.mSide, 4));
   //   mUIPtr->receiverFilter->RemoveItem(columns);
   //}
}

void PostProcessor::Receivers::RestoreDefaults()
{
   // TODO
}