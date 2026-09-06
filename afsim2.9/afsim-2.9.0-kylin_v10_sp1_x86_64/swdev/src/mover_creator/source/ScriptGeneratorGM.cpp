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

#include "ScriptGeneratorGM.hpp"

#include <sstream>

#include "AeroEngine.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "Vehicle.hpp"

namespace Designer
{

ScriptGeneratorGM::ScriptGeneratorGM()
   : ScriptGenerator()
{
   mPlatformName = mVehicleName.toLower() + "_gm";
   mPlatformType = mVehicleName.toUpper() + "_GM";

   mTestDir.setPath(mVehicleDir.absolutePath() + "/tests/gm/");
   mOutputDir.setPath(mVehicleDir.absolutePath() + "/output/gm/");
}

void ScriptGeneratorGM::CreateFolderStructure()
{
   QString fullPath = mVehicleDir.absolutePath();

   // Create main folder
   if (!QDir(fullPath).exists())
   {
      QDir().mkdir(fullPath);
   }

   CreateFolderIfDoesntExist(fullPath + "/platforms");
   CreateFolderIfDoesntExist(fullPath + "/output");
   CreateFolderIfDoesntExist(fullPath + "/model_3d");
   CreateFolderIfDoesntExist(fullPath + "/tests");
   CreateFolderIfDoesntExist(mTestDir.absolutePath());
   CreateFolderIfDoesntExist(fullPath + "/output");
   CreateFolderIfDoesntExist(mOutputDir.absolutePath());

   if (mVehiclePtr == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }
}

void ScriptGeneratorGM::CreatePlatformFile()
{
   if (mVehiclePtr == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   QString platformDir = mVehicleDir.absolutePath() + "/platforms";
   QFile platformFile(platformDir + "/" + mPlatformName + ".txt");
   if (!platformFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   QTextStream outStream(&platformFile);

   outStream.setRealNumberNotation(QTextStream::FixedNotation);

   CreateAutogenHeader(mVehiclePtr, outStream);
   double maxMach = 0;
   OutputAero(outStream, maxMach);
   outStream << "\n";
   OutputMover(outStream, maxMach);
   outStream << "\n";
   outStream << "platform_type  " << mPlatformType << "  WSF_PLATFORM\n";
   outStream << "\n";

   std::string domain = "air";
   outStream << "   spatial_domain  ";
   OutputData(outStream, QString::fromStdString(domain), 32, QTextStream::FieldAlignment::AlignLeft);
   outStream << "   # This vehicle operates in the air domain\n";

   std::string side = "green";
   std::string icon = mVehiclePtr->GetVehicleName();

   outStream << "   side            ";
   OutputData(outStream, QString::fromStdString(side), 32, QTextStream::FieldAlignment::AlignLeft);
   outStream << "   # Neutral force color\n";
   outStream << "   icon            ";
   OutputData(outStream, QString::fromStdString(icon), 32, QTextStream::FieldAlignment::AlignLeft);
   outStream << "   # Icon based on geometry\n";
   outStream << "\n";
   outStream << "mover " << mPlatformType << "\n";
   outStream << "end_mover\n";
   outStream << "end_platform_type\n\n";

   platformFile.close();
}

void ScriptGeneratorGM::CreateTestFiles()
{

   switch (GetPerformanceTestType())
   {
      case ScriptGenerator::PerformanceTestType::cNO_CONTROL_NO_ENGINE:
      {
         // Do nothing special, for now.
         break;
      }
      case ScriptGenerator::PerformanceTestType::cNO_CONTROL_ENGINE:
      {
         CreateBallisticRangeTestFile(mTestDir.absolutePath());
         break;
      }
      case ScriptGenerator::PerformanceTestType::cBANK_TO_TURN:
      {
         CreateRangeTestFile(mTestDir.absolutePath());
         CreateEnduranceTestFile(mTestDir.absolutePath());
         CreateThrustDragTestFile(mTestDir.absolutePath(), 0);
         CreateThrustDragTestFile(mTestDir.absolutePath(), 5000);
         CreateThrustDragTestFile(mTestDir.absolutePath(), 15000);
         CreateThrustDragTestFile(mTestDir.absolutePath(), 30000);
         CreateThrustDragTestFile(mTestDir.absolutePath(), 45000);
         CreateThrustDragTestFile(mTestDir.absolutePath(), 60000);
         break;
      }
      case ScriptGenerator::PerformanceTestType::cSKID_TO_TURN:
      {
         CreateBallisticRangeTestFile(mTestDir.absolutePath());
         CreateLevelFlightRangeTestFile(mTestDir.absolutePath());
         CreateMaximumSpeedTestFile(mTestDir.absolutePath());
         break;
      }
      default:
         break;
   }
}

void ScriptGeneratorGM::CreateBallisticRangeTestFile(const QString& aTestDir)
{
   QFile testFile(aTestDir + "/ballistic_range_test.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return;
   }

   QTextStream outStream(&testFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "//TEST_HEADER=====================================================\n";
   outStream << "//\n";
   outStream << "//TITLE Ballistic Range Test\n";
   outStream << "//\n";
   outStream << "//TEST_GROUP Weapon Tests\n";
   outStream << "//\n";
   outStream << "//DESCRIPTION This test launches the weapon on ballistic trajectories using multiple launch angles to determine ballistic range\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTION\n";
   outStream << "// This test launches the weapon on ballistic trajectories using multiple launch angles to determine its ballistic range. No weapon guidance/control is used. The maximum range is plotted as a function of launch angle.\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "//**********************************************************\n";
   outStream << "\n";
   outStream << "include_once ../../platforms/" << mPlatformName << ".txt\n\n";
   outStream << "end_time 1000 sec\n";
   outStream << "\n";
   outStream << "script_struct DataStruct\n";
   outStream << "   script_variables\n";
   outStream << "      double maxRange_m          = 0;\n";
   outStream << "      double launchAngle_deg     = 0;\n";
   outStream << "   end_script_variables\n";
   outStream << "end_script_struct\n\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   double START_LOFT_ANGLE = 10;\n";
   outStream << "   double STOP_LOFT_ANGLE  = 80;\n";
   outStream << "   double INCREMENT_ANGLE   = 1;\n";
   outStream << "   double LAST_SHOT = 0;\n\n";
   outStream << "   Map<int, struct> mData = Map<int, struct>();\n\n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_ballistic_range_test.csv\";\n";
   outStream << "end_script_variables\n\n";
   outStream << "\n";
   outStream << "platform_type  TEST_PLATFORM  " << mPlatformType << "\n\n";
   outStream << "   weapon_effects WSF_GRADUATED_LETHALITY\n\n";
   outStream << "   on_update\n";
   outStream << "      if (PLATFORM.Altitude() <= 0)\n";
   outStream << "      {\n";
   outStream << "         double range_m = PLATFORM.GroundRangeTo(WsfSimulation.FindPlatform(\"launcher\"));\n";
   outStream << "         int index = PLATFORM.Index();\n";
   outStream << "         if (mData.Exists(index))\n";
   outStream << "         {\n";
   outStream << "            mData[index]->maxRange_m = range_m;\n";
   outStream << "         }\n";
   outStream << "         WsfSimulation.DeletePlatform(index);\n";
   outStream << "      }\n";
   outStream << "   end_on_update\n\n";
   outStream << "end_platform_type\n\n";
   outStream << "\n";
   outStream << "weapon AIM-MR_GM WSF_EXPLICIT_WEAPON\n";
   outStream << "   launched_platform_type TEST_PLATFORM\n";
   outStream << "   quantity 1\n";
   outStream << "end_weapon\n";
   outStream << "\n";
   outStream << "platform launcher WSF_PLATFORM\n";
   outStream << "   side blue\n";
   outStream << "   position 0n 0e\n";
   outStream << "   altitude 1000 ft\n";
   outStream << "   add weapon missile AIM-MR_GM\n";
   outStream << "      quantity 200\n";
   outStream << "   end_weapon\n\n";
   outStream << "\n";
   outStream << "   add processor launch_missiles WSF_SCRIPT_PROCESSOR\n";
   outStream << "      update_interval 1 sec\n";
   outStream << "      on_update\n";
   outStream << "         WsfWeapon weapon = PLATFORM.Weapon(\"missile\");\n";
   outStream << "         if(LAST_SHOT == 0)\n";
   outStream << "         {\n";
   outStream << "            weapon.SetPitch(START_LOFT_ANGLE);\n";
   outStream << "            weapon.Fire();\n\n";
   outStream << "            LAST_SHOT = START_LOFT_ANGLE;\n";
   outStream << "         }\n";
   outStream << "         else\n";
   outStream << "         {\n";
   outStream << "            if (LAST_SHOT >= STOP_LOFT_ANGLE)\n";
   outStream << "            {\n";
   outStream << "               PROCESSOR.TurnOff();\n";
   outStream << "            }\n";
   outStream << "            else\n";
   outStream << "            {\n";
   outStream << "               LAST_SHOT = LAST_SHOT + INCREMENT_ANGLE;\n";
   outStream << "               weapon.SetPitch(LAST_SHOT);\n";
   outStream << "               weapon.Fire();\n";
   outStream << "            }\n";
   outStream << "         }\n";
   outStream << "      end_on_update\n";
   outStream << "   end_processor\n";
   outStream << "end_platform\n\n";
   outStream << "\n";
   outStream << "script void WeaponFired(WsfWeaponEngagement aWeaponEngagement, WsfTrack aTrack)\n";
   outStream << "   struct data = struct.New(\"DataStruct\");\n";
   outStream << "   data->launchAngle_deg = aWeaponEngagement.WeaponPlatform().Pitch();\n";
   outStream << "   mData.Set(aWeaponEngagement.WeaponPlatform().Index(), data);\n";
   outStream << "end_script\n\n";
   outStream << "\n";
   outStream << "script void WriteOutput()\n";
   outStream << "   FileIO output = FileIO();\n";
   outStream << "   if(output.Open(outputFileName, \"out\"))\n";
   outStream << "   {\n";
   outStream << "      output.Writeln(\"Meta,title=Ballistic Range (1000 ft launch altitude),y_label=Range (nm)\");\n";
   outStream << "      output.Writeln(\"Launch Angle (deg), Weapon Range\");\n\n";
   outStream << "      foreach (int key : struct value in mData)\n";
   outStream << "      {\n";
   outStream << "         output.Writeln(write_str(value->launchAngle_deg, \", \", value->maxRange_m * MATH.NM_PER_M()));\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "end_script\n\n";
   outStream << "\n";
   outStream << "observer\n";
   outStream << "   enable WEAPON_FIRED WeaponFired\n";
   outStream << "   enable SIMULATION_COMPLETE WriteOutput\n";
   outStream << "end_observer\n";
}

void Designer::ScriptGeneratorGM::CreateThrustDragTestFile(const QString& aTestDir, double aAltitude_ft)
{
   std::stringstream testNameBuilder{};
   testNameBuilder << "Thrust Drag (" << static_cast<int>(aAltitude_ft) << " ft)";
   QString testHeading = QString::fromStdString(testNameBuilder.str());
   testNameBuilder << " Test";
   QString testName = QString::fromStdString(testNameBuilder.str());

   QFile testFile(aTestDir + "/" + testName.toLower().replace(" ", "_") + ".txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return;
   }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "//TEST_HEADER=====================================================\n";
   outStream << "//\n";
   outStream << "//TITLE " << testName << "\n";
   outStream << "//\n";
   outStream << "//TEST_GROUP Aircraft Tests\n";
   outStream << "//\n";
   outStream << "//DESCRIPTION This test compares the drag and thrust curves at a specified altitude\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTIONS\n";
   outStream << "// This test compares the drag and thrust curves at a specified altitude. It plots, as a function of Mach, the drag, and military power thrust.\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   double gTestAltitude_ft = " << aAltitude_ft << ";\n";
   outStream << "   double gInitialMach = 0.0;\n";
   outStream << "   double gFinalMach   = 3.0;\n";
   outStream << "   double gStepMach    = 0.1;\n";
   outStream << "\n";
   outStream << "   Array<struct> mData          = Array<struct>();\n";
   outStream << "\n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_" << testName.toLower().replace(" ", "_") << ".csv\";\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "script_struct DataSet\n";
   outStream << "   script_variables\n";
   outStream << "      double mach       = 0;\n";
   outStream << "      double thrust_lbs = 0;\n";
   outStream << "      double drag_lbs   = 0;\n";
   outStream << "   end_script_variables\n";
   outStream << "end_script_struct\n";
   outStream << "\n";
   outStream << "include_once ../../platforms/" << mPlatformName << ".txt\n\n";
   outStream << "\n";
   outStream << "script void CollectData(WsfPlatform aPlatform)\n";
   outStream << "   WsfMover mover = aPlatform.Mover();\n";
   outStream << "   if (mover.IsA_TypeOf(\"WSF_GUIDED_MOVER\"))\n";
   outStream << "   {\n";
   outStream << "      WsfGuidedMover guidedMover = (WsfGuidedMover)mover;\n";
   outStream << "      WsfGuidedMoverTestObject testObj = guidedMover.__getTestObject();\n\n";   
   outStream << "      int machSteps = MATH.Floor((gFinalMach - gInitialMach) / gStepMach) + 1;\n";
   outStream << "      for (int machIdx = 0; machIdx <= machSteps; machIdx = machIdx + 1)\n";
   outStream << "      {\n";
   outStream << "         double mach = gInitialMach + machIdx * gStepMach;\n";
   outStream << "         struct data = struct.New(\"DataSet\");\n";
   outStream << "         data->mach = mach;\n";
   outStream << "         data->thrust_lbs = testObj.GetLevelFlightThrust(gTestAltitude_ft, mach);\n";
   outStream << "         data->drag_lbs   = testObj.GetLevelFlightDrag(gTestAltitude_ft, mach, MATH.LB_PER_NT() * (guidedMover.EmptyMass() + 0.5 * guidedMover.InitialFuelMass()) * guidedMover.GravitationalAcceleration());\n";
   outStream << "         mData.PushBack(data);\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "end_script\n\n";
   outStream << "script void WriteOutput()\n";
   outStream << "   FileIO output = FileIO();\n";
   outStream << "   if (output.Open(outputFileName, \"out\"))\n";
   outStream << "   {\n";
   outStream << "      output.Writeln(\"Meta,title=" << testHeading << ",y_label=Forces (lbs)\");\n";
   outStream << "      output.Writeln(\"Mach,Drag,,Mach,Thrust - Military\");\n\n";
   outStream << "      for (int i = 0; i < mData.Size(); i = i + 1)\n";
   outStream << "      {\n";
   outStream << "         double mach   = mData[i]->mach;\n";
   outStream << "         double thr    = mData[i]->thrust_lbs;\n";
   outStream << "         double drag   = mData[i]->drag_lbs;\n";
   outStream << "         output.Writeln(write_str(mach, \",\", drag, \",,\", mach, \",\", thr));\n";
   outStream << "      }\n\n";
   outStream << "      output.Close();\n";
   outStream << "   }\n";
   outStream << "end_script\n\n";
   outStream << "platform test_platform " << mPlatformType << "\n";
   outStream << "   on_initialize2\n";
   outStream << "      CollectData(PLATFORM);\n";
   outStream << "      WriteOutput();\n";
   outStream << "   end_on_initialize2\n";
   outStream << "end_platform\n";
   outStream << "end_time 4 hr\n";
}

void Designer::ScriptGeneratorGM::CreateEnduranceTestFile(const QString& aTestDir)
{
   QFile testFile(aTestDir + "/endurance_test.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return;
   }

   QTextStream outStream(&testFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "//TEST_HEADER=====================================================\n";
   outStream << "//\n";
   outStream << "//TITLE Endurance Test\n";
   outStream << "//\n";
   outStream << "//TEST_GROUP Aircraft Tests\n";
   outStream << "//\n";
   outStream << "//DESCRIPTION This test determines that maximum flight time (endurance) of an aircraft at different altitudes and speeds\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTION\n";
   outStream << "// This test determines that maximum flight time (endurance) of an aircraft at different altitudes and speeds. A series of plots are provided that present the maximum flight time (endurance) as a function of altitude for a specified speed.\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   // Altitude and Velocity settings  \n";
   outStream << "   double initialAltitude_ft   = 0;\n";
   outStream << "   double finalAltitude_ft     = 50000;\n";
   outStream << "   double altitudeStep_ft      = 1000;\n";
   outStream << "   double initialVelocity_ktas = 100;\n";
   outStream << "   double finalVelocity_ktas   = 700;\n";
   outStream << "   double velocityStep_ktas    = 50;\n";
   outStream << "   \n";
   outStream << "   // Fuel at which the aircraft is considered out of fuel, and it has reach maximum range.\n";
   outStream << "   double finalFuelQuantity_lbs = 50; \n";
   outStream << "\n";
   outStream << "   Array<double>        mSpeeds = Array<double>();\n";
   outStream << "   Array<Array<struct>> mData = Array<Array<struct>>();\n";
   outStream << "\n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_endurance_test.csv\";\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "//**********************************************************\n";
   outStream << "\n";
   outStream << "script_struct DataSet\n";
   outStream << "   script_variables\n";
   outStream << "      double altitude_ft  = 0;\n";
   outStream << "      double endurance_hr = 0;\n";
   outStream << "   end_script_variables\n";
   outStream << "end_script_struct\n";
   outStream << "\n";
   outStream << "include_once ../../platforms/" << mPlatformName << ".txt\n\n";
   outStream << "script double IntegrateEndurance(WsfPlatform aPlatform, double aSpeed_ktas, double aAltitude_ft)\n";
   outStream << "   if (!aPlatform.Mover().IsA_TypeOf(\"WSF_GUIDED_MOVER\"))\n";
   outStream << "   {\n";
   outStream << "      writeln(\"--ERROR-- platform '\", aPlatform.Name(), \"' does not have a guided mover\");\n";
   outStream << "      return 0.0;\n";
   outStream << "   }\n";
   outStream << "   WsfGuidedMover mover = (WsfGuidedMover)aPlatform.Mover();\n";
   outStream << "   WsfGuidedMoverTestObject testObj = mover.__getTestObject();\n";
   outStream << "\n";
   outStream << "   double emptyMass_lbs = mover.EmptyMass() * MATH.LB_PER_KG();\n";
   outStream << "   double fuelMass_lbs = mover.InitialFuelMass() * MATH.LB_PER_KG();\n";
   outStream << "\n";
   outStream << "   double initialMass_lbs = emptyMass_lbs + fuelMass_lbs;\n";
   outStream << "   double finalMass_lbs = initialMass_lbs - (fuelMass_lbs - finalFuelQuantity_lbs);\n";
   outStream << "   double stepMass_lbs = (finalMass_lbs - initialMass_lbs) / 100.0;\n";
   outStream << "   double endurance_hr = 0.0;\n";
   outStream << "\n";
   outStream << "   for (double currentMass = initialMass_lbs; currentMass > finalMass_lbs; currentMass = currentMass + stepMass_lbs)\n";
   outStream << "   {\n";
   outStream << "      double fuelRate_lbsphr = testObj.GetLevelFlightFuelBurnRate(aAltitude_ft, aSpeed_ktas, currentMass);\n";
   outStream << "      if (fuelRate_lbsphr > 0.0)\n";
   outStream << "      {\n";
   outStream << "         double integrand = -stepMass_lbs / fuelRate_lbsphr;\n";
   outStream << "         endurance_hr = endurance_hr + integrand;\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "\n";
   outStream << "   return endurance_hr;\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void CollectData(WsfPlatform aPlatform)\n";
   outStream << "\n";
   outStream << "   WsfGuidedMover mover = (WsfGuidedMover)aPlatform.Mover();\n";
   outStream << "   WsfGuidedMoverTestObject testObj = mover.__getTestObject();\n";
   outStream << "\n";
   outStream << "   int nSpeedSteps = MATH.Floor((finalVelocity_ktas - initialVelocity_ktas) / velocityStep_ktas) + 1;\n";
   outStream << "   int nAltSteps = MATH.Floor((finalAltitude_ft - initialAltitude_ft) / altitudeStep_ft) + 1;\n";
   outStream << "   for (int speedIdx = 0; speedIdx < nSpeedSteps; speedIdx = speedIdx + 1)\n";
   outStream << "   {\n";
   outStream << "      double speed_ktas = initialVelocity_ktas + velocityStep_ktas * speedIdx;\n";
   outStream << "      mSpeeds.PushBack(speed_ktas);\n";
   outStream << "\n";
   outStream << "      Array<struct> loopData = Array<struct>();\n";
   outStream << "      for (int altIdx = 0; altIdx < nAltSteps; altIdx = altIdx + 1)\n";
   outStream << "      {\n";
   outStream << "         double alt_ft = initialAltitude_ft + altitudeStep_ft * altIdx;\n";
   outStream << "         struct data = struct.New(\"DataSet\");\n";
   outStream << "         data->altitude_ft = alt_ft;\n";
   outStream << "         data->endurance_hr = IntegrateEndurance(aPlatform, speed_ktas, alt_ft);\n";
   outStream << "\n";
   outStream << "         loopData.PushBack(data);\n";
   outStream << "      }\n";
   outStream << "      mData.PushBack(loopData);\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void GenerateFileHeaders(FileIO aFile)\n";
   outStream << "   aFile.Writeln(\"Meta,title=Endurance Test,y_label=Altitude (ft)\");\n";
   outStream << "   string line = \"\";\n";
   outStream << "   for (int i = 0; i < mSpeeds.Size(); i = i + 1)\n";
   outStream << "   {\n";
   outStream << "      line = line + \"Endurance (hr),\" + (string)mSpeeds[i] + \" KTAS\";\n";
   outStream << "      if (i != mSpeeds.Size() - 1)\n";
   outStream << "      {\n";
   outStream << "         line = line + \",,\";\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "   aFile.Writeln(line);\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void WriteOutput()\n";
   outStream << "   FileIO output = FileIO();\n";
   outStream << "   if (output.Open(outputFileName, \"out\"))\n";
   outStream << "   {\n";
   outStream << "      GenerateFileHeaders(output);\n";
   outStream << "\n";
   outStream << "      int dataSize = mData[0].Size();\n";
   outStream << "      for (int i = 0; i < dataSize; i = i + 1)\n";
   outStream << "      {\n";
   outStream << "         string line = \"\";\n";
   outStream << "         for (int j = 0; j < mData.Size(); j = j + 1)\n";
   outStream << "         {\n";
   outStream << "            line = line + write_str(mData[j][i]->endurance_hr, \",\", mData[j][i]->altitude_ft);\n";
   outStream << "            if (j != mData.Size() - 1)\n";
   outStream << "            {\n";
   outStream << "               line = line + \",,\";\n";
   outStream << "            }\n";
   outStream << "         }\n";
   outStream << "         output.Writeln(line);\n";
   outStream << "      }\n";
   outStream << "\n";
   outStream << "      output.Close();\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "platform test_platform " << mPlatformType << "\n";
   outStream << "   on_initialize2\n";
   outStream << "      CollectData(PLATFORM);\n";
   outStream << "      WriteOutput();\n";
   outStream << "   end_on_initialize2\n";
   outStream << "end_platform\n";
   outStream << "end_time 4 hr\n";
}

void Designer::ScriptGeneratorGM::CreateRangeTestFile(const QString& aTestDir)
{
   QFile testFile(aTestDir + "/range_test.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return;
   }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "//TEST_HEADER=====================================================\n";
   outStream << "//\n";
   outStream << "//TITLE Range Test\n";
   outStream << "//\n";
   outStream << "//TEST_GROUP Aircraft Tests\n";
   outStream << "//\n";
   outStream << "//DESCRIPTION This test determines the maximum range of an aircraft at different altitudes and speeds.\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTION\n";
   outStream << "// This test determines the maximum range of an aircraft at different altitudes and speeds. A series of plots are provided that present the maximum range as a function of altitude for a specified speed.\n";
   outStream << "//\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "\n";
   outStream << "// Use these variables to change the test parameters\n";
   outStream << "script_variables\n";
   outStream << "   // Altitude and Velocity settings\n";
   outStream << "   double initialAltitude_ft = 0;\n";
   outStream << "   double finalAltitude_ft = 50000;\n";
   outStream << "   double altitudeStep_ft = 1000;\n";
   outStream << "   double initialVelocity_ktas = 100;\n";
   outStream << "   double finalVelocity_ktas = 700;\n";
   outStream << "   double velocityStep_ktas = 50;\n";
   outStream << "\n";
   outStream << "   // Fuel at which the aircraft is considered out of fuel, and it has reach maximum range.\n";
   outStream << "   double finalFuelQuantity_lbs = 100;\n";
   outStream << "\n";
   outStream << "   Array<double>        mSpeeds = Array<double>();\n";
   outStream << "   Array<Array<struct>> mData = Array<Array<struct>>();\n";
   outStream << "\n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_range_test.csv\";\n";
   outStream << "end_script_variables\n\n";
   outStream << "script_struct DataSet\n";
   outStream << "   script_variables\n";
   outStream << "   double altitude_ft = 0;\n";
   outStream << "   double range_nm = 0;\n";
   outStream << "   end_script_variables\n";
   outStream << "end_script_struct\n\n";
   outStream << "include_once ../../platforms/" << mPlatformName << ".txt\n\n";
   outStream << "script double IntegrateRange(WsfPlatform aPlatform, double aSpeed_ktas, double aAltitude_ft)\n";
   outStream << "   if (!aPlatform.Mover().IsA_TypeOf(\"WSF_GUIDED_MOVER\"))\n";
   outStream << "   {\n";
   outStream << "      writeln(\"--ERROR-- platform '\", aPlatform.Name(), \"' does not have a guided mover\");\n";
   outStream << "      return 0.0;\n";
   outStream << "   }\n";
   outStream << "   WsfGuidedMover mover = (WsfGuidedMover)aPlatform.Mover();\n";
   outStream << "   WsfGuidedMoverTestObject testObj = mover.__getTestObject();\n";
   outStream << "\n";
   outStream << "   double emptyMass_lbs = mover.EmptyMass() * MATH.LB_PER_KG();\n";
   outStream << "   double fuelMass_lbs = mover.InitialFuelMass() * MATH.LB_PER_KG();\n";
   outStream << "\n";
   outStream << "   double initialMass_lbs = emptyMass_lbs + fuelMass_lbs;\n";
   outStream << "   double finalMass_lbs = initialMass_lbs - (fuelMass_lbs - finalFuelQuantity_lbs);\n";
   outStream << "   double stepMass_lbs = (finalMass_lbs - initialMass_lbs) / 100.0;\n";
   outStream << "   double range_nm = 0.0;\n";
   outStream << "\n";
   outStream << "   for (double currentMass = initialMass_lbs; currentMass > finalMass_lbs; currentMass = currentMass + stepMass_lbs)\n";
   outStream << "   {\n";
   outStream << "      double fuelRate_lbsphr = testObj.GetLevelFlightFuelBurnRate(aAltitude_ft, aSpeed_ktas, currentMass);\n";
   outStream << "      if (fuelRate_lbsphr > 0.0)\n";
   outStream << "      {\n";
   outStream << "         double integrand = -(aSpeed_ktas / fuelRate_lbsphr) * stepMass_lbs;\n";
   outStream << "         range_nm = range_nm + integrand;\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "\n";
   outStream << "   return range_nm;\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void CollectData(WsfPlatform aPlatform)\n";
   outStream << "   int nSpeedSteps = MATH.Floor((finalVelocity_ktas - initialVelocity_ktas) / velocityStep_ktas) + 1;\n";
   outStream << "   int nAltSteps = MATH.Floor((finalAltitude_ft - initialAltitude_ft) / altitudeStep_ft) + 1;\n";
   outStream << "   for (int speedIdx = 0; speedIdx < nSpeedSteps; speedIdx = speedIdx + 1)\n";
   outStream << "   {\n";
   outStream << "      double speed_ktas = initialVelocity_ktas + velocityStep_ktas * speedIdx;\n";
   outStream << "      mSpeeds.PushBack(speed_ktas);\n";
   outStream << "\n";
   outStream << "      Array<struct> loopData = Array<struct>();\n";
   outStream << "      for (int altIdx = 0; altIdx < nAltSteps; altIdx = altIdx + 1)\n";
   outStream << "      {\n";
   outStream << "         double alt_ft = initialAltitude_ft + altitudeStep_ft * altIdx;\n";
   outStream << "         struct data = struct.New(\"DataSet\");\n";
   outStream << "         data->altitude_ft = alt_ft;\n";
   outStream << "         data->range_nm = IntegrateRange(aPlatform, speed_ktas, alt_ft);\n";
   outStream << "\n";
   outStream << "         loopData.PushBack(data);\n";
   outStream << "      }\n";
   outStream << "      mData.PushBack(loopData);\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void GenerateFileHeaders(FileIO aFile)\n";
   outStream << "   aFile.Writeln(\"Meta,title=Range Test,y_label=Altitude (ft)\");\n";
   outStream << "   string line = \"\";\n";
   outStream << "   for (int i = 0; i < mSpeeds.Size(); i = i + 1)\n";
   outStream << "   {\n";
   outStream << "      line = line + \"Range (nm),\" + (string)mSpeeds[i] + \" KTAS\";\n";
   outStream << "      if (i != mSpeeds.Size() - 1)\n";
   outStream << "      {\n";
   outStream << "         line = line + \",,\";\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "   aFile.Writeln(line);\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void WriteOutput()\n";
   outStream << "   FileIO output = FileIO();\n";
   outStream << "   if (output.Open(outputFileName, \"out\"))\n";
   outStream << "   {\n";
   outStream << "      GenerateFileHeaders(output);\n";
   outStream << "\n";
   outStream << "      int dataSize = mData[0].Size();\n";
   outStream << "      for (int i = 0; i < dataSize; i = i + 1)\n";
   outStream << "      {\n";
   outStream << "         string line = \"\";\n";
   outStream << "         for (int j = 0; j < mData.Size(); j = j + 1)\n";
   outStream << "         {\n";
   outStream << "            line = line + write_str(mData[j][i]->range_nm, \",\", mData[j][i]->altitude_ft);\n";
   outStream << "            if (j != mData.Size() - 1)\n";
   outStream << "            {\n";
   outStream << "               line = line + \",,\";\n";
   outStream << "            }\n";
   outStream << "         }\n";
   outStream << "         output.Writeln(line);\n";
   outStream << "      }\n";
   outStream << "\n";
   outStream << "      output.Close();\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "platform test_platform " << mPlatformType << "\n";
   outStream << "   on_initialize2\n";
   outStream << "      CollectData(PLATFORM);\n";
   outStream << "      WriteOutput();\n";
   outStream << "   end_on_initialize2\n";
   outStream << "end_platform\n";
   outStream << "end_time 4 hr\n";
}

void Designer::ScriptGeneratorGM::CreateLevelFlightRangeTestFile(const QString& aTestDir)
{
   QFile testFile(aTestDir + "/level_flight_range_test.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return;
   }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "//TEST_HEADER=====================================================\n";
   outStream << "//\n";
   outStream << "//TITLE Level Flight Range Test\n";
   outStream << "//\n";
   outStream << "//TEST_GROUP Weapon Tests\n";
   outStream << "//\n";
   outStream << "//DESCRIPTION This test displays that maximum level flight range of a weapon at different altitudes\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTION\n";
   outStream << "//This test displays that maximum level flight range of a weapon at different altitudes for a launch speed of Mach 0.85. The maximum range is determined when the minimum speed (Mach 0.7) is reached. A plot of the altitude versus maximum range is provided.\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "//**********************************************************\n";
   outStream << "script_variables\n";
   outStream << "   double MINIMUM_LAUNCH_ALTITUDE_FT = 1000;\n";
   outStream << "   double MAXIMUM_LAUNCH_ALTITUDE_FT = 45000;\n";
   outStream << "   double ALTITUDE_STEP_FT           = 1000;\n";
   outStream << "   double LAUNCH_SPEED_MACH          = 0.85;\n";
   outStream << "   double MIN_TERMINATION_SPEED_MACH = 0.7;\n";
   outStream << "   double ACTIVATION_SPEED_MACH      = MIN_TERMINATION_SPEED_MACH * 1.2;\n";
   outStream << "   double INITIAL_LATITUDE = 21.4;\n";
   outStream << "   double INITIAL_LONGITUDE = -158.0;\n";
   outStream << "   int totalTests = 1 + (MAXIMUM_LAUNCH_ALTITUDE_FT - MINIMUM_LAUNCH_ALTITUDE_FT) / ALTITUDE_STEP_FT;\n";
   outStream << "   int completedCount = 0;\n";
   outStream << "   Map<double, double> rangeAltMap = Map<double, double>();\n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_level_flight_range_test.csv\";\n";
   outStream << "   Atmosphere atmosphere = Atmosphere.Construct(\"\");\n";
   outStream << "end_script_variables\n";
   outStream << "//**********************************************************\n";
   outStream << "\n";
   outStream << "include_once ../../platforms/" << mPlatformName << ".txt\n\n";
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 10 min\n";
   outStream << "\n";
   outStream << "script void RegisterCompleted()\n";
   outStream << "   completedCount = completedCount + 1;\n";
   outStream << "   \n";
   outStream << "   if(completedCount == totalTests)\n";
   outStream << "   {\n";
   outStream << "      WsfSimulation.Terminate();\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM " << mPlatformType << "\n";
   outStream << "   \n";
   outStream << "   weapon_effects WSF_GRADUATED_LETHALITY\n";
   outStream << "   \n";
   outStream << "   aux_data\n";
   outStream << "      double commandedAltitude_m = 0 // meters\n";
   outStream << "   end_aux_data\n";
   outStream << "   \n";
   outStream << "   script_variables\n";
   outStream << "      bool   completed       = false;\n";
   outStream << "      bool   activated       = false;\n";
   outStream << "      double maxSpeed_mach   = 0.0;\n";
   outStream << "   end_script_variables\n";
   outStream << "   processor guidance WSF_GUIDANCE_COMPUTER\n";
   outStream << "   end_processor\n";
   outStream << "\n";
   outStream << "   on_update\n";
   outStream << "   if (!completed)\n";
   outStream << "   {\n";
   outStream << "      double speed_mps = PLATFORM.VelocityNED().X();\n";
   outStream << "      double altitude_m = PLATFORM.Altitude();\n";
   outStream << "\n";
   outStream << "      double mach = speed_mps / atmosphere.SonicVelocity(altitude_m);\n";
   outStream << "\n";
   outStream << "      if (mach > maxSpeed_mach)\n";
   outStream << "      {\n";
   outStream << "         maxSpeed_mach = mach;\n";
   outStream << "      }\n";
   outStream << "\n";
   outStream << "      if (!activated && mach > ACTIVATION_SPEED_MACH)\n";
   outStream << "      {\n";
   outStream << "         activated = true;\n";
   outStream << "      }\n";
   outStream << "\n";
   outStream << "      if (activated && mach < MIN_TERMINATION_SPEED_MACH)\n";
   outStream << "      {\n";
   outStream << "         completed = true;\n";
   outStream << "         double altitude_m = PLATFORM.AuxDataDouble(\"commandedAltitude_m\");\n";
   outStream << "         double altitude_ft = altitude_m * MATH.FT_PER_M();\n";
   outStream << "         double maxRange_nm = MATH.NM_PER_M() * PLATFORM.GroundRangeTo(INITIAL_LATITUDE, INITIAL_LONGITUDE, altitude_m);\n";
   outStream << "         rangeAltMap.Set(altitude_ft, maxRange_nm);\n";
   outStream << "         writeln(write_str(\"TestCompleted: Alt \", altitude_ft, \" ft Range \", maxRange_nm, \" nm at t=\", TIME_NOW, \" sec\"));\n";
   outStream << "         RegisterCompleted();\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "   end_on_update\n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "weapon " << mPlatformType << " WSF_EXPLICIT_WEAPON\n";
   outStream << "   launched_platform_type TEST_PLATFORM\n";
   outStream << "end_weapon\n";
   outStream << "\n";
   outStream << "platform_type LAUNCHER WSF_PLATFORM\n";
   outStream << "   position 21.4n 158.0w\n";
   outStream << "   heading 0 deg\n";
   outStream << "   side blue\n";
   outStream << "   weapon weapon " << mPlatformType << "\n";
   outStream << "      quantity 1\n";
   outStream << "   end_weapon\n";
   outStream << "   mover WSF_AIR_MOVER\n";
   outStream << "   end_mover\n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "execute at_time 0.01 s absolute\n";
   outStream << "   for (double alt = MINIMUM_LAUNCH_ALTITUDE_FT; alt <= MAXIMUM_LAUNCH_ALTITUDE_FT; alt = alt + ALTITUDE_STEP_FT)\n";
   outStream << "   {\n";
   outStream << "      double commandedAltitude_m = alt * MATH.M_PER_FT();\n";
   outStream << "      \n";
   outStream << "      // Create a platform instance of type \"LAUNCHER\"\n";
   outStream << "      WsfPlatform tmpPlatform = WsfSimulation.CreatePlatform(\"LAUNCHER\");\n";
   outStream << "      \n";
   outStream << "      // Add the platform to the simulation\n";
   outStream << "      WsfPlatform p = WsfSimulation.AddPlatform(tmpPlatform, \"\");\n";
   outStream << "      \n";
   outStream << "      // Set the altitude to the desired commanded altitude\n";
   outStream << "      p.SetLocation(tmpPlatform.Latitude(), tmpPlatform.Longitude(), commandedAltitude_m);\n";
   outStream << "      \n";
   outStream << "      // Give it a commanded mach number\n";
   outStream << "      p.GoToMachNumber(LAUNCH_SPEED_MACH);\n";
   outStream << "   }\n";
   outStream << "end_execute\n";
   outStream << "\n";
   outStream << "execute at_time 30 sec absolute\n";
   outStream << "   for (int i = 0; i < WsfSimulation.PlatformCount(); i += 1)\n";
   outStream << "   {\n";
   outStream << "      WsfPlatform p = WsfSimulation.PlatformEntry(i);\n";
   outStream << "      if (p.IsA_TypeOf(\"LAUNCHER\"))\n";
   outStream << "      {\n";
   outStream << "         p.Weapon(\"weapon\").Fire();\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "end_execute\n";
   outStream << "\n";
   outStream << "script void WeaponFired(WsfWeaponEngagement aWeaponEngagement, WsfTrack aTrack)\n";
   outStream << "   double commandedAltitude_m = aWeaponEngagement.FiringPlatform().Altitude();\n";
   outStream << "   aWeaponEngagement.WeaponPlatform().SetAuxData(\"commandedAltitude_m\", commandedAltitude_m);\n";
   outStream << "   WsfProcessor proc = aWeaponEngagement.WeaponPlatform().Processor(\"guidance\");\n";
   outStream << "   if (!proc.IsNull())\n";
   outStream << "   {\n";
   outStream << "      WsfGuidanceComputer gc = (WsfGuidanceComputer)proc;\n";
   outStream << "      if (!gc.IsNull())\n";
   outStream << "      {\n";
   outStream << "         gc.SetCommandedAltitude(commandedAltitude_m);\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void SimulationComplete()\n";
   outStream << "   FileIO output = FileIO();\n";
   outStream << "   if (output.Open(outputFileName, \"out\"))\n";
   outStream << "   {\n";
   outStream << "   output.Writeln(\"Meta,title=Level Flight Range (Launch Mach 0.85),y_label=Altitude (ft)\");\n";
   outStream << "   output.Writeln(\"Range (nm), Weapon Range\");\n";
   outStream << "   \n";
   outStream << "   foreach(double key : double value in rangeAltMap)\n";
   outStream << "   {\n";
   outStream << "      output.Writeln(write_str(value, \", \", key));\n";
   outStream << "   }\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "observer\n";
   outStream << "   enable SIMULATION_COMPLETE  SimulationComplete\n";
   outStream << "   enable WEAPON_FIRED         WeaponFired\n";
   outStream << "end_observer\n";
}

void Designer::ScriptGeneratorGM::CreateMaximumSpeedTestFile(const QString& aTestDir)
{
   QFile testFile(aTestDir + "/maximum_speed_test.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return;
   }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);
   outStream << "//TEST_HEADER=====================================================\n";
   outStream << "//\n";
   outStream << "//TITLE Maximum Speed Test\n";
   outStream << "//\n";
   outStream << "//TEST_GROUP Weapon Tests\n";
   outStream << "//\n";
   outStream << "//DESCRIPTION This test is used to determine the maximum level speed of the vehicle\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTION\n";
   outStream << "// This test is used to determine the maximum level speed of the vehicle. A plot of the altitude versus maximum level speed is provided.\n";
   outStream << "//\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "//**********************************************************\n";
   outStream << "script_variables\n";
   outStream << "   double MINIMUM_LAUNCH_ALTITUDE_FT = 1000;\n";
   outStream << "   double MAXIMUM_LAUNCH_ALTITUDE_FT = 45000;\n";
   outStream << "   double ALTITUDE_STEP_FT           = 1000;\n";
   outStream << "   double LAUNCH_SPEED_MACH          = 0.85;\n";
   outStream << "   double MIN_TERMINATION_SPEED_MACH = 0.7;\n";
   outStream << "   double ACTIVATION_SPEED_MACH      = MIN_TERMINATION_SPEED_MACH * 1.2;\n";
   outStream << "   double INITIAL_LATITUDE = 21.4;\n";
   outStream << "   double INITIAL_LONGITUDE = -158.0;\n";
   outStream << "   int totalTests = 1 + (MAXIMUM_LAUNCH_ALTITUDE_FT - MINIMUM_LAUNCH_ALTITUDE_FT) / ALTITUDE_STEP_FT;\n";
   outStream << "   int completedCount = 0;\n";
   outStream << "   Map<double, double> speedAltMap = Map<double, double>();\n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_maximum_speed_test.csv\";\n";
   outStream << "   Atmosphere atmosphere = Atmosphere.Construct(\"\");\n";
   outStream << "end_script_variables\n";
   outStream << "//**********************************************************\n";
   outStream << "\n";
   outStream << "include_once ../../platforms/" << mPlatformName << ".txt\n\n";
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 10 min\n";
   outStream << "\n";
   outStream << "script void RegisterCompleted()\n";
   outStream << "   completedCount = completedCount + 1;\n";
   outStream << "   \n";
   outStream << "   if(completedCount == totalTests)\n";
   outStream << "   {\n";
   outStream << "      WsfSimulation.Terminate();\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM " << mPlatformType << "\n";
   outStream << "   \n";
   outStream << "   weapon_effects WSF_GRADUATED_LETHALITY\n";
   outStream << "   \n";
   outStream << "   aux_data\n";
   outStream << "      double commandedAltitude_m = 0 // meters\n";
   outStream << "   end_aux_data\n";
   outStream << "   \n";
   outStream << "   script_variables\n";
   outStream << "      bool   completed       = false;\n";
   outStream << "      bool   activated       = false;\n";
   outStream << "      double maxSpeed_mach   = 0.0;\n";
   outStream << "   end_script_variables\n";
   outStream << "   processor guidance WSF_GUIDANCE_COMPUTER\n";
   outStream << "   end_processor\n";
   outStream << "\n";
   outStream << "   on_update\n";
   outStream << "   if (!completed)\n";
   outStream << "   {\n";
   outStream << "      double speed_mps = PLATFORM.VelocityNED().X();\n";
   outStream << "      double altitude_m = PLATFORM.Altitude();\n";
   outStream << "\n";
   outStream << "      double mach = speed_mps / atmosphere.SonicVelocity(altitude_m);\n";
   outStream << "\n";
   outStream << "      if (mach > maxSpeed_mach)\n";
   outStream << "      {\n";
   outStream << "         maxSpeed_mach = mach;\n";
   outStream << "      }\n";
   outStream << "\n";
   outStream << "      if (!activated && mach > ACTIVATION_SPEED_MACH)\n";
   outStream << "      {\n";
   outStream << "         activated = true;\n";
   outStream << "      }\n";
   outStream << "\n";
   outStream << "      if (activated && mach < MIN_TERMINATION_SPEED_MACH)\n";
   outStream << "      {\n";
   outStream << "         completed = true;\n";
   outStream << "         double altitude_m = PLATFORM.AuxDataDouble(\"commandedAltitude_m\");\n";
   outStream << "         double altitude_ft = altitude_m * MATH.FT_PER_M();\n";
   outStream << "         double maxRange_nm = MATH.NM_PER_M() * PLATFORM.GroundRangeTo(INITIAL_LATITUDE, INITIAL_LONGITUDE, altitude_m);\n";
   outStream << "         speedAltMap.Set(altitude_ft, maxSpeed_mach);\n";
   outStream << "         writeln(write_str(\"TestCompleted: Alt \", altitude_ft, \" ft Max Mach \", maxSpeed_mach, \" at t=\", TIME_NOW, \" sec\"));\n";
   outStream << "         RegisterCompleted();\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "   end_on_update\n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "weapon " << mPlatformType << " WSF_EXPLICIT_WEAPON\n";
   outStream << "   launched_platform_type TEST_PLATFORM\n";
   outStream << "end_weapon\n";
   outStream << "\n";
   outStream << "platform_type LAUNCHER WSF_PLATFORM\n";
   outStream << "   position 21.4n 158.0w\n";
   outStream << "   heading 0 deg\n";
   outStream << "   side blue\n";
   outStream << "   weapon weapon " << mPlatformType << "\n";
   outStream << "      quantity 1\n";
   outStream << "   end_weapon\n";
   outStream << "   mover WSF_AIR_MOVER\n";
   outStream << "   end_mover\n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "execute at_time 0.01 s absolute\n";
   outStream << "   for (double alt = MINIMUM_LAUNCH_ALTITUDE_FT; alt <= MAXIMUM_LAUNCH_ALTITUDE_FT; alt = alt + ALTITUDE_STEP_FT)\n";
   outStream << "   {\n";
   outStream << "      double commandedAltitude_m = alt * MATH.M_PER_FT();\n";
   outStream << "      \n";
   outStream << "      // Create a platform instance of type \"LAUNCHER\"\n";
   outStream << "      WsfPlatform tmpPlatform = WsfSimulation.CreatePlatform(\"LAUNCHER\");\n";
   outStream << "      \n";
   outStream << "      // Add the platform to the simulation\n";
   outStream << "      WsfPlatform p = WsfSimulation.AddPlatform(tmpPlatform, \"\");\n";
   outStream << "      \n";
   outStream << "      // Set the altitude to the desired commanded altitude\n";
   outStream << "      p.SetLocation(tmpPlatform.Latitude(), tmpPlatform.Longitude(), commandedAltitude_m);\n";
   outStream << "      \n";
   outStream << "      // Give it a commanded mach number\n";
   outStream << "      p.GoToMachNumber(LAUNCH_SPEED_MACH);\n";
   outStream << "   }\n";
   outStream << "end_execute\n";
   outStream << "\n";
   outStream << "execute at_time 30 sec absolute\n";
   outStream << "   for (int i = 0; i < WsfSimulation.PlatformCount(); i += 1)\n";
   outStream << "   {\n";
   outStream << "      WsfPlatform p = WsfSimulation.PlatformEntry(i);\n";
   outStream << "      if (p.IsA_TypeOf(\"LAUNCHER\"))\n";
   outStream << "      {\n";
   outStream << "         p.Weapon(\"weapon\").Fire();\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "end_execute\n";
   outStream << "\n";
   outStream << "script void WeaponFired(WsfWeaponEngagement aWeaponEngagement, WsfTrack aTrack)\n";
   outStream << "   double commandedAltitude_m = aWeaponEngagement.FiringPlatform().Altitude();\n";
   outStream << "   aWeaponEngagement.WeaponPlatform().SetAuxData(\"commandedAltitude_m\", commandedAltitude_m);\n";
   outStream << "   WsfProcessor proc = aWeaponEngagement.WeaponPlatform().Processor(\"guidance\");\n";
   outStream << "   if (!proc.IsNull())\n";
   outStream << "   {\n";
   outStream << "      WsfGuidanceComputer gc = (WsfGuidanceComputer)proc;\n";
   outStream << "      if (!gc.IsNull())\n";
   outStream << "      {\n";
   outStream << "         gc.SetCommandedAltitude(commandedAltitude_m);\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void SimulationComplete()\n";
   outStream << "   FileIO output = FileIO();\n";
   outStream << "   if (output.Open(outputFileName, \"out\"))\n";
   outStream << "   {\n";
   outStream << "   output.Writeln(\"Meta,title=Level Flight Maximum Speed (Launch Mach 0.85),y_label=Altitude (ft)\");\n";
   outStream << "   output.Writeln(\"Speed (Mach), Maximum Speed\");\n";
   outStream << "   \n";
   outStream << "   foreach(double key : double value in speedAltMap)\n";
   outStream << "   {\n";
   outStream << "      output.Writeln(write_str(value, \", \", key));\n";
   outStream << "   }\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "observer\n";
   outStream << "   enable SIMULATION_COMPLETE  SimulationComplete\n";
   outStream << "   enable WEAPON_FIRED         WeaponFired\n";
   outStream << "end_observer\n";
}

void Designer::ScriptGeneratorGM::OutputMover(QTextStream& aStream, double aMaxMach)
{
   aStream << "# Setup the Guided Mover\n";
   aStream << "mover " << mPlatformType << " WSF_GUIDED_MOVER\n";
   aStream << "   update_interval   0.01 sec\n";
   aStream << "   aero             " << mVehicleName.toUpper() << "_AERO\n";

   OutputPropulsion(aStream, aMaxMach);

   aStream << "end_mover\n";
}

void ScriptGeneratorGM::OutputAero(QTextStream& aStream, double& aMaxMach)
{
   VehicleAero* vehicleAero = mVehiclePtr->GetAero();
   if (vehicleAero == nullptr) { return; }

   double referenceArea_sqft = (mVehiclePtr->IsAircraft() ? vehicleAero->GetVehicleWingArea_ft2() : vehicleAero->GetVehicleRefArea_ft2());

   std::vector<std::pair<double, double>> cd_vs_mach;
   auto table = mVehiclePtr->GetAero()->GetCoreAero().GetCd_MachTablePtr();
   if (table->GetModifiedData(cd_vs_mach))
   {
      aMaxMach = cd_vs_mach.back().first;
   }
   aStream << "\n";
   aStream << "aero  " << mVehicleName.toUpper() << "_AERO  WSF_AERO\n";
   aStream << "   reference_area " << referenceArea_sqft << " ft2\n";
   aStream << "   aspect_ratio   " << vehicleAero->GetAspectRatio() << "\n";
   aStream << "   cl_max         " << vehicleAero->GetCLMax() << "\n";

   for (const auto& mach_and_cd : cd_vs_mach)
   {
      aStream << "   mach_and_cd    " << mach_and_cd.first << " " << mach_and_cd.second << "\n";
   }

   aStream << "end_aero\n";
}

void ScriptGeneratorGM::OutputPropulsion(QTextStream& aStream, double aMaxMach)
{
   Designer::VehicleGeometry& vehicleGeometry = mVehiclePtr->GetGeometry();
   Designer::GeometryMassProperties* massProperties = vehicleGeometry.GetMassProperties();

   double total_mass = massProperties->GetTotalMass_lbs();

   AeroEngine::PropulsionParameters propulsion = ComputePropulsion(aMaxMach);

   aStream << "   total_mass                 " << total_mass << " lbm\n";
   aStream << "   fuel_mass                  " << propulsion.mFuel_mass << " lbm\n";

   aStream << "   sea_level_specific_impulse " << propulsion.mIsp_sea_level << " s\n";
   aStream << "   vacuum_specific_impulse    " << propulsion.mIsp_vacuum << " s\n";
   if (!propulsion.mThrustVsTime.empty())
   {
      aStream << "   sea_level_thrust_table\n";
      for (const auto& thrust_table : propulsion.mThrustVsTime)
      {
	      aStream << "      " << thrust_table.first << " sec " << thrust_table.second << " lbf\n";
      }
      aStream << "   end_sea_level_thrust_table\n\n";
   }
   else
   {
      aStream << "   sea_level_thrust           0.0 lbf\n";
   }

   if (!propulsion.mThrottleVsMach.empty())
   {
      aStream << "   throttle regular_table\n";
      aStream << "      independent_variable mach precision double\n";
      aStream << "        ";
      for (const auto& throttle : propulsion.mThrottleVsMach)
      {
         aStream << " " << throttle.first;
      }
      aStream << "\n";
      aStream << "      end_independent_variable\n";
      aStream << "      dependent_variable\n";
      aStream << "        ";
      for (const auto& throttle : propulsion.mThrottleVsMach)
      {
         aStream << " " << throttle.second;
      }
      aStream << "\n";
      aStream << "      end_dependent_variable\n";
      aStream << "   end_regular_table\n";
   }
}

AeroEngine::PropulsionParameters ScriptGeneratorGM::ComputePropulsion(double aMaxMach) const
{
   AeroEngine::PropulsionParameters vehiclePropulsion;

   std::vector<std::vector<std::pair<double, double>>> thrust_vs_time_vector;
   std::vector<std::vector<std::pair<double, double>>> throttle_vs_mach_vector;

   // Weights used to compute the "weighted average" of the throttle vs mach table
   std::vector<double> weights;
   double weights_total = 0.0;

   for (auto curEngine : mEngineList)
   {
      AeroEngine::PropulsionParameters enginePropulsion = curEngine->GetPropulsionData(aMaxMach);

      vehiclePropulsion.mFuel_mass += enginePropulsion.mFuel_mass; // lbm
      vehiclePropulsion.mMass_flow_rate += enginePropulsion.mMass_flow_rate; // pph
      vehiclePropulsion.mIsp_sea_level += (enginePropulsion.mMass_flow_rate * enginePropulsion.mIsp_sea_level); // s * pph (normalized to s later)
      vehiclePropulsion.mIsp_vacuum += (enginePropulsion.mMass_flow_rate * enginePropulsion.mIsp_vacuum); // s * pph (normalized to s later)
      thrust_vs_time_vector.push_back(enginePropulsion.mThrustVsTime); // lbf
      throttle_vs_mach_vector.push_back(enginePropulsion.mThrottleVsMach);

      // Compute weights used to compute the "weighted average" of the throttle vs mach table
      double approximateThrust = enginePropulsion.mIsp_sea_level * enginePropulsion.mMass_flow_rate / 3600.0;
      weights.push_back(approximateThrust); // average thrust, lbf
      weights_total += approximateThrust;
   }
   if (!mEngineList.empty())
   {
      vehiclePropulsion.mIsp_sea_level /= vehiclePropulsion.mMass_flow_rate; // s
      vehiclePropulsion.mIsp_vacuum /= vehiclePropulsion.mMass_flow_rate; // s
   }

   vehiclePropulsion.mThrustVsTime = AddData(thrust_vs_time_vector);
   vehiclePropulsion.mThrottleVsMach = AverageData(throttle_vs_mach_vector, weights, weights_total);

   // Add any fuel not included in the engine directly (e.g. Jet Engines)
   Designer::VehicleGeometry& vehicleGeometry = mVehiclePtr->GetGeometry();
   Designer::GeometryPropulsionData* propulsionData = vehicleGeometry.GetPropulsionData();
   if (propulsionData->FuelTankPresent())
   {
      vehiclePropulsion.mFuel_mass += propulsionData->GetFuelQtyCurrent();
   }

   return vehiclePropulsion;
}

std::map<double, std::vector<double>> ScriptGeneratorGM::CombineData(std::vector<std::vector<std::pair<double, double>>> aData)
{
   // Get vector of independent values
   std::vector<double> independentValues;
   for (const auto& data : aData)
   {
      for (const auto& dataPoint : data)
      {
         auto iter = std::find(independentValues.begin(), independentValues.end(), dataPoint.first);

         if (iter == independentValues.end())
         {
            independentValues.push_back(dataPoint.first);
         }
      }
   }

   std::map<double, std::vector<double>> dataComplete;

   // For each vector in aData, compute the completed (interpolated) data set
   for (const auto& data : aData)
   {
      for (const auto& xCoord : independentValues)
      {
         // Find the first element in data that is greater than or equal to fullDataPoint
         auto it = std::lower_bound(data.begin(), data.end(), xCoord,
            [](const std::pair<double, double>& aDataPoint, double aIV)
            -> bool {return aDataPoint.first < aIV; });

         if (it != data.end())
         {
            if (it->first == xCoord) // it->first is equal to fullDataPoint.first
            {
               dataComplete[xCoord].push_back(it->second);
            }
            else // it->first is greater than fullDataPoint.first
            {
               auto it_prev = std::prev(it);
               double slope = (it->second - it_prev->second) / (it->first - it_prev->first);

               dataComplete[xCoord].push_back(it_prev->second + slope * (xCoord - it_prev->first));
            }
         }
         else
         {
            dataComplete[xCoord].push_back(0.0);
         }
      }
   }
   return dataComplete;
}


std::vector<std::pair<double, double>> ScriptGeneratorGM::AverageData(const std::vector<std::vector<std::pair<double, double>>>& aData, const std::vector<double>& aWeights, double aNormalizingFactor)
{
   std::vector<std::pair<double, double>> averageData;

   auto combineData = CombineData(aData);

   for (const auto& data : combineData)
   {
      double dependentValuesSum = 0.0;
      size_t index = 0;
      for (const auto& dependentValue : data.second)
      {
         dependentValuesSum += dependentValue * aWeights.at(index);
         ++index;
      }
      averageData.emplace_back(data.first, dependentValuesSum/aNormalizingFactor);
   }

   return averageData;
}

std::vector<std::pair<double, double>> ScriptGeneratorGM::AddData(const std::vector<std::vector<std::pair<double, double>>>& aData)
{
   return AverageData(aData, std::vector<double>(aData.size(), 1.0), 1.0);
}

} // namespace Designer
