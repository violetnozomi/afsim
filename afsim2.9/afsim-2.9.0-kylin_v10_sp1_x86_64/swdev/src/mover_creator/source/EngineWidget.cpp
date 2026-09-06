// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "EngineWidget.hpp"

#include "UtMemory.hpp"

Designer::EngineWidget::EngineWidget(const QString& aName, size_t aNumBaseSpecs, size_t aNumBaseSpecsOptions)
   : QWidget()
   , mName(aName)
   , mNumBaseSpecs(aNumBaseSpecs)
   , mNumBaseSpecsOptions(aNumBaseSpecsOptions)
   , mPositiveValidator(std::shared_ptr<QDoubleValidator>(new QDoubleValidator(0, std::numeric_limits<double>::max(), 10)))
{
}

void Designer::EngineWidget::LengthChanged(double aLength)
{
   mEngineSpecs.at(0) = aLength;
   EngineSpecsChanged();
}

void Designer::EngineWidget::DiameterChanged(double aDiameter)
{
   mEngineSpecs.at(1) = aDiameter;
   EngineSpecsChanged();
}

void Designer::EngineWidget::ThrustOffsetChanged(double aThrustOffset)
{
   mEngineSpecs.at(3) = aThrustOffset;
   EngineSpecsChanged();
}

void Designer::EngineWidget::LoadEngine(AeroEngine* aAeroEngine)
{
   mEngineSpecs = aAeroEngine->GetEngineSpecs();
   mEngineSpecsOptions = aAeroEngine->GetEngineSpecsOptions();

   PopulateLineEdits();
}

void Designer::EngineWidget::UpdateEngine(AeroEngine* aAeroEngine)
{
   aAeroEngine->CalcEngineData();
   mEngineSpecs = aAeroEngine->GetEngineSpecs();
   mEngineSpecsOptions = aAeroEngine->GetEngineSpecsOptions();

   PopulateLineEdits();
}

QStringList Designer::EngineWidget::Audit() const
{
   // Check to make sure all specifications are valid
   QStringList errorList;

   if (mName.isEmpty())
   {
      QString errorString("Engine requires a name");
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(0) < std::numeric_limits<double>::epsilon()) // length
   {
      QString errorString(" requires a positive length");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(1) < std::numeric_limits<double>::epsilon()) // diameter
   {
      QString errorString(" requires a positive diameter");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   // Mass (mEngineSpecs.at(2) checked in derived classes
   // No restrictions on thrust offset (mEngineSpecs.at(3))

   return errorList;
}
