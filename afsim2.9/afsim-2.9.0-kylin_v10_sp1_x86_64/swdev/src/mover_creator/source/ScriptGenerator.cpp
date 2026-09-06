// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "ScriptGenerator.hpp"

#include <QDate>

#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorWidget.hpp"

namespace Designer
{

ScriptGenerator::ScriptGenerator()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   mVehiclePtr = mw.GetMoverCreatorWidget()->GetVehicle();

   if (mVehiclePtr == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   // Set the vehicle name which will be used throughout functions in this class...
   mVehicleName = QString::fromStdString(mVehiclePtr->GetVehicleName());

   // Set the vehicle type
   if (mVehiclePtr->IsAircraft())
   {
      mVehicleType = "aircraft";
   }
   else if (mVehiclePtr->IsWeapon())
   {
      mVehicleType = "weapons";
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle needs to be either an aircraft or a weapon!");
      return;
   }

   // Determine what set of tests the vehicle needs
   if (mVehiclePtr->GetVehicleControlConfiguration() == Vehicle::cNO_CONTROL)
   {
      if (mEngineList.empty())
      {
         mPerformanceTestType = PerformanceTestType::cNO_CONTROL_NO_ENGINE;
      }
      else
      {
         mPerformanceTestType = PerformanceTestType::cNO_CONTROL_ENGINE;
      }
   }
   else
   {
      if (mVehiclePtr->UsesBankToTurnControl())
      {
         mPerformanceTestType = PerformanceTestType::cBANK_TO_TURN;
      }
      else
      {
         mPerformanceTestType = PerformanceTestType::cSKID_TO_TURN;
      }
   }

   VehiclePropulsion* propulsion = mVehiclePtr->GetPropulsion();
   if (propulsion != nullptr)
   {
      const std::map<size_t, AeroEngine*>& engineMap = propulsion->GetEngineMap();
      for (auto& engineItem : engineMap)
      {
         AeroEngine* engine = engineItem.second;
         if (engine != nullptr)
         {
            mEngineList.emplace_back(engine);
         }
      }
   }

   // Setup the working directory
   mVehicleDir.setPath(MoverCreatorMainWindow::GetDataPath() + "/AFSIM_Scripts");
   mVehicleDir.mkdir(mVehicleName);  // This will make dir only if it doesn't exist
   mVehicleDir.cd(mVehicleName);
}

bool ScriptGenerator::GenerateDataFiles()
{
   if (mVehiclePtr == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return false;
   }

   CreateFolderStructure();
   CreatePlatformFile();
   CreateAdditionalFiles();
   CreateTestFiles();
   return true;
}

void ScriptGenerator::CreateFolderIfDoesntExist(const QString& aPath)
{
   if (!QDir(aPath).exists())
   {
      QDir().mkdir(aPath);
   }
}

void ScriptGenerator::CreateAutogenHeader(Vehicle* aVehiclePtr, QTextStream& aOutStream)
{
   QDate Today = QDate::currentDate();
   aOutStream << "# ##################################################################################################\n";
   aOutStream << "# \n";
   VersionInfo::OutputVersionStringInOutputStream(aOutStream);
   aOutStream << "# \n";
   aOutStream << "# Creation Date:  " << Today.toString(Qt::RFC2822Date) << "\n";
   aOutStream << "# \n";
   aOutStream << "# ##################################################################################################\n";
   aOutStream << "\n";
}

void ScriptGenerator::OutputData(QTextStream& aOutputStream, double aData, int aWidth, int aPrecision, QTextStream::FieldAlignment aAlignment)
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

void ScriptGenerator::OutputData(QTextStream& aOutputStream, const QString& aString, int aWidth, QTextStream::FieldAlignment aAlignment)
{
   int currentWidth = aOutputStream.fieldWidth();
   QTextStream::FieldAlignment currentAlignment = aOutputStream.fieldAlignment();

   aOutputStream.setFieldWidth(aWidth);
   aOutputStream.setFieldAlignment(aAlignment);

   aOutputStream << aString;

   aOutputStream.setFieldWidth(currentWidth);
   aOutputStream.setFieldAlignment(currentAlignment);
}

} // namespace Designer
