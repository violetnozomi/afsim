// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "AeroEngine.hpp"

Designer::AeroEngine::AeroEngine(const std::string& aModelName)
   : mModelName(aModelName)
{
}

std::vector<double> Designer::AeroEngine::GetEngineSpecs() const
{
   return std::vector<double>({ mLength_ft,
                                mDiameter_ft,
                                mMass_lbs,
                                mThrustOffset_ft
                              });
}

void Designer::AeroEngine::SetEngineSpecs(const std::vector<double>& aEngineSpecs)
{
   mLength_ft       = aEngineSpecs[0];
   mDiameter_ft     = aEngineSpecs[1];
   mMass_lbs        = aEngineSpecs[2];
   mThrustOffset_ft = aEngineSpecs[3];
}

void Designer::AeroEngine::SetData(const std::string&         aName,
                                   GeometryEngine::EngineType aEngineType,
                                   UtVec3dX                   aRefPoint,
                                   double                     aYaw_deg,
                                   double                     aPitch_deg,
                                   double                     aRoll_deg,
                                   double                     aLength_ft,
                                   double                     aDiameter_ft,
                                   double                     aMass_lbs,
                                   double                     aThrustOffset_ft,
                                   double                     aIgnitionDelay_sec)
{
   mName              = aName;
   mEngineType        = aEngineType;
   mRefPoint_ft       = aRefPoint;
   mYaw_deg           = aYaw_deg;
   mPitch_deg         = aPitch_deg;
   mRoll_deg          = aRoll_deg;
   mLength_ft         = aLength_ft;
   mDiameter_ft       = aDiameter_ft;
   mMass_lbs          = aMass_lbs;
   mThrustOffset_ft   = aThrustOffset_ft;
   mIgnitionDelay_sec = aIgnitionDelay_sec;

   LoadFromFile();
}

void Designer::AeroEngine::OutputData(QTextStream& aOutputStream, double aData, int aWidth, int aPrecision, QTextStream::FieldAlignment aAlignment) const
{
   int currentPrecision = aOutputStream.realNumberPrecision();
   int currentWidth = aOutputStream.fieldWidth();
   QTextStream::FieldAlignment currentAlignment = aOutputStream.fieldAlignment();

   aOutputStream.setFieldWidth(aWidth);
   aOutputStream.setRealNumberPrecision(aPrecision);
   aOutputStream.setFieldAlignment(aAlignment);

   aOutputStream << aData;

   aOutputStream.setFieldWidth(currentWidth);
   aOutputStream.setRealNumberPrecision(currentPrecision);
   aOutputStream.setFieldAlignment(currentAlignment);
}

void Designer::AeroEngine::OutputData(QTextStream& aOutputStream, QString aData, int aWidth, int aPrependedSpaces, QTextStream::FieldAlignment aAlignment) const
{
   int currentWidth = aOutputStream.fieldWidth();
   QTextStream::FieldAlignment currentAlignment = aOutputStream.fieldAlignment();

   for (int i = 0; i < aPrependedSpaces; ++i)
   {
      aOutputStream << " ";
   }

   aOutputStream.setFieldWidth(aWidth);
   aOutputStream.setFieldAlignment(aAlignment);

   aOutputStream << aData;

   aOutputStream.setFieldWidth(currentWidth);
   aOutputStream.setFieldAlignment(currentAlignment);
}

void Designer::AeroEngine::CalcEngineData()
{
   for (const auto& n : mRawData)
   {
      auto currentTable = mEngineTables.at(n.first);

      // Clear the base data and recompute it.
      // The data in mRawData will not have changed, but
      // the data in mEngineDataMultiplier could have changed.
      // Then, recalculate the modified data based on the new base data.
      currentTable->SetDirty();
      currentTable->ClearBaseData();
      double maxVal = mMaxIndepVar.at(n.first);
      for (const auto& a : n.second)
      {
         if (maxVal == 0 || a.first < maxVal)
         {
            currentTable->AddData(a.first, mEngineDataOffset.at(n.first) + a.second * mEngineDataMultiplier.at(n.first));
         }
         else
         {
            currentTable->AddData(maxVal, 0);
            break;
         }
      }
      // Recompute the modified data based on the new base data
      currentTable->UpdateModifiedData();
   }
}

void Designer::AeroEngine::GenerateEngineData()
{
   // Create new aero tables for the data in mRawData.
   // Anything already existing in mEngineTables will be blown away.

   mEngineTables.clear();
   for (const auto& n : mRawData)
   {
      AeroTable2d* newTable = new AeroTable2d(mPlotNameMap[n.first].yLabel, mPlotNameMap[n.first].xLabel);
      double maxVal = mMaxIndepVar.at(n.first);
      for (const auto& a : n.second)
      {
         if (maxVal == 0 || a.first < maxVal)
         {
            newTable->AddData(a.first, mEngineDataOffset.at(n.first) + a.second * mEngineDataMultiplier.at(n.first));
         }
         else
         {
            newTable->AddData(maxVal, 0);
            break;
         }
      }

      switch (GetModifierType(n.first))
      {
         case Designer::AeroEngine::ModifierType::cMULTIPLIER_LINE:
            newTable->SetListBasedGainModifier(GetModifierMultiplierLine(n.first));
            break;
         case Designer::AeroEngine::ModifierType::cMULTIPLIER_VALUE:
            newTable->SetSingleGainModifier(GetModifierMultiplier(n.first));
            break;
         case Designer::AeroEngine::ModifierType::cOFFSET_LINE:
            newTable->SetListBasedOffsetModifier(GetModifierOffsetLine(n.first));
            break;
         case Designer::AeroEngine::ModifierType::cOFFSET_VALUE:
            newTable->SetSingleOffsetModifier(GetModifierOffset(n.first));
            break;
         default:
            break;
      }

      newTable->UpdateModifiedData();
      mEngineTables.emplace(n.first, newTable);
   }
}

Designer::AeroEngine::ModifierType Designer::AeroEngine::GetModifierType(DataKey aKey) const
{
   auto modifierType = mModifierTypes.find(aKey);
   if (modifierType != mModifierTypes.end())
   {
      return modifierType->second;
   }

   return ModifierType::cNONE;
}

std::vector<std::pair<double, double>> Designer::AeroEngine::GetModifierMultiplierLine(DataKey aKey) const
{
   auto modifier = mModifierMultiplierLines.find(aKey);
   if (modifier != mModifierMultiplierLines.end())
   {
      return modifier->second;
   }
   return std::vector<std::pair<double, double>>();
}

std::vector<std::pair<double, double>> Designer::AeroEngine::GetModifierOffsetLine(DataKey aKey) const
{
   auto modifier = mModifierOffsetLines.find(aKey);
   if (modifier != mModifierOffsetLines.end())
   {
      return modifier->second;
   }
   return std::vector<std::pair<double, double>>();
}

double Designer::AeroEngine::GetModifierMultiplier(DataKey aKey) const
{
   auto modifier = mModifierMultipliers.find(aKey);
   if (modifier != mModifierMultipliers.end())
   {
      return modifier->second;
   }
   return 1.0;
}

double Designer::AeroEngine::GetModifierOffset(DataKey aKey) const
{
   auto modifier = mModifierOffsets.find(aKey);
   if (modifier != mModifierOffsets.end())
   {
      return modifier->second;
   }
   return 0.0;
}
