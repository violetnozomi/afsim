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

#include "OptionsWE.hpp"
#include "ui_OptionsWE.h"

#include <QMessageBox>

PostProcessor::OptionsWE::OptionsWE()
   : OptionWidget("weapon_expenditures")
   , mUIPtr(new Ui::OptionsWE)
{
   mUIPtr->setupUi(this);
   connect(mUIPtr->toolButton, &QToolButton::pressed, this, [this]() {emit ReportTypePressed(); });

   connect(this, &OptionWidget::WeaponAdded, this, &OptionsWE::AddWeapons);
   connect(this, &OptionWidget::WeaponRemoved, this, &OptionsWE::RemoveWeapons);

   mUIPtr->weaponFilter->SetColumnHeaders(QStringList() << "Name" << "Type" << "Platform Name" << "Platform Type" << "Side");
}

PostProcessor::OptionsWE::~OptionsWE()
{
   delete mUIPtr;
}

void PostProcessor::OptionsWE::WriteData(std::ostream& aStream)
{
   // TODO
}

bool PostProcessor::OptionsWE::CheckValidity()
{
   bool isValid = true;
   if (mUIPtr->weaponFilter->AllUnchecked())
   {
      QMessageBox::critical(this, "Error", "At least one weapon must be selected for the Weapon Expeditures Report.");
      isValid = false;
   }
   return isValid;
}

void PostProcessor::OptionsWE::AddWeapons(const QVector<ProxyInterface::PlatformPart>& aWeapons)
{
   // TODO: uncomment once post processor is equiped to handle this filter
   //for (auto w : aWeapons)
   //{
   //   QColor color("white");
   //   if (QColor::isValidColor(w.mPlatform.mSide))
   //   {
   //      color = QColor(w.mPlatform.mSide);
   //      color.setAlpha(50);
   //   }
   //   mUIPtr->weaponFilter->AddItem(QStringList() << w.mName << w.mType << w.mPlatform.mName << w.mPlatform.mType << w.mPlatform.mSide, color);
   //}
}

void PostProcessor::OptionsWE::RemoveWeapons(const QVector<ProxyInterface::PlatformPart>& aWeapons)
{
   // TODO: uncomment once post processor is equiped to handle this filter
   //for (auto w : aWeapons)
   //{
   //   QVector<QPair<QString, int> > columns;
   //   columns.push_back(qMakePair(w.mName, 0));
   //   columns.push_back(qMakePair(w.mType, 1));
   //   columns.push_back(qMakePair(w.mPlatform.mName, 2));
   //   columns.push_back(qMakePair(w.mPlatform.mType, 3));
   //   columns.push_back(qMakePair(w.mPlatform.mSide, 4));
   //   mUIPtr->weaponFilter->RemoveItem(columns);
   //}
}
