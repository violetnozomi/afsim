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

#include "Participants.hpp"
#include "ui_Participants.h"

#include <QMessageBox>

PostProcessor::Participants::Participants(ParticipantType aParticipantType)
   : ReportWidget()
   , mUIPtr(new Ui::Participants)
   , mParticipantType(aParticipantType)
{
   mUIPtr->setupUi(this);
   mUIPtr->platformFilter->SetColumnHeaders(QStringList() << "Name" << "Type" << "Side");
   if (mParticipantType == ParticipantType::cACTING)
   {
      mUIPtr->groupBox->setTitle("Acting Platforms");
   }
   else if (mParticipantType == ParticipantType::cAGAINST)
   {
      mUIPtr->groupBox->setTitle("Against Platforms");
   }
}

PostProcessor::Participants::~Participants()
{
   delete mUIPtr;
}

void PostProcessor::Participants::WriteData(std::ostream& aStream)
{
   QStringList platforms = mUIPtr->platformFilter->GetCheckedItems(0);

   std::string tag = "";
   if (mParticipantType == ParticipantType::cACTING)
   {
      tag = "acting_platforms";
   }
   else if (mParticipantType == ParticipantType::cAGAINST)
   {
      tag = "against_platforms";
   }

   if (!mUIPtr->platformFilter->AllChecked())
   {
      aStream << tag << "\n";

      for (const auto& p : platforms)
      {
         aStream << "   " << p.toStdString() << "\n";
      }
      aStream << "end_" << tag << "\n";
   }
}

bool PostProcessor::Participants::CheckValidity()
{
   bool isValid = true;
   if (mUIPtr->platformFilter->AllUnchecked())
   {
      QMessageBox::critical(this, "Error", "At least one platform must be selected in Participants.");
      isValid = false;
   }
   return isValid;
}

void PostProcessor::Participants::AddPlatforms(const QMap<ProxyInterface::Platform,
                                               bool>& aPlatforms)
{
   for (auto p : aPlatforms.keys())
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

void PostProcessor::Participants::RemovePlatforms(const QVector<ProxyInterface::Platform>& aPlatforms)
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
