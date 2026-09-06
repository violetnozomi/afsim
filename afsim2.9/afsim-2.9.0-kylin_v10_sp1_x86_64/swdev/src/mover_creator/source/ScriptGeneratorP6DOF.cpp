// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "ScriptGeneratorP6DOF.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <utility>
#include <QDate>
#include <QFile>
#include <QString>

#include "AeroEngine.hpp"
#include "AeroLandingGear.hpp"
#include "AeroSolidRocketEngine.hpp"
#include "AeroSpeedBrake.hpp"
#include "AeroTable2d.hpp"
#include "AeroTable3d.hpp"
#include "AeroTable4d.hpp"
#include "ControlsWidget.hpp"
#include "EngineDesignerWidget.hpp"
#include "EngineWidget.hpp"
#include "GeometryEngine.hpp"
#include "G_LimitSettings.hpp"
#include "MoverCreatorEnvironment.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorWidget.hpp"
#include "UtLog.hpp"
#include "Vehicle.hpp"
#include "VehicleAero.hpp"
#include "VehicleAeroCore.hpp"
#include "VehicleAeroMovable.hpp"
#include "VehiclePropulsion.hpp"

namespace Designer
{

   ScriptGeneratorP6DOF::ScriptGeneratorP6DOF()
   : ScriptGenerator()
   {
   mPlatformName = mVehicleName.toLower() + "_p6dof";
   mPlatformType = mVehicleName.toUpper() + "_P6DOF";

   mTestDir.setPath(mVehicleDir.absolutePath() + "/tests/p6dof/");
   mOutputDir.setPath(mVehicleDir.absolutePath() + "/output/p6dof/");
   }

   void ScriptGeneratorP6DOF::CreateFolderStructure()
   {
   QString fullPath = mVehicleDir.absolutePath();

   // Create main folder
   if (!QDir(fullPath).exists())
   {
      QDir().mkdir(fullPath);
   }

   CreateFolderIfDoesntExist(fullPath + "/platforms");
   CreateFolderIfDoesntExist(fullPath + "/model_3d");
   CreateFolderIfDoesntExist(fullPath + "/p6dof_types");
   CreateFolderIfDoesntExist(fullPath + "/p6dof_types/environment");
   CreateFolderIfDoesntExist(fullPath + "/p6dof_types/scripts");

   if (mVehiclePtr == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   QString newFullPath;

   if (mVehiclePtr->IsAircraft())
   {
      CreateFolderIfDoesntExist(fullPath + "/p6dof_types/aircraft");
      newFullPath = mVehicleDir.absolutePath() + "/p6dof_types/aircraft";
   }
   else if (mVehiclePtr->IsWeapon())
   {
      CreateFolderIfDoesntExist(fullPath + "/p6dof_types/weapons");
      newFullPath = mVehicleDir.absolutePath() + "/p6dof_types/weapons";
   }

   QString lowercaseVehicleName = mVehicleName.toLower();

   newFullPath = newFullPath + "/" + lowercaseVehicleName;
   CreateFolderIfDoesntExist(newFullPath);

   CreateFolderIfDoesntExist(newFullPath + "/aero");
   CreateFolderIfDoesntExist(newFullPath + "/aero/data");
   CreateFolderIfDoesntExist(newFullPath + "/prop");
   if (mVehiclePtr->GetVehicleControlConfiguration() != Vehicle::cNO_CONTROL)
   {
      CreateFolderIfDoesntExist(newFullPath + "/controls");
   }
   CreateFolderIfDoesntExist(fullPath + "/tests");
   CreateFolderIfDoesntExist(mTestDir.absolutePath());
   CreateFolderIfDoesntExist(fullPath + "/output");
   CreateFolderIfDoesntExist(mOutputDir.absolutePath());

   }

   void ScriptGeneratorP6DOF::CreateBaseScenarioFile()
   {
   QFile baseFile(mTestDir.absolutePath() + "/" + mVehicleName.toLower() + "_scenario.txt");

   if (!baseFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   QTextStream outStream(&baseFile);

   outStream.setRealNumberNotation(QTextStream::FixedNotation);

   CreateAutogenHeader(mVehiclePtr, outStream);
   outStream << "# ==================================================================================================\n";
   outStream << "# This scenario file is used for testing the vehicle within AFSIM. It can be used for autopilot\n";
   outStream << "# tuning and flight testing within the Mover Creator tool development environment.\n";
   outStream << "# ==================================================================================================\n";
   outStream << "\n";
   outStream << "include_once ../../p6dof_types/environment/p6dof_environment.txt\n";
   outStream << "include_once ../../platforms/" << mPlatformName << ".txt\n";
   outStream << "\n";
   outStream << "# Set a start time that has a daytime sun position for our test in the Oahu region\n";
   outStream << "start_epoch 2018354.75   # Southern Solstice 2018\n";
   outStream << "\n";
   outStream << "# Allow Warlock to update more frequently and miss fewer updates\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "# Provide plenty of runtime, to prevent the scenario from ending before testing is complete.\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";

   outStream << "# Setup the test vehicle\n";

   if (mVehiclePtr->IsAircraft())
   {
      outStream << "platform  TestAicraft  " << mPlatformType << "\n";
      outStream << "\n";
   }
   else if (mVehiclePtr->IsWeapon())
   {
      outStream << "platform  TestWeapon  " << mPlatformType << "\n";
      outStream << "\n";
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle needs to be either an aircraft or a weapon!");
      return;
   }

   std::string side = "green";
   std::string icon = mVehiclePtr->GetVehicleName();

   outStream << "   side  ";
   OutputData(outStream, QString(side.c_str()), 32, QTextStream::FieldAlignment::AlignLeft);
   outStream << "   # Neutral force color\n";
   outStream << "   icon  ";
   OutputData(outStream, QString(icon.c_str()), 32, QTextStream::FieldAlignment::AlignLeft);
   outStream << "   # Icon based on geometry\n";
   outStream << "\n";

   double mach        = mVehiclePtr->GetTestingMach();
   double altitude_ft = mVehiclePtr->GetVehicleAltitude_ft();
   double altitude_m  = altitude_ft * UtMath::cM_PER_FT;

   UtAtmosphere atm = mVehiclePtr->GetAtmosphere();
   double ktas = atm.CalcKtasFromMach(altitude_m, mach);

   outStream << "   # Setup a northbound test route roughly 10nm south of Pearl Harbor in the Pacific\n";
   outStream << "   route\n";

   outStream << "      position   21.1888n 157.9534w   altitude ";
   OutputData(outStream, altitude_ft, 9, 1, QTextStream::FieldAlignment::AlignRight);
   outStream << " ft   speed ";
   OutputData(outStream, ktas, 7, 1, QTextStream::FieldAlignment::AlignRight);
   outStream << " kts\n";

   outStream << "      position   21.3618n 157.9534w   altitude ";
   OutputData(outStream, altitude_ft, 9, 1, QTextStream::FieldAlignment::AlignRight);
   outStream << " ft   speed ";
   OutputData(outStream, ktas, 7, 1, QTextStream::FieldAlignment::AlignRight);
   outStream << " kts\n";

   outStream << "      position   21.5348n 157.9534w   altitude ";
   OutputData(outStream, altitude_ft, 9, 1, QTextStream::FieldAlignment::AlignRight);
   outStream << " ft   speed ";
   OutputData(outStream, ktas, 7, 1, QTextStream::FieldAlignment::AlignRight);
   outStream << " kts\n";

   outStream << "   end_route\n";
   outStream << "\n";
   outStream << "   # Climb/dive uses on slope between points\n";
   outStream << "   follow_vertical_track\n";
   outStream << "\n";

   outStream << "end_platform\n";
   baseFile.close();
   }

   void ScriptGeneratorP6DOF::CreateEnvironmentFiles()
   {
   QString pathAddition = "/p6dof_types/environment/p6dof_environment.txt";
   QFile baseFile1(mVehicleDir.absolutePath() + pathAddition);
   if (!baseFile1.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }
   QTextStream outStream1(&baseFile1);
   outStream1.setRealNumberNotation(QTextStream::FixedNotation);

   CreateAutogenHeader(mVehiclePtr, outStream1);
   outStream1 << "# --------------------------------------------------------------------------------------------------\n";
   outStream1 << "# This provides a definition of various environmental components used by the P6DOF model.\n";
   outStream1 << "# It includes atmosphere, wind, terrain, and gravity.\n";
   outStream1 << "#\n";
   outStream1 << "# Atmosphere provides data pertaining to air density, pressure, temperature, etc.\n";
   outStream1 << "#\n";
   outStream1 << "# Wind provides a means for location-dependent wind, providing a means for wind to vary as\n";
   outStream1 << "# a function of lat/lon and/or altitude.\n";
   outStream1 << "#\n";
   outStream1 << "# Terrain provides the altitude of terrain and/or other objects on which aircraft may land\n";
   outStream1 << "# or with which an aircraft may collide. It also provides a surface normal and friction\n";
   outStream1 << "# modifier. Moving objects(such as the deck of an aircraft carrier) will also provide\n";
   outStream1 << "# their velocity.\n";
   outStream1 << "#\n";
   outStream1 << "# Gravity provides a means for detailed gravity models as a function of lat/lon and altitude.\n";
   outStream1 << "# --------------------------------------------------------------------------------------------------\n";
   outStream1 << "\n";
   outStream1 << "p6dof_object_types\n";
   outStream1 << "\n";
   outStream1 << "   p6dof_atmosphere         atm_config.txt\n";
   outStream1 << "   p6dof_wind               wind_config.txt\n";
   outStream1 << "   p6dof_terrain            terrain_config.txt\n";
   outStream1 << "   p6dof_gravity            gravity_config.txt\n";
   outStream1 << "   p6dof_integrators        integrators_config.txt\n";
   outStream1 << "\n";
   outStream1 << "end_p6dof_object_types\n";
   outStream1 << "\n";

   baseFile1.close();

   // ...................................

   pathAddition = "/p6dof_types/environment/integrators_config.txt";
   QFile baseFile2(mVehicleDir.absolutePath() + pathAddition);
   if (!baseFile2.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }
   QTextStream outStream2(&baseFile2);
   outStream2.setRealNumberNotation(QTextStream::FixedNotation);

   CreateAutogenHeader(mVehiclePtr, outStream2);
   outStream2 << "p6dof_integrators\n";
   outStream2 << "\n";
   outStream2 << "   create_integrator  STANDARD_P6DOF_INTEGRATOR\n";
   outStream2 << "\n";
   outStream2 << "end_p6dof_integrators\n";
   outStream2 << "\n";

   baseFile2.close();

   // ...................................

   pathAddition = "/p6dof_types/environment/terrain_config.txt";
   QFile baseFile3(mVehicleDir.absolutePath() + pathAddition);
   if (!baseFile3.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }
   QTextStream outStream3(&baseFile3);
   outStream3.setRealNumberNotation(QTextStream::FixedNotation);

   CreateAutogenHeader(mVehiclePtr, outStream3);
   outStream3 << "p6dof_terrain\n";
   outStream3 << "\n";
   outStream3 << "   region\n";
   outStream3 << "      # Oahu, HI\n";
   outStream3 << "\n";
   outStream3 << "      location\n";
   outStream3 << "         # Honolulu Intl, PHNL\n";
   outStream3 << "         lat    21.3178275\n";
   outStream3 << "         lon  -157.92026310\n";
   outStream3 << "         alt_ft  5.0\n";
   outStream3 << "      end_location\n";
   outStream3 << "\n";
   outStream3 << "      location\n";
   outStream3 << "         # Kalaeloa, PHJR\n";
   outStream3 << "         lat    21.3073539\n";
   outStream3 << "         lon  -158.0703017\n";
   outStream3 << "         alt_ft 30.0\n";
   outStream3 << "      end_location\n";
   outStream3 << "\n";
   outStream3 << "      location\n";
   outStream3 << "         # Kaneohe Bay MCAS, PHNG\n";
   outStream3 << "         lat    21.4504556\n";
   outStream3 << "         lon  -157.7679486\n";
   outStream3 << "         alt_ft 24.0\n";
   outStream3 << "      end_location\n";
   outStream3 << "\n";
   outStream3 << "      location\n";
   outStream3 << "         # Wheeler AFB, HHI/PHHI\n";
   outStream3 << "         lat     21.4814475\n";
   outStream3 << "         lon   -158.0378379\n";
   outStream3 << "         alt_ft 843.0\n";
   outStream3 << "      end_location\n";
   outStream3 << "\n";
   outStream3 << "      location\n";
   outStream3 << "         # Dillingham, PHDH\n";
   outStream3 << "         lat    21.5794736\n";
   outStream3 << "         lon  -158.1972814\n";
   outStream3 << "         alt_ft 14.0\n";
   outStream3 << "      end_location\n";
   outStream3 << "\n";
   outStream3 << "   end_region\n";
   outStream3 << "\n";
   outStream3 << "end_p6dof_terrain\n";
   outStream3 << "\n";

   baseFile3.close();

   // ...................................

   pathAddition = "/p6dof_types/environment/atm_config.txt";
   QFile baseFile4(mVehicleDir.absolutePath() + pathAddition);
   if (!baseFile4.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }
   QTextStream outStream4(&baseFile4);
   outStream4.setRealNumberNotation(QTextStream::FixedNotation);

   CreateAutogenHeader(mVehiclePtr, outStream4);
   outStream4 << "p6dof_atmosphere\n";
   outStream4 << "density_altitude_table\n";
   outStream4 << "# altitude_ft  density_slugs_ft3\n";
   outStream4 << "0             0.002377170\n";
   outStream4 << "1000          0.002308390\n";
   outStream4 << "2000          0.002241140\n";
   outStream4 << "3000          0.002175390\n";
   outStream4 << "4000          0.002111140\n";
   outStream4 << "5000          0.002048340\n";
   outStream4 << "6000          0.001986980\n";
   outStream4 << "7000          0.001927040\n";
   outStream4 << "8000          0.001868500\n";
   outStream4 << "9000          0.001811320\n";
   outStream4 << "10000         0.001755490\n";
   outStream4 << "11000         0.001700990\n";
   outStream4 << "12000         0.001647790\n";
   outStream4 << "13000         0.001595880\n";
   outStream4 << "14000         0.001545220\n";
   outStream4 << "15000         0.001495810\n";
   outStream4 << "16000         0.001447610\n";
   outStream4 << "17000         0.001400610\n";
   outStream4 << "18000         0.001354790\n";
   outStream4 << "19000         0.001310120\n";
   outStream4 << "20000         0.001266590\n";
   outStream4 << "21000         0.001224170\n";
   outStream4 << "22000         0.001182850\n";
   outStream4 << "23000         0.001142600\n";
   outStream4 << "24000         0.001103410\n";
   outStream4 << "25000         0.001065260\n";
   outStream4 << "26000         0.001028120\n";
   outStream4 << "27000         0.000991984\n";
   outStream4 << "28000         0.000956827\n";
   outStream4 << "29000         0.000922631\n";
   outStream4 << "30000         0.000889378\n";
   outStream4 << "31000         0.000857050\n";
   outStream4 << "32000         0.000825628\n";
   outStream4 << "33000         0.000795096\n";
   outStream4 << "34000         0.000765434\n";
   outStream4 << "35000         0.000736627\n";
   outStream4 << "36000         0.000708657\n";
   outStream4 << "37000         0.000675954\n";
   outStream4 << "38000         0.000644234\n";
   outStream4 << "39000         0.000614002\n";
   outStream4 << "40000         0.000585189\n";
   outStream4 << "41000         0.000557728\n";
   outStream4 << "42000         0.000531556\n";
   outStream4 << "43000         0.000506612\n";
   outStream4 << "44000         0.000482838\n";
   outStream4 << "45000         0.000460180\n";
   outStream4 << "46000         0.000438586\n";
   outStream4 << "47000         0.000418004\n";
   outStream4 << "48000         0.000398389\n";
   outStream4 << "49000         0.000379694\n";
   outStream4 << "50000         0.000361876\n";
   outStream4 << "51000         0.000344894\n";
   outStream4 << "52000         0.000328709\n";
   outStream4 << "53000         0.000313284\n";
   outStream4 << "54000         0.000298583\n";
   outStream4 << "55000         0.000284571\n";
   outStream4 << "56000         0.000271217\n";
   outStream4 << "57000         0.000258490\n";
   outStream4 << "58000         0.000246360\n";
   outStream4 << "59000         0.000234799\n";
   outStream4 << "60000         0.000223781\n";
   outStream4 << "61000         0.000213279\n";
   outStream4 << "62000         0.000203271\n";
   outStream4 << "63000         0.000193732\n";
   outStream4 << "64000         0.000184641\n";
   outStream4 << "65000         0.000175976\n";
   outStream4 << "66000         0.000167629\n";
   outStream4 << "67000         0.000159548\n";
   outStream4 << "68000         0.000151867\n";
   outStream4 << "69000         0.000144566\n";
   outStream4 << "70000         0.000137625\n";
   outStream4 << "71000         0.000131026\n";
   outStream4 << "72000         0.000124753\n";
   outStream4 << "73000         0.000118788\n";
   outStream4 << "74000         0.000113116\n";
   outStream4 << "75000         0.000107722\n";
   outStream4 << "76000         0.000102592\n";
   outStream4 << "77000         0.000097713\n";
   outStream4 << "78000         0.000093073\n";
   outStream4 << "79000         0.000088658\n";
   outStream4 << "80000         0.000084459\n";
   outStream4 << "81000         0.000080464\n";
   outStream4 << "82000         0.000076663\n";
   outStream4 << "83000         0.000073047\n";
   outStream4 << "84000         0.000069605\n";
   outStream4 << "85000         0.000066331\n";
   outStream4 << "86000         0.000063214\n";
   outStream4 << "87000         0.000060248\n";
   outStream4 << "88000         0.000057425\n";
   outStream4 << "89000         0.000054738\n";
   outStream4 << "90000         0.000052179\n";
   outStream4 << "91000         0.000049744\n";
   outStream4 << "92000         0.000047425\n";
   outStream4 << "93000         0.000045218\n";
   outStream4 << "94000         0.000043116\n";
   outStream4 << "95000         0.000041114\n";
   outStream4 << "96000         0.000039208\n";
   outStream4 << "97000         0.000037392\n";
   outStream4 << "98000         0.000035663\n";
   outStream4 << "99000         0.000034016\n";
   outStream4 << "100000        0.000032447\n";
   outStream4 << "101000        0.000030953\n";
   outStream4 << "102000        0.000029529\n";
   outStream4 << "103000        0.000028172\n";
   outStream4 << "104000        0.000026880\n";
   outStream4 << "105000        0.000025647\n";
   outStream4 << "106000        0.000024416\n";
   outStream4 << "107000        0.000023249\n";
   outStream4 << "108000        0.000022141\n";
   outStream4 << "109000        0.000021090\n";
   outStream4 << "110000        0.000020093\n";
   outStream4 << "111000        0.000019146\n";
   outStream4 << "112000        0.000018246\n";
   outStream4 << "113000        0.000017392\n";
   outStream4 << "114000        0.000016581\n";
   outStream4 << "115000        0.000015811\n";
   outStream4 << "116000        0.000015079\n";
   outStream4 << "117000        0.000014383\n";
   outStream4 << "118000        0.000013722\n";
   outStream4 << "119000        0.000013093\n";
   outStream4 << "120000        0.000012495\n";
   outStream4 << "121000        0.000011926\n";
   outStream4 << "122000        0.000011385\n";
   outStream4 << "123000        0.000010871\n";
   outStream4 << "124000        0.000010381\n";
   outStream4 << "125000        0.000009915\n";
   outStream4 << "126000        0.000009472\n";
   outStream4 << "127000        0.000009049\n";
   outStream4 << "128000        0.000008647\n";
   outStream4 << "129000        0.000008264\n";
   outStream4 << "130000        0.000007899\n";
   outStream4 << "131000        0.000007552\n";
   outStream4 << "132000        0.000007221\n";
   outStream4 << "133000        0.000006905\n";
   outStream4 << "134000        0.000006604\n";
   outStream4 << "135000        0.000006318\n";
   outStream4 << "136000        0.000006044\n";
   outStream4 << "137000        0.000005784\n";
   outStream4 << "138000        0.000005535\n";
   outStream4 << "139000        0.000005298\n";
   outStream4 << "140000        0.000005072\n";
   outStream4 << "141000        0.000004856\n";
   outStream4 << "142000        0.000004650\n";
   outStream4 << "143000        0.000004453\n";
   outStream4 << "144000        0.000004265\n";
   outStream4 << "145000        0.000004086\n";
   outStream4 << "146000        0.000003915\n";
   outStream4 << "147000        0.000003751\n";
   outStream4 << "148000        0.000003595\n";
   outStream4 << "149000        0.000003446\n";
   outStream4 << "150000        0.000003303\n";
   outStream4 << "151000        0.000003167\n";
   outStream4 << "152000        0.000003037\n";
   outStream4 << "153000        0.000002912\n";
   outStream4 << "154000        0.000002793\n";
   outStream4 << "155000        0.000002686\n";
   outStream4 << "156000        0.000002585\n";
   outStream4 << "157000        0.000002487\n";
   outStream4 << "158000        0.000002393\n";
   outStream4 << "159000        0.000002303\n";
   outStream4 << "160000        0.000002216\n";
   outStream4 << "161000        0.000002132\n";
   outStream4 << "162000        0.000002052\n";
   outStream4 << "163000        0.000001975\n";
   outStream4 << "164000        0.000001900\n";
   outStream4 << "165000        0.000001828\n";
   outStream4 << "166000        0.000001759\n";
   outStream4 << "167000        0.000001693\n";
   outStream4 << "168000        0.000001632\n";
   outStream4 << "169000        0.000001576\n";
   outStream4 << "170000        0.000001521\n";
   outStream4 << "171000        0.000001467\n";
   outStream4 << "172000        0.000001416\n";
   outStream4 << "173000        0.000001366\n";
   outStream4 << "174000        0.000001317\n";
   outStream4 << "175000        0.000001271\n";
   outStream4 << "176000        0.000001225\n";
   outStream4 << "177000        0.000001182\n";
   outStream4 << "178000        0.000001139\n";
   outStream4 << "179000        0.000001098\n";
   outStream4 << "180000        0.000001059\n";
   outStream4 << "181000        0.000001020\n";
   outStream4 << "182000        0.000000983\n";
   outStream4 << "183000        0.000000948\n";
   outStream4 << "184000        0.000000913\n";
   outStream4 << "185000        0.000000879\n";
   outStream4 << "186000        0.000000847\n";
   outStream4 << "187000        0.000000816\n";
   outStream4 << "188000        0.000000786\n";
   outStream4 << "189000        0.000000756\n";
   outStream4 << "190000        0.000000728\n";
   outStream4 << "191000        0.000000701\n";
   outStream4 << "192000        0.000000675\n";
   outStream4 << "193000        0.000000649\n";
   outStream4 << "194000        0.000000625\n";
   outStream4 << "195000        0.000000601\n";
   outStream4 << "196000        0.000000578\n";
   outStream4 << "197000        0.000000556\n";
   outStream4 << "198000        0.000000535\n";
   outStream4 << "199000        0.000000514\n";
   outStream4 << "200000        0.000000495\n";
   outStream4 << "201000        0.000000475\n";
   outStream4 << "202000        0.000000457\n";
   outStream4 << "203000        0.000000439\n";
   outStream4 << "204000        0.000000422\n";
   outStream4 << "205000        0.000000405\n";
   outStream4 << "206000        0.000000390\n";
   outStream4 << "207000        0.000000374\n";
   outStream4 << "208000        0.000000359\n";
   outStream4 << "209000        0.000000345\n";
   outStream4 << "210000        0.000000331\n";
   outStream4 << "211000        0.000000318\n";
   outStream4 << "212000        0.000000305\n";
   outStream4 << "213000        0.000000293\n";
   outStream4 << "214000        0.000000281\n";
   outStream4 << "215000        0.000000270\n";
   outStream4 << "216000        0.000000259\n";
   outStream4 << "217000        0.000000248\n";
   outStream4 << "218000        0.000000238\n";
   outStream4 << "219000        0.000000228\n";
   outStream4 << "220000        0.000000219\n";
   outStream4 << "221000        0.000000209\n";
   outStream4 << "222000        0.000000201\n";
   outStream4 << "223000        0.000000192\n";
   outStream4 << "224000        0.000000184\n";
   outStream4 << "225000        0.000000176\n";
   outStream4 << "226000        0.000000169\n";
   outStream4 << "227000        0.000000162\n";
   outStream4 << "228000        0.000000155\n";
   outStream4 << "229000        0.000000148\n";
   outStream4 << "230000        0.000000142\n";
   outStream4 << "231000        0.000000136\n";
   outStream4 << "232000        0.000000130\n";
   outStream4 << "233000        0.000000124\n";
   outStream4 << "234000        0.000000119\n";
   outStream4 << "235000        0.000000113\n";
   outStream4 << "236000        0.000000108\n";
   outStream4 << "237000        0.000000103\n";
   outStream4 << "238000        0.000000099\n";
   outStream4 << "239000        0.000000094\n";
   outStream4 << "240000        0.000000090\n";
   outStream4 << "241000        0.000000086\n";
   outStream4 << "242000        0.000000082\n";
   outStream4 << "243000        0.000000078\n";
   outStream4 << "244000        0.000000075\n";
   outStream4 << "245000        0.000000071\n";
   outStream4 << "246000        0.000000068\n";
   outStream4 << "247000        0.000000065\n";
   outStream4 << "248000        0.000000062\n";
   outStream4 << "249000        0.000000059\n";
   outStream4 << "250000        0.000000056\n";
   outStream4 << "251000        0.000000053\n";
   outStream4 << "252000        0.000000051\n";
   outStream4 << "253000        0.000000049\n";
   outStream4 << "254000        0.000000046\n";
   outStream4 << "255000        0.000000044\n";
   outStream4 << "256000        0.000000042\n";
   outStream4 << "257000        0.000000040\n";
   outStream4 << "258000        0.000000038\n";
   outStream4 << "259000        0.000000036\n";
   outStream4 << "260000        0.000000034\n";
   outStream4 << "261000        0.000000033\n";
   outStream4 << "262000        0.000000031\n";
   outStream4 << "263000        0.000000030\n";
   outStream4 << "264000        0.000000028\n";
   outStream4 << "265000        0.000000027\n";
   outStream4 << "266000        0.000000026\n";
   outStream4 << "267000        0.000000024\n";
   outStream4 << "268000        0.000000023\n";
   outStream4 << "269000        0.000000022\n";
   outStream4 << "270000        0.000000021\n";
   outStream4 << "271000        0.000000020\n";
   outStream4 << "272000        0.000000019\n";
   outStream4 << "273000        0.000000018\n";
   outStream4 << "274000        0.000000017\n";
   outStream4 << "275000        0.000000016\n";
   outStream4 << "276000        0.000000015\n";
   outStream4 << "277000        0.000000015\n";
   outStream4 << "278000        0.000000014\n";
   outStream4 << "279000        0.000000013\n";
   outStream4 << "280000        0.000000012\n";
   outStream4 << "281000        0.000000012\n";
   outStream4 << "282000        0.000000011\n";
   outStream4 << "400000        0.000000000\n";
   outStream4 << "end_density_altitude_table\n";
   outStream4 << "\n";
   outStream4 << "pressure_altitude_table\n";
   outStream4 << "# altitude_ft  pressure_lbs_ft2\n";
   outStream4 << "0           2116.2300\n";
   outStream4 << "1000        2040.8600\n";
   outStream4 << "2000        1967.6900\n";
   outStream4 << "3000        1896.6500\n";
   outStream4 << "4000        1827.7100\n";
   outStream4 << "5000        1760.8000\n";
   outStream4 << "6000        1695.9000\n";
   outStream4 << "7000        1632.9400\n";
   outStream4 << "8000        1571.9000\n";
   outStream4 << "9000        1512.7100\n";
   outStream4 << "10000       1455.3400\n";
   outStream4 << "11000       1399.7400\n";
   outStream4 << "12000       1345.8800\n";
   outStream4 << "13000       1293.7100\n";
   outStream4 << "14000       1243.1900\n";
   outStream4 << "15000       1194.2800\n";
   outStream4 << "16000       1146.9300\n";
   outStream4 << "17000       1101.1200\n";
   outStream4 << "18000       1056.8100\n";
   outStream4 << "19000       1013.9400\n";
   outStream4 << "20000       972.4990\n";
   outStream4 << "21000       932.4380\n";
   outStream4 << "22000       893.7240\n";
   outStream4 << "23000       856.3210\n";
   outStream4 << "24000       820.1960\n";
   outStream4 << "25000       785.3160\n";
   outStream4 << "26000       751.6460\n";
   outStream4 << "27000       719.1550\n";
   outStream4 << "28000       687.8100\n";
   outStream4 << "29000       657.5820\n";
   outStream4 << "30000       628.4380\n";
   outStream4 << "31000       600.3480\n";
   outStream4 << "32000       573.2850\n";
   outStream4 << "33000       547.2170\n";
   outStream4 << "34000       522.1180\n";
   outStream4 << "35000       497.9590\n";
   outStream4 << "36000       474.7140\n";
   outStream4 << "37000       452.4380\n";
   outStream4 << "38000       431.2070\n";
   outStream4 << "39000       410.9720\n";
   outStream4 << "40000       391.6860\n";
   outStream4 << "41000       373.3050\n";
   outStream4 << "42000       355.7870\n";
   outStream4 << "43000       339.0920\n";
   outStream4 << "44000       323.1790\n";
   outStream4 << "45000       308.0130\n";
   outStream4 << "46000       293.5590\n";
   outStream4 << "47000       279.7840\n";
   outStream4 << "48000       266.6540\n";
   outStream4 << "49000       254.1410\n";
   outStream4 << "50000       242.2150\n";
   outStream4 << "51000       230.8490\n";
   outStream4 << "52000       220.0160\n";
   outStream4 << "53000       209.6910\n";
   outStream4 << "54000       199.8510\n";
   outStream4 << "55000       190.4730\n";
   outStream4 << "56000       181.5340\n";
   outStream4 << "57000       173.0160\n";
   outStream4 << "58000       164.8970\n";
   outStream4 << "59000       157.1590\n";
   outStream4 << "60000       149.7840\n";
   outStream4 << "61000       142.7550\n";
   outStream4 << "62000       136.0560\n";
   outStream4 << "63000       129.6710\n";
   outStream4 << "64000       123.5860\n";
   outStream4 << "65000       117.7870\n";
   outStream4 << "66000       112.2600\n";
   outStream4 << "67000       106.9980\n";
   outStream4 << "68000       101.9900\n";
   outStream4 << "69000       97.2230\n";
   outStream4 << "70000       92.6848\n";
   outStream4 << "71000       88.3644\n";
   outStream4 << "72000       84.2510\n";
   outStream4 << "73000       80.3344\n";
   outStream4 << "74000       76.6049\n";
   outStream4 << "75000       73.0534\n";
   outStream4 << "76000       69.6712\n";
   outStream4 << "77000       66.4499\n";
   outStream4 << "78000       63.3817\n";
   outStream4 << "79000       60.4591\n";
   outStream4 << "80000       57.6751\n";
   outStream4 << "81000       55.0228\n";
   outStream4 << "82000       52.4959\n";
   outStream4 << "83000       50.0882\n";
   outStream4 << "84000       47.7941\n";
   outStream4 << "85000       45.6080\n";
   outStream4 << "86000       43.5246\n";
   outStream4 << "87000       41.5391\n";
   outStream4 << "88000       39.6467\n";
   outStream4 << "89000       37.8429\n";
   outStream4 << "90000       36.1234\n";
   outStream4 << "91000       34.4843\n";
   outStream4 << "92000       32.9216\n";
   outStream4 << "93000       31.4317\n";
   outStream4 << "94000       30.0111\n";
   outStream4 << "95000       28.6565\n";
   outStream4 << "96000       27.3648\n";
   outStream4 << "97000       26.1329\n";
   outStream4 << "98000       24.9580\n";
   outStream4 << "99000       23.8374\n";
   outStream4 << "100000      22.7686\n";
   outStream4 << "101000      21.7490\n";
   outStream4 << "102000      20.7763\n";
   outStream4 << "103000      19.8483\n";
   outStream4 << "104000      18.9630\n";
   outStream4 << "105000      18.1182\n";
   outStream4 << "106000      17.3131\n";
   outStream4 << "107000      16.5466\n";
   outStream4 << "108000      15.8166\n";
   outStream4 << "109000      15.1214\n";
   outStream4 << "110000      14.4591\n";
   outStream4 << "111000      13.8281\n";
   outStream4 << "112000      13.2267\n";
   outStream4 << "113000      12.6536\n";
   outStream4 << "114000      12.1072\n";
   outStream4 << "115000      11.5863\n";
   outStream4 << "116000      11.0895\n";
   outStream4 << "117000      10.6157\n";
   outStream4 << "118000      10.1638\n";
   outStream4 << "119000      9.7325\n";
   outStream4 << "120000      9.3210\n";
   outStream4 << "121000      8.9283\n";
   outStream4 << "122000      8.5534\n";
   outStream4 << "123000      8.1955\n";
   outStream4 << "124000      7.8537\n";
   outStream4 << "125000      7.5273\n";
   outStream4 << "126000      7.2155\n";
   outStream4 << "127000      6.9177\n";
   outStream4 << "128000      6.6331\n";
   outStream4 << "129000      6.3611\n";
   outStream4 << "130000      6.1011\n";
   outStream4 << "131000      5.8526\n";
   outStream4 << "132000      5.6151\n";
   outStream4 << "133000      5.3879\n";
   outStream4 << "134000      5.1706\n";
   outStream4 << "135000      4.9628\n";
   outStream4 << "136000      4.7640\n";
   outStream4 << "137000      4.5738\n";
   outStream4 << "138000      4.3917\n";
   outStream4 << "139000      4.2175\n";
   outStream4 << "140000      4.0508\n";
   outStream4 << "141000      3.8911\n";
   outStream4 << "142000      3.7382\n";
   outStream4 << "143000      3.5919\n";
   outStream4 << "144000      3.4516\n";
   outStream4 << "145000      3.3173\n";
   outStream4 << "146000      3.1887\n";
   outStream4 << "147000      3.0654\n";
   outStream4 << "148000      2.9472\n";
   outStream4 << "149000      2.8340\n";
   outStream4 << "150000      2.7255\n";
   outStream4 << "151000      2.6214\n";
   outStream4 << "152000      2.5216\n";
   outStream4 << "153000      2.4260\n";
   outStream4 << "154000      2.3342\n";
   outStream4 << "155000      2.2461\n";
   outStream4 << "156000      2.1613\n";
   outStream4 << "157000      2.0797\n";
   outStream4 << "158000      2.0012\n";
   outStream4 << "159000      1.9257\n";
   outStream4 << "160000      1.8530\n";
   outStream4 << "161000      1.7831\n";
   outStream4 << "162000      1.7158\n";
   outStream4 << "163000      1.6510\n";
   outStream4 << "164000      1.5887\n";
   outStream4 << "165000      1.5288\n";
   outStream4 << "166000      1.4711\n";
   outStream4 << "167000      1.4155\n";
   outStream4 << "168000      1.3621\n";
   outStream4 << "169000      1.3105\n";
   outStream4 << "170000      1.2607\n";
   outStream4 << "171000      1.2126\n";
   outStream4 << "172000      1.1663\n";
   outStream4 << "173000      1.1215\n";
   outStream4 << "174000      1.0784\n";
   outStream4 << "175000      1.0367\n";
   outStream4 << "176000      0.9966\n";
   outStream4 << "177000      0.9579\n";
   outStream4 << "178000      0.9206\n";
   outStream4 << "179000      0.8846\n";
   outStream4 << "180000      0.8499\n";
   outStream4 << "181000      0.8164\n";
   outStream4 << "182000      0.7842\n";
   outStream4 << "183000      0.7531\n";
   outStream4 << "184000      0.7232\n";
   outStream4 << "185000      0.6944\n";
   outStream4 << "186000      0.6666\n";
   outStream4 << "187000      0.6399\n";
   outStream4 << "188000      0.6141\n";
   outStream4 << "189000      0.5893\n";
   outStream4 << "190000      0.5654\n";
   outStream4 << "191000      0.5424\n";
   outStream4 << "192000      0.5203\n";
   outStream4 << "193000      0.4990\n";
   outStream4 << "194000      0.4785\n";
   outStream4 << "195000      0.4588\n";
   outStream4 << "196000      0.4398\n";
   outStream4 << "197000      0.4216\n";
   outStream4 << "198000      0.4040\n";
   outStream4 << "199000      0.3872\n";
   outStream4 << "200000      0.3709\n";
   outStream4 << "201000      0.3553\n";
   outStream4 << "202000      0.3403\n";
   outStream4 << "203000      0.3259\n";
   outStream4 << "204000      0.3121\n";
   outStream4 << "205000      0.2988\n";
   outStream4 << "206000      0.2860\n";
   outStream4 << "207000      0.2737\n";
   outStream4 << "208000      0.2619\n";
   outStream4 << "209000      0.2506\n";
   outStream4 << "210000      0.2397\n";
   outStream4 << "211000      0.2293\n";
   outStream4 << "212000      0.2192\n";
   outStream4 << "213000      0.2096\n";
   outStream4 << "214000      0.2004\n";
   outStream4 << "215000      0.1915\n";
   outStream4 << "216000      0.1830\n";
   outStream4 << "217000      0.1749\n";
   outStream4 << "218000      0.1671\n";
   outStream4 << "219000      0.1596\n";
   outStream4 << "220000      0.1524\n";
   outStream4 << "221000      0.1455\n";
   outStream4 << "222000      0.1389\n";
   outStream4 << "223000      0.1326\n";
   outStream4 << "224000      0.1265\n";
   outStream4 << "225000      0.1207\n";
   outStream4 << "226000      0.1152\n";
   outStream4 << "227000      0.1099\n";
   outStream4 << "228000      0.1048\n";
   outStream4 << "229000      0.0999\n";
   outStream4 << "230000      0.0952\n";
   outStream4 << "231000      0.0908\n";
   outStream4 << "232000      0.0865\n";
   outStream4 << "233000      0.0824\n";
   outStream4 << "234000      0.0785\n";
   outStream4 << "235000      0.0748\n";
   outStream4 << "236000      0.0712\n";
   outStream4 << "237000      0.0678\n";
   outStream4 << "238000      0.0645\n";
   outStream4 << "239000      0.0614\n";
   outStream4 << "240000      0.0585\n";
   outStream4 << "241000      0.0556\n";
   outStream4 << "242000      0.0529\n";
   outStream4 << "243000      0.0504\n";
   outStream4 << "244000      0.0479\n";
   outStream4 << "245000      0.0456\n";
   outStream4 << "246000      0.0433\n";
   outStream4 << "247000      0.0412\n";
   outStream4 << "248000      0.0392\n";
   outStream4 << "249000      0.0372\n";
   outStream4 << "250000      0.0354\n";
   outStream4 << "251000      0.0336\n";
   outStream4 << "252000      0.0319\n";
   outStream4 << "253000      0.0303\n";
   outStream4 << "254000      0.0288\n";
   outStream4 << "255000      0.0274\n";
   outStream4 << "256000      0.0260\n";
   outStream4 << "257000      0.0247\n";
   outStream4 << "258000      0.0234\n";
   outStream4 << "259000      0.0222\n";
   outStream4 << "260000      0.0211\n";
   outStream4 << "261000      0.0200\n";
   outStream4 << "262000      0.0190\n";
   outStream4 << "263000      0.0180\n";
   outStream4 << "264000      0.0171\n";
   outStream4 << "265000      0.0162\n";
   outStream4 << "266000      0.0153\n";
   outStream4 << "267000      0.0145\n";
   outStream4 << "268000      0.0138\n";
   outStream4 << "269000      0.0131\n";
   outStream4 << "270000      0.0124\n";
   outStream4 << "271000      0.0117\n";
   outStream4 << "272000      0.0111\n";
   outStream4 << "273000      0.0105\n";
   outStream4 << "274000      0.0099\n";
   outStream4 << "275000      0.0094\n";
   outStream4 << "276000      0.0089\n";
   outStream4 << "277000      0.0084\n";
   outStream4 << "278000      0.0080\n";
   outStream4 << "279000      0.0075\n";
   outStream4 << "280000      0.0071\n";
   outStream4 << "281000      0.0067\n";
   outStream4 << "282000      0.0064\n";
   outStream4 << "400000      0.0000\n";
   outStream4 << "end_pressure_altitude_table\n";
   outStream4 << "\n";
   outStream4 << "temperature_altitude_table\n";
   outStream4 << "# altitude_ft  rankine_deg\n";
   outStream4 << "0           518.67\n";
   outStream4 << "1000        515.10\n";
   outStream4 << "2000        511.54\n";
   outStream4 << "3000        507.97\n";
   outStream4 << "4000        504.41\n";
   outStream4 << "5000        500.84\n";
   outStream4 << "6000        497.27\n";
   outStream4 << "7000        493.71\n";
   outStream4 << "8000        490.14\n";
   outStream4 << "9000        486.58\n";
   outStream4 << "10000       483.01\n";
   outStream4 << "11000       479.44\n";
   outStream4 << "12000       475.88\n";
   outStream4 << "13000       472.31\n";
   outStream4 << "14000       468.74\n";
   outStream4 << "15000       465.18\n";
   outStream4 << "16000       461.61\n";
   outStream4 << "17000       458.05\n";
   outStream4 << "18000       454.48\n";
   outStream4 << "19000       450.91\n";
   outStream4 << "20000       447.35\n";
   outStream4 << "21000       443.78\n";
   outStream4 << "22000       440.21\n";
   outStream4 << "23000       436.65\n";
   outStream4 << "24000       433.08\n";
   outStream4 << "25000       429.52\n";
   outStream4 << "26000       425.95\n";
   outStream4 << "27000       422.38\n";
   outStream4 << "28000       418.82\n";
   outStream4 << "29000       415.25\n";
   outStream4 << "30000       411.69\n";
   outStream4 << "31000       408.12\n";
   outStream4 << "32000       404.55\n";
   outStream4 << "33000       400.99\n";
   outStream4 << "34000       397.42\n";
   outStream4 << "35000       393.85\n";
   outStream4 << "36000       390.29\n";
   outStream4 << "37000       389.97\n";
   outStream4 << "38000       389.97\n";
   outStream4 << "39000       389.97\n";
   outStream4 << "40000       389.97\n";
   outStream4 << "41000       389.97\n";
   outStream4 << "42000       389.97\n";
   outStream4 << "43000       389.97\n";
   outStream4 << "44000       389.97\n";
   outStream4 << "45000       389.97\n";
   outStream4 << "46000       389.97\n";
   outStream4 << "47000       389.97\n";
   outStream4 << "48000       389.97\n";
   outStream4 << "49000       389.97\n";
   outStream4 << "50000       389.97\n";
   outStream4 << "51000       389.97\n";
   outStream4 << "52000       389.97\n";
   outStream4 << "53000       389.97\n";
   outStream4 << "54000       389.97\n";
   outStream4 << "55000       389.97\n";
   outStream4 << "56000       389.97\n";
   outStream4 << "57000       389.97\n";
   outStream4 << "58000       389.97\n";
   outStream4 << "59000       389.97\n";
   outStream4 << "60000       389.97\n";
   outStream4 << "61000       389.97\n";
   outStream4 << "62000       389.97\n";
   outStream4 << "63000       389.97\n";
   outStream4 << "64000       389.97\n";
   outStream4 << "65000       389.97\n";
   outStream4 << "66000       390.18\n";
   outStream4 << "67000       390.73\n";
   outStream4 << "68000       391.28\n";
   outStream4 << "69000       391.83\n";
   outStream4 << "70000       392.38\n";
   outStream4 << "71000       392.92\n";
   outStream4 << "72000       393.47\n";
   outStream4 << "73000       394.02\n";
   outStream4 << "74000       394.57\n";
   outStream4 << "75000       395.12\n";
   outStream4 << "76000       395.67\n";
   outStream4 << "77000       396.22\n";
   outStream4 << "78000       396.76\n";
   outStream4 << "79000       397.31\n";
   outStream4 << "80000       397.86\n";
   outStream4 << "81000       398.41\n";
   outStream4 << "82000       398.96\n";
   outStream4 << "83000       399.51\n";
   outStream4 << "84000       400.06\n";
   outStream4 << "85000       400.60\n";
   outStream4 << "86000       401.15\n";
   outStream4 << "87000       401.70\n";
   outStream4 << "88000       402.25\n";
   outStream4 << "89000       402.80\n";
   outStream4 << "90000       403.35\n";
   outStream4 << "91000       403.90\n";
   outStream4 << "92000       404.45\n";
   outStream4 << "93000       404.99\n";
   outStream4 << "94000       405.54\n";
   outStream4 << "95000       406.09\n";
   outStream4 << "96000       406.64\n";
   outStream4 << "97000       407.19\n";
   outStream4 << "98000       407.74\n";
   outStream4 << "99000       408.29\n";
   outStream4 << "100000      408.83\n";
   outStream4 << "101000      409.38\n";
   outStream4 << "102000      409.93\n";
   outStream4 << "103000      410.48\n";
   outStream4 << "104000      411.03\n";
   outStream4 << "105000      411.59\n";
   outStream4 << "106000      413.13\n";
   outStream4 << "107000      414.66\n";
   outStream4 << "108000      416.20\n";
   outStream4 << "109000      417.74\n";
   outStream4 << "110000      419.27\n";
   outStream4 << "111000      420.81\n";
   outStream4 << "112000      422.34\n";
   outStream4 << "113000      423.88\n";
   outStream4 << "114000      425.42\n";
   outStream4 << "115000      426.95\n";
   outStream4 << "116000      428.49\n";
   outStream4 << "117000      430.02\n";
   outStream4 << "118000      431.56\n";
   outStream4 << "119000      433.10\n";
   outStream4 << "120000      434.63\n";
   outStream4 << "121000      436.17\n";
   outStream4 << "122000      437.71\n";
   outStream4 << "123000      439.24\n";
   outStream4 << "124000      440.78\n";
   outStream4 << "125000      442.31\n";
   outStream4 << "126000      443.85\n";
   outStream4 << "127000      445.39\n";
   outStream4 << "128000      446.92\n";
   outStream4 << "129000      448.46\n";
   outStream4 << "130000      450.00\n";
   outStream4 << "131000      451.53\n";
   outStream4 << "132000      453.07\n";
   outStream4 << "133000      454.60\n";
   outStream4 << "134000      456.14\n";
   outStream4 << "135000      457.68\n";
   outStream4 << "136000      459.21\n";
   outStream4 << "137000      460.75\n";
   outStream4 << "138000      462.28\n";
   outStream4 << "139000      463.82\n";
   outStream4 << "140000      465.36\n";
   outStream4 << "141000      466.89\n";
   outStream4 << "142000      468.43\n";
   outStream4 << "143000      469.97\n";
   outStream4 << "144000      471.50\n";
   outStream4 << "145000      473.04\n";
   outStream4 << "146000      474.57\n";
   outStream4 << "147000      476.11\n";
   outStream4 << "148000      477.65\n";
   outStream4 << "149000      479.18\n";
   outStream4 << "150000      480.72\n";
   outStream4 << "151000      482.26\n";
   outStream4 << "152000      483.79\n";
   outStream4 << "153000      485.33\n";
   outStream4 << "154000      486.86\n";
   outStream4 << "155000      487.17\n";
   outStream4 << "156000      487.17\n";
   outStream4 << "157000      487.17\n";
   outStream4 << "158000      487.17\n";
   outStream4 << "159000      487.17\n";
   outStream4 << "160000      487.17\n";
   outStream4 << "161000      487.17\n";
   outStream4 << "162000      487.17\n";
   outStream4 << "163000      487.17\n";
   outStream4 << "164000      487.17\n";
   outStream4 << "165000      487.17\n";
   outStream4 << "166000      487.17\n";
   outStream4 << "167000      487.17\n";
   outStream4 << "168000      486.13\n";
   outStream4 << "169000      484.59\n";
   outStream4 << "170000      483.06\n";
   outStream4 << "171000      481.52\n";
   outStream4 << "172000      479.99\n";
   outStream4 << "173000      478.45\n";
   outStream4 << "174000      476.91\n";
   outStream4 << "175000      475.38\n";
   outStream4 << "176000      473.84\n";
   outStream4 << "177000      472.30\n";
   outStream4 << "178000      470.77\n";
   outStream4 << "179000      469.23\n";
   outStream4 << "180000      467.70\n";
   outStream4 << "181000      466.16\n";
   outStream4 << "182000      464.62\n";
   outStream4 << "183000      463.09\n";
   outStream4 << "184000      461.55\n";
   outStream4 << "185000      460.01\n";
   outStream4 << "186000      458.48\n";
   outStream4 << "187000      456.94\n";
   outStream4 << "188000      455.41\n";
   outStream4 << "189000      453.87\n";
   outStream4 << "190000      452.33\n";
   outStream4 << "191000      450.80\n";
   outStream4 << "192000      449.26\n";
   outStream4 << "193000      447.73\n";
   outStream4 << "194000      446.19\n";
   outStream4 << "195000      444.65\n";
   outStream4 << "196000      443.12\n";
   outStream4 << "197000      441.58\n";
   outStream4 << "198000      440.04\n";
   outStream4 << "199000      438.51\n";
   outStream4 << "200000      436.97\n";
   outStream4 << "201000      435.44\n";
   outStream4 << "202000      433.90\n";
   outStream4 << "203000      432.36\n";
   outStream4 << "204000      430.83\n";
   outStream4 << "205000      429.29\n";
   outStream4 << "206000      427.75\n";
   outStream4 << "207000      426.22\n";
   outStream4 << "208000      424.68\n";
   outStream4 << "209000      423.15\n";
   outStream4 << "210000      421.61\n";
   outStream4 << "211000      420.07\n";
   outStream4 << "212000      418.54\n";
   outStream4 << "213000      417.00\n";
   outStream4 << "214000      415.47\n";
   outStream4 << "215000      413.93\n";
   outStream4 << "216000      412.39\n";
   outStream4 << "217000      410.86\n";
   outStream4 << "218000      409.32\n";
   outStream4 << "219000      407.78\n";
   outStream4 << "220000      406.25\n";
   outStream4 << "221000      404.71\n";
   outStream4 << "222000      403.18\n";
   outStream4 << "223000      401.64\n";
   outStream4 << "224000      400.10\n";
   outStream4 << "225000      398.57\n";
   outStream4 << "226000      397.03\n";
   outStream4 << "227000      395.49\n";
   outStream4 << "228000      393.96\n";
   outStream4 << "229000      392.42\n";
   outStream4 << "230000      390.89\n";
   outStream4 << "231000      389.35\n";
   outStream4 << "232000      387.81\n";
   outStream4 << "233000      386.30\n";
   outStream4 << "234000      385.21\n";
   outStream4 << "235000      384.11\n";
   outStream4 << "236000      383.01\n";
   outStream4 << "237000      381.92\n";
   outStream4 << "238000      380.82\n";
   outStream4 << "239000      379.72\n";
   outStream4 << "240000      378.62\n";
   outStream4 << "241000      377.53\n";
   outStream4 << "242000      376.43\n";
   outStream4 << "243000      375.33\n";
   outStream4 << "244000      374.23\n";
   outStream4 << "245000      373.14\n";
   outStream4 << "246000      372.04\n";
   outStream4 << "247000      370.94\n";
   outStream4 << "248000      369.85\n";
   outStream4 << "249000      368.75\n";
   outStream4 << "250000      367.65\n";
   outStream4 << "251000      366.55\n";
   outStream4 << "252000      365.46\n";
   outStream4 << "253000      364.36\n";
   outStream4 << "254000      363.26\n";
   outStream4 << "255000      362.16\n";
   outStream4 << "256000      361.07\n";
   outStream4 << "257000      359.97\n";
   outStream4 << "258000      358.87\n";
   outStream4 << "259000      357.77\n";
   outStream4 << "260000      356.68\n";
   outStream4 << "261000      355.58\n";
   outStream4 << "262000      354.48\n";
   outStream4 << "263000      353.39\n";
   outStream4 << "264000      352.29\n";
   outStream4 << "265000      351.19\n";
   outStream4 << "266000      350.09\n";
   outStream4 << "267000      349.00\n";
   outStream4 << "268000      347.90\n";
   outStream4 << "269000      346.80\n";
   outStream4 << "270000      345.70\n";
   outStream4 << "271000      344.61\n";
   outStream4 << "272000      343.51\n";
   outStream4 << "273000      342.41\n";
   outStream4 << "274000      341.32\n";
   outStream4 << "275000      340.22\n";
   outStream4 << "276000      339.12\n";
   outStream4 << "277000      338.02\n";
   outStream4 << "278000      336.93\n";
   outStream4 << "279000      336.50\n";
   outStream4 << "280000      336.50\n";
   outStream4 << "281000      336.50\n";
   outStream4 << "282000      336.50\n";
   outStream4 << "400000      336.50\n";
   outStream4 << "end_temperature_altitude_table\n";
   outStream4 << "\n";
   outStream4 << "sonic_speed_altitude_table\n";
   outStream4 << "# altitude_ft  sonic_speed_ft_s\n";
   outStream4 << "0           1116.45\n";
   outStream4 << "1000        1112.61\n";
   outStream4 << "2000        1108.75\n";
   outStream4 << "3000        1104.88\n";
   outStream4 << "4000        1100.99\n";
   outStream4 << "5000        1097.09\n";
   outStream4 << "6000        1093.18\n";
   outStream4 << "7000        1089.25\n";
   outStream4 << "8000        1085.31\n";
   outStream4 << "9000        1081.36\n";
   outStream4 << "10000       1077.39\n";
   outStream4 << "11000       1073.40\n";
   outStream4 << "12000       1069.40\n";
   outStream4 << "13000       1065.39\n";
   outStream4 << "14000       1061.36\n";
   outStream4 << "15000       1057.31\n";
   outStream4 << "16000       1053.25\n";
   outStream4 << "17000       1049.18\n";
   outStream4 << "18000       1045.08\n";
   outStream4 << "19000       1040.97\n";
   outStream4 << "20000       1036.85\n";
   outStream4 << "21000       1032.71\n";
   outStream4 << "22000       1028.55\n";
   outStream4 << "23000       1024.38\n";
   outStream4 << "24000       1020.19\n";
   outStream4 << "25000       1015.98\n";
   outStream4 << "26000       1011.75\n";
   outStream4 << "27000       1007.51\n";
   outStream4 << "28000       1003.24\n";
   outStream4 << "29000       998.96\n";
   outStream4 << "30000       994.66\n";
   outStream4 << "31000       990.35\n";
   outStream4 << "32000       986.01\n";
   outStream4 << "33000       981.66\n";
   outStream4 << "34000       977.28\n";
   outStream4 << "35000       972.89\n";
   outStream4 << "36000       968.47\n";
   outStream4 << "37000       968.08\n";
   outStream4 << "38000       968.08\n";
   outStream4 << "39000       968.08\n";
   outStream4 << "40000       968.08\n";
   outStream4 << "41000       968.08\n";
   outStream4 << "42000       968.08\n";
   outStream4 << "43000       968.08\n";
   outStream4 << "44000       968.08\n";
   outStream4 << "45000       968.08\n";
   outStream4 << "46000       968.08\n";
   outStream4 << "47000       968.08\n";
   outStream4 << "48000       968.08\n";
   outStream4 << "49000       968.08\n";
   outStream4 << "50000       968.08\n";
   outStream4 << "51000       968.08\n";
   outStream4 << "52000       968.08\n";
   outStream4 << "53000       968.08\n";
   outStream4 << "54000       968.08\n";
   outStream4 << "55000       968.08\n";
   outStream4 << "56000       968.08\n";
   outStream4 << "57000       968.08\n";
   outStream4 << "58000       968.08\n";
   outStream4 << "59000       968.08\n";
   outStream4 << "60000       968.08\n";
   outStream4 << "61000       968.08\n";
   outStream4 << "62000       968.08\n";
   outStream4 << "63000       968.08\n";
   outStream4 << "64000       968.08\n";
   outStream4 << "65000       968.08\n";
   outStream4 << "66000       968.34\n";
   outStream4 << "67000       969.02\n";
   outStream4 << "68000       969.70\n";
   outStream4 << "69000       970.38\n";
   outStream4 << "70000       971.06\n";
   outStream4 << "71000       971.74\n";
   outStream4 << "72000       972.41\n";
   outStream4 << "73000       973.09\n";
   outStream4 << "74000       973.77\n";
   outStream4 << "75000       974.45\n";
   outStream4 << "76000       975.12\n";
   outStream4 << "77000       975.80\n";
   outStream4 << "78000       976.47\n";
   outStream4 << "79000       977.15\n";
   outStream4 << "80000       977.82\n";
   outStream4 << "81000       978.50\n";
   outStream4 << "82000       979.17\n";
   outStream4 << "83000       979.84\n";
   outStream4 << "84000       980.52\n";
   outStream4 << "85000       981.19\n";
   outStream4 << "86000       981.86\n";
   outStream4 << "87000       982.53\n";
   outStream4 << "88000       983.20\n";
   outStream4 << "89000       983.87\n";
   outStream4 << "90000       984.54\n";
   outStream4 << "91000       985.21\n";
   outStream4 << "92000       985.88\n";
   outStream4 << "93000       986.55\n";
   outStream4 << "94000       987.22\n";
   outStream4 << "95000       987.88\n";
   outStream4 << "96000       988.55\n";
   outStream4 << "97000       989.22\n";
   outStream4 << "98000       989.88\n";
   outStream4 << "99000       990.55\n";
   outStream4 << "100000      991.21\n";
   outStream4 << "101000      991.88\n";
   outStream4 << "102000      992.54\n";
   outStream4 << "103000      993.21\n";
   outStream4 << "104000      993.87\n";
   outStream4 << "105000      994.55\n";
   outStream4 << "106000      996.40\n";
   outStream4 << "107000      998.25\n";
   outStream4 << "108000      1000.10\n";
   outStream4 << "109000      1001.95\n";
   outStream4 << "110000      1003.79\n";
   outStream4 << "111000      1005.62\n";
   outStream4 << "112000      1007.46\n";
   outStream4 << "113000      1009.29\n";
   outStream4 << "114000      1011.12\n";
   outStream4 << "115000      1012.94\n";
   outStream4 << "116000      1014.76\n";
   outStream4 << "117000      1016.58\n";
   outStream4 << "118000      1018.39\n";
   outStream4 << "119000      1020.20\n";
   outStream4 << "120000      1022.01\n";
   outStream4 << "121000      1023.81\n";
   outStream4 << "122000      1025.62\n";
   outStream4 << "123000      1027.41\n";
   outStream4 << "124000      1029.21\n";
   outStream4 << "125000      1031.00\n";
   outStream4 << "126000      1032.79\n";
   outStream4 << "127000      1034.58\n";
   outStream4 << "128000      1036.36\n";
   outStream4 << "129000      1038.14\n";
   outStream4 << "130000      1039.91\n";
   outStream4 << "131000      1041.69\n";
   outStream4 << "132000      1043.46\n";
   outStream4 << "133000      1045.23\n";
   outStream4 << "134000      1046.99\n";
   outStream4 << "135000      1048.75\n";
   outStream4 << "136000      1050.51\n";
   outStream4 << "137000      1052.27\n";
   outStream4 << "138000      1054.02\n";
   outStream4 << "139000      1055.77\n";
   outStream4 << "140000      1057.52\n";
   outStream4 << "141000      1059.26\n";
   outStream4 << "142000      1061.00\n";
   outStream4 << "143000      1062.74\n";
   outStream4 << "144000      1064.47\n";
   outStream4 << "145000      1066.21\n";
   outStream4 << "146000      1067.94\n";
   outStream4 << "147000      1069.66\n";
   outStream4 << "148000      1071.39\n";
   outStream4 << "149000      1073.11\n";
   outStream4 << "150000      1074.83\n";
   outStream4 << "151000      1076.55\n";
   outStream4 << "152000      1078.26\n";
   outStream4 << "153000      1079.97\n";
   outStream4 << "154000      1081.68\n";
   outStream4 << "155000      1082.02\n";
   outStream4 << "156000      1082.02\n";
   outStream4 << "157000      1082.02\n";
   outStream4 << "158000      1082.02\n";
   outStream4 << "159000      1082.02\n";
   outStream4 << "160000      1082.02\n";
   outStream4 << "161000      1082.02\n";
   outStream4 << "162000      1082.02\n";
   outStream4 << "163000      1082.02\n";
   outStream4 << "164000      1082.02\n";
   outStream4 << "165000      1082.02\n";
   outStream4 << "166000      1082.02\n";
   outStream4 << "167000      1082.02\n";
   outStream4 << "168000      1080.86\n";
   outStream4 << "169000      1079.15\n";
   outStream4 << "170000      1077.44\n";
   outStream4 << "171000      1075.73\n";
   outStream4 << "172000      1074.01\n";
   outStream4 << "173000      1072.29\n";
   outStream4 << "174000      1070.57\n";
   outStream4 << "175000      1068.84\n";
   outStream4 << "176000      1067.11\n";
   outStream4 << "177000      1065.38\n";
   outStream4 << "178000      1063.65\n";
   outStream4 << "179000      1061.91\n";
   outStream4 << "180000      1060.17\n";
   outStream4 << "181000      1058.43\n";
   outStream4 << "182000      1056.68\n";
   outStream4 << "183000      1054.93\n";
   outStream4 << "184000      1053.18\n";
   outStream4 << "185000      1051.43\n";
   outStream4 << "186000      1049.67\n";
   outStream4 << "187000      1047.91\n";
   outStream4 << "188000      1046.15\n";
   outStream4 << "189000      1044.38\n";
   outStream4 << "190000      1042.61\n";
   outStream4 << "191000      1040.84\n";
   outStream4 << "192000      1039.07\n";
   outStream4 << "193000      1037.29\n";
   outStream4 << "194000      1035.51\n";
   outStream4 << "195000      1033.72\n";
   outStream4 << "196000      1031.94\n";
   outStream4 << "197000      1030.15\n";
   outStream4 << "198000      1028.35\n";
   outStream4 << "199000      1026.56\n";
   outStream4 << "200000      1024.76\n";
   outStream4 << "201000      1022.95\n";
   outStream4 << "202000      1021.15\n";
   outStream4 << "203000      1019.34\n";
   outStream4 << "204000      1017.53\n";
   outStream4 << "205000      1015.71\n";
   outStream4 << "206000      1013.89\n";
   outStream4 << "207000      1012.07\n";
   outStream4 << "208000      1010.24\n";
   outStream4 << "209000      1008.41\n";
   outStream4 << "210000      1006.58\n";
   outStream4 << "211000      1004.75\n";
   outStream4 << "212000      1002.91\n";
   outStream4 << "213000      1001.07\n";
   outStream4 << "214000      999.22\n";
   outStream4 << "215000      997.37\n";
   outStream4 << "216000      995.52\n";
   outStream4 << "217000      993.66\n";
   outStream4 << "218000      991.80\n";
   outStream4 << "219000      989.94\n";
   outStream4 << "220000      988.07\n";
   outStream4 << "221000      986.20\n";
   outStream4 << "222000      984.33\n";
   outStream4 << "223000      982.45\n";
   outStream4 << "224000      980.57\n";
   outStream4 << "225000      978.69\n";
   outStream4 << "226000      976.80\n";
   outStream4 << "227000      974.91\n";
   outStream4 << "228000      973.01\n";
   outStream4 << "229000      971.12\n";
   outStream4 << "230000      969.21\n";
   outStream4 << "231000      967.31\n";
   outStream4 << "232000      965.40\n";
   outStream4 << "233000      963.52\n";
   outStream4 << "234000      962.15\n";
   outStream4 << "235000      960.77\n";
   outStream4 << "236000      959.40\n";
   outStream4 << "237000      958.03\n";
   outStream4 << "238000      956.65\n";
   outStream4 << "239000      955.27\n";
   outStream4 << "240000      953.89\n";
   outStream4 << "241000      952.50\n";
   outStream4 << "242000      951.12\n";
   outStream4 << "243000      949.73\n";
   outStream4 << "244000      948.34\n";
   outStream4 << "245000      946.95\n";
   outStream4 << "246000      945.56\n";
   outStream4 << "247000      944.16\n";
   outStream4 << "248000      942.77\n";
   outStream4 << "249000      941.37\n";
   outStream4 << "250000      939.96\n";
   outStream4 << "251000      938.56\n";
   outStream4 << "252000      937.15\n";
   outStream4 << "253000      935.75\n";
   outStream4 << "254000      934.34\n";
   outStream4 << "255000      932.92\n";
   outStream4 << "256000      931.51\n";
   outStream4 << "257000      930.09\n";
   outStream4 << "258000      928.67\n";
   outStream4 << "259000      927.25\n";
   outStream4 << "260000      925.83\n";
   outStream4 << "261000      924.41\n";
   outStream4 << "262000      922.98\n";
   outStream4 << "263000      921.55\n";
   outStream4 << "264000      920.12\n";
   outStream4 << "265000      918.68\n";
   outStream4 << "266000      917.25\n";
   outStream4 << "267000      915.81\n";
   outStream4 << "268000      914.37\n";
   outStream4 << "269000      912.92\n";
   outStream4 << "270000      911.48\n";
   outStream4 << "271000      910.03\n";
   outStream4 << "272000      908.58\n";
   outStream4 << "273000      907.13\n";
   outStream4 << "274000      905.67\n";
   outStream4 << "275000      904.22\n";
   outStream4 << "276000      902.76\n";
   outStream4 << "277000      901.30\n";
   outStream4 << "278000      899.83\n";
   outStream4 << "279000      899.27\n";
   outStream4 << "280000      899.27\n";
   outStream4 << "281000      899.27\n";
   outStream4 << "282000      899.27\n";
   outStream4 << "400000      899.27\n";
   outStream4 << "end_sonic_speed_altitude_table\n";
   outStream4 << "\n";
   outStream4 << "contrail_min_altitude 26000.0 ft\n";
   outStream4 << "contrail_max_altitude 39000.0 ft\n";
   outStream4 << "\n";
   outStream4 << "end_p6dof_atmosphere\n";
   outStream4 << "\n";

   baseFile4.close();
   }

   void ScriptGeneratorP6DOF::CreatePlatformFile()
   {
   QString platformDir = mVehicleDir.absolutePath() + "/platforms";
   QFile platformFile(platformDir + "/" + mPlatformName + ".txt");
   if (!platformFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   QTextStream outStream(&platformFile);

   outStream.setRealNumberNotation(QTextStream::FixedNotation);

   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "include_once ../p6dof_types/environment/p6dof_environment.txt\n";

   if (mVehiclePtr->IsAircraft())
   {
      outStream << "include_once ../p6dof_types/aircraft/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << ".txt\n";
   }
   else if (mVehiclePtr->IsWeapon())
   {
      outStream << "include_once ../p6dof_types/weapons/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << ".txt\n";
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle needs to be either an aircraft or a weapon!");
      return;
   }

   outStream << "\n";
   outStream << "platform_type  " << mPlatformType << "  WSF_PLATFORM\n";
   outStream << "\n";

   std::string domain = "air";
   outStream << "   spatial_domain  ";
   OutputData(outStream, QString(domain.c_str()), 32, QTextStream::FieldAlignment::AlignLeft);
   outStream << "   # This vehicle operates in the air domain\n";

   std::string side = "green";
   std::string icon = mVehiclePtr->GetVehicleName();

   outStream << "   side            ";
   OutputData(outStream, QString(side.c_str()), 32, QTextStream::FieldAlignment::AlignLeft);
   outStream << "   # Neutral force color\n";
   outStream << "   icon            ";
   OutputData(outStream, QString(icon.c_str()), 32, QTextStream::FieldAlignment::AlignLeft);
   outStream << "   # Icon based on geometry\n";
   outStream << "\n";
   outStream << "   # Setup the P6DOF mover\n";
   outStream << "   mover WSF_P6DOF_MOVER\n";
   outStream << "      p6dof_object_type " << mVehicleName.toUpper() << "\n";
   outStream << "      update_interval   0.01 sec\n";
   outStream << "   end_mover\n";
   outStream << "\n";

      // TODO: This section below can be restored to provide an SA Processor, if needed
      // outStream << "   # processor situational_awareness WSF_SA_PROCESSOR \n";
      // outStream << "   #   on\n";
      // outStream << "   #   update_interval      1.0 sec\n";
      // outStream << "   #\n";
      // outStream << "   #   # Data collection rates\n";
      // outStream << "   #   report_interval      5.0 sec\n";
      // outStream << "   # end_processor\n";
      // outStream << "\n";

   outStream << "end_platform_type\n";
   platformFile.close();
   }

   void ScriptGeneratorP6DOF::CreateAdditionalFiles()
   {
   CreateBaseScenarioFile();
   CreateEnvironmentFiles();
   CreateSignaturesFile();
   CreateMoverFile();
   CreatePropulsionFile();
   }

   void ScriptGeneratorP6DOF::CreateMoverFile()
   {
   if (!mVehiclePtr->IsAircraft() && !mVehiclePtr->IsWeapon())
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle needs to be either an aircraft or a weapon!");
      return;
   }

   QString moverFolderPath = mVehicleDir.absolutePath() + "/p6dof_types/" + mVehicleType + "/" + mVehicleName.toLower();
   QString moverPath = moverFolderPath + "/" + mVehicleName.toLower() + ".txt";
   QFile moverFile(moverPath);
   if (!moverFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }
   QTextStream outStream(&moverFile);

   outStream.setRealNumberNotation(QTextStream::FixedNotation);

   Designer::VehicleGeometry& vehicleGeometry = mVehiclePtr->GetGeometry();
   Designer::GeometryMassProperties* massProperties = vehicleGeometry.GetMassProperties();

   CreateAutogenHeader(mVehiclePtr, outStream);

   std::vector<QString> engineIncludeList;

   // Include all of the engines, but list each type only once
   for (auto curEngine : mEngineList)
   {
      QString engineModel = QString::fromStdString(curEngine->GetModelName()).toLower();
      QString engineType = QString();
      switch (curEngine->GetEngineType())
      {
         case GeometryEngine::EngineType::cJET:
            engineType = "jet_engines";
            break;
         case GeometryEngine::EngineType::cRAMJET:
            engineType = "ramjets";
            break;
         case GeometryEngine::EngineType::cLIQUIDROCKET:
            engineType = "liquid_propellant_rockets";
            break;
         case GeometryEngine::EngineType::cSOLIDROCKET:
            engineType = "solid_propellant_rockets";
            break;
      }

      // Set the temporary string for this engine
      QString tempString = "include_once prop/" + engineType + "/" + engineModel + "/" + engineModel + ".txt";



      // Test to see if the current string is already in the list
      bool matchFound = std::any_of(engineIncludeList.begin(), engineIncludeList.end(), 
         [ = ](const QString & engineInclude) {return engineInclude == tempString; });

      // If not found in the list, add it to the list
      if (!matchFound)
      {
         engineIncludeList.emplace_back(tempString);
      }
   }

   // Output the unique engine include strings
   for (auto& tmp : engineIncludeList)
   {
      outStream << tmp << "\n";
   }

   outStream << "\n";
   outStream << "p6dof_object_types\n";
   outStream << "   p6dof_object_type " << mVehicleName.toUpper() << " P6DOF_BASE_TYPE\n\n";
   outStream << "      include aero/aero.txt                 # Defines the vehicle aerodynamics\n";

   if (mVehiclePtr->GetVehicleControlConfiguration() != Vehicle::cNO_CONTROL)
   {
      outStream << "      include controls/control_inputs.txt   # Defines the control inputs (stick, rudder, throttle, etc)\n";
      outStream << "      include controls/flight_controls.txt  # Defines how the control inputs move the control surfaces\n";
   }
   outStream << "\n";

   int width = 16;
   int precision = 5;
   QTextStream::FieldAlignment alignment = QTextStream::FieldAlignment::AlignRight;

   outStream.setRealNumberNotation(QTextStream::FixedNotation);

   outStream << "      mass_properties\n";
   outStream << "         mass                       ";
   OutputData(outStream, massProperties->GetEmptyMass_lbs(), width, precision, alignment);
   outStream << " lbs\n";
   outStream << "         moment_of_inertia_ixx      ";
   OutputData(outStream, massProperties->GetIxx(), width, precision, alignment);
   outStream << " slug*ft^2\n";
   outStream << "         moment_of_inertia_iyy      ";
   OutputData(outStream, massProperties->GetIyy(), width, precision, alignment);
   outStream << " slug*ft^2\n";
   outStream << "         moment_of_inertia_izz      ";
   OutputData(outStream, massProperties->GetIzz(), width, precision, alignment);
   outStream << " slug*ft^2\n";
   outStream << "         center_of_mass_x           ";
   OutputData(outStream, massProperties->GetCgX(), width, precision, alignment);
   outStream << " ft\n";
   outStream << "         center_of_mass_y           ";
   OutputData(outStream, massProperties->GetCgY(), width, precision, alignment);
   outStream << " ft\n";
   outStream << "         center_of_mass_z           ";
   OutputData(outStream, massProperties->GetCgZ(), width, precision, alignment);
   outStream << " ft\n";
   outStream << "      end_mass_properties\n\n";

   const UtVec3dX originalCgLocation_ft = massProperties->GetOriginalCg();
   if (originalCgLocation_ft.Magnitude() > 0.0)
   {
      outStream << "         # Original center of mass, for load and balance reference only.\n";
      outStream << "         # Leave these lines commented. Use of this c.m. within P6DOF is incorrect.\n";
      outStream << "         # center_of_mass_x           ";
      OutputData(outStream, originalCgLocation_ft.X(), width, precision, alignment);
      outStream << " ft\n";
      outStream << "         # center_of_mass_y           ";
      OutputData(outStream, originalCgLocation_ft.Y(), width, precision, alignment);
      outStream << " ft\n";
      outStream << "         # center_of_mass_z           ";
      OutputData(outStream, originalCgLocation_ft.Z(), width, precision, alignment);
      outStream << " ft\n";
   }

   // Vehicles should include a propulsion system definition, even if no engines/fuel are present

   VehiclePropulsion* propulsion = mVehiclePtr->GetPropulsion();

   if (propulsion == nullptr)
   {
      outStream << "      # Note: This vehicle lacks any propulsion system\n";
      outStream << "\n";
   }
   else
   {
      GeometryPropulsionData* propData = vehicleGeometry.GetPropulsionData();
      bool fuelTankPresent = false;

      if (propData != nullptr)
      {
         if (propData->FuelTankPresent())
         {
            fuelTankPresent = true;
         }
      }

      // If we lack a fuel tank and any engines, we lack propulsion
      if (mEngineList.empty() && !fuelTankPresent)
      {
         outStream << "      # Note: This vehicle lacks any propulsion system\n";
         outStream << "\n";
      }
      else
      {
         // We have a propulsion system
         outStream << "      propulsion_data\n";
         outStream << "\n";

         bool hasJetEngine = false;
         if (!mEngineList.empty())
         {
            // Loop through and add each engine
            for (auto curEngine : mEngineList)
            {
               if (curEngine->GetEngineType() == GeometryEngine::EngineType::cJET) { hasJetEngine = true; }

               QString curEngineName = QString::fromStdString(curEngine->GetName()).replace(" ", "_");

               outStream << "         engine " << curEngineName << "  " << QString::fromStdString(curEngine->GetModelName()).toUpper() << "\n";
               outStream << "            rel_pos_x               ";
               OutputData(outStream, curEngine->GetRefPoint_ft().X(), width, precision, alignment);
               outStream << " ft\n";
               outStream << "            rel_pos_y               ";
               OutputData(outStream, curEngine->GetRefPoint_ft().Y(), width, precision, alignment);
               outStream << " ft\n";
               outStream << "            rel_pos_z               ";
               OutputData(outStream, curEngine->GetRefPoint_ft().Z(), width, precision, alignment);
               outStream << " ft\n";
               outStream << "            rel_yaw                 ";
               OutputData(outStream, curEngine->GetYaw_deg(), width, precision, alignment);
               outStream << " deg\n";
               outStream << "            rel_pitch               ";
               OutputData(outStream, curEngine->GetPitch_deg(), width, precision, alignment);
               outStream << " deg\n";

               // Include a fuel feed unless it is a solid rocket
               if (curEngine->GetEngineType() != GeometryEngine::EngineType::cSOLIDROCKET)
               {
                  outStream << "            fuel_feed               ";
                  OutputData(outStream, QString("MainFuelTank"), width, alignment);
                  outStream << "\n";
               }

               outStream << "         end_engine\n";
               outStream << "\n";
            }
         }

         // Fuel tank(s)
         if (fuelTankPresent)
         {
            outStream << "         # Current fuel tank support is limited to a single tank.\n";
            outStream << "         # Future Mover Creator upgrades may support more complex, multi-tank fuel systems.\n";
            outStream << "\n";

            double fuel_max = propData->GetFuelQtyMax();
            double fuel_current = propData->GetFuelQtyCurrent();

            outStream << "         fuel_tank  MainFuelTank\n";
            outStream << "            max_fuel_quantity       ";
            OutputData(outStream, fuel_max, width, precision, alignment);
            outStream << " lbs\n";
            outStream << "            current_fuel_quantity   ";
            OutputData(outStream, fuel_current, width, precision, alignment);
            outStream << " lbs\n";
            outStream << "            max_flow_rate           ";
            OutputData(outStream, 1000000.0, width, precision, alignment);
            outStream << " lbs/sec\n";
            outStream << "            max_fill_rate           ";
            OutputData(outStream, 1000000.0, width, precision, alignment);
            outStream << " lbs/sec\n";
            outStream << "            max_xfer_rate           ";
            OutputData(outStream, 1000000.0, width, precision, alignment);
            outStream << " lbs/sec\n";

            UtVec3dX fuelCg_ft(0.0, 0.0, 0.0);
            if (propData->GetCG_ft(fuelCg_ft))
            {
               double cgX = fuelCg_ft.X();
               double cgY = fuelCg_ft.Y();
               double cgZ = fuelCg_ft.Z();

               outStream << "            cg_full_x               ";
               OutputData(outStream, cgX, width, precision, alignment);
               outStream << " ft\n";
               outStream << "            cg_full_y               ";
               OutputData(outStream, cgY, width, precision, alignment);
               outStream << " ft\n";
               outStream << "            cg_full_z               ";
               OutputData(outStream, cgZ, width, precision, alignment);
               outStream << " ft\n";
               outStream << "            cg_empty_x              ";
               OutputData(outStream, cgX, width, precision, alignment);
               outStream << " ft\n";
               outStream << "            cg_empty_y              ";
               OutputData(outStream, cgY, width, precision, alignment);
               outStream << " ft\n";
               outStream << "            cg_empty_z              ";
               OutputData(outStream, cgZ, width, precision, alignment);
               outStream << " ft\n";
            }
            else
            {
               // Use a default of zero
               outStream << "            cg_full_x               ";
               OutputData(outStream, 0.0, width, precision, alignment);
               outStream << " ft\n";
               outStream << "            cg_full_y               ";
               OutputData(outStream, 0.0, width, precision, alignment);
               outStream << " ft\n";
               outStream << "            cg_full_z               ";
               OutputData(outStream, 0.0, width, precision, alignment);
               outStream << " ft\n";
               outStream << "            cg_empty_x              ";
               OutputData(outStream, 0.0, width, precision, alignment);
               outStream << " ft\n";
               outStream << "            cg_empty_y              ";
               OutputData(outStream, 0.0, width, precision, alignment);
               outStream << " ft\n";
               outStream << "            cg_empty_z              ";
               OutputData(outStream, 0.0, width, precision, alignment);
               outStream << " ft\n";
            }

            outStream << "         end_fuel_tank\n";
            outStream << "\n";
         }
         else
         {
            outStream << "         # This vehicle lacks a fuel tank\n";
            outStream << "\n";
         }

         if (!mEngineList.empty())
         {
            outStream << "         # Throttle controls:\n";
            outStream << "         throttle_setting_mil         ThrottleMilitary\n";
            if (hasJetEngine)
            {
               outStream << "         throttle_setting_ab          ThrottleAfterburner\n";
               outStream << "         throttle_setting_reverser    ThrottleThrustReverser\n";
            }
            outStream << "\n";

            QString enableThrustVectoringString;
            if (propData->GetEnableThrustVectoringString(enableThrustVectoringString))
            {
               outStream << "         # Thrust vectoring controls:\n";

               if (enableThrustVectoringString.contains("Yaw"))
               {
                  outStream << "         throttle_setting_yaw         ThrustVectoringYaw\n";
               }
               else
               {
                  outStream << "         #throttle_setting_yaw        ThrustVectoringYaw\n";
               }

               if (enableThrustVectoringString.contains("Pitch"))
               {
                  outStream << "         throttle_setting_pitch       ThrustVectoringPitch\n";
               }
               else
               {
                  outStream << "         #throttle_setting_pitch      ThrustVectoringPitch\n";
               }
               outStream << "\n";
            }
         }

         outStream << "      end_propulsion_data\n\n";
      }
   }

   // If the vehicle is a weapon and has an engine, include a sequencer to "ignite" the engine(s)
   if (mVehiclePtr->IsWeapon())
   {
      if (!mEngineList.empty())
      {
         outStream << "      # This sequencer will ignite/start the engine(s)\n";
         outStream << "      sequencer ReleaseFromVehicle\n";
         outStream << "         event_released_from_parent\n";
         for (const auto& engine : mEngineList)
         {
            QString engineName = QString::fromStdString(engine->GetName());
            double ignitionDelay = engine->GetIgnitionDelay_sec();
            if (ignitionDelay != 0)
            {
               outStream << "         sequencer Ignite_" << engineName << "\n";
               outStream << "            event_timer " << ignitionDelay << " sec\n";
               outStream << "            action_ignite_engine " << engineName << "\n";
               outStream << "         end_sequencer\n";
            }
            else
            {
               outStream << "         action_ignite_engine " << engineName << "\n";
            }

         }
         outStream << "      end_sequencer\n";
      }
   }

   outStream << "   end_p6dof_object_type\n";
   outStream << "end_p6dof_object_types\n";
   moverFile.close();

   QString aeroFolderPath = moverFolderPath + "/aero";
   CreateAeroFile(aeroFolderPath);
   if (mVehiclePtr->GetVehicleControlConfiguration() != Vehicle::cNO_CONTROL)
   {
      QString controlsFolderPath = moverFolderPath + "/controls";
      CreateControlsInputFile(controlsFolderPath);
      CreateFlightControlsFile(controlsFolderPath);
      CreateStubAutopilotSupportFile(controlsFolderPath);
   }
   }

   void ScriptGeneratorP6DOF::CreatePropulsionFile()
   {
   QString propulsionFolderPath = mVehicleDir.absolutePath() + "/p6dof_types/" + mVehicleType + "/" + mVehicleName.toLower() + "/prop";
   QString enginePath = propulsionFolderPath + "/" + mVehicleName.toLower() + ".txt";

   // For each engine, create a directory and engine file
   for (auto curEngine : mEngineList)
   {
      QString engineType = QString();
      switch (curEngine->GetEngineType())
      {
         case GeometryEngine::EngineType::cJET:
         {
            engineType = "jet_engines";
            break;
         }
         case GeometryEngine::EngineType::cRAMJET:
         {
            engineType = "ramjets";
            break;
         }
         case GeometryEngine::EngineType::cLIQUIDROCKET:
         {
            engineType = "liquid_propellant_rockets";
            break;
         }
         case GeometryEngine::EngineType::cSOLIDROCKET:
         {
            engineType = "solid_propellant_rockets";
            break;
         }
      }

      // Create engine folder and related parent folders
      CreateFolderIfDoesntExist(propulsionFolderPath);
      QString engineTypePath = propulsionFolderPath + "/" + engineType;
      CreateFolderIfDoesntExist(engineTypePath);
      QString engineName = QString::fromStdString(curEngine->GetModelName());
      QString engineFolderPath = propulsionFolderPath + "/" + engineType + "/" + engineName.toLower();
      CreateFolderIfDoesntExist(engineFolderPath);

      // Output the engine file
      QString engineFilename = engineFolderPath + "/" + engineName.toLower() + ".txt";

      curEngine->Export(engineFilename.toStdString(), AeroEngine::ExportMoverType::cP6DOF);
   }
   }

   void ScriptGeneratorP6DOF::CreateSignaturesFile()
   {
   QString signaturesDir = mVehicleDir.absolutePath() + "/signatures";
   QFile signatureFile(signaturesDir + "/simple_signatures.txt");
   if (!signatureFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   QTextStream outStream(&signatureFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "# --------------------------------------------------------------------------------------------------\n";
   outStream << "\n";
   outStream << "# This file includes various \"simple\" signatures (radar, ir, and optical)\n";
   outStream << "#\n";
   outStream << "#   SIMPLE_FIGHTER_RCS which is a 5 square meter target, typical for a fighter-sized aircraft\n";
   outStream << "#\n";
   outStream << "#   SIMPLE_FIGHTER_IR_SIG which is a 200 watts/steradian infrared target, typical for\n";
   outStream << "#      non-afterburning aircraft\n";
   outStream << "#\n";
   outStream << "#   SIMPLE_FIGHTER_IR_SIG_AFTERBURNER which is a 1000 watts/steradian infrared target, typical for\n";
   outStream << "#      afterburning aircraft\n";
   outStream << "#\n";
   outStream << "#   SIMPLE_FIGHTER_OPTICAL_SIG which is a 3 square meter optical target, typical for a fighter-sized\n";
   outStream << "#      aircraft\n";
   outStream << "#\n";
   outStream << "#   SIMPLE_WEAPON_RCS which is a 1 square meter target, typical for weapons\n";
   outStream << "#\n";
   outStream << "#   SIMPLE_WEAPON_IR_SIG which is a 100 watts/steradian infrared target, typical for\n";
   outStream << "#      a weapon that is not under propulsion (the rocket is not burning)\n";
   outStream << "#\n";
   outStream << "#   SIMPLE_WEAPON_IR_SIG_THRUSTING which is a 1000 watts/steradian infrared target, typical for\n";
   outStream << "#       a weapon that is under propulsion (the rocket is burning)\n";
   outStream << "#\n";
   outStream << "#   SIMPLE_WEAPON_OPTICAL_SIG which is a 1 square meter optical target, typical for a weapon\n";
   outStream << "#\n";
   outStream << "#   SIMPLE_1M_RCS which is a 1 square meter target\n";
   outStream << "#\n";
   outStream << "#   SIMPLE_100WSR_IR_SIG which is a 100 watts/steradian infrared target\n";
   outStream << "#\n";
   outStream << "#   SIMPLE_1M_OPTICAL_SIG which is a 1 square meter optical target\n";
   outStream << "#\n";
   outStream << "# --------------------------------------------------------------------------------------------------\n";
   outStream << "\n";
   outStream << "# Radar cross sections\n";
   outStream << "\n";
   outStream << "   radar_signature SIMPLE_FIGHTER_RCS\n";
   outStream << "      constant 5 m^2\n";
   outStream << "   end_radar_signature\n";
   outStream << "\n";
   outStream << "   radar_signature SIMPLE_WEAPON_RCS\n";
   outStream << "      constant 1 m^2\n";
   outStream << "   end_radar_signature\n";
   outStream << "\n";
   outStream << "   radar_signature SIMPLE_1M_RCS\n";
   outStream << "      constant 1 m^2\n";
   outStream << "   end_radar_signature\n";
   outStream << "\n";
   outStream << "# --------------------------------------------------------------------------------------------------\n";
   outStream << "\n";
   outStream << "# Infrared signatures\n";
   outStream << "\n";
   outStream << "   infrared_signature SIMPLE_FIGHTER_IR_SIG\n";
   outStream << "      state default\n";
   outStream << "      band default\n";
   outStream << "      constant 200 w/sr\n";
   outStream << "   end_infrared_signature\n";
   outStream << "\n";
   outStream << "   infrared_signature SIMPLE_FIGHTER_IR_SIG_AFTERBURNER\n";
   outStream << "      state default\n";
   outStream << "      band default\n";
   outStream << "      constant 1000 w/sr\n";
   outStream << "   end_infrared_signature\n";
   outStream << "\n";
   outStream << "   infrared_signature SIMPLE_WEAPON_IR_SIG\n";
   outStream << "      state default\n";
   outStream << "      band default\n";
   outStream << "      constant 100 w/sr\n";
   outStream << "   end_infrared_signature\n";
   outStream << "\n";
   outStream << "   infrared_signature SIMPLE_WEAPON_IR_SIG_THRUSTING\n";
   outStream << "      state default\n";
   outStream << "      band default\n";
   outStream << "      constant 1000 w/sr\n";
   outStream << "   end_infrared_signature\n";
   outStream << "\n";
   outStream << "   infrared_signature SIMPLE_100WSR_IR_SIG\n";
   outStream << "      state default\n";
   outStream << "      band default\n";
   outStream << "      constant 100 w/sr\n";
   outStream << "   end_infrared_signature\n";
   outStream << "\n";
   outStream << "# --------------------------------------------------------------------------------------------------\n";
   outStream << "\n";
   outStream << "# Optical signatures\n";
   outStream << "\n";
   outStream << "   optical_signature SIMPLE_FIGHTER_OPTICAL_SIG\n";
   outStream << "      constant 3 m^2\n";
   outStream << "   end_optical_signature\n";
   outStream << "\n";
   outStream << "   optical_signature SIMPLE_WEAPON_OPTICAL_SIG\n";
   outStream << "      constant 1 m^2\n";
   outStream << "   end_optical_signature\n";
   outStream << "\n";
   outStream << "   optical_signature SIMPLE_1M_OPTICAL_SIG\n";
   outStream << "      constant 1 m^2\n";
   outStream << "   end_optical_signature\n";
   outStream << "\n";
   outStream << "# --------------------------------------------------------------------------------------------------\n";
   outStream << "\n";

   signatureFile.close();
   }

   void ScriptGeneratorP6DOF::CreateTestFiles()
   {
   PerformanceTestType performanceTestType = GetPerformanceTestType();
   QString testDir = mTestDir.absolutePath();
   if (performanceTestType == PerformanceTestType::cBANK_TO_TURN ||
      performanceTestType == PerformanceTestType::cSKID_TO_TURN)
   {
      CreateAutopilotSupportGeneratorFile(testDir);
   }

   CreatePerformanceTestFiles(testDir);
   }

   const QString ScriptGeneratorP6DOF::CreateAutopilotTestFile(double  aLatitude,
                                                            double  aLongitude,
                                                            double  aAltitude,
                                                            double  aHeading,
                                                            double  aSpeed)
   {
   QString testDir = mTestDir.absolutePath();
   QString testFile = testDir + "/autopilot_tuning_scenario.txt";
   QFile genFile(testFile);
   if (!genFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return "";
   }

   QTextStream outStream(&genFile);

   CreateAutogenHeader(mVehiclePtr, outStream);
   outStream << "# ==================================================================================================\n";
   outStream << "# This file is used for tuning the P6DOF autopilot using Warlock for a vehicle that was created\n";
   outStream << "# by the AFSIM Mover Creator tool.\n";
   outStream << "# ==================================================================================================\n";
   outStream << "\n";
   outStream << "include_once ../../p6dof_types/environment/p6dof_environment.txt\n";
   outStream << "include_once ../../platforms/" << mPlatformName << ".txt\n";
   outStream << "\n";
   outStream << "end_time 365 days           # Use a long time to provide sufficient time for any amount of tuning/testing\n";
   outStream << "minimum_mover_timestep 1 ms # Allow Warlock to update more frequently and miss fewer updates\n";
   outStream << "\n";
   outStream << "platform  p6dof_test_platform  " << mPlatformType << "\n";
   outStream << "\n";

   double heading_rad = aHeading * UtMath::cRAD_PER_DEG;
   double vel_fps = aSpeed * UtMath::cFT_PER_M;
   double north_fps = cos(heading_rad) * vel_fps;
   double east_fps = sin(heading_rad) * vel_fps;

   double heading_deg = aHeading;
   while (heading_deg < 0.0)
   {
      heading_deg += 360.0;
   }
   while (heading_deg > 360.0)
   {
      heading_deg -= 360.0;
   }

   outStream << "   p6dof_set_velocity_ned_fps  " << north_fps << "  " << east_fps << "  0\n";
   outStream << "   p6dof_position              " << aLatitude << "  " << aLongitude << "\n";
   outStream << "   p6dof_alt                   " << aAltitude* UtMath::cFT_PER_M << " ft\n";
   outStream << "   p6dof_ned_heading           " << heading_deg << " deg\n";
   outStream << "\n";

   outStream << "end_platform\n";
   outStream << "\n";

   genFile.close();

   return testFile;
   }

   void ScriptGeneratorP6DOF::CreatePerformanceTestFiles(QString& aTestDir)
   {
   if (mVehiclePtr == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   // Performance tests that will be used for all cases:
   CreateCommonTestFile(aTestDir);
   CreatePitchingMomentTest(aTestDir, 20000.0, 0.25);
   CreatePitchingMomentTest(aTestDir, 20000.0, 0.50);
   CreatePitchingMomentTest(aTestDir, 20000.0, 0.85);
   CreatePitchingMomentTest(aTestDir, 20000.0, 1.00);
   CreatePitchingMomentTest(aTestDir, 20000.0, 1.25);
   CreatePitchingMomentTest(aTestDir, 20000.0, 1.50);
   CreatePitchingMomentTest(aTestDir, 20000.0, 1.75);
   CreatePitchingMomentTest(aTestDir, 20000.0, 2.00);
   CreatePitchingMomentTest(aTestDir, 20000.0, 3.00);
   CreatePitchingMomentTest(aTestDir, 20000.0, 4.00);
   CreatePitchingMomentTest(aTestDir, 20000.0, 5.00);
   CreatePitchingMomentTest(aTestDir, 20000.0, 6.00);
   CreateFlightTestFile(21.12, -157.95, 20000.0, 0, 750);

   switch (GetPerformanceTestType())
   {
      // Case for vehicles with no control and no engine.
      // Examples: spent stage of a missile, unguided munitions
      case Designer::ScriptGeneratorP6DOF::PerformanceTestType::cNO_CONTROL_NO_ENGINE:
      {
         // Do nothing special, for now.
         break;
      }
      // Case for vehicles with no control with an engine
      // Example: boost stage on a missile
      case Designer::ScriptGeneratorP6DOF::PerformanceTestType::cNO_CONTROL_ENGINE:
      {
         CreateBallisticRangeTestFile(aTestDir);
         break;
      }
      // Case for a bank-to-turn vehicle.
      // Examples: aircraft, guided drone missiles
      case Designer::ScriptGeneratorP6DOF::PerformanceTestType::cBANK_TO_TURN:
      {
         CreateEnvelopeCommonTestFile(aTestDir);
         CreateFlightEnvelopeSimpleTestFile(aTestDir);

         if (mVehicleType == "aircraft")
         {
            CreateFlightEnvelopeAbTestFile(aTestDir);
            CreateFlightEnvelopeMilTestFile(aTestDir);
            CreateTurnPerformanceTestFile(aTestDir, 5000, true);
            CreateTurnPerformanceTestFile(aTestDir, 5000, false);
            CreateTurnPerformanceTestFile(aTestDir, 15000, true);
            CreateTurnPerformanceTestFile(aTestDir, 15000, false);
            CreateTurnPerformanceTestFile(aTestDir, 25000, true);
            CreateTurnPerformanceTestFile(aTestDir, 25000, false);
            CreateTurnPerformanceTestFile(aTestDir, 35000, true);
            CreateTurnPerformanceTestFile(aTestDir, 35000, false);
            CreateTurnPerformanceTestFile(aTestDir, 45000, true);
            CreateTurnPerformanceTestFile(aTestDir, 45000, false);
         }
         CreateRangeTestFile(aTestDir);
         CreateEnduranceTestFile(aTestDir);
         CreateThrustDragTest(aTestDir, 0.0);
         CreateThrustDragTest(aTestDir, 5000.0);
         CreateThrustDragTest(aTestDir, 15000.0);
         CreateThrustDragTest(aTestDir, 30000.0);
         CreateThrustDragTest(aTestDir, 45000.0);
         CreateThrustDragTest(aTestDir, 60000.0);
         CreateBaseScenarioFile();
         break;
      }
      // Cases for a skid-to-turn vehicle
      // Example: intercept air missiles
      case Designer::ScriptGeneratorP6DOF::PerformanceTestType::cSKID_TO_TURN:
      {
         CreateBallisticRangeTestFile(aTestDir);
         CreateGCapabilityTestFile(aTestDir);
         CreateLevelFlightRangeTestFile(aTestDir);
         CreateMaximumSpeedTestFile(aTestDir);
         CreateBaseScenarioFile();
         break;
      }
      default:
         break;
      }
   }

   const QString ScriptGeneratorP6DOF::CreateFlightTestFile(double aLatitude,
                                                    double aLongitude,
                                                    double aAltitude_ft,
                                                    double aHeading_deg,
                                                    double aSpeed_fps)
   {
   QString testFile = mTestDir.absolutePath() + "/flight_test_scenario.txt";
   QFile genFile(testFile);
   if (!genFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return "";
   }

   QTextStream outStream(&genFile);

   CreateAutogenHeader(mVehiclePtr, outStream);
   outStream << "# ==================================================================================================\n";
   outStream << "# This file is used for flight testing a vehicle that was created by the AFSIM Mover Creator tool.\n";
   outStream << "# ==================================================================================================\n";
   outStream << "\n";
   outStream << "include_once ../../p6dof_types/environment/p6dof_environment.txt\n";
   outStream << "include_once ../../platforms/" << mPlatformName << ".txt\n";
   outStream << "\n";
   outStream << "# Setup a replay file for post-run viewing\n";
   outStream << "event_pipe\n";
   outStream << "   file ../../output/flight_test_scenario_replay.aer\n";
   outStream << "   enable P6DOF\n";
   outStream << "end_event_pipe\n";
   outStream << "\n";
   outStream << "# Set a start time that has a daytime sun position in the Oahu region (default test area)\n";
   outStream << "start_epoch 2018354.75   # Southern Solstice 2018\n";
   outStream << "\n";
   outStream << "# Allow Warlock to update more frequently and miss fewer updates\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "# Provide plenty of runtime, to prevent the scenario from ending before testing is complete.\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";
   outStream << "# Setup the test vehicle\n";
   outStream << "platform  p6dof_test_platform  " << mPlatformType << "\n";
   outStream << "\n";
   outStream << "   # Output P6DOF Event Pipe data\n";
   outStream << "   edit mover\n";
   outStream << "      event_core_data             30 Hz\n";
   outStream << "      event_kinematic_data        30 Hz\n";
   outStream << "      event_engine_fuel_data      30 Hz\n";
   outStream << "      event_autopilot_data        10 Hz\n";
   outStream << "      event_autopilot_limits_data  1 Hz\n";
   outStream << "      event_control_inputs_data   30 Hz\n";
   outStream << "      event_control_surfaces_data 30 Hz\n";
   outStream << "      event_force_moment_data     30 Hz\n";
   outStream << "   end_mover\n";
   outStream << "\n";

   double heading_rad = aHeading_deg * UtMath::cRAD_PER_DEG;
   double vel_fps     = aSpeed_fps;
   double north_fps   = cos(heading_rad) * vel_fps;
   double east_fps    = sin(heading_rad) * vel_fps;

   double heading_deg = aHeading_deg;
   while (heading_deg < 0.0)
   {
      heading_deg += 360.0;
   }
   while (heading_deg > 360.0)
   {
      heading_deg -= 360.0;
   }

   outStream << "   p6dof_set_velocity_ned_fps  " << north_fps << "  " << east_fps << "  0\n";
   outStream << "   p6dof_position              " << aLatitude << "  " << aLongitude << "\n";
   outStream << "   p6dof_alt                   " << aAltitude_ft << " ft\n";
   outStream << "   p6dof_ned_heading           " << heading_deg << " deg\n";
   outStream << "\n";

   outStream << "end_platform\n";
   outStream << "\n";

   genFile.close();

   return testFile;
   }

   QString ScriptGeneratorP6DOF::ControlsFolderPath() const
   {
   return mVehicleDir.absolutePath() + "/p6dof_types/" + mVehicleType + "/" + mVehicleName.toLower() + "/controls";
   }

   void ScriptGeneratorP6DOF::CreateAutopilotSupportGeneratorFile(QString& aTestDir)
   {
   QFile genFile(aTestDir + "/generate_autopilot_support_file.txt");
   if (!genFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   QTextStream outStream(&genFile);

   CreateAutogenHeader(mVehiclePtr, outStream);
   outStream << "# ==================================================================================================\n";
   outStream << "# This file is used for creating the autopilot support file that is used by the P6DOF autopilot.\n";
   outStream << "# ==================================================================================================\n";
   outStream << "\n";
   outStream << "include_once ../../p6dof_types/environment/p6dof_environment.txt\n";
   outStream << "include_once ../../platforms/" << mPlatformName << ".txt\n";
   outStream << "\n";
   outStream << "# Allow Warlock to update more frequently and miss fewer updates\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "# Provide plenty of runtime, in order to support multiple autopilot tests\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";
   outStream << "platform  p6dof_test_platform  " << mPlatformType << "\n";
   outStream << "\n";
   outStream << "   script void CreateAutopilotSupportFileWithMaxMach(WsfPlatform aPlatform, string aFilename, double aMaxMach)\n";
   outStream << "      WsfMover mover = aPlatform.Mover();\n";
   outStream << "      if (mover.IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n";
   outStream << "      {\n";
   outStream << "         WsfP6DOF_Mover p6dofMover = (WsfP6DOF_Mover)mover;\n";
   outStream << "         p6dofMover.__getTestObject().CreateAutopilotSupportFileWithMaxMach(aFilename,aMaxMach);\n";
   outStream << "      }\n";
   outStream << "      else\n";
   outStream << "      {\n";
   outStream << "         writeln(\"--ERROR-- Not a P6DOF Mover.\");\n";
   outStream << "      }\n";
   outStream << "   end_script\n";
   outStream << "\n";
   outStream << "   p6dof_set_velocity_ned_fps  100 0 0\n";
   outStream << "   p6dof_position              0 0\n";
   outStream << "   p6dof_alt                   1000 ft\n";
   outStream << "\n";
   outStream << "   on_initialize2\n";
   outStream << "      writeln("");\n";
   outStream << "      writeln(\"Generating autopilot support file for " << mVehicleName.toUpper() << " ...\");\n";
   outStream << "      writeln("");\n";

   QString machString = QString::number(mVehiclePtr->GetAeroDataMaxMach());

   QString outputFilePath = ControlsFolderPath() + "/autopilot_support_tables.txt";
   outStream << "      CreateAutopilotSupportFileWithMaxMach(PLATFORM, \"" << outputFilePath << "\", " << machString << ");\n";

   outStream << "      writeln(\"Autopilot support file is complete.\");\n";
   outStream << "   end_on_initialize2\n";
   outStream << "\n";
   outStream << "end_platform\n";
   outStream << "\n";

   genFile.close();
   }

   void ScriptGeneratorP6DOF::CreateAeroFile(QString& aAeroDir)
   {
   VehicleAero* vehicleAero = mVehiclePtr->GetAero();
   if (vehicleAero == nullptr) { return; }

   QFile aeroFile(aAeroDir + "/aero.txt");
   if (!aeroFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   CreateAeroTables(aAeroDir);

   QStringList TwoDTablesList = {"cmq_mach_table",
                                 "cmp_mach_table",
                                 "cm_alphadot_mach_table",
                                 "cn_betadot_mach_table",
                                 "cnr_mach_table",
                                 "cnp_mach_table",
                                 "clp_mach_table",
                                 "cl_alphadot_mach_table",
                                 "cl_betadot_mach_table",
                                 "clr_mach_table",
                                 "clq_mach_table"
                                };

   QTextStream outStream(&aeroFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "# --------------------------------------------------------------------------------------------------\n";
   outStream << "# This file defines the aerodynamics of the vehicle.\n";
   outStream << "# The aero is composed of two types --primary aero (defined within the aero_data block) and\n";
   outStream << "# component aero (defined within aero_component blocks).\n";
   outStream << "# --------------------------------------------------------------------------------------------------\n\n";
   outStream << "# --------------------------------------------------------------------------------------------------\n";
   outStream << "# This is the 'primary aero' data block. It defines the aerodynamics that are not produced by\n";
   outStream << "# a control surface or other movable component on the vehicle. It also specifies reference areas\n";
   outStream << "# and other reference values.\n";
   outStream << "# --------------------------------------------------------------------------------------------------\n\n";
   outStream << "# **************************************************************************************************\n\n";
   outStream << "aero_data\n\n";

   int width = 15;
   int precision = 4;
   QTextStream::FieldAlignment alignment = QTextStream::FieldAlignment::AlignRight;

   outStream.setRealNumberNotation(QTextStream::FixedNotation);

   double wingChord_ft  = vehicleAero->GetVehicleWingChord_ft();
   double wingSpan_ft   = vehicleAero->GetVehicleWingSpan_ft();
   double wingArea_sqft = vehicleAero->GetVehicleWingArea_ft2();
   double refArea_sqft  = vehicleAero->GetVehicleRefArea_ft2();

   if (mVehiclePtr->IsAircraft())
   {
      outStream << "   wing_chord_ft    ";
      OutputData(outStream, wingChord_ft, width, precision, alignment);
      outStream << "\n";
      outStream << "   wing_span_ft     ";
      OutputData(outStream, wingSpan_ft, width, precision, alignment);
      outStream << "\n";
      outStream << "   wing_area_sqft   ";
      OutputData(outStream, wingArea_sqft, width, precision, alignment);
      outStream << "\n";
   }
   else if (mVehiclePtr->IsWeapon())
   {
      if (mVehiclePtr->GetGeometry().GetWing() != nullptr)
      {
         outStream << "   wing_chord_ft    ";
         OutputData(outStream, wingChord_ft, width, precision, alignment);
         outStream << "\n";
         outStream << "   wing_span_ft     ";
         OutputData(outStream, wingSpan_ft, width, precision, alignment);
         outStream << "\n";
         outStream << "   wing_area_sqft   ";
         OutputData(outStream, wingArea_sqft, width, precision, alignment);
         outStream << "\n";
      }
      else
      {
         outStream << "   ref_area_sqft    ";
         OutputData(outStream, refArea_sqft, width, precision, alignment);
         outStream << "\n";
      }
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle needs to be either an aircraft or a weapon!");
      return;
   }

   const std::vector<std::string> enabledTablesList = mVehiclePtr->GetAero()->GetCoreAero().GetEnabledTables();
   for (const auto& curTable : enabledTablesList)
   {
      QStringList tableStrList = QString::fromStdString(curTable).split(" ");
      QString tableFileName = "";
      QString tableKeyWord = "";
      for (auto curStr : tableStrList)
      {
         if (curStr == "vs")
         {
            continue;
         }
         else
         {
            if (tableFileName != "")
            {
               tableFileName.append("_");
               tableKeyWord.append("_");
            }

            if (curStr == "CL")
            {
               curStr = "cL";
               tableKeyWord.append(curStr);

               curStr = "cLift";
               tableFileName.append(curStr);
            }
            else if (curStr == "CLq")
            {
               curStr = "cLq";
               tableKeyWord.append(curStr);

               curStr = "cLiftq";
               tableFileName.append(curStr);
            }
            else if (curStr == "CL_alphadot")
            {
               curStr = "cL_alphadot";
               tableKeyWord.append(curStr);

               curStr = "cLift_alphadot";
               tableFileName.append(curStr);
            }
            else
            {
               curStr = curStr.toLower();
               tableKeyWord.append(curStr);
               tableFileName.append(curStr);
            }
         }
      }
      tableKeyWord.append("_table");
      tableFileName.append("_table.txt");
      outStream << "# --------------------------------------------------------------------------------------------------\n";
      outStream << "\n";
      outStream << "   " << tableKeyWord << "\n";

      bool isTwoD = TwoDTablesList.contains(tableKeyWord);
      if (!isTwoD)
      {
         outStream << "      file data/" << tableFileName << "\n";
      }
      else
      {
         if (tableKeyWord == "cmq_mach_table")
         {
            Create2dTableInline(&outStream, mVehiclePtr->GetAero()->GetCoreAero().GetCmq_MachTablePtr());
         }
         else if (tableKeyWord == "cmp_mach_table")
         {
            Create2dTableInline(&outStream, mVehiclePtr->GetAero()->GetCoreAero().GetCmp_MachTablePtr());
         }
         else if (tableKeyWord == "cm_alphadot_mach_table")
         {
            Create2dTableInline(&outStream, mVehiclePtr->GetAero()->GetCoreAero().GetCm_AlphaDotMachTablePtr());
         }
         else if (tableKeyWord == "cn_betadot_mach_table")
         {
            Create2dTableInline(&outStream, mVehiclePtr->GetAero()->GetCoreAero().GetCn_BetaDotMachTablePtr());
         }
         else if (tableKeyWord == "cnr_mach_table")
         {
            Create2dTableInline(&outStream, mVehiclePtr->GetAero()->GetCoreAero().GetCnr_MachTablePtr());
         }
         else if (tableKeyWord == "cnp_mach_table")
         {
            Create2dTableInline(&outStream, mVehiclePtr->GetAero()->GetCoreAero().GetCnp_MachTablePtr());
         }
         else if (tableKeyWord == "clp_mach_table")
         {
            Create2dTableInline(&outStream, mVehiclePtr->GetAero()->GetCoreAero().GetClp_MachTablePtr());
         }
         else if (tableKeyWord == "cl_alphadot_mach_table")
         {
            Create2dTableInline(&outStream, mVehiclePtr->GetAero()->GetCoreAero().GetCl_AlphaDotMachTablePtr());
         }
         else if (tableKeyWord == "cl_betadot_mach_table")
         {
            Create2dTableInline(&outStream, mVehiclePtr->GetAero()->GetCoreAero().GetCl_BetaDotMachTablePtr());
         }
         else if (tableKeyWord == "clr_mach_table")
         {
            Create2dTableInline(&outStream, mVehiclePtr->GetAero()->GetCoreAero().GetClr_MachTablePtr());
         }
         else if (tableKeyWord == "clq_mach_table")
         {
            Create2dTableInline(&outStream, mVehiclePtr->GetAero()->GetCoreAero().GetClq_MachTablePtr());
         }
      }
      outStream << "   end_" << tableKeyWord << "\n";
      outStream << "\n";
   }
   outStream << "# --------------------------------------------------------------------------------------------------\n";
   outStream << "\n";
   outStream << "end_aero_data\n";
   outStream << "\n";
   outStream << "# **************************************************************************************************\n";
   outStream << "\n";
   outStream << "# --------------------------------------------------------------------------------------------------\n";
   outStream << "# Listed below are the 'component aero' data blocks. These define the aerodynamics that are produced\n";
   outStream << "# by control surfaces or other movable components on the vehicle.\n";
   outStream << "\n";

   std::vector<VehicleAeroMovable> moveableComponentList = mVehiclePtr->GetAero()->GetMovableAeroList();
   for (auto curComponent : moveableComponentList)
   {
      QString curPart = QString::fromStdString(curComponent.GetName()).replace(" ", "_");
      const std::vector<std::string> enabledMovableTablesList = curComponent.GetEnabledTables();
      if (enabledMovableTablesList.size() == 0)
      {
         continue;
      }
      outStream << "# --------------------------------------------------------------------------------------------------\n\n";
      outStream << "aero_component\n";
      outStream << "   type                    " << curPart << "\n";
      outStream << "   #ref_area_sqft          12.0\n\n";
      for (const auto& curTable : enabledMovableTablesList)
      {
         QStringList tableStrList = QString::fromStdString(curTable).split(" ");
         QString tableFileName = "";
         QString tableKeyWord = "";
         for (auto curStr : tableStrList)
         {
            if (curStr == "vs")
            {
               continue;
            }
            else
            {
               if (tableFileName != "")
               {
                  tableFileName.append("_");
                  tableKeyWord.append("_");
               }

               if (curStr == "CL")
               {
                  curStr = "cL";
                  tableKeyWord.append(curStr);

                  curStr = "cLift";
                  tableFileName.append(curStr);
               }
               else if (curStr == "CLq")
               {
                  curStr = "cLq";
                  tableKeyWord.append(curStr);

                  curStr = "cLiftq";
                  tableFileName.append(curStr);
               }
               else if (curStr == "CL_alphadot")
               {
                  curStr = "cL_alphadot";
                  tableKeyWord.append(curStr);

                  curStr = "cLift_alphadot";
                  tableFileName.append(curStr);
               }
               else
               {
                  curStr = curStr.toLower();
                  tableKeyWord.append(curStr);
                  tableFileName.append(curStr);
               }
            }
         }
         tableKeyWord.append("_table");
         tableFileName.append("_table.txt");

         std::string tableName = tableKeyWord.toStdString();
         if ((tableName == "cd_angle_alpha_mach_table") || (tableName == "cd_angle_beta_mach_table"))
         {
            // Skip these tables

         }
         else
         {
            outStream << "   " << tableKeyWord << "\n";
            outStream << "      file data/" << curPart << "/" << tableFileName << "\n";
            outStream << "   end_" << tableKeyWord << "\n\n";
         }
      }
      outStream << "end_aero_component\n";
   }
   aeroFile.close();
   }

   void ScriptGeneratorP6DOF::CreateAeroTables(QString& aAeroDir)
   {
   QString aeroTablesDir = aAeroDir + "/data";

   //Create core aero table files

   // Lift
   Create4dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetCL_AlphaBetaMachTablePtr());
   Create3dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetCLq_AlphaMachTablePtr());
   Create3dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetCL_AlphaDotAlphaMachTablePtr());

   // Drag
   Create4dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetCd_AlphaBetaMachTablePtr());

   // Side force
   Create4dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetCY_AlphaBetaMachTablePtr());
   Create3dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetCYr_BetaMachTablePtr());
   Create3dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetCY_BetaDotBetaMachTablePtr());

   // Pitching moments -- Note that the 2D tables for Cmq, Cmp, and Cm-AlphaDot are not created here.
   // They are output directly in the aero file
   Create4dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetCm_AlphaBetaMachTablePtr());

   // Yawing moments -- Note that the 2D tables for Cn-BetaDot, Cnr, and Cnp are not created here.
   // They are output directly in the aero file
   Create4dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetCn_AlphaBetaMachTablePtr());

   // Rolling moments -- Note that the 2D tables for Clp, Cl-AlphaDot, Cl-BetaDot, Clr, and Clq) are not created here.
   // They are output directly in the aero file
   Create4dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetCl_AlphaBetaMachTablePtr());

   //Get the list of components and generate tables for each.
   std::vector<VehicleAeroMovable> moveableComponentList = mVehiclePtr->GetAero()->GetMovableAeroList();
   for (const auto& curComponent : moveableComponentList)
   {
      CreateAeroComponentTables(aeroTablesDir, curComponent);
   }
   }

   void ScriptGeneratorP6DOF::Create4dTableFile(const QString& aAeroDir, AeroTable4d* aTablePtr)
   {
   if (!aTablePtr)
   {
      return;
   }

   QString primaryDependentVarName(aTablePtr->GetDependentVarName().c_str());
   QString primaryIndependentVarName(aTablePtr->GetPrimaryIndependentVarName().c_str());
   QString secondaryIndependentVarName(aTablePtr->GetSecondaryIndependentVarName().c_str());
   QString tertiaryIndependentVarName(aTablePtr->GetTertiaryIndependentVarName().c_str());

   if (primaryDependentVarName == "CL")
   {
      primaryDependentVarName = "cLift";  //creating two files beginning with 'cL' or 'cl' is not allowed in Windows
   }
   else if (primaryDependentVarName == "CLq")
   {
      primaryDependentVarName = "cLiftq";  //creating two files beginning with 'cL' or 'cl' is not allowed in Windows
   }
   else if (primaryDependentVarName == "CL_alphadot")
   {
      primaryDependentVarName = "cLift_alphadot";
   }
   else
   {
      primaryDependentVarName = primaryDependentVarName.toLower();
   }
   primaryIndependentVarName = primaryIndependentVarName.toLower();
   secondaryIndependentVarName = secondaryIndependentVarName.toLower();
   tertiaryIndependentVarName = tertiaryIndependentVarName.toLower();

   QString fileName = "/" + primaryDependentVarName + "_" + primaryIndependentVarName + "_" + secondaryIndependentVarName + "_" + tertiaryIndependentVarName + "_table.txt";

   QFile aeroFile(aAeroDir + fileName);
   if (!aeroFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return;
   }

   if (tertiaryIndependentVarName == "angle")
   {
      tertiaryIndependentVarName = "surface_angle";
   }
   else if (secondaryIndependentVarName == "angle")
   {
      secondaryIndependentVarName = "surface_angle";
   }
   else if (primaryIndependentVarName == "angle")
   {
      primaryIndependentVarName = "surface_angle";
   }

   QTextStream outStream(&aeroFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream.setRealNumberPrecision(17);
   outStream << "regular_table\n";
   outStream << "\n";
   if (tertiaryIndependentVarName != "mach")
   {
      outStream << "independent_variable " << tertiaryIndependentVarName << " units rad\n";
   }
   else
   {
      outStream << "independent_variable " << tertiaryIndependentVarName << " precision double\n";
   }
   const std::vector<double> tertiaryValues = aTablePtr->GetTertiaryIndepenentValues();
   for (auto curTertiary : tertiaryValues)
   {
      if (tertiaryIndependentVarName == "alpha" || tertiaryIndependentVarName == "beta")
      {
         outStream << curTertiary* UtMath::cRAD_PER_DEG << " ";
      }
      else
      {
         outStream << curTertiary << " ";
      }
   }
   outStream << "\n";
   outStream << "end_independent_variable\n";
   outStream << "\n";

   if (secondaryIndependentVarName == "surface_angle")
   {
      outStream << "independent_variable " << secondaryIndependentVarName << " units deg\n";
   }
   else
   {
      outStream << "independent_variable " << secondaryIndependentVarName << " units rad\n";
   }
   const std::vector<double> secondaryValues = aTablePtr->GetSecondaryIndependentValues();
   for (auto curSecondary : secondaryValues)
   {
      if (secondaryIndependentVarName == "alpha" || secondaryIndependentVarName == "beta")
      {
         outStream << curSecondary* UtMath::cRAD_PER_DEG << " ";
      }
      else
      {
         outStream << curSecondary << " ";
      }
   }
   outStream << "\n";
   outStream << "end_independent_variable\n";
   outStream << "\n";

   if (primaryIndependentVarName == "surface_angle")
   {
      outStream << "independent_variable " << primaryIndependentVarName << " units deg\n";
   }
   else
   {
      outStream << "independent_variable " << primaryIndependentVarName << " units rad\n";
   }
   const std::vector<double> primaryValues = aTablePtr->GetPrimaryIndependentValues();
   for (auto curPrimary : primaryValues)
   {
      if (primaryIndependentVarName == "alpha" || primaryIndependentVarName == "beta")
      {
         outStream << curPrimary* UtMath::cRAD_PER_DEG << " ";
      }
      else
      {
         outStream << curPrimary << " ";
      }
   }
   outStream << "\n";
   outStream << "end_independent_variable\n";
   outStream << "\n";

   outStream << "dependent_variable precision double\n";
   for (auto curTertiary : tertiaryValues)
   {
      for (auto curSecondary : secondaryValues)
      {
         std::vector<std::pair<double, double>> curData;
         aTablePtr->GetPrimaryModifiedData(curSecondary, curTertiary, curData);
         for (auto curPair : curData)
         {
            outStream << curPair.second << " ";
         }
      }
   }
   outStream << "\n";
   outStream << "end_dependent_variable\n";
   outStream << "\n";
   outStream << "end_regular_table\n";
   }

   void ScriptGeneratorP6DOF::Create3dTableFile(const QString& aAeroDir, AeroTable3d* aTablePtr)
   {
   if (!aTablePtr) { return; }

   QString primaryDependentVarName(aTablePtr->GetDependentVarName().c_str());
   QString primaryIndependentVarName(aTablePtr->GetPrimaryIndependentVarName().c_str());
   QString secondaryIndependentVarName(aTablePtr->GetSecondaryIndependentVarName().c_str());

   if (primaryDependentVarName == "CL")
   {
      primaryDependentVarName = "cLift";  //creating two files beginning with 'cL' or 'cl' is not allowed in Windows
   }
   else if (primaryDependentVarName == "CLq")
   {
      primaryDependentVarName = "cLiftq";  //creating two files beginning with 'cL' or 'cl' is not allowed in Windows
   }
   else if (primaryDependentVarName == "CL_alphadot")
   {
      primaryDependentVarName = "cLift_alphadot";
   }
   else
   {
      primaryDependentVarName = primaryDependentVarName.toLower();
   }

   primaryIndependentVarName = primaryIndependentVarName.toLower();
   secondaryIndependentVarName = secondaryIndependentVarName.toLower();

   QString fileName = "/" + primaryDependentVarName + "_" + primaryIndependentVarName + "_" + secondaryIndependentVarName + "_table.txt";

   QFile aeroFile(aAeroDir + fileName);
   if (!aeroFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   if (secondaryIndependentVarName == "angle")
   {
      secondaryIndependentVarName = "surface_angle";
   }
   else if (primaryIndependentVarName == "angle")
   {
      primaryIndependentVarName = "surface_angle";
   }

   QTextStream outStream(&aeroFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream.setRealNumberPrecision(17);
   outStream << "regular_table\n";
   outStream << "\n";

   if (secondaryIndependentVarName != "mach")
   {
      outStream << "independent_variable " << secondaryIndependentVarName << " units rad\n";
   }
   else
   {
      outStream << "independent_variable " << secondaryIndependentVarName << " precision double\n";
   }
   const std::vector<double> secondaryValues = aTablePtr->GetSecondaryIndependentValues();
   for (auto curSecondary : secondaryValues)
   {
      if (secondaryIndependentVarName == "alpha" || secondaryIndependentVarName == "beta")
      {
         outStream << curSecondary* UtMath::cRAD_PER_DEG << " ";
      }
      else
      {
         outStream << curSecondary << " ";
      }
   }
   outStream << "\n";
   outStream << "end_independent_variable\n";
   outStream << "\n";

   if (primaryIndependentVarName == "surface_angle")
   {
      outStream << "independent_variable " << primaryIndependentVarName << " units deg\n";
   }
   else
   {
      outStream << "independent_variable " << primaryIndependentVarName << " units rad\n";
   }
   const std::vector<double> primaryValues = aTablePtr->GetPrimaryIndependentValues();
   for (auto curPrimary : primaryValues)
   {
      if (primaryIndependentVarName == "alpha" || primaryIndependentVarName == "beta")
      {
         outStream << curPrimary* UtMath::cRAD_PER_DEG << " ";
      }
      else
      {
         outStream << curPrimary << " ";
      }
   }
   outStream << "\n";
   outStream << "end_independent_variable\n";
   outStream << "\n";

   outStream << "dependent_variable precision double\n";
   for (auto curSecondary : secondaryValues)
   {
      std::vector<std::pair<double, double>> curData;
      aTablePtr->GetPrimaryModifiedData(curSecondary, curData);
      for (auto curPair : curData)
      {
         outStream << curPair.second << " ";
      }
   }
   outStream << "\n";
   outStream << "end_dependent_variable\n";
   outStream << "\n";
   outStream << "end_regular_table\n";
   }

   void ScriptGeneratorP6DOF::Create2dTableInline(QTextStream* aStreamPtr, AeroTable2d* aTablePtr)
   {
   aStreamPtr->setRealNumberPrecision(17);
   std::vector<std::pair<double, double>> curData;
   aTablePtr->GetModifiedData(curData);
   for (auto curPrimary : curData)
   {
      *aStreamPtr << "      ";
      OutputData(*aStreamPtr, curPrimary.first, 8, 4, QTextStream::FieldAlignment::AlignRight);
      *aStreamPtr << "  ";
      OutputData(*aStreamPtr, curPrimary.second, 24, 15, QTextStream::FieldAlignment::AlignRight);
      *aStreamPtr << "\n";
   }

   aStreamPtr->flush();
   }

   void ScriptGeneratorP6DOF::CreateAeroComponentTables(QString& aAeroDir, VehicleAeroMovable aComponent)
   {
   // For each component write out their tables...
   const std::vector<std::string> enabledTablesList = aComponent.GetEnabledTables();

   if (enabledTablesList.size() == 0)
   {
      // No enabled tables, do nothing
      return;
   }

   QDir aeroTablesDir(aAeroDir);
   QString componentNameStr = QString::fromStdString(aComponent.GetName()).replace(" ", "_");
   aeroTablesDir.mkdir(componentNameStr);
   aeroTablesDir.cd(componentNameStr);

   for (const auto& curTableStr : enabledTablesList)
   {
      // Lift force
      if (curTableStr == "CL vs Angle Alpha Mach")
      {
         Create4dTableFile(aeroTablesDir.absolutePath(), aComponent.GetCL_AngleAlphaMachTablePtr());
      }

      // Drag force
      else if (curTableStr == "Cd vs Angle Mach")
      {
         Create3dTableFile(aeroTablesDir.absolutePath(), aComponent.GetCd_AngleMachTablePtr()); // This should be used when drag is a function of alpha and beta
      }

      // Side force
      else if (curTableStr == "CY vs Angle Beta Mach")
      {
         Create4dTableFile(aeroTablesDir.absolutePath(), aComponent.GetCY_AngleBetaMachTablePtr());
      }

      // Pitching moment
      else if (curTableStr == "Cm vs Angle Alpha Mach")
      {
         Create4dTableFile(aeroTablesDir.absolutePath(), aComponent.GetCm_AngleAlphaMachTablePtr());
      }

      // Yawing moment
      else if (curTableStr == "Cn vs Angle Beta Mach")
      {
         Create4dTableFile(aeroTablesDir.absolutePath(), aComponent.GetCn_AngleBetaMachTablePtr());
      }

      // Rolling moment
      else if (curTableStr == "Cl vs Angle Alpha Beta")
      {
         Create4dTableFile(aeroTablesDir.absolutePath(), aComponent.GetCl_AngleAlphaBetaTablePtr());
      }

      // Pitching moment due to pitch rate
      else if (curTableStr == "Cmq vs Angle Mach")
      {
         Create3dTableFile(aeroTablesDir.absolutePath(), aComponent.GetCmq_AngleMachTablePtr());
      }

      // Yaw moment due to yaw rate
      else if (curTableStr == "Cnr vs Angle Mach")
      {
         Create3dTableFile(aeroTablesDir.absolutePath(), aComponent.GetCnr_AngleMachTablePtr());
      }

      // Roll moment due to roll rate
      else if (curTableStr == "Clp vs Angle Mach")
      {
         Create3dTableFile(aeroTablesDir.absolutePath(), aComponent.GetClp_AngleMachTablePtr());
      }

      // Roll moment due to pitch rate
      else if (curTableStr == "Clq vs Angle Mach")
      {
         Create3dTableFile(aeroTablesDir.absolutePath(), aComponent.GetClq_AngleMachTablePtr());
      }

      // Roll moment due to yaw rate
      else if (curTableStr == "Clr vs Angle Mach")
      {
         Create3dTableFile(aeroTablesDir.absolutePath(), aComponent.GetClr_AngleMachTablePtr());
      }

   }
   }

   void ScriptGeneratorP6DOF::CreateStubAutopilotSupportFile(QString& aControlsDir)
   {
   QFile apSupportFile(aControlsDir + "/autopilot_support_tables.txt");
   if (!apSupportFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   QTextStream outStream(&apSupportFile);

   CreateAutogenHeader(mVehiclePtr, outStream);
   outStream << "autopilot_support_tables\n";
   outStream << "\n";
   outStream << "   cl_max_mach_table\n";
   outStream << "      0.000    0.934\n";
   outStream << "      1.000    0.931\n";
   outStream << "      50.000   0.909\n";
   outStream << "   end_cl_max_mach_table\n";
   outStream << "\n";
   outStream << "   cl_min_mach_table\n";
   outStream << "      0.000   -0.934\n";
   outStream << "      1.000   -0.931\n";
   outStream << "      50.000  -0.909\n";
   outStream << "   end_cl_min_mach_table\n";
   outStream << "\n";
   outStream << "   alpha_max_mach_table\n";
   outStream << "      0.000    26.010\n";
   outStream << "      1.000    26.010\n";
   outStream << "      50.000   26.010\n";
   outStream << "   end_alpha_max_mach_table\n";
   outStream << "\n";
   outStream << "   alpha_min_mach_table\n";
   outStream << "      0.000   -26.010\n";
   outStream << "      1.000   -26.010\n";
   outStream << "      50.000  -26.010\n";
   outStream << "   end_alpha_min_mach_table\n";
   outStream << "\n";
   outStream << "   alpha_versus_mach_cl_table\n";
   outStream << "      irregular_table\n";
   outStream << "      independent_variable mach precision float\n";
   outStream << "      independent_variable cl precision float\n";
   outStream << "      dependent_variable precision float\n";
   outStream << "      mach   0.00\n";
   outStream << "         cl     -0.9340  0.000  0.9340\n";
   outStream << "         values -26.010  0.000  26.010\n";
   outStream << "      mach   1.00\n";
   outStream << "         cl     -0.9340  0.000  0.9340\n";
   outStream << "         values -26.010  0.000  26.010\n";
   outStream << "      mach  50.00\n";
   outStream << "         cl     -0.9340  0.000  0.9340\n";
   outStream << "         values -26.010  0.000  26.010\n";
   outStream << "      end_irregular_table\n";
   outStream << "   end_alpha_versus_mach_cl_table\n";
   outStream << "\n";
   outStream << "   stick_zero_moment_mach_alpha_table\n";
   outStream << "      irregular_table\n";
   outStream << "      independent_variable mach precision float\n";
   outStream << "      independent_variable alpha units deg\n";
   outStream << "      dependent_variable precision float\n";
   outStream << "      mach   0.00\n";
   outStream << "         alpha  -26.00   0.000    26.00\n";
   outStream << "         values -0.828   0.000    0.828\n";
   outStream << "      mach   1.00\n";
   outStream << "         alpha  -26.00   0.000    26.00\n";
   outStream << "         values -0.828   0.000    0.828\n";
   outStream << "      mach  50.00\n";
   outStream << "         alpha  -26.00   0.000    26.00\n";
   outStream << "         values -0.828   0.000    0.828\n";
   outStream << "      end_irregular_table\n";
   outStream << "   end_stick_zero_moment_mach_alpha_table\n";
   outStream << "\n";
   outStream << "   effective_CL_versus_mach_alpha_table\n";
   outStream << "      irregular_table\n";
   outStream << "      independent_variable mach precision float\n";
   outStream << "      independent_variable alpha units deg\n";
   outStream << "      dependent_variable precision float\n";
   outStream << "      mach   0.00\n";
   outStream << "         alpha  -26.00  0.000   26.01\n";
   outStream << "         values -0.934  0.000   0.934\n";
   outStream << "      mach   1.00\n";
   outStream << "         alpha  -26.00  0.000   26.01\n";
   outStream << "         values -0.934  0.000   0.934\n";
   outStream << "      mach  50.00\n";
   outStream << "         alpha  -26.00  0.000   26.01\n";
   outStream << "         values -0.934  0.000   0.934\n";
   outStream << "      end_irregular_table\n";
   outStream << "   end_effective_CL_versus_mach_alpha_table\n";
   outStream << "\n";
   outStream << "end_autopilot_support_tables\n";
   outStream << "\n";

   apSupportFile.close();
   }

   void ScriptGeneratorP6DOF::CreateFlightControlsFile(QString& aControlsDir)
   {
   QFile controlFile(aControlsDir + "/flight_controls.txt");
   if (!controlFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   QTextStream outStream(&controlFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "flight_controls\n";
   outStream << "\n";
   outStream << "mapping_table SignalMappingTableName\n";
   outStream << "   type signal_mapping\n";
   outStream << "   table_data\n";
   outStream << "      irregular_table\n";
   outStream << "         independent_variable control_value precision float\n";
   outStream << "         independent_variable input         precision float\n";
   outStream << "         dependent_variable                 precision float\n";
   outStream << "         control_value 0.0\n";
   outStream << "            input -1.0 -0.5  0.0  0.5  1.0\n";
   outStream << "            values -0.8 -0.4  0.0  0.4  0.8\n";
   outStream << "         control_value 1.0\n";
   outStream << "            input -1.0 -0.5  0.0  0.5  1.0\n";
   outStream << "            values -0.8 -0.4  0.0  0.4  0.8\n";
   outStream << "         control_value 2.0\n";
   outStream << "            input -1.0 -0.5  0.0  0.5  1.0\n";
   outStream << "            values -0.8 -0.4  0.0  0.4  0.8\n";
   outStream << "      end_irregular_table\n";
   outStream << "   end_table_data\n";
   outStream << "end_mapping_table\n";
   outStream << "\n";
   outStream << "scalar_gain NegativeGain\n";
   outStream << "   gain -1.0\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain HalfGain\n";
   outStream << "   gain 0.5\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain DoubleGain\n";
   outStream << "   gain 2.0\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain Gain60\n";
   outStream << "   gain 60.0\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain Gain90\n";
   outStream << "   gain 90.0\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain UnityGain\n";
   outStream << "   gain 1.0\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain ZeroGain\n";
   outStream << "   gain 0.0\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain TenthGain\n";
   outStream << "   gain 0.1\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain Gain90Percent\n";
   outStream << "   gain 0.9\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain Gain80Percent\n";
   outStream << "   gain 0.8\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain Gain70Percent\n";
   outStream << "   gain 0.7\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain Gain60Percent\n";
   outStream << "   gain 0.6\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain Gain40Percent\n";
   outStream << "   gain 0.4\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain Gain30Percent\n";
   outStream << "   gain 0.3\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain Gain20Percent\n";
   outStream << "   gain 0.2\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain Gain10Percent\n";
   outStream << "   gain 0.1\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain Gain5Percent\n";
   outStream << "   gain 0.05\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "clamp_gain Clamp_PosNeg\n";
   outStream << "   min_clamp -1.0\n";
   outStream << "   max_clamp  1.0\n";
   outStream << "end_clamp_gain\n";
   outStream << "\n";
   outStream << "clamp_gain Clamp_ZeroOne\n";
   outStream << "   min_clamp  0.0\n";
   outStream << "   max_clamp  1.0\n";
   outStream << "end_clamp_gain\n";
   outStream << "\n";
   outStream << "clamp_gain Clamp_NegOneZero\n";
   outStream << "   min_clamp  -1.0\n";
   outStream << "   max_clamp  0.0\n";
   outStream << "end_clamp_gain\n";
   outStream << "\n";
   outStream << "clamp_gain Clamp_Zero90\n";
   outStream << "   min_clamp  0.0\n";
   outStream << "   max_clamp  90.0\n";
   outStream << "end_clamp_gain\n";
   outStream << "\n";
   outStream << "clamp_gain Clamp_Zero60\n";
   outStream << "   min_clamp  0.0\n";
   outStream << "   max_clamp  60.0\n";
   outStream << "end_clamp_gain\n";
   outStream << "\n";
   outStream << "clamp_gain TestItem\n";
   outStream << "   min_clamp  0.0\n";
   outStream << "   max_clamp  2.0\n";
   outStream << "end_clamp_gain\n";
   outStream << "\n";
   outStream << "gain_table MachGainTableName\n";
   outStream << "  type mach_gain\n";
   outStream << "  simple_table\n";
   outStream << "   0.00          1.0\n";
   outStream << "   1.00          0.5\n";
   outStream << "   2.00          0.1\n";
   outStream << "  end_simple_table\n";
   outStream << "end_gain_table\n";
   outStream << "\n";
   outStream << "gain_table AlphaGainTableName\n";
   outStream << "  type alpha_gain\n";
   outStream << "  simple_table\n";
   outStream << "   -12.0         0.0\n";
   outStream << "   -10.0         1.0\n";
   outStream << "   0.00          1.0\n";
   outStream << "   10.0          1.0\n";
   outStream << "   12.0          0.8\n";
   outStream << "   20.0          0.1\n";
   outStream << "   30.0          0.0\n";
   outStream << "  end_simple_table\n";
   outStream << "end_gain_table\n";
   outStream << "\n";
   outStream << "gain_table BetaGainTableName\n";
   outStream << "  type beta_gain\n";
   outStream << "  simple_table\n";
   outStream << "   0.00          1.0\n";
   outStream << "   1.00          0.5\n";
   outStream << "   2.00          0.1\n";
   outStream << "  end_simple_table\n";
   outStream << "end_gain_table\n";
   outStream << "\n";

   // Output the control data for each movable component
   OutputControlDataForMovableComponents(outStream);

   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_boolean NWS_EnableActive\n";
   outStream << "  current_value              0\n";
   outStream << "  threshold_value            0.5\n";
   outStream << "  inputs\n";
   outStream << "   control_input NWS_Enable\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << "end_control_boolean\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_value Wheel_Brake_Rgt\n";
   outStream << " min_value                  0.0\n";
   outStream << " max_value                  1.0\n";
   outStream << " current_value              0.0\n";
   outStream << "  inputs\n";
   outStream << "   control_input RgtWheelBrake\n";
   outStream << "    modifier                 Clamp_ZeroOne\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << " end_control_value\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_value LeftWheelBrake\n";
   outStream << "  min_value                  0.0\n";
   outStream << "  max_value                  1.0\n";
   outStream << "  current_value              0.0\n";
   outStream << "  inputs\n";
   outStream << "   control_input LftWheelBrake\n";
   outStream << "    modifier                 Clamp_ZeroOne\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << " end_control_value\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_value ParkingBrake\n";
   outStream << "  min_value                  0.0\n";
   outStream << "  max_value                  1.0\n";
   outStream << "  current_value              0.0\n";
   outStream << "  inputs\n";
   outStream << "   control_input SpeedBrakesOut\n";
   outStream << "    modifier                 Clamp_ZeroOne\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << " end_control_value\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_value ThrottleMilitary\n";
   outStream << "  min_value                  0.0\n";
   outStream << "  max_value                  1.0\n";
   outStream << "  current_value              0.0\n";
   outStream << "  inputs\n";
   outStream << "   control_input ThrottleMil\n";
   outStream << "    modifier                 Clamp_ZeroOne\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << " end_control_value\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_value ThrottleAfterburner\n";
   outStream << "  min_value                  0.0\n";
   outStream << "  max_value                  1.0\n";
   outStream << "  current_value              0.0\n";
   outStream << "  inputs\n";
   outStream << "   control_input ThrottleAB\n";
   outStream << "    modifier                 Clamp_ZeroOne\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << " end_control_value\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_value ThrottleThrustReverser\n";
   outStream << "  min_value                  0.0\n";
   outStream << "  max_value                  1.0\n";
   outStream << "  current_value              0.0\n";
   outStream << "  inputs\n";
   outStream << "   control_input ThrustReverser\n";
   outStream << "    modifier                 Clamp_ZeroOne\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << " end_control_value\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_value ThrustVectoringYaw\n";

   VehiclePropulsion* propulsion = mVehiclePtr->GetPropulsion();

   if (propulsion)
   {
      Designer::VehicleGeometry& vehicleGeometry = mVehiclePtr->GetGeometry();
      GeometryPropulsionData* propData = vehicleGeometry.GetPropulsionData();

      if (propData)
      {
         double minimumYawThrustVectorAngle;
         double maximumYawThrustVectorAngle;

         if (propData->GetMinimumThrustVectoringYawAngle_deg(minimumYawThrustVectorAngle))
         {
            outStream << "  min_value                  " << minimumYawThrustVectorAngle << "\n";
         }
         else
         {
            outStream << "  min_value                  0.0\n";
         }

         if (propData->GetMaximumThrustVectoringYawAngle_deg(maximumYawThrustVectorAngle))
         {
            outStream << "  max_value                  " << maximumYawThrustVectorAngle << "\n";
         }
         else
         {
            outStream << "  max_value                  0.0\n";
         }
      }
   }

   outStream << "  current_value              0.0\n";
   outStream << "  inputs\n";
   outStream << "   control_input RudderRight\n";
   outStream << "    modifier                 Gain60\n";
   outStream << "    modifier                 NegativeGain\n";
   outStream << "    modifier                 DoubleGain\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << " end_control_value\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_value ThrustVectoringPitch\n";

   if (propulsion)
   {
      Designer::VehicleGeometry& vehicleGeometry = mVehiclePtr->GetGeometry();
      GeometryPropulsionData* propData = vehicleGeometry.GetPropulsionData();

      if (propData)
      {
         double minimumPitchThrustVectorAngle;
         double maximumPitchThrustVectorAngle;

         if (propData->GetMinimumThrustVectoringPitchAngle_deg(minimumPitchThrustVectorAngle))
         {
            outStream << "  min_value                  " << minimumPitchThrustVectorAngle << "\n";
         }
         else
         {
            outStream << "  min_value                  0.0\n";
         }

         if (propData->GetMaximumThrustVectoringPitchAngle_deg(maximumPitchThrustVectorAngle))
         {
            outStream << "  max_value                  " << maximumPitchThrustVectorAngle << "\n";
         }
         else
         {
            outStream << "  max_value                  0.0\n";
         }
      }
   }

   outStream << "  current_value              0.0\n";
   outStream << "  inputs\n";
   outStream << "   control_input StickBack\n";
   outStream << "    modifier                 Gain60\n";
   outStream << "    modifier                 NegativeGain\n";
   outStream << "    modifier                 DoubleGain\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << " end_control_value\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_value ThrustVectoringRoll\n";
   outStream << "  min_value                  -1.0\n";
   outStream << "  max_value                  1.0\n";
   outStream << "  current_value              0.0\n";
   outStream << "  inputs\n";
   outStream << "   control_input ThrustVectorRoll\n";
   outStream << "    modifier                 Clamp_PosNeg\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << " end_control_value\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_boolean TriggerPulled\n";
   outStream << "  current_value              0\n";
   outStream << "  threshold_value            0.5\n";
   outStream << "  inputs\n";
   outStream << "   control_input TriggerPulled\n";
   outStream << "    modifier                 Clamp_ZeroOne\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << " end_control_boolean\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_boolean ButtonPressed\n";
   outStream << "  current_value              0\n";
   outStream << "  threshold_value            0.5\n";
   outStream << "  inputs\n";
   outStream << "   control_input ButtonPressed\n";
   outStream << "    modifier                 Clamp_ZeroOne\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << " end_control_boolean\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << "end_flight_controls\n";
   outStream << "\n";

   controlFile.close();
   }

   void ScriptGeneratorP6DOF::OutputControlDataForMovableComponents(QTextStream& aOutputStream)
   {
   // The concept of "hooking up controls" is to first determine the type of control configuration
   Vehicle::VehicleControlConfig controlConfig = mVehiclePtr->GetVehicleControlConfiguration();

   switch (controlConfig)
   {
      case Vehicle::VehicleControlConfig::cCONFIG_STABS_ONE_TAIL:
      case Vehicle::VehicleControlConfig::cCONFIG_STABS_TWO_TAILS:
         OutputConfigStabsVerticalTail(aOutputStream);
         break;
      case Vehicle::VehicleControlConfig::cCONFIG_STABS_ONE_VENTRAL_TAIL:
      case Vehicle::VehicleControlConfig::cCONFIG_STABS_TWO_VENTRAL_TAILS:
         OutputConfigStabsVentralTail(aOutputStream);
         break;
      case Vehicle::VehicleControlConfig::cCONFIG_STABS_UPPER_LOWER_TAILS:
         OutputConfigStabsUpperLowerTails(aOutputStream);
         break;
      case Vehicle::VehicleControlConfig::cCONFIG_VTAIL:
      case Vehicle::VehicleControlConfig::cCONFIG_INVERTED_VTAIL:
         OutputConfigVTail(aOutputStream);
         break;
      case Vehicle::VehicleControlConfig::cCONFIG_CANARDS_ONE_TAIL:
      case Vehicle::VehicleControlConfig::cCONFIG_CANARDS_TWO_TAILS:
         OutputConfigCanardsVerticalTail(aOutputStream);
         break;
      case Vehicle::VehicleControlConfig::cCONFIG_CANARDS_STABS_ONE_TAIL:
      case Vehicle::VehicleControlConfig::cCONFIG_CANARDS_STABS_TWO_TAILS:
         OutputConfigCanardsStabsVerticalTail(aOutputStream);
         break;
      case Vehicle::VehicleControlConfig::cCONFIG_TAIL_FINS:
         OutputConfigTailFins(aOutputStream);
         break;
      case Vehicle::VehicleControlConfig::cCONFIG_MID_FINS:
         OutputConfigMidFins(aOutputStream);
         break;
      case Vehicle::VehicleControlConfig::cCONFIG_CANARD_FINS:
         OutputConfigCanardFins(aOutputStream);
         break;
      case Vehicle::VehicleControlConfig::cCONFIG_FLYING_WING:
         OutputConfigFlyingWing(aOutputStream);
         break;
      case Vehicle::VehicleControlConfig::cCONFIG_STABS_FLYING_WING:
         OutputConfigStabsFlyingWing(aOutputStream);
         break;
      case Vehicle::VehicleControlConfig::cNO_CONTROL:
         break;
      default:
         ut::log::error() << "Unknown control config.";
   }

   // Output speed brakes (this function will check internally whether speed brakes are present)
   OutputSpeedBrakes(aOutputStream);

   // Output landing gear (this function will check internally whether landing gear are present)
   OutputLandingGear(aOutputStream);
   }

   void ScriptGeneratorP6DOF::OutputConfigStabsVerticalTail(QTextStream& aOutputStream)
   {
   VehicleGeometry& geometry = mVehiclePtr->GetGeometry();

   // Get controls
   VehicleControls& controls = mVehiclePtr->GetControls();

   // Verify bank-to-turn
   if (!mVehiclePtr->UsesBankToTurnControl())
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks bank-to-turn control!");
      return;
   }

   if (!OutputVerticalTailControl(aOutputStream, geometry, controls))
   {
      return;
   }
   if (!OutputHorizontalTailControl(aOutputStream, geometry, controls))
   {
      return;
   }
   OutputWingControl(aOutputStream, geometry, controls);
   }

   void ScriptGeneratorP6DOF::OutputConfigStabsVentralTail(QTextStream& aOutputStream)
   {
   VehicleGeometry& geometry = mVehiclePtr->GetGeometry();

   // Get controls
   VehicleControls& controls = mVehiclePtr->GetControls();

   // Verify bank-to-turn
   if (!mVehiclePtr->UsesBankToTurnControl())
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks bank-to-turn control!");
      return;
   }

   if (!OutputVentralTailControl(aOutputStream, geometry, controls))
   {
      return;
   }
   if (!OutputHorizontalTailControl(aOutputStream, geometry, controls))
   {
      return;
   }
   OutputWingControl(aOutputStream, geometry, controls);
   }

   void ScriptGeneratorP6DOF::OutputConfigStabsUpperLowerTails(QTextStream& aOutputStream)
   {
   VehicleGeometry& geometry = mVehiclePtr->GetGeometry();

   // Get controls
   VehicleControls& controls = mVehiclePtr->GetControls();

   // Verify bank-to-turn
   if (!mVehiclePtr->UsesBankToTurnControl())
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks bank-to-turn control!");
      return;
   }

   if (!OutputVerticalTailControl(aOutputStream, geometry, controls))
   {
      return;
   }
   if (!OutputVentralTailControl(aOutputStream, geometry, controls))
   {
      return;
   }
   if (!OutputHorizontalTailControl(aOutputStream, geometry, controls))
   {
      return;
   }
   OutputWingControl(aOutputStream, geometry, controls);
   }

   void ScriptGeneratorP6DOF::OutputConfigVTail(QTextStream& aOutputStream)
   {
   VehicleGeometry& geometry = mVehiclePtr->GetGeometry();

   // Get controls
   VehicleControls& controls = mVehiclePtr->GetControls();

   // Verify bank-to-turn
   if (!mVehiclePtr->UsesBankToTurnControl())
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks bank-to-turn control!");
      return;
   }

   if (!OutputVTailControl(aOutputStream, geometry, controls))
   {
      return;
   }
   OutputWingControl(aOutputStream, geometry, controls);
   }

   void ScriptGeneratorP6DOF::OutputConfigCanardsVerticalTail(QTextStream& aOutputStream)
   {
   VehicleGeometry& geometry = mVehiclePtr->GetGeometry();

   // Get controls
   VehicleControls& controls = mVehiclePtr->GetControls();

   // Verify bank-to-turn
   if (!mVehiclePtr->UsesBankToTurnControl())
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks bank-to-turn control!");
      return;
   }

   if (!OutputVerticalTailControl(aOutputStream, geometry, controls))
   {
      return;
   }
   if (!OutputCanardControl(aOutputStream, geometry, controls))
   {
      return;
   }
   OutputWingControl(aOutputStream, geometry, controls);
   }

   void ScriptGeneratorP6DOF::OutputConfigCanardsStabsVerticalTail(QTextStream& aOutputStream)
   {
   VehicleGeometry& geometry = mVehiclePtr->GetGeometry();

   // Get controls
   VehicleControls& controls = mVehiclePtr->GetControls();

   // Verify bank-to-turn
   if (!mVehiclePtr->UsesBankToTurnControl())
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks bank-to-turn control!");
      return;
   }

   if (!OutputVerticalTailControl(aOutputStream, geometry, controls))
   {
      return;
   }
   if (!OutputHorizontalTailControl(aOutputStream, geometry, controls))
   {
      return;
   }
   if (!OutputCanardControl(aOutputStream, geometry, controls))
   {
      return;
   }
   OutputWingControl(aOutputStream, geometry, controls);
   }

   void ScriptGeneratorP6DOF::OutputConfigFlyingWing(QTextStream& aOutputStream)
   {
   VehicleGeometry& geometry = mVehiclePtr->GetGeometry();

   // Get controls
   VehicleControls& controls = mVehiclePtr->GetControls();

   OutputWingControl(aOutputStream, geometry, controls);
   }

   void ScriptGeneratorP6DOF::OutputConfigStabsFlyingWing(QTextStream& aOutputStream)
   {
   VehicleGeometry& geometry = mVehiclePtr->GetGeometry();

   // Get controls
   VehicleControls& controls = mVehiclePtr->GetControls();

   if (!OutputHorizontalTailControl(aOutputStream, geometry, controls))
   {
      return;
   }

   OutputWingControl(aOutputStream, geometry, controls);
   }

   bool ScriptGeneratorP6DOF::OutputAileronControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
   {
   GeometryWing* wing = geometry.GetWing();
   // We check for a null wing pointer in OutputWingControl

   if (!wing->GetAileronsPresent())
   {
      return false;
   }

   // Setup aileron data
   double minAngle_deg                = wing->GetAileronsControlSurfaceMinAngle_deg();
   double maxAngle_deg                = wing->GetAileronsControlSurfaceMaxAngle_deg();
   double currentAngle_deg            = 0.0;
   double minActuatorAngle_deg        = wing->GetAileronsActuatorMinAngle_deg();
   double maxActuatorAngle_deg        = wing->GetAileronsActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = wing->GetAileronsActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = wing->GetAileronsActuatorMaxRate_dps();
   double currentActuatorAngle_deg    = 0.0;
   bool useExpo                       = wing->GetAileronsUseExponentialAngleMapping();

   // Set alpha control gain to false by default
   VehicleControls::VehicleControlBankToTurnRollControl bankControl = VehicleControls::VehicleControlBankToTurnRollControl::cAILERONS_ONLY;
   VehicleControls::VehicleControlBankToTurnYawControlType yawControlType = VehicleControls::VehicleControlBankToTurnYawControlType::cNO_YAW_CONTROL;
   bool useAlphaControlGain = false;

   controls.GetBankToTurnControlSettings(bankControl, yawControlType, useAlphaControlGain);

   bool useStickBack = false;
   bool useRudderRight = false;
   bool useStickRight = false;
   bool useSpeedBrakesOut = false;
   std::vector<QString> rightAileronStickBackModifierList;
   std::vector<QString>  leftAileronStickBackModifierList;
   std::vector<QString> rightAileronRudderRightModifierList;
   std::vector<QString>  leftAileronRudderRightModifierList;
   std::vector<QString> rightAileronStickRightModifierList;
   std::vector<QString>  leftAileronStickRightModifierList;
   std::vector<QString> rightAileronSpeedBrakesOutModifierList;
   std::vector<QString>  leftAileronSpeedBrakesOutModifierList;

   QString strClampPosNeg  = "modifier        Clamp_PosNeg";
   QString strNegativeGain = "modifier        NegativeGain";
   QString strAlphaGain    = "modifier        AlphaGainTableName";

   switch (bankControl)
   {
      case Designer::VehicleControls::cAILERONS_ONLY:
      {
         useStickRight = true;
         rightAileronStickRightModifierList.emplace_back(strClampPosNeg);
         rightAileronStickRightModifierList.emplace_back(strNegativeGain);
         leftAileronStickRightModifierList.emplace_back(strClampPosNeg);
         break;
      }
      case Designer::VehicleControls::cSTABILIZERS_ONLY:
      {
         // No additional control
         break;
      }
      case Designer::VehicleControls::cAILERONS_AND_STABILIZERS:
      {
         useStickRight = true;
         rightAileronStickRightModifierList.emplace_back(strClampPosNeg);
         rightAileronStickRightModifierList.emplace_back(strNegativeGain);
         leftAileronStickRightModifierList.emplace_back(strClampPosNeg);
         break;
      }
      default:
         // We can't find a valid control mode, so deliberately fail
         MoverCreatorMainWindow().CreateErrorBox("The bank-to-turn control type is undefined!");
         return false;
   }

   // Output the right horizontal stab
   OutputControlSurface(aOutputStream,
                        "Aileron_Right",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        rightAileronStickBackModifierList,
                        useStickRight,
                        rightAileronStickRightModifierList,
                        useRudderRight,
                        rightAileronRudderRightModifierList,
                        useSpeedBrakesOut,
                        rightAileronSpeedBrakesOutModifierList);

   // Output the left horizontal stab
   OutputControlSurface(aOutputStream,
                        "Aileron_Left",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        leftAileronStickBackModifierList,
                        useStickRight,
                        leftAileronStickRightModifierList,
                        useRudderRight,
                        leftAileronRudderRightModifierList,
                        useSpeedBrakesOut,
                        leftAileronSpeedBrakesOutModifierList);

   return true;
   }

   bool ScriptGeneratorP6DOF::OutputSpoileronControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
   {
   GeometryWing* wing = geometry.GetWing();
   // We check for a null wing pointer in OutputWingControl

   if (!wing->GetSpoileronsPresent())
   {
      return false;
   }

   // Setup spoileron data
   double minAngle_deg                = wing->GetSpoileronsControlSurfaceMinAngle_deg();
   double maxAngle_deg                = wing->GetSpoileronsControlSurfaceMaxAngle_deg();
   double currentAngle_deg            = 0.0;
   double minActuatorAngle_deg        = wing->GetSpoileronsActuatorMinAngle_deg();
   double maxActuatorAngle_deg        = wing->GetSpoileronsActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = wing->GetSpoileronsActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = wing->GetSpoileronsActuatorMaxRate_dps();
   double currentActuatorAngle_deg    = 0.0;
   bool useExpo                       = wing->GetSpoileronsUseExponentialAngleMapping();

   // Set alpha control gain to false by default
   VehicleControls::VehicleControlBankToTurnRollControl bankControl = VehicleControls::VehicleControlBankToTurnRollControl::cAILERONS_ONLY;
   VehicleControls::VehicleControlBankToTurnYawControlType yawControlType = VehicleControls::VehicleControlBankToTurnYawControlType::cNO_YAW_CONTROL;
   bool useAlphaControlGain = false;

   controls.GetBankToTurnControlSettings(bankControl, yawControlType, useAlphaControlGain);

   bool useStickBack = false;
   bool useRudderRight = false;
   bool useStickRight = false;
   bool useSpeedBrakesOut = true;
   std::vector<QString> rightSpoileronStickBackModifierList;
   std::vector<QString>  leftSpoileronStickBackModifierList;
   std::vector<QString> rightSpoileronRudderRightModifierList;
   std::vector<QString>  leftSpoileronRudderRightModifierList;
   std::vector<QString> rightSpoileronStickRightModifierList;
   std::vector<QString>  leftSpoileronStickRightModifierList;
   std::vector<QString> rightSpoileronSpeedBrakesOutModifierList;
   std::vector<QString>  leftSpoileronSpeedBrakesOutModifierList;

   QString strZeroOne      = "modifier        Clamp_ZeroOne";
   QString strNegativeGain = "modifier        NegativeGain";
   QString strAlphaGain    = "modifier        AlphaGainTableName";

   switch (bankControl)
   {
      case Designer::VehicleControls::cAILERONS_ONLY:
      {
         useStickRight = true;
         rightSpoileronStickRightModifierList.emplace_back(strZeroOne);
         leftSpoileronStickRightModifierList.emplace_back(strNegativeGain);
         leftSpoileronStickRightModifierList.emplace_back(strZeroOne);
         break;
      }
      case Designer::VehicleControls::cSTABILIZERS_ONLY:
      {
         // No additional control
         break;
      }
      case Designer::VehicleControls::cAILERONS_AND_STABILIZERS:
      {
         useStickRight = true;
         rightSpoileronStickRightModifierList.emplace_back(strZeroOne);
         leftSpoileronStickRightModifierList.emplace_back(strNegativeGain);
         leftSpoileronStickRightModifierList.emplace_back(strZeroOne);
         break;
      }
      default:
         // We can't find a valid control mode, so deliberately fail
         MoverCreatorMainWindow().CreateErrorBox("The bank-to-turn control type is undefined!");
         return false;
   }

   rightSpoileronSpeedBrakesOutModifierList.emplace_back(strZeroOne);
   leftSpoileronSpeedBrakesOutModifierList.emplace_back(strZeroOne);

   // Output the right horizontal stab
   OutputControlSurface(aOutputStream,
                        "Spoileron_Right",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        rightSpoileronStickBackModifierList,
                        useStickRight,
                        rightSpoileronStickRightModifierList,
                        useRudderRight,
                        rightSpoileronRudderRightModifierList,
                        useSpeedBrakesOut,
                        rightSpoileronSpeedBrakesOutModifierList);

   // Output the left horizontal stab
   OutputControlSurface(aOutputStream,
                        "Spoileron_Left",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        leftSpoileronStickBackModifierList,
                        useStickRight,
                        leftSpoileronStickRightModifierList,
                        useRudderRight,
                        leftSpoileronRudderRightModifierList,
                        useSpeedBrakesOut,
                        leftSpoileronSpeedBrakesOutModifierList);

   return true;
   }

   bool ScriptGeneratorP6DOF::OutputSpoilerControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
   {
   GeometryWing* wing = geometry.GetWing();
   // We check for a null wing pointer in OutputWingControl

   if (!wing->GetSpoilersPresent())
   {
      return false;
   }

   // Setup spoiler data
   double minAngle_deg                = wing->GetSpoilersControlSurfaceMinAngle_deg();
   double maxAngle_deg                = wing->GetSpoilersControlSurfaceMaxAngle_deg();
   double currentAngle_deg            = 0.0;
   double minActuatorAngle_deg        = wing->GetSpoilersActuatorMinAngle_deg();
   double maxActuatorAngle_deg        = wing->GetSpoilersActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = wing->GetSpoilersActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = wing->GetSpoilersActuatorMaxRate_dps();
   double currentActuatorAngle_deg    = 0.0;
   bool useExpo                       = wing->GetSpoilersUseExponentialAngleMapping();

   // Set alpha control gain to false by default
   VehicleControls::VehicleControlBankToTurnRollControl bankControl = VehicleControls::VehicleControlBankToTurnRollControl::cAILERONS_ONLY;
   VehicleControls::VehicleControlBankToTurnYawControlType yawControlType = VehicleControls::VehicleControlBankToTurnYawControlType::cNO_YAW_CONTROL;
   bool useAlphaControlGain = false;

   controls.GetBankToTurnControlSettings(bankControl, yawControlType, useAlphaControlGain);

   bool useStickBack = false;
   bool useRudderRight = false;
   bool useStickRight = false;
   bool useSpeedBrakesOut = true;
   std::vector<QString> rightSpoileronStickBackModifierList;
   std::vector<QString>  leftSpoileronStickBackModifierList;
   std::vector<QString> rightSpoileronRudderRightModifierList;
   std::vector<QString>  leftSpoileronRudderRightModifierList;
   std::vector<QString> rightSpoileronStickRightModifierList;
   std::vector<QString>  leftSpoileronStickRightModifierList;
   std::vector<QString> rightSpoileronSpeedBrakesOutModifierList;
   std::vector<QString>  leftSpoileronSpeedBrakesOutModifierList;

   QString strZeroOne      = "modifier        Clamp_ZeroOne";

   rightSpoileronSpeedBrakesOutModifierList.emplace_back(strZeroOne);
   leftSpoileronSpeedBrakesOutModifierList.emplace_back(strZeroOne);

   // Output the right horizontal stab
   OutputControlSurface(aOutputStream,
                        "Spoiler_Right",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        rightSpoileronStickBackModifierList,
                        useStickRight,
                        rightSpoileronStickRightModifierList,
                        useRudderRight,
                        rightSpoileronRudderRightModifierList,
                        useSpeedBrakesOut,
                        rightSpoileronSpeedBrakesOutModifierList);

   // Output the left horizontal stab
   OutputControlSurface(aOutputStream,
                        "Spoiler_Left",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        leftSpoileronStickBackModifierList,
                        useStickRight,
                        leftSpoileronStickRightModifierList,
                        useRudderRight,
                        leftSpoileronRudderRightModifierList,
                        useSpeedBrakesOut,
                        leftSpoileronSpeedBrakesOutModifierList);

   return true;
   }

   bool ScriptGeneratorP6DOF::OutputDrageronControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
   {
   GeometryWing* wing = geometry.GetWing();
   // We check for a null wing pointer in OutputWingControl

   if (!wing->GetDrageronsPresent())
   {
      return false;
   }

   // Setup drageron data
   double minAngle_deg = wing->GetDrageronsControlSurfaceMinAngle_deg();
   double maxAngle_deg = wing->GetDrageronsControlSurfaceMaxAngle_deg();
   double currentAngle_deg = 0.0;
   double minActuatorAngle_deg = wing->GetDrageronsActuatorMinAngle_deg();
   double maxActuatorAngle_deg = wing->GetDrageronsActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = wing->GetDrageronsActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = wing->GetDrageronsActuatorMaxRate_dps();
   double currentActuatorAngle_deg = 0.0;
   bool   useExpo = wing->GetDrageronsUseExponentialAngleMapping();

   bool useStickBack = false;
   bool useRudderRight = true;
   bool useStickRight = true;
   bool useSpeedBrakesOut = true;

   // -------------------- Top surfaces -------------------- //

   std::vector<QString>  rightTopDrageronStickBackModifierList;
   std::vector<QString>   leftTopDrageronStickBackModifierList;
   std::vector<QString>  rightTopDrageronRudderRightModifierList;
   std::vector<QString>   leftTopDrageronRudderRightModifierList;
   std::vector<QString>  rightTopDrageronStickRightModifierList;
   std::vector<QString>   leftTopDrageronStickRightModifierList;
   std::vector<QString>  rightTopDrageronSpeedBrakesOutModifierList;
   std::vector<QString>   leftTopDrageronSpeedBrakesOutModifierList;

   QString strClampZeroOne = "modifier        Clamp_ZeroOne";
   QString strClampPosNeg = "modifier        Clamp_PosNeg";
   QString strClampNegOneZero = "modifier        Clamp_NegOneZero";
   QString strNegativeGain = "modifier        NegativeGain";

   // stick back
   rightTopDrageronStickBackModifierList.push_back(strClampPosNeg);
   rightTopDrageronStickBackModifierList.push_back(strNegativeGain);

   // stick right
   rightTopDrageronStickRightModifierList.push_back(strClampPosNeg);
   rightTopDrageronStickRightModifierList.push_back(strNegativeGain);

   // rudder right
   rightTopDrageronRudderRightModifierList.push_back(strClampNegOneZero);
   rightTopDrageronRudderRightModifierList.push_back(strNegativeGain);

   // speed brakes out
   rightTopDrageronSpeedBrakesOutModifierList.push_back(strClampNegOneZero);
   rightTopDrageronSpeedBrakesOutModifierList.push_back(strNegativeGain);

   // Output the top right drageron
   OutputControlSurface(aOutputStream,
                        "Drageron_Right_Top",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        rightTopDrageronStickBackModifierList,
                        useStickRight,
                        rightTopDrageronStickRightModifierList,
                        useRudderRight,
                        rightTopDrageronRudderRightModifierList,
                        useSpeedBrakesOut,
                        rightTopDrageronSpeedBrakesOutModifierList);

   // stick back
   leftTopDrageronStickBackModifierList.push_back(strClampPosNeg);
   leftTopDrageronStickBackModifierList.push_back(strNegativeGain);

   // stick right
   leftTopDrageronStickRightModifierList.push_back(strClampPosNeg);

   // rudder right
   leftTopDrageronRudderRightModifierList.push_back(strClampZeroOne);

   // speed brakes out
   leftTopDrageronSpeedBrakesOutModifierList.push_back(strClampNegOneZero);
   leftTopDrageronSpeedBrakesOutModifierList.push_back(strNegativeGain);

   // Output the top left drageron
   OutputControlSurface(aOutputStream,
                        "Drageron_Left_Top",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        leftTopDrageronStickBackModifierList,
                        useStickRight,
                        leftTopDrageronStickRightModifierList,
                        useRudderRight,
                        leftTopDrageronRudderRightModifierList,
                        useSpeedBrakesOut,
                        leftTopDrageronSpeedBrakesOutModifierList);

   // -------------------- Bottom surfaces -------------------- //

   std::vector<QString>  rightBottomDrageronStickBackModifierList;
   std::vector<QString>   leftBottomDrageronStickBackModifierList;
   std::vector<QString>  rightBottomDrageronRudderRightModifierList;
   std::vector<QString>   leftBottomDrageronRudderRightModifierList;
   std::vector<QString>  rightBottomDrageronStickRightModifierList;
   std::vector<QString>   leftBottomDrageronStickRightModifierList;
   std::vector<QString>  rightBottomDrageronSpeedBrakesOutModifierList;
   std::vector<QString>   leftBottomDrageronSpeedBrakesOutModifierList;

   // stick back
   rightBottomDrageronStickBackModifierList.push_back(strClampPosNeg);
   rightBottomDrageronStickBackModifierList.push_back(strNegativeGain);

   // stick right
   rightBottomDrageronStickRightModifierList.push_back(strClampPosNeg);
   rightBottomDrageronStickRightModifierList.push_back(strNegativeGain);

   // rudder right
   rightBottomDrageronRudderRightModifierList.push_back(strClampZeroOne);

   // speed brakes out
   rightBottomDrageronSpeedBrakesOutModifierList.push_back(strClampZeroOne);

   // Output the top right drageron
   OutputControlSurface(aOutputStream,
                        "Drageron_Right_Bottom",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        rightBottomDrageronStickBackModifierList,
                        useStickRight,
                        rightBottomDrageronStickRightModifierList,
                        useRudderRight,
                        rightBottomDrageronRudderRightModifierList,
                        useSpeedBrakesOut,
                        rightBottomDrageronSpeedBrakesOutModifierList);

   // stick back
   leftBottomDrageronStickBackModifierList.push_back(strClampPosNeg);
   leftBottomDrageronStickBackModifierList.push_back(strNegativeGain);

   // stick right
   leftBottomDrageronStickRightModifierList.push_back(strClampPosNeg);

   // rudder right
   leftBottomDrageronRudderRightModifierList.push_back(strClampNegOneZero);
   leftBottomDrageronRudderRightModifierList.push_back(strNegativeGain);

   // speed brakes out
   leftBottomDrageronSpeedBrakesOutModifierList.push_back(strClampZeroOne);


   // Output the top left drageron
   OutputControlSurface(aOutputStream,
                        "Drageron_Left_Bottom",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        leftBottomDrageronStickBackModifierList,
                        useStickRight,
                        leftBottomDrageronStickRightModifierList,
                        useRudderRight,
                        leftBottomDrageronRudderRightModifierList,
                        useSpeedBrakesOut,
                        leftBottomDrageronSpeedBrakesOutModifierList);

   return true;
   }

   bool ScriptGeneratorP6DOF::OutputElevonControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
   {
   GeometryWing* wing = geometry.GetWing();
   // We check for a null wing pointer in OutputWingControl

   if (!wing->GetElevonsPresent())
   {
      return false;
   }

   // Setup elevon data
   double minAngle_deg = wing->GetElevonsControlSurfaceMinAngle_deg();
   double maxAngle_deg = wing->GetElevonsControlSurfaceMaxAngle_deg();
   double currentAngle_deg = 0.0;
   double minActuatorAngle_deg = wing->GetElevonsActuatorMinAngle_deg();
   double maxActuatorAngle_deg = wing->GetElevonsActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = wing->GetElevonsActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = wing->GetElevonsActuatorMaxRate_dps();
   double currentActuatorAngle_deg = 0.0;
   bool useExpo = wing->GetElevonsUseExponentialAngleMapping();

   bool useStickBack = true;
   bool useRudderRight = false;
   bool useStickRight = true;
   bool useSpeedBrakesOut = false;

   std::vector<QString> rightElevonStickBackModifierList;
   std::vector<QString>  leftElevonStickBackModifierList;
   std::vector<QString> rightElevonRudderRightModifierList;
   std::vector<QString>  leftElevonRudderRightModifierList;
   std::vector<QString> rightElevonStickRightModifierList;
   std::vector<QString>  leftElevonStickRightModifierList;
   std::vector<QString> rightElevonSpeedBrakesOutModifierList;
   std::vector<QString>  leftElevonSpeedBrakesOutModifierList;

   QString strNegativeGain = "modifier        NegativeGain";
   QString strClampPosNeg = "modifier        Clamp_PosNeg";

   rightElevonStickBackModifierList.push_back(strClampPosNeg);
   rightElevonStickBackModifierList.push_back(strNegativeGain);
   leftElevonStickBackModifierList.push_back(strClampPosNeg);
   leftElevonStickBackModifierList.push_back(strNegativeGain);


   rightElevonStickRightModifierList.push_back(strClampPosNeg);
   rightElevonStickRightModifierList.push_back(strNegativeGain);
   leftElevonStickRightModifierList.push_back(strClampPosNeg);

   // Output the right horizontal stab
   OutputControlSurface(aOutputStream,
                        "Elevon_Right",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        rightElevonStickBackModifierList,
                        useStickRight,
                        rightElevonStickRightModifierList,
                        useRudderRight,
                        rightElevonRudderRightModifierList,
                        useSpeedBrakesOut,
                        rightElevonSpeedBrakesOutModifierList);

   // Output the left horizontal stab
   OutputControlSurface(aOutputStream,
                        "Elevon_Left",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        leftElevonStickBackModifierList,
                        useStickRight,
                        leftElevonStickRightModifierList,
                        useRudderRight,
                        leftElevonRudderRightModifierList,
                        useSpeedBrakesOut,
                        leftElevonSpeedBrakesOutModifierList);

   return true;

   }

   bool ScriptGeneratorP6DOF::OutputWingControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
   {
   GeometryWing* wing = geometry.GetWing();
   if (wing == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks a Wing!");
      return false;
   }

   OutputAileronControl(aOutputStream, geometry, controls);
   OutputElevonControl(aOutputStream, geometry, controls);
   OutputSpoileronControl(aOutputStream, geometry, controls);
   OutputSpoilerControl(aOutputStream, geometry, controls);
   OutputDrageronControl(aOutputStream, geometry, controls);

   return true;
   }

   bool ScriptGeneratorP6DOF::OutputHorizontalTailControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
   {
   GeometrySurface* horizontalStab = geometry.GetHorizontalStab();
   if (horizontalStab == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks a Horizontal Stab!");
      return false;
   }

   // Setup horizontal stab data
   double minAngle_deg                = horizontalStab->GetControlSurfaceMinAngle_deg();
   double maxAngle_deg                = horizontalStab->GetControlSurfaceMaxAngle_deg();
   double currentAngle_deg            = 0.0;
   double minActuatorAngle_deg        = horizontalStab->GetActuatorMinAngle_deg();
   double maxActuatorAngle_deg        = horizontalStab->GetActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = horizontalStab->GetActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = horizontalStab->GetActuatorMaxRate_dps();
   double currentActuatorAngle_deg    = 0.0;
   bool useExpo                       = horizontalStab->GetUseExponentialAngleMapping();

   // Set alpha control gain to false by default
   VehicleControls::VehicleControlBankToTurnRollControl bankControl = VehicleControls::VehicleControlBankToTurnRollControl::cAILERONS_ONLY;
   VehicleControls::VehicleControlBankToTurnYawControlType yawControlType = VehicleControls::VehicleControlBankToTurnYawControlType::cNO_YAW_CONTROL;
   bool useAlphaControlGain = false;

   controls.GetBankToTurnControlSettings(bankControl, yawControlType, useAlphaControlGain);

   bool useStickBack = true;
   bool useRudderRight = false;
   bool useStickRight = false;
   std::vector<QString> rightStabStickBackModifierList;
   std::vector<QString>  leftStabStickBackModifierList;
   std::vector<QString> rightStabRudderRightModifierList;
   std::vector<QString>  leftStabRudderRightModifierList;
   std::vector<QString> rightStabStickRightModifierList;
   std::vector<QString>  leftStabStickRightModifierList;

   QString strClampPosNeg  = "modifier        Clamp_PosNeg";
   QString strNegativeGain = "modifier        NegativeGain";
   QString strAlphaGain    = "modifier        AlphaGainTableName";
   QString strHalfGain     = "modifier        HalfGain";

   switch (bankControl)
   {
      case Designer::VehicleControls::cAILERONS_ONLY:
      {
         // No additional control
         break;
      }
      case Designer::VehicleControls::cSTABILIZERS_ONLY:
      {
         useStickRight = true;

         rightStabStickRightModifierList.emplace_back(strClampPosNeg);
         rightStabStickRightModifierList.emplace_back(strNegativeGain);

         leftStabStickRightModifierList.emplace_back(strClampPosNeg);

         break;
      }
      case Designer::VehicleControls::cAILERONS_AND_STABILIZERS:
      {
         useStickRight = true;

         rightStabStickRightModifierList.emplace_back(strClampPosNeg);
         rightStabStickRightModifierList.emplace_back(strNegativeGain);
         rightStabStickRightModifierList.emplace_back(strHalfGain);

         leftStabStickRightModifierList.emplace_back(strClampPosNeg);
         leftStabStickRightModifierList.emplace_back(strHalfGain);

         break;
      }
      default:
         // We can't find a valid control mode, so deliberately fail
         MoverCreatorMainWindow().CreateErrorBox("The bank-to-turn control type is undefined!");
         return false;
   }

   rightStabStickBackModifierList.emplace_back(strClampPosNeg);
   if (useAlphaControlGain)
   {
      rightStabStickBackModifierList.emplace_back(strAlphaGain);
   }
   rightStabStickBackModifierList.emplace_back(strNegativeGain);

   leftStabStickBackModifierList.emplace_back(strClampPosNeg);
   if (useAlphaControlGain)
   {
      leftStabStickBackModifierList.emplace_back(strAlphaGain);
   }
   leftStabStickBackModifierList.emplace_back(strNegativeGain);

   // Output the right horizontal stab
   OutputControlSurface(aOutputStream,
                        "Horizontal_Stab_Right",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        rightStabStickBackModifierList,
                        useStickRight,
                        rightStabStickRightModifierList,
                        useRudderRight,
                        rightStabRudderRightModifierList);

   // Output the left horizontal stab
   OutputControlSurface(aOutputStream,
                        "Horizontal_Stab_Left",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        leftStabStickBackModifierList,
                        useStickRight,
                        leftStabStickRightModifierList,
                        useRudderRight,
                        leftStabRudderRightModifierList);

   return true;
   }

   bool ScriptGeneratorP6DOF::OutputVerticalTailControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
   {
   GeometrySurface* verticalTail = geometry.GetVerticalTail();
   if (verticalTail == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks a Vertical Tail!");
      return false;
   }

   // Setup horizontal stab data
   double minAngle_deg                = verticalTail->GetControlSurfaceMinAngle_deg();
   double maxAngle_deg                = verticalTail->GetControlSurfaceMaxAngle_deg();
   double currentAngle_deg            = 0.0;
   double minActuatorAngle_deg        = verticalTail->GetActuatorMinAngle_deg();
   double maxActuatorAngle_deg        = verticalTail->GetActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = verticalTail->GetActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = verticalTail->GetActuatorMaxRate_dps();
   double currentActuatorAngle_deg    = 0.0;
   bool useExpo                       = verticalTail->GetUseExponentialAngleMapping();
   
   bool useStickBack = false;
   bool useRudderRight = true;
   bool useStickRight = false;

   QString strClampPosNeg  = "modifier        Clamp_PosNeg";
   QString strNegativeGain = "modifier        NegativeGain";

   std::string symmetryString = verticalTail->GetSymmetryString().toStdString();
   if (symmetryString == "Horizontal")
   {
      bool useSpeedBrakesOut = false;
      std::vector<QString> rightTailStickBackModifierList;
      std::vector<QString> rightTailRudderRightModifierList;
      std::vector<QString> rightTailStickRightModifierList;
      std::vector<QString> rightTailSpeedBrakesOutModifierList;

      std::vector<QString> leftTailStickBackModifierList;
      std::vector<QString> leftTailRudderRightModifierList;
      std::vector<QString> leftTailStickRightModifierList;
      std::vector<QString> leftTailSpeedBrakesOutModifierList;

      rightTailRudderRightModifierList.emplace_back(strClampPosNeg);
      leftTailRudderRightModifierList.emplace_back(strNegativeGain);
      leftTailRudderRightModifierList.emplace_back(strClampPosNeg);

      rightTailSpeedBrakesOutModifierList.emplace_back(strClampPosNeg);
      leftTailSpeedBrakesOutModifierList.emplace_back(strClampPosNeg);
      leftTailSpeedBrakesOutModifierList.emplace_back(strNegativeGain);

      // Output the right vertical tail
      OutputControlSurface(aOutputStream,
                           "Vertical_Tail_Right",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           rightTailStickBackModifierList,
                           useStickRight,
                           rightTailStickRightModifierList,
                           useRudderRight,
                           rightTailRudderRightModifierList,
                           useSpeedBrakesOut,
                           rightTailSpeedBrakesOutModifierList);

      // Output the left vertical tail
      OutputControlSurface(aOutputStream,
                           "Vertical_Tail_Left",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           leftTailStickBackModifierList,
                           useStickRight,
                           leftTailStickRightModifierList,
                           useRudderRight,
                           leftTailRudderRightModifierList,
                           useSpeedBrakesOut,
                           leftTailSpeedBrakesOutModifierList);
   }
   else
   {
      std::vector<QString> stickBackModifierList;
      std::vector<QString> rudderRightModifierList;
      std::vector<QString> stickRightModifierList;
      rudderRightModifierList.emplace_back(strClampPosNeg);

      // Output the vertical tail
      OutputControlSurface(aOutputStream,
         "Vertical_Tail",
         minAngle_deg,
         maxAngle_deg,
         currentAngle_deg,
         minActuatorAngle_deg,
         maxActuatorAngle_deg,
         maxNegativeActuatorRate_dps,
         maxPositiveActuatorRate_dps,
         currentActuatorAngle_deg,
         useExpo,
         useStickBack,
         stickBackModifierList,
         useStickRight,
         stickRightModifierList,
         useRudderRight,
         rudderRightModifierList);
   }
   return true;
   }

   bool ScriptGeneratorP6DOF::OutputVentralTailControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
   {
   GeometrySurface* ventralTail = geometry.GetVentralTail();
   if (ventralTail == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks a Ventral Tail!");
      return false;
   }

   // Setup horizontal stab data
   double minAngle_deg                = ventralTail->GetControlSurfaceMinAngle_deg();
   double maxAngle_deg                = ventralTail->GetControlSurfaceMaxAngle_deg();
   double currentAngle_deg            = 0.0;
   double minActuatorAngle_deg        = ventralTail->GetActuatorMinAngle_deg();
   double maxActuatorAngle_deg        = ventralTail->GetActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = ventralTail->GetActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = ventralTail->GetActuatorMaxRate_dps();
   double currentActuatorAngle_deg    = 0.0;
   bool useExpo                       = ventralTail->GetUseExponentialAngleMapping();
   
   bool useStickBack = false;
   bool useRudderRight = true;
   bool useStickRight = false;

   QString strClampPosNeg  = "modifier        Clamp_PosNeg";
   QString strNegativeGain = "modifier        NegativeGain";

   std::string symmetryString = ventralTail->GetSymmetryString().toStdString();
   if (symmetryString == "Horizontal")
   {
      bool useSpeedBrakesOut = false;
      std::vector<QString> rightTailStickBackModifierList;
      std::vector<QString> rightTailRudderRightModifierList;
      std::vector<QString> rightTailStickRightModifierList;
      std::vector<QString> rightTailSpeedBrakesOutModifierList;

      std::vector<QString> leftTailStickBackModifierList;
      std::vector<QString> leftTailRudderRightModifierList;
      std::vector<QString> leftTailStickRightModifierList;
      std::vector<QString> leftTailSpeedBrakesOutModifierList;

      rightTailRudderRightModifierList.emplace_back(strClampPosNeg);
      rightTailRudderRightModifierList.emplace_back(strNegativeGain);

      leftTailRudderRightModifierList.emplace_back(strClampPosNeg);

      rightTailSpeedBrakesOutModifierList.emplace_back(strClampPosNeg);
      leftTailSpeedBrakesOutModifierList.emplace_back(strClampPosNeg);
      rightTailSpeedBrakesOutModifierList.emplace_back(strNegativeGain);

      // Output the right vertical tail
      OutputControlSurface(aOutputStream,
                           "Ventral_Tail_Right",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           rightTailStickBackModifierList,
                           useStickRight,
                           rightTailStickRightModifierList,
                           useRudderRight,
                           rightTailRudderRightModifierList,
                           useSpeedBrakesOut,
                           rightTailSpeedBrakesOutModifierList);

      // Output the left vertical tail
      OutputControlSurface(aOutputStream,
                           "Ventral_Tail_Left",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           leftTailStickBackModifierList,
                           useStickRight,
                           leftTailStickRightModifierList,
                           useRudderRight,
                           leftTailRudderRightModifierList,
                           useSpeedBrakesOut,
                           leftTailSpeedBrakesOutModifierList);
   }
   else
   {
      std::vector<QString> stickBackModifierList;
      std::vector<QString> rudderRightModifierList;
      std::vector<QString> stickRightModifierList;
      rudderRightModifierList.emplace_back(strClampPosNeg);
      rudderRightModifierList.emplace_back(strNegativeGain);

      // Output the vertical tail
      OutputControlSurface(aOutputStream,
         "Ventral_Tail",
         minAngle_deg,
         maxAngle_deg,
         currentAngle_deg,
         minActuatorAngle_deg,
         maxActuatorAngle_deg,
         maxNegativeActuatorRate_dps,
         maxPositiveActuatorRate_dps,
         currentActuatorAngle_deg,
         useExpo,
         useStickBack,
         stickBackModifierList,
         useStickRight,
         stickRightModifierList,
         useRudderRight,
         rudderRightModifierList);
   }
   return true;
   }

   bool ScriptGeneratorP6DOF::OutputVTailControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
   {
   bool inverted = false;
   GeometrySurface* veeTail = geometry.GetVTail();
   if (veeTail == nullptr)
   {
      veeTail = geometry.GetInvertedVTail();
      inverted = true;
      if (veeTail == nullptr)
      {
         MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks a V-Tail!");
         return false;
      }
   }

   // Setup horizontal stab data
   double minAngle_deg                = veeTail->GetControlSurfaceMinAngle_deg();
   double maxAngle_deg                = veeTail->GetControlSurfaceMaxAngle_deg();
   double currentAngle_deg            = 0.0;
   double minActuatorAngle_deg        = veeTail->GetActuatorMinAngle_deg();
   double maxActuatorAngle_deg        = veeTail->GetActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = veeTail->GetActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = veeTail->GetActuatorMaxRate_dps();
   double currentActuatorAngle_deg    = 0.0;
   bool useExpo                       = veeTail->GetUseExponentialAngleMapping();

   // Set alpha control gain to false by default
   VehicleControls::VehicleControlBankToTurnRollControl bankControl = VehicleControls::VehicleControlBankToTurnRollControl::cAILERONS_ONLY;
   VehicleControls::VehicleControlBankToTurnYawControlType yawControlType = VehicleControls::VehicleControlBankToTurnYawControlType::cNO_YAW_CONTROL;
   bool useAlphaControlGain = false;

   controls.GetBankToTurnControlSettings(bankControl, yawControlType, useAlphaControlGain);

   bool useStickBack = true;
   bool useRudderRight = true;
   bool useStickRight = false;
   bool useSpeedBrakesOut = false;
   std::vector<QString> rightStabStickBackModifierList;
   std::vector<QString>  leftStabStickBackModifierList;
   std::vector<QString> rightStabRudderRightModifierList;
   std::vector<QString>  leftStabRudderRightModifierList;
   std::vector<QString> rightStabStickRightModifierList;
   std::vector<QString>  leftStabStickRightModifierList;
   std::vector<QString> rightStabSpeedBrakesOutModifierList;
   std::vector<QString>  leftStabSpeedBrakesOutModifierList;

   QString strClampPosNeg  = "modifier        Clamp_PosNeg";
   QString strNegativeGain = "modifier        NegativeGain";
   QString strAlphaGain    = "modifier        AlphaGainTableName";
   QString strHalfGain     = "modifier        HalfGain";


   // Pitch control
   rightStabStickBackModifierList.emplace_back(strClampPosNeg);
   rightStabStickBackModifierList.emplace_back(strNegativeGain);
   if (useAlphaControlGain)
   {
      rightStabStickBackModifierList.emplace_back(strAlphaGain);
   }

   leftStabStickBackModifierList.emplace_back(strClampPosNeg);
   leftStabStickBackModifierList.emplace_back(strNegativeGain);
   if (useAlphaControlGain)
   {
      leftStabStickBackModifierList.emplace_back(strAlphaGain);
   }


   // Rudder control
   rightStabRudderRightModifierList.emplace_back(strClampPosNeg);
   leftStabRudderRightModifierList.emplace_back(strClampPosNeg);
   rightStabRudderRightModifierList.emplace_back(strHalfGain);
   leftStabRudderRightModifierList.emplace_back(strHalfGain);
   if (inverted)
   {
      rightStabRudderRightModifierList.emplace_back(strNegativeGain);
   }
   else
   {
      leftStabRudderRightModifierList.emplace_back(strNegativeGain);
   }

   // Brake/flare control
   rightStabSpeedBrakesOutModifierList.emplace_back(strClampPosNeg);
   rightStabSpeedBrakesOutModifierList.emplace_back(strHalfGain);
   leftStabSpeedBrakesOutModifierList.emplace_back(strClampPosNeg);
   leftStabSpeedBrakesOutModifierList.emplace_back(strHalfGain);
   if (inverted)
   {
      rightStabSpeedBrakesOutModifierList.emplace_back(strNegativeGain);
   }
   else
   {
      leftStabSpeedBrakesOutModifierList.emplace_back(strNegativeGain);
   }

   QString rightStabName;
   QString leftStabName;

   if (inverted)
   {
      rightStabName = "Inverted_V-Tail_Right";
      leftStabName = "Inverted_V-Tail_Left";
   }
   else
   {
      rightStabName = "V-Tail_Right";
      leftStabName = "V-Tail_Left";
   }

   // Output the right V-tail stab
   OutputControlSurface(aOutputStream,
                        rightStabName,
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        rightStabStickBackModifierList,
                        useStickRight,
                        rightStabStickRightModifierList,
                        useRudderRight,
                        rightStabRudderRightModifierList,
                        useSpeedBrakesOut,
                        rightStabSpeedBrakesOutModifierList);

   // Output the left V-tail stab
   OutputControlSurface(aOutputStream,
                        leftStabName,
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        leftStabStickBackModifierList,
                        useStickRight,
                        leftStabStickRightModifierList,
                        useRudderRight,
                        leftStabRudderRightModifierList,
                        useSpeedBrakesOut,
                        leftStabSpeedBrakesOutModifierList);

   return true;
   }

   bool ScriptGeneratorP6DOF::OutputCanardControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
   {
   GeometrySurface* canard = geometry.GetCanard();
   if (canard == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks a Canard!");
      return false;
   }

   // Setup horizontal stab data
   double minAngle_deg                = canard->GetControlSurfaceMinAngle_deg();
   double maxAngle_deg                = canard->GetControlSurfaceMaxAngle_deg();
   double currentAngle_deg            = 0.0;
   double minActuatorAngle_deg        = canard->GetActuatorMinAngle_deg();
   double maxActuatorAngle_deg        = canard->GetActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = canard->GetActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = canard->GetActuatorMaxRate_dps();
   double currentActuatorAngle_deg    = 0.0;
   bool useExpo                       = canard->GetUseExponentialAngleMapping();

   // Set alpha control gain to false by default
   VehicleControls::VehicleControlBankToTurnRollControl bankControl = VehicleControls::VehicleControlBankToTurnRollControl::cAILERONS_ONLY;
   VehicleControls::VehicleControlBankToTurnYawControlType yawControlType = VehicleControls::VehicleControlBankToTurnYawControlType::cNO_YAW_CONTROL;
   bool useAlphaControlGain = false;

   controls.GetBankToTurnControlSettings(bankControl, yawControlType, useAlphaControlGain);

   bool useStickBack = true;
   bool useRudderRight = false;
   bool useStickRight = false;
   std::vector<QString> rightStabStickBackModifierList;
   std::vector<QString>  leftStabStickBackModifierList;
   std::vector<QString> rightStabRudderRightModifierList;
   std::vector<QString>  leftStabRudderRightModifierList;
   std::vector<QString> rightStabStickRightModifierList;
   std::vector<QString>  leftStabStickRightModifierList;

   QString strClampPosNeg  = "modifier        Clamp_PosNeg";
   QString strNegativeGain = "modifier        NegativeGain";
   QString strAlphaGain    = "modifier        AlphaGainTableName";
   QString strHalfGain     = "modifier        HalfGain";

   switch (bankControl)
   {
      case Designer::VehicleControls::cAILERONS_ONLY:
      {
         // No additional control
         break;
      }
      case Designer::VehicleControls::cSTABILIZERS_ONLY:
      {
         useStickRight = true;

         rightStabStickRightModifierList.emplace_back(strClampPosNeg);
         rightStabStickRightModifierList.emplace_back(strNegativeGain);

         leftStabStickRightModifierList.emplace_back(strClampPosNeg);

         break;
      }
      case Designer::VehicleControls::cAILERONS_AND_STABILIZERS:
      {
         useStickRight = true;

         rightStabStickRightModifierList.emplace_back(strClampPosNeg);
         rightStabStickRightModifierList.emplace_back(strNegativeGain);
         rightStabStickRightModifierList.emplace_back(strHalfGain);

         leftStabStickRightModifierList.emplace_back(strClampPosNeg);
         leftStabStickRightModifierList.emplace_back(strHalfGain);

         break;
      }
      default:
         // We can't find a valid control mode, so deliberately fail
         MoverCreatorMainWindow().CreateErrorBox("The bank-to-turn control type is undefined!");
         return false;
   }

   rightStabStickBackModifierList.emplace_back(strClampPosNeg);
   if (useAlphaControlGain)
   {
      rightStabStickBackModifierList.emplace_back(strAlphaGain);
   }

   leftStabStickBackModifierList.emplace_back(strClampPosNeg);
   if (useAlphaControlGain)
   {
      leftStabStickBackModifierList.emplace_back(strAlphaGain);
   }

   // Output the right canard
   OutputControlSurface(aOutputStream,
                        "Canard_Right",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        rightStabStickBackModifierList,
                        useStickRight,
                        rightStabStickRightModifierList,
                        useRudderRight,
                        rightStabRudderRightModifierList);

   // Output the left canard
   OutputControlSurface(aOutputStream,
                        "Canard_Left",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        leftStabStickBackModifierList,
                        useStickRight,
                        leftStabStickRightModifierList,
                        useRudderRight,
                        leftStabRudderRightModifierList);

   return true;
   }

   void ScriptGeneratorP6DOF::OutputControlSurface(QTextStream&                aOutputStream,
                                                     const QString&              aControlSurfaceName,
                                                     double                      aMinAngle_deg,
                                                     double                      aMaxAngle_deg,
                                                     double                      aCurrentAngle_deg,
                                                     double                      aMinActuatorAngle_deg,
                                                     double                      aMaxActuatorAngle_deg,
                                                     double                      aMaxNegativeActuatorRate_dps,
                                                     double                      aMaxPositiveActuatorRate_dps,
                                                     double                      aCurrentActuatorAngle_deg,
                                                     bool                        aUseExpo,
                                                     bool                        aUseStickBack,
                                                     const std::vector<QString>& aStickBackModifierList,
                                                     bool                        aUseStickRight,
                                                     const std::vector<QString>& aStickRightModifierList,
                                                     bool                        aUseRudderRight,
                                                     const std::vector<QString>& aRudderRightModifierList)
   {
   OutputControlSurface(aOutputStream,
                        aControlSurfaceName,
                        aMinAngle_deg,
                        aMaxAngle_deg,
                        aCurrentAngle_deg,
                        aMinActuatorAngle_deg,
                        aMaxActuatorAngle_deg,
                        aMaxNegativeActuatorRate_dps,
                        aMaxPositiveActuatorRate_dps,
                        aCurrentActuatorAngle_deg,
                        aUseExpo,
                        aUseStickBack,
                        aStickBackModifierList,
                        aUseStickRight,
                        aStickRightModifierList,
                        aUseRudderRight,
                        aRudderRightModifierList,
                        false,
                        std::vector<QString>());
   }

   void ScriptGeneratorP6DOF::OutputControlSurface(QTextStream&                aOutputStream,
                                           const QString&              aControlSurfaceName,
                                           double                      aMinAngle_deg,
                                           double                      aMaxAngle_deg,
                                           double                      aCurrentAngle_deg,
                                           double                      aMinActuatorAngle_deg,
                                           double                      aMaxActuatorAngle_deg,
                                           double                      aMaxNegativeActuatorRate_dps,
                                           double                      aMaxPositiveActuatorRate_dps,
                                           double                      aCurrentActuatorAngle_deg,
                                           bool                        aUseExpo,
                                           bool                        aUseStickBack,
                                           const std::vector<QString>& aStickBackModifierList,
                                           bool                        aUseStickRight,
                                           const std::vector<QString>& aStickRightModifierList,
                                           bool                        aUseRudderRight,
                                           const std::vector<QString>& aRudderRightModifierList,
                                           bool                        aUseSpeedBrakesOut,
                                           const std::vector<QString>& aSpeedBrakesOutModifierList)
   {
   // Output format
   aOutputStream.setRealNumberNotation(QTextStream::FixedNotation);
   QTextStream::FieldAlignment alignment = QTextStream::FieldAlignment::AlignRight;
   int width = 8;
   int precision = 2;

   aOutputStream << "#--------------------------------------------------------------------------------\n";
   aOutputStream << "control_surface  " << aControlSurfaceName << "\n";

   aOutputStream << "   min_angle      ";
   OutputData(aOutputStream, aMinAngle_deg, width, precision, alignment);
   aOutputStream << " deg\n";
   aOutputStream << "   max_angle      ";
   OutputData(aOutputStream, aMaxAngle_deg, width, precision, alignment);
   aOutputStream << " deg\n";

   aOutputStream << "   current_angle  ";
   OutputData(aOutputStream, 0.0, width, precision, alignment);
   aOutputStream << " deg\n";

   // Inputs section

   aOutputStream << "   inputs\n";

   if (aUseStickBack)
   {
      OutputControlInputBlock(aOutputStream, "StickBack", aStickBackModifierList);
   }

   if (aUseRudderRight)
   {
      OutputControlInputBlock(aOutputStream, "RudderRight", aRudderRightModifierList);
   }

   if (aUseStickRight)
   {
      OutputControlInputBlock(aOutputStream, "StickRight", aStickRightModifierList);
   }

   if (aUseSpeedBrakesOut)
   {
      OutputControlInputBlock(aOutputStream, "SpeedBrakesOut", aSpeedBrakesOutModifierList);
   }

   aOutputStream << "   end_inputs\n";

   // Angle mapping section

   OutputAngleMappingTable(aOutputStream, aUseExpo, aMinAngle_deg, aMaxAngle_deg);

   // Actuator section

   OutputActuatorData(aOutputStream,
                      aMaxPositiveActuatorRate_dps,
                      aMaxNegativeActuatorRate_dps,
                      aMaxActuatorAngle_deg,
                      aMinActuatorAngle_deg,
                      aCurrentActuatorAngle_deg);

   aOutputStream << "end_control_surface\n";
   aOutputStream << "\n";
   }

   void ScriptGeneratorP6DOF::OutputActuatorData(QTextStream& aOutputStream,
                                         double       aMax_positive_rate_dps,
                                         double       aMax_negative_rate_dps,
                                         double       aMax_angle_deg,
                                         double       aMin_angle_deg,
                                         double       aCurrent_angle_deg)
   {
   // Output format
   aOutputStream.setRealNumberNotation(QTextStream::FixedNotation);
   QTextStream::FieldAlignment alignment = QTextStream::FieldAlignment::AlignRight;
   int width     = 8;
   int precision = 2;

   aOutputStream << "   actuator\n";

   aOutputStream << "      max_positive_rate  ";
   OutputData(aOutputStream, aMax_positive_rate_dps, width, precision, alignment);
   aOutputStream << " deg/sec\n";

   aOutputStream << "      max_negative_rate  ";
   OutputData(aOutputStream, aMax_negative_rate_dps, width, precision, alignment);
   aOutputStream << " deg/sec\n";

   aOutputStream << "      max_angle          ";
   OutputData(aOutputStream, aMax_angle_deg, width, precision, alignment);
   aOutputStream << " deg\n";

   aOutputStream << "      min_angle          ";
   OutputData(aOutputStream, aMin_angle_deg, width, precision, alignment);
   aOutputStream << " deg\n";

   aOutputStream << "      current_angle      ";
   OutputData(aOutputStream, aCurrent_angle_deg, width, precision, alignment);
   aOutputStream << " deg\n";

   aOutputStream << "   end_actuator\n";
   }

   void ScriptGeneratorP6DOF::OutputControlInputBlock(QTextStream&                aOutputStream,
                                              const QString&              aControlInputName,
                                              const std::vector<QString>& aModifierList)
   {
   aOutputStream << "      control_input  " << aControlInputName << "\n";

   for (auto& tmpStr : aModifierList)
   {
      aOutputStream << "         " << tmpStr << "\n";
   }

   aOutputStream << "      end_control_input\n";
   }

   void ScriptGeneratorP6DOF::OutputAngleMappingTable(QTextStream& aOutputStream,
                                              bool         aUseExponential,
                                              double       aMinAngle_deg,
                                              double       aMaxAngle_deg)
   {
   // Note: Output format will be set within OutputPairsInAngleMappingTable

   double minAngle_deg = std::abs(aMinAngle_deg);
   double maxAngle_deg = std::abs(aMaxAngle_deg);

   if (aUseExponential)
   {
      aOutputStream << "   angle_mapping_table\n";

      std::vector<std::pair<double, double>> tmpVectorPair;
      tmpVectorPair.emplace_back(-1.00, -1.000000 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.95, -0.866650 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.90, -0.746100 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.85, -0.637700 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.80, -0.540800 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.75, -0.454700 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.70, -0.378700 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.65, -0.312150 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.60, -0.254400 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.55, -0.204750 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.50, -0.162500 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.45, -0.127000 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.40, -0.097600 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.35, -0.073600 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.30, -0.054300 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.25, -0.039050 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.20, -0.027200 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.15, -0.018050 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.10, -0.010900 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.05, -0.005100 * minAngle_deg);
      tmpVectorPair.emplace_back(0.00,   0.000000);
      tmpVectorPair.emplace_back(0.05,   0.005100 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.10,   0.010900 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.15,   0.018050 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.20,   0.027200 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.25,   0.039050 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.30,   0.054300 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.35,   0.073600 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.40,   0.097600 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.45,   0.127000 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.50,   0.162500 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.55,   0.204750 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.60,   0.254400 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.65,   0.312150 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.70,   0.378700 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.75,   0.454700 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.80,   0.540800 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.85,   0.637700 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.90,   0.746100 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.95,   0.866650 * maxAngle_deg);
      tmpVectorPair.emplace_back(1.00,   1.000000 * maxAngle_deg);

      OutputPairsInAngleMappingTable(aOutputStream, tmpVectorPair);

      aOutputStream << "   end_angle_mapping_table\n";
   }
   else
   {
      aOutputStream << "   angle_mapping_table\n";

      std::vector<std::pair<double, double>> tmpVectorPair;
      tmpVectorPair.emplace_back(-1.0, -1.000000 * minAngle_deg);
      tmpVectorPair.emplace_back(0.0,  0.000000);
      tmpVectorPair.emplace_back(1.0,  1.000000 * maxAngle_deg);

      OutputPairsInAngleMappingTable(aOutputStream, tmpVectorPair);

      aOutputStream << "   end_angle_mapping_table\n";
   }
   }

   void ScriptGeneratorP6DOF::OutputPairsInAngleMappingTable(QTextStream&                                  aOutputStream,
                                                     const std::vector<std::pair<double, double>>& aVectorPair)
   {
   // Output format
   aOutputStream.setRealNumberNotation(QTextStream::FixedNotation);
   QTextStream::FieldAlignment alignment = QTextStream::FieldAlignment::AlignRight;
   int width     = 12;
   int precision = 6;

   for (auto& pair : aVectorPair)
   {
      aOutputStream << "   ";
      OutputData(aOutputStream, pair.first, width, precision, alignment);
      aOutputStream << "    ";
      OutputData(aOutputStream, pair.second, width, precision, alignment);
      aOutputStream << "\n";
   }
   }

   void ScriptGeneratorP6DOF::OutputConfigTailFins(QTextStream& aOutputStream)
   {
   VehicleGeometry& geometry = mVehiclePtr->GetGeometry();
   GeometrySurface* tailFin = geometry.GetTailControlFins();

   if (tailFin == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks a Tail Fin!");
      return;
   }

   // Setup data
   double minAngle_deg                = tailFin->GetControlSurfaceMinAngle_deg();
   double maxAngle_deg                = tailFin->GetControlSurfaceMaxAngle_deg();
   double currentAngle_deg            = 0.0;
   double minActuatorAngle_deg        = tailFin->GetActuatorMinAngle_deg();
   double maxActuatorAngle_deg        = tailFin->GetActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = tailFin->GetActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = tailFin->GetActuatorMaxRate_dps();
   double currentActuatorAngle_deg    = 0.0;
   bool useExpo                       = tailFin->GetUseExponentialAngleMapping();
   std::vector<QString> stickBackModifierList;
   std::vector<QString> rudderRightModifierList;
   std::vector<QString> stickRightModifierList;

   QString symmetryString = tailFin->GetSymmetryString();

   if (symmetryString == "X Pattern")
   {
      // Output the top right fin

      bool useStickBack = true;
      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickBackModifierList.emplace_back("modifier        NegativeGain");

      bool useRudderRight = true;
      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");

      bool useStickRight = true;
      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Tail_Control_Fin_TopRight",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the top left fin

      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");

      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Tail_Control_Fin_TopLeft",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the bottom right fin

      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickBackModifierList.emplace_back("modifier        NegativeGain");

      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      rudderRightModifierList.emplace_back("modifier        NegativeGain");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Tail_Control_Fin_BottomRight",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the bottom left fin

      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");

      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      rudderRightModifierList.emplace_back("modifier        NegativeGain");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Tail_Control_Fin_BottomLeft",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);
   }
   else if (symmetryString == "+ Pattern")
   {
      // Output the right fin

      bool useStickBack = true;
      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickBackModifierList.emplace_back("modifier        NegativeGain");

      bool useRudderRight = false;

      bool useStickRight = true;
      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Tail_Control_Fin_Right",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the top fin

      useStickBack = false;

      useRudderRight = true;
      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Tail_Control_Fin_Top",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the left fin

      useStickBack = true;
      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");

      useRudderRight = false;

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Tail_Control_Fin_Left",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the bottom fin

      useStickBack = false;

      useRudderRight = true;
      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      rudderRightModifierList.emplace_back("modifier        NegativeGain");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Tail_Control_Fin_Bottom",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("The specified Tail Fin needs to have a '+' or 'X' pattern!");
      return;
   }
   }

   void ScriptGeneratorP6DOF::OutputConfigMidFins(QTextStream& aOutputStream)
   {
   VehicleGeometry& geometry = mVehiclePtr->GetGeometry();
   GeometrySurface* midFin = geometry.GetMidControlFins();

   if (midFin == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks a Mid Fin!");
      return;
   }

   // Setup data
   double minAngle_deg                = midFin->GetControlSurfaceMinAngle_deg();
   double maxAngle_deg                = midFin->GetControlSurfaceMaxAngle_deg();
   double currentAngle_deg            = 0.0;
   double minActuatorAngle_deg        = midFin->GetActuatorMinAngle_deg();
   double maxActuatorAngle_deg        = midFin->GetActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = midFin->GetActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = midFin->GetActuatorMaxRate_dps();
   double currentActuatorAngle_deg    = 0.0;
   bool useExpo                       = midFin->GetUseExponentialAngleMapping();
   std::vector<QString> stickBackModifierList;
   std::vector<QString> rudderRightModifierList;
   std::vector<QString> stickRightModifierList;

   QString symmetryString = midFin->GetSymmetryString();

   if (symmetryString == "X Pattern")
   {
      // Output the top right fin

      bool useStickBack = true;
      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");

      bool useRudderRight = true;
      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      rudderRightModifierList.emplace_back("modifier        NegativeGain");

      bool useStickRight = true;
      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Mid_Control_Fin_TopRight",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the top left fin

      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickBackModifierList.emplace_back("modifier        NegativeGain");

      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      rudderRightModifierList.emplace_back("modifier        NegativeGain");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Mid_Control_Fin_TopLeft",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the bottom right fin

      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");

      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Mid_Control_Fin_BottomRight",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the bottom left fin

      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickBackModifierList.emplace_back("modifier        NegativeGain");

      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Mid_Control_Fin_BottomLeft",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);
   }
   else if (symmetryString == "+ Pattern")
   {
      // Output the right fin

      bool useStickBack = true;
      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");

      bool useRudderRight = false;

      bool useStickRight = true;
      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Mid_Control_Fin_Right",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the top fin

      useStickBack = false;

      useRudderRight = true;
      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      rudderRightModifierList.emplace_back("modifier        NegativeGain");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Mid_Control_Fin_Top",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the left fin

      useStickBack = true;
      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickBackModifierList.emplace_back("modifier        NegativeGain");

      useRudderRight = false;

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Mid_Control_Fin_Left",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the bottom fin

      useStickBack = false;

      useRudderRight = true;
      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Mid_Control_Fin_Bottom",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("The specified Mid Fin needs to have a '+' or 'X' pattern!");
      return;
   }
   }

   void ScriptGeneratorP6DOF::OutputConfigCanardFins(QTextStream& aOutputStream)
   {
   VehicleGeometry& geometry  = mVehiclePtr->GetGeometry();
   GeometrySurface* canardFin = geometry.GetCanardControlFins();

   if (canardFin == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks a Canard Fin!");
      return;
   }

   // Setup data
   double minAngle_deg                = canardFin->GetControlSurfaceMinAngle_deg();
   double maxAngle_deg                = canardFin->GetControlSurfaceMaxAngle_deg();
   double currentAngle_deg            = 0.0;
   double minActuatorAngle_deg        = canardFin->GetActuatorMinAngle_deg();
   double maxActuatorAngle_deg        = canardFin->GetActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = canardFin->GetActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = canardFin->GetActuatorMaxRate_dps();
   double currentActuatorAngle_deg    = 0.0;
   bool useExpo                       = canardFin->GetUseExponentialAngleMapping();
   std::vector<QString> stickBackModifierList;
   std::vector<QString> rudderRightModifierList;
   std::vector<QString> stickRightModifierList;

   QString symmetryString = canardFin->GetSymmetryString();

   if (symmetryString == "X Pattern")
   {
      // Output the top right fin

      bool useStickBack = true;
      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");

      bool useRudderRight = true;
      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      rudderRightModifierList.emplace_back("modifier        NegativeGain");

      bool useStickRight = true;
      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Canard_Control_Fin_TopRight",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the top left fin

      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickBackModifierList.emplace_back("modifier        NegativeGain");

      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      rudderRightModifierList.emplace_back("modifier        NegativeGain");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Canard_Control_Fin_TopLeft",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the bottom right fin

      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");

      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Canard_Control_Fin_BottomRight",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the bottom left fin

      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickBackModifierList.emplace_back("modifier        NegativeGain");

      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Canard_Control_Fin_BottomLeft",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);
   }
   else if (symmetryString == "+ Pattern")
   {
      // Output the right fin

      bool useStickBack = true;
      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");

      bool useRudderRight = false;

      bool useStickRight = true;
      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Canard_Control_Fin_Right",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the top fin

      useStickBack = false;

      useRudderRight = true;
      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      rudderRightModifierList.emplace_back("modifier        NegativeGain");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Canard_Control_Fin_Top",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the left fin

      useStickBack = true;
      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickBackModifierList.emplace_back("modifier        NegativeGain");

      useRudderRight = false;

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Canard_Control_Fin_Left",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the bottom fin

      useStickBack = false;

      useRudderRight = true;
      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Canard_Control_Fin_Bottom",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("The specified Canard Fin needs to have a '+' or 'X' pattern!");
      return;
   }
   }

   void ScriptGeneratorP6DOF::OutputWingControlSurfaces(QTextStream& aOutputStream)
   {
   // TODO: For each control surface that is present, output in a control_surface block
   }

   void ScriptGeneratorP6DOF::OutputSpeedBrakes(QTextStream& aOutputStream)
   {
   VehicleAero* vehicleAero = mVehiclePtr->GetAero();
   if (vehicleAero == nullptr) { return; }

   auto aeroObjVector = vehicleAero->GetAeroObjectBasedOnType("AeroSpeedBrake");

   for (auto& aeroObj : aeroObjVector)
   {
      auto speedBrake = dynamic_cast<AeroSpeedBrake*>(aeroObj);

      aOutputStream.setRealNumberNotation(QTextStream::FixedNotation);

      aOutputStream << "#--------------------------------------------------------------------------------\n";
      aOutputStream << "control_surface  " << QString(speedBrake->GetName().c_str()) << "\n";
      aOutputStream << "   min_angle      ";
      OutputData(aOutputStream, 0.0, 5, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "   max_angle      ";
      OutputData(aOutputStream, speedBrake->GetMaxAngle_deg(), 5, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "   current_angle  ";
      OutputData(aOutputStream, 0.0, 5, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "   inputs\n";
      aOutputStream << "      control_input  SpeedBrakesOut\n";
      aOutputStream << "         modifier  Clamp_PosNeg\n";
      aOutputStream << "      end_control_input\n";
      aOutputStream << "   end_inputs\n";
      aOutputStream << "   angle_mapping_table\n";
      aOutputStream << "       0.000   ";
      OutputData(aOutputStream, 0.0, 5, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << "\n";
      aOutputStream << "       1.000   ";
      OutputData(aOutputStream, speedBrake->GetMaxAngle_deg(), 5, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << "\n";
      aOutputStream << "   end_angle_mapping_table\n";
      aOutputStream << "   actuator\n";
      aOutputStream << "      max_positive_rate  ";
      OutputData(aOutputStream, speedBrake->GetMaxExtensionRate_dps(), 7, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg/sec\n";
      aOutputStream << "      max_negative_rate  ";
      OutputData(aOutputStream, speedBrake->GetMaxRetractionRate_dps(), 7, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg/sec\n";
      aOutputStream << "      max_angle          ";
      OutputData(aOutputStream, speedBrake->GetMaxAngle_deg(), 7, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "      min_angle          ";
      OutputData(aOutputStream, 0.0, 7, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "      current_angle      ";
      OutputData(aOutputStream, 0.0, 7, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "   end_actuator\n";
      aOutputStream << "end_control_surface\n";
      aOutputStream << "\n";
   }
   }

   void ScriptGeneratorP6DOF::OutputLandingGear(QTextStream& aOutputStream)
   {
   VehicleAero* vehicleAero = mVehiclePtr->GetAero();
   if (vehicleAero == nullptr) { return; }

   auto aeroObjVector = vehicleAero->GetAeroObjectBasedOnType("AeroLandingGear");

   for (auto& aeroObj : aeroObjVector)
   {
      auto landingGear = dynamic_cast<AeroLandingGear*>(aeroObj);

      aOutputStream.setRealNumberNotation(QTextStream::FixedNotation);

      aOutputStream << "#--------------------------------------------------------------------------------\n";
      aOutputStream << "control_surface  " << QString(landingGear->GetName().c_str()) << "\n";
      aOutputStream << "   min_angle      ";
      OutputData(aOutputStream, 0.0, 5, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "   max_angle      ";
      OutputData(aOutputStream, landingGear->GetMaxAngle_deg(), 5, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "   current_angle  ";
      OutputData(aOutputStream, 0.0, 5, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "   inputs\n";
      aOutputStream << "      control_input  LandingGearDown\n";
      aOutputStream << "         modifier  Clamp_PosNeg\n";
      aOutputStream << "      end_control_input\n";
      aOutputStream << "   end_inputs\n";
      aOutputStream << "   angle_mapping_table\n";
      aOutputStream << "       0.000   ";
      OutputData(aOutputStream, 0.0, 5, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << "\n";
      aOutputStream << "       1.000   ";
      OutputData(aOutputStream, landingGear->GetMaxAngle_deg(), 5, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << "\n";
      aOutputStream << "   end_angle_mapping_table\n";
      aOutputStream << "   actuator\n";
      aOutputStream << "      max_positive_rate  ";
      OutputData(aOutputStream, landingGear->GetMaxExtensionRate_dps(), 7, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg/sec\n";
      aOutputStream << "      max_negative_rate  ";
      OutputData(aOutputStream, landingGear->GetMaxRetractionRate_dps(), 7, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg/sec\n";
      aOutputStream << "      max_angle          ";
      OutputData(aOutputStream, landingGear->GetMaxAngle_deg(), 7, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "      min_angle          ";
      OutputData(aOutputStream, 0.0, 7, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "      current_angle      ";
      OutputData(aOutputStream, 0.0, 7, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "   end_actuator\n";
      aOutputStream << "end_control_surface\n";
      aOutputStream << "\n";
   }
   }

   void ScriptGeneratorP6DOF::CreateAutogenHeader(Vehicle* aVehiclePtr, QTextStream& aOutStream)
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

   QString ScriptGeneratorP6DOF::GenerateControlMethodString()
   {
   QString controlMethodStr = "BANK_TO_TURN_NO_YAW";

   if (mVehiclePtr == nullptr)
   {
      ut::log::error() << "Null vehicle in ScriptGeneratorP6DOF::GenerateControlMethodString().";
      return controlMethodStr;
   }

   // Generate the appropriate control type string
   if (mVehiclePtr->UsesBankToTurnControl())
   {
      VehicleControls::VehicleControlBankToTurnRollControl bankControl = VehicleControls::cAILERONS_ONLY;
      VehicleControls::VehicleControlBankToTurnYawControlType yawControlType = VehicleControls::cNO_YAW_CONTROL;
      bool alphaControlGain = false;

      mVehiclePtr->GetControls().GetBankToTurnControlSettings(bankControl, yawControlType, alphaControlGain);

      switch (yawControlType)
      {
         case (VehicleControls::cNO_YAW_CONTROL):
            controlMethodStr = "BANK_TO_TURN_NO_YAW";
            break;
         case (VehicleControls::cYAW_DAMPER):
            controlMethodStr = "BANK_TO_TURN_WITH_YAW";
            break;
         default:
            break;
      }
   }
   else
   {
      VehicleControls::VehicleControlSkidToTurnRollControlType rollControlType = VehicleControls::cNO_ROLL_CONTROL;
      bool alphaControlGain = false;

      mVehiclePtr->GetControls().GetSkidToTurnControlSettings(rollControlType, alphaControlGain);

      switch (rollControlType)
      {
         case (VehicleControls::cNO_ROLL_CONTROL):
            controlMethodStr = "YAW_TO_TURN_NO_ROLL";
            break;
         case (VehicleControls::cROLL_DAMPER):
            controlMethodStr = "YAW_TO_TURN_ROLL_RATE";
            break;
         case (VehicleControls::cFULL_ROLL_CONTROL):
            controlMethodStr = "YAW_TO_TURN_ZERO_BANK";
            break;
         default:
            break;
      }
   }

   return controlMethodStr;
   }

   void ScriptGeneratorP6DOF::CreateControlsInputFile(QString& aControlsDir)
   {
   QFile controlFile(aControlsDir + "/control_inputs.txt");
   if (!controlFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return;
   }

   VehicleControls& controls = mVehiclePtr->GetControls();

   QTextStream outStream(&controlFile);

   CreateAutogenHeader(mVehiclePtr, outStream);
   outStream << "pilot_manager\n";
   outStream << "\n";
   outStream << "   control_inputs\n";
   outStream << "\n";
   outStream << "      control_name StickBack\n";
   outStream << "      control_name StickRight\n";
   outStream << "      control_name RudderRight\n";
   outStream << "      control_name ThrottleMil\n";
   outStream << "      control_name ThrottleAB\n";
   outStream << "      control_name ThrustReverser\n";
   outStream << "      control_name ThrustVectorYaw\n";
   outStream << "      control_name ThrustVectorPitch\n";
   outStream << "      control_name ThrustVectorRoll\n";
   outStream << "      control_name LandingGearDown\n";
   outStream << "      control_name FlapsDown\n";
   outStream << "      control_name SpeedBrakesOut\n";
   outStream << "      control_name SpoilersOut\n";
   outStream << "      control_name NoseWheelRight\n";
   outStream << "      control_name NWS_Right\n";
   outStream << "      control_name NWS_Enable\n";
   outStream << "      control_name RgtWheelBrake\n";
   outStream << "      control_name LftWheelBrake\n";
   outStream << "      control_name TrimPitchUp\n";
   outStream << "      control_name TrimPitchDown\n";
   outStream << "      control_name TrimRollRight\n";
   outStream << "      control_name TrimRollLeft\n";
   outStream << "      control_name TriggerPulled\n";
   outStream << "      control_name ButtonPressed\n";
   outStream << "\n";
   outStream << "      std_stick_back              StickBack\n";
   outStream << "      std_stick_right             StickRight\n";
   outStream << "      std_rudder_right            RudderRight\n";
   outStream << "      std_throttle_mil            ThrottleMil\n";
   outStream << "      std_throttle_ab             ThrottleAB\n";
   outStream << "      std_thrust_reverser         ThrustReverser\n";
   outStream << "      std_thrust_vectoring_yaw    ThrustVectorYaw\n";
   outStream << "      std_thrust_vectoring_pitch  ThrustVectorPitch\n";
   outStream << "      std_thrust_vectoring_roll   ThrustVectorRoll\n";
   outStream << "      std_speed_brakes_out        SpeedBrakesOut\n";
   outStream << "      std_spoilers_out            SpoilersOut\n";
   outStream << "      std_flaps_down              FlapsDown\n";
   outStream << "      std_landing_gear_down       LandingGearDown\n";
   outStream << "      std_nose_wheel_steering     NoseWheelRight\n";
   outStream << "      std_nws_steering            NWS_Right\n";
   outStream << "      std_nws_enabled             NWS_Enable\n";
   outStream << "      std_wheel_brake_left        LftWheelBrake\n";
   outStream << "      std_wheel_brake_right       RgtWheelBrake\n";
   outStream << "\n";
   outStream << "   end_control_inputs\n";
   outStream << "\n";

   bool syntheticPilotPresent    = false;
   bool manualPilotPresent       = false;
   bool hardwareAutopilotPresent = false;
   bool guidanceSystemPresent    = false;
   VehicleControls::VehicleControlPilotType activePilot;

   controls.GetPilotTypes(syntheticPilotPresent,
                          manualPilotPresent,
                          hardwareAutopilotPresent,
                          guidanceSystemPresent,
                          activePilot);

   if (syntheticPilotPresent)
   {
      outStream << "   synthetic_pilot\n";
      outStream << "      # Define the autopilot configuration\n";
      outStream << "      include autopilot_config.txt\n";
      outStream << "   end_synthetic_pilot\n";
      outStream << "\n";
   }
   else
   {
      outStream << "/* synthetic_pilot\n";
      outStream << "      # Define the autopilot configuration\n";
      outStream << "      include autopilot_config.txt\n";
      outStream << "   end_synthetic_pilot\n";
      outStream << "*/\n";
   }

   if (manualPilotPresent)
   {
      bool useExponentialYawControlMapping = false;
      bool useExponentialPitchControlMapping = false;
      bool useExponentialRollControlMapping = false;
      double trimFactorYaw = 0.0;
      double trimFactorPitch = 0.0;
      double trimFactorRoll = 0.0;
      VehicleControls::VehicleControlAugmentationType pilotAugmentation = VehicleControls::cDIRECT_CONTROL;
      bool augmentStabilityYawAxis = false;
      bool augmentStabilityPitchAxis = false;
      bool augmentStabilityRollAxis = false;
      double pitchControlAugmented_Gs = 0.0;
      double rollControlAugmented_dps = 0.0;

      controls.GetManualPilotControlSettings(useExponentialYawControlMapping,
                                             useExponentialPitchControlMapping,
                                             useExponentialRollControlMapping,
                                             trimFactorYaw,
                                             trimFactorPitch,
                                             trimFactorRoll,
                                             pilotAugmentation,
                                             augmentStabilityPitchAxis,
                                             augmentStabilityYawAxis,
                                             augmentStabilityRollAxis,
                                             pitchControlAugmented_Gs,
                                             rollControlAugmented_dps);

      if (pilotAugmentation == VehicleControls::cDIRECT_CONTROL)
      {
         outStream << "   manual_pilot_simple_controls\n";
         outStream << "      simple_yaw_damper   false\n";
      }
      else if (pilotAugmentation == VehicleControls::cCONTROL_AUGMENTATION)
      {
         outStream << "   manual_pilot_augmented_controls\n";
         outStream << "      # Define the CAS autopilot configuration\n";
         outStream << "      include autopilot_config.txt\n";
         outStream << "\n";
      }
      else if (pilotAugmentation == VehicleControls::cSTABILITY_AUGMENTATION)
      {
         outStream << "   manual_pilot_augmented_stability\n";
         outStream << "      # Define the SAS autopilot configuration\n";
         outStream << "      include autopilot_config.txt\n";
         outStream << "\n";
      }

      if (controls.UseExponentialYawControlMapping())
      {
         outStream << "      yaw_control_mapping_table\n";
         outStream << "         -1.00   -1.00\n";
         outStream << "         -0.90   -0.75\n";
         outStream << "         -0.75   -0.45\n";
         outStream << "         -0.50   -0.15\n";
         outStream << "         -0.25   -0.05\n";
         outStream << "          0.00    0.00\n";
         outStream << "          0.25    0.05\n";
         outStream << "          0.50    0.15\n";
         outStream << "          0.75    0.45\n";
         outStream << "          0.90    0.75\n";
         outStream << "          1.00    1.00\n";
         outStream << "      end_yaw_control_mapping_table\n";
         outStream << "\n";
      }

      if (controls.UseExponentialPitchControlMapping())
      {
         outStream << "      pitch_control_mapping_table\n";
         outStream << "         -1.00   -1.00\n";
         outStream << "         -0.90   -0.75\n";
         outStream << "         -0.75   -0.45\n";
         outStream << "         -0.50   -0.15\n";
         outStream << "         -0.25   -0.05\n";
         outStream << "          0.00    0.00\n";
         outStream << "          0.25    0.05\n";
         outStream << "          0.50    0.15\n";
         outStream << "          0.75    0.45\n";
         outStream << "          0.90    0.75\n";
         outStream << "          1.00    1.00\n";
         outStream << "      end_pitch_control_mapping_table\n";
         outStream << "\n";
      }

      if (controls.UseExponentialRollControlMapping())
      {
         outStream << "      roll_control_mapping_table\n";
         outStream << "         -1.00   -1.00\n";
         outStream << "         -0.90   -0.75\n";
         outStream << "         -0.75   -0.45\n";
         outStream << "         -0.50   -0.15\n";
         outStream << "         -0.25   -0.05\n";
         outStream << "          0.00    0.00\n";
         outStream << "          0.25    0.05\n";
         outStream << "          0.50    0.15\n";
         outStream << "          0.75    0.45\n";
         outStream << "          0.90    0.75\n";
         outStream << "          1.00    1.00\n";
         outStream << "      end_roll_control_mapping_table\n";
         outStream << "\n";
      }

      outStream << "\n";
      outStream << "      pitch_trim_factor                     " << trimFactorPitch << "\n";
      outStream << "      roll_trim_factor                      " << trimFactorRoll << "\n";
      outStream << "      yaw_trim_factor                       " << trimFactorYaw << "\n";
      outStream << "\n";

      if (pilotAugmentation == VehicleControls::cCONTROL_AUGMENTATION)
      {
         outStream << "      pitch_control_augmentation_factor_g   " << pitchControlAugmented_Gs << "\n";
         outStream << "      roll_control_augmentation_factor_dps  " << rollControlAugmented_dps << "\n";
      }

      if (pilotAugmentation == VehicleControls::cSTABILITY_AUGMENTATION)
      {
         if (augmentStabilityPitchAxis)
         {
            outStream << "      pitch_stability_augmentation          true\n";
         }
         else
         {
            outStream << "      pitch_stability_augmentation          false\n";
         }

         if (augmentStabilityYawAxis)
         {
            outStream << "      yaw_stability_augmentation            true\n";
         }
         else
         {
            outStream << "      yaw_stability_augmentation            false\n";
         }

         if (augmentStabilityRollAxis)
         {
            outStream << "      roll_stability_augmentation           true\n";
         }
         else
         {
            outStream << "      roll_stability_augmentation           false\n";
         }
      }

      if (pilotAugmentation == VehicleControls::cDIRECT_CONTROL)
      {
         outStream << "   end_manual_pilot_simple_controls\n";
      }
      else if (pilotAugmentation == VehicleControls::cCONTROL_AUGMENTATION)
      {
         outStream << "   end_manual_pilot_augmented_controls\n";
      }
      else if (pilotAugmentation == VehicleControls::cSTABILITY_AUGMENTATION)
      {
         outStream << "   end_manual_pilot_augmented_stability\n";
      }
      outStream << "\n";
   }
   else
   {
      outStream << "/* manual_pilot_simple_controls\n";
      outStream << "      simple_yaw_damper   false\n";
      outStream << "      pitch_trim_factor   0.1\n";
      outStream << "      roll_trim_factor    0.1\n";
      outStream << "      yaw_trim_factor     0.1\n";
      outStream << "\n";
      outStream << "   end_manual_pilot_simple_controls\n";
      outStream << "*/\n";
   }

   if (mVehiclePtr->UsesBankToTurnControl())
   {
      if (hardwareAutopilotPresent)
      {
         outStream << "   hardware_autopilot_bank_to_turn\n";
         outStream << "      # Define the hardware autopilot configuration\n";
         outStream << "      include hw_autopilot_config.txt\n";
         outStream << "   end_hardware_autopilot_bank_to_turn\n";
         outStream << "\n";
      }

      if (guidanceSystemPresent)
      {
         outStream << "   guidance_autopilot_bank_to_turn\n";
         outStream << "      # Define the guidance configuration\n";
         outStream << "      include autopilot_config.txt\n";
         outStream << "   end_guidance_autopilot_bank_to_turn\n";
         outStream << "\n";
      }
   }
   else
   {
      if (hardwareAutopilotPresent)
      {
         outStream << "   hardware_autopilot_skid_to_turn\n";
         outStream << "      # Define the hardware autopilot configuration\n";
         outStream << "      include hw_autopilot_config.txt\n";
         outStream << "   end_hardware_autopilot_skid_to_turn\n";
         outStream << "\n";
      }

      if (guidanceSystemPresent)
      {
         outStream << "   guidance_autopilot_skid_to_turn\n";
         outStream << "      # Define the guidance configuration\n";
         outStream << "      include autopilot_config.txt\n";
         outStream << "   end_guidance_autopilot_skid_to_turn\n";
         outStream << "\n";
      }
   }

   outStream << "   # Make the synthetic pilot active\n";
   outStream << "   active_pilot synthetic_pilot\n";
   outStream << "\n";
   outStream << "   # This autopilot support file is common to all pilots\n";
   outStream << "   # that use a P6DofCommonController (autopilot)\n";
   outStream << "   common_autopilot_support_file      autopilot_support_tables.txt\n";
   outStream << "\n";
   outStream << "end_pilot_manager\n";
   controlFile.close();

   //create autopilot_config.txt file
   CreateAutopilotConfigFile(aControlsDir);
   CreateAutopilotG_LimitConfigFile(aControlsDir);

   if (MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetControlsWidget()->IsHardwareAutoPilotAvailable())
   {
      QFile hwAutopilotConfigFile(aControlsDir + "/hw_autopilot_config.txt");
      if (!hwAutopilotConfigFile.exists())
      {
         CreateHardwareAutopilotConfigFile(aControlsDir);
      }
      CreateHardwareG_LimitsConfigFile(aControlsDir);
   }
   }

   void ScriptGeneratorP6DOF::CreateAutopilotConfigFile(QString& aControlsDir)
   {
   if (mVehiclePtr == nullptr)
   {
      ut::log::error() << "Null vehicle in ScriptGeneratorP6DOF::CreateAutopilotConfigFile().";
      return;
   }

   QFile genFile(aControlsDir + "/autopilot_config.txt");

   // If the file already exists, we want to avoid overwriting tuned PID values.
   // Update the control method, and return.
   if (genFile.exists())
   {
      // Retrieve the contents of the existing file
      genFile.open(QIODevice::ReadOnly);
      QByteArray fileData = genFile.readAll();
      QString text(fileData);

      QString controlMethodStr = GenerateControlMethodString();

      // Assume we're only using one control method, and replace any instance of a stale control method string.
      // If we wanted to be safer, we might instead search for the full control_method line.
      text.replace(QString("BANK_TO_TURN_NO_YAW"), controlMethodStr);
      text.replace(QString("BANK_TO_TURN_WITH_YAW"), controlMethodStr);
      text.replace(QString("YAW_TO_TURN_NO_ROLL"), controlMethodStr);
      text.replace(QString("YAW_TO_TURN_ROLL_RATE"), controlMethodStr);
      text.replace(QString("YAW_TO_TURN_ZERO_BANK"), controlMethodStr);
      genFile.close();

      // Re-open the file to post the edited version
      genFile.open(QIODevice::WriteOnly);
      genFile.write(text.toUtf8());
      genFile.close();
      return;
   }


   if (!genFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   QTextStream outStream(&genFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "# **************************************************************************************************\n";
   outStream << "# This file defines the configuration of the autopilot, including its PIDs. A PID is a proportional,\n";
   outStream << "# integral, derivative feedback control technique. Multiple PIDs are used to 'define' the autopilot.\n";
   outStream << "# Each control output may be influenced by one or more PIDs.\n";
   outStream << "# **************************************************************************************************\n";
   outStream << "\n";
   outStream << "autopilot_config\n\n";

   QString controlMethodStr = GenerateControlMethodString();

   outStream << "  vertical_middle_loop_rate_factor  4\n";
   outStream << "  vertical_outer_loop_rate_factor   4\n";
   outStream << "  lateral_middle_loop_rate_factor   4\n";
   outStream << "  lateral_outer_loop_rate_factor    4\n";
   outStream << "  speed_middle_loop_rate_factor     200\n";
   outStream << "  speed_outer_loop_rate_factor      4\n";
   outStream << "\n";
   outStream << "  control_method                    " << controlMethodStr << "\n";
   outStream << "\n";
   outStream << "  use_legacy_beta                   false\n";
   outStream << "\n";
   outStream << "  min_taxi_turn_radius              50.0 ft   # This can be manually 'tuned' for improved performance\n";
   outStream << "\n";
   outStream << "  use_simple_yaw_damper             false     # This can be set to true to damp yaw oscillations\n";
   outStream << "\n";
   outStream << "# ------------------------------------------------------------\n";
   outStream << "\n";

   outStream << "  pid_group\n";
   outStream << "\n";
   outStream << "    pid_alpha\n";
   outStream << "      kp 0.001\n";
   outStream << "      ki 0.1\n";
   outStream << "      kd 0.0001\n";
   outStream << "      max_error_accum 0\n";
   outStream << "      low_pass_alpha 0.7\n";
   outStream << "    end_pid_alpha\n";
   outStream << "\n";
   outStream << "    pid_altitude\n";
   outStream << "      kp 10\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 100\n";
   outStream << "    end_pid_altitude\n";
   outStream << "\n";
   outStream << "    pid_bank_angle\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 1\n";
   outStream << "    end_pid_bank_angle\n";
   outStream << "\n";
   outStream << "    pid_beta\n";
   outStream << "      kp 0.001\n";
   outStream << "      ki 0.1\n";
   outStream << "      kd 0.0001\n";
   outStream << "      max_error_accum 0\n";
   outStream << "      low_pass_alpha 0.7\n";
   outStream << "    end_pid_beta\n";
   outStream << "\n";
   outStream << "    pid_delta_pitch\n";
   outStream << "      kp 0.6\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 10\n";
   outStream << "    end_pid_delta_pitch\n";
   outStream << "\n";
   outStream << "    pid_delta_roll\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 1\n";
   outStream << "    end_pid_delta_roll\n";
   outStream << "\n";
   outStream << "    pid_flightpath_angle\n";
   outStream << "      kp 0.5\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0.0\n";
   outStream << "      max_error_accum 0.5\n";
   outStream << "    end_pid_flightpath_angle\n";
   outStream << "\n";
   outStream << "    pid_forward_accel\n";
   outStream << "      kp 20\n";
   outStream << "      ki 2\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 1\n";
   outStream << "    end_pid_forward_accel\n";
   outStream << "\n";
   outStream << "    pid_pitch_angle\n";
   outStream << "      kp 0.5\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0.0\n";
   outStream << "      max_error_accum 0.5\n";
   outStream << "    end_pid_pitch_angle\n";
   outStream << "\n";
   outStream << "    pid_pitch_rate\n";
   outStream << "      kp 0\n";
   outStream << "      ki 0.2\n";
   outStream << "      kd 0.0001\n";
   outStream << "      max_error_accum 0\n";
   outStream << "    end_pid_pitch_rate\n";
   outStream << "\n";
   outStream << "    pid_roll_heading\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 2\n";
   outStream << "      low_pass_alpha 0.5\n";
   outStream << "    end_pid_roll_heading\n";
   outStream << "\n";
   outStream << "    pid_roll_rate\n";
   outStream << "      kp 0.005\n";
   outStream << "      ki 0.05\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 0\n";
   outStream << "      low_pass_alpha 0.7\n";
   outStream << "    end_pid_roll_rate\n";
   outStream << "\n";
   outStream << "    pid_speed\n";
   outStream << "      kp 0.5\n";
   outStream << "      ki 0.05\n";
   outStream << "      kd 0.5\n";
   outStream << "      max_error_accum 10\n";
   outStream << "    end_pid_speed\n";
   outStream << "\n";
   outStream << "    pid_taxi_forward_accel\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 1\n";
   outStream << "    end_pid_taxi_forward_accel\n";
   outStream << "\n";
   outStream << "    pid_taxi_heading\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0.01\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 10\n";
   outStream << "    end_pid_taxi_heading\n";
   outStream << "\n";
   outStream << "    pid_taxi_speed\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 1\n";
   outStream << "    end_pid_taxi_speed\n";
   outStream << "\n";
   outStream << "    pid_taxi_yaw_rate\n";
   outStream << "      kp 0.1\n";
   outStream << "      ki 0.2\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 10\n";
   outStream << "    end_pid_taxi_yaw_rate\n";
   outStream << "\n";
   outStream << "    pid_vert_speed\n";
   outStream << "      kp 0.0012\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 60\n";
   outStream << "      ignore_large_error_accum 500\n";
   outStream << "    end_pid_vert_speed\n";
   outStream << "\n";
   outStream << "    pid_yaw_heading\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 1\n";
   outStream << "    end_pid_yaw_heading\n";
   outStream << "\n";
   outStream << "    pid_yaw_rate\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 0\n";
   outStream << "    end_pid_yaw_rate\n";
   outStream << "\n";
   outStream << "\n";

   if (mVehiclePtr->IsAircraft())
   {
      outStream << "    limits_and_settings\n";
      outStream << "     include autopilot_config_g_limits.txt\n";
      outStream << "     afterburner_threshold       1\n";
      outStream << "     speedbrake_threshold        0\n";
      outStream << "     alpha_min                   -16\n";
      outStream << "     alpha_max                   20\n";
      outStream << "     pitch_rate_min              -10\n";
      outStream << "     pitch_rate_max              40\n";
      outStream << "     vert_speed_min              -15000\n";
      outStream << "     vert_speed_max              15000\n";
      outStream << "     beta_max                    10\n";
      outStream << "     yaw_rate_max                5\n";
      outStream << "     roll_rate_max               180\n";
      outStream << "     bank_angle_max              60\n";
      outStream << "     forward_accel_min           -1\n";
      outStream << "     forward_accel_max           2\n";
      outStream << "     taxi_speed_max_fps          10\n";
      outStream << "     taxi_yaw_rate_max           10\n";
      outStream << "     turn_roll_in_multiplier     4.0       # This can be manually 'tuned' for improved performance\n";
      outStream << "     route_allowable_angle_error 5.0 deg   # This can be manually 'tuned' for improved performance\n";
      outStream << "    end_limits_and_settings\n";
   }
   else if (mVehiclePtr->IsWeapon())
   {
      if (mVehiclePtr->UsesBankToTurnControl())
      {
         outStream << "    limits_and_settings\n";
         outStream << "     include autopilot_config_g_limits.txt\n";
         outStream << "     afterburner_threshold       1\n";
         outStream << "     speedbrake_threshold        0\n";
         outStream << "     alpha_min                   -16\n";
         outStream << "     alpha_max                   20\n";
         outStream << "     pitch_rate_min              -10\n";
         outStream << "     pitch_rate_max              40\n";
         outStream << "     vert_speed_min              -15000\n";
         outStream << "     vert_speed_max              15000\n";
         outStream << "     beta_max                    10\n";
         outStream << "     yaw_rate_max                5\n";
         outStream << "     roll_rate_max               180\n";
         outStream << "     bank_angle_max              60\n";
         outStream << "     forward_accel_min           -1\n";
         outStream << "     forward_accel_max           2\n";
         outStream << "     taxi_speed_max_fps          10\n";
         outStream << "     taxi_yaw_rate_max           10\n";
         outStream << "     turn_roll_in_multiplier     4.0       # This can be manually 'tuned' for improved performance\n";
         outStream << "     route_allowable_angle_error 5.0 deg   # This can be manually 'tuned' for improved performance\n";
         outStream << "    end_limits_and_settings\n";
      }
      else
      {
         outStream << "    limits_and_settings\n";
         outStream << "     include autopilot_config_g_limits.txt\n";
         outStream << "     afterburner_threshold       1\n";
         outStream << "     speedbrake_threshold        0\n";
         outStream << "     alpha_min                   -20\n";
         outStream << "     alpha_max                   20\n";
         outStream << "     pitch_rate_min              -60\n";
         outStream << "     pitch_rate_max              60\n";
         outStream << "     vert_speed_min              -60000\n";
         outStream << "     vert_speed_max              60000\n";
         outStream << "     beta_max                    20.0\n";
         outStream << "     yaw_rate_max                60.0\n";
         outStream << "     roll_rate_max               360.0\n";
         outStream << "     bank_angle_max              89.0\n";
         outStream << "     forward_accel_min           -1\n";
         outStream << "     forward_accel_max           2\n";
         outStream << "     taxi_speed_max_fps          10\n";
         outStream << "     taxi_yaw_rate_max           10\n";
         outStream << "     turn_roll_in_multiplier     4.0       # This can be manually 'tuned' for improved performance\n";
         outStream << "     route_allowable_angle_error 5.0 deg   # This can be manually 'tuned' for improved performance\n";
         outStream << "    end_limits_and_settings\n";
      }
   }

   outStream << "\n";
   outStream << "  end_pid_group\n";
   outStream << "\n";
   outStream << "end_autopilot_config\n";
   outStream << "\n";

   genFile.close();
   }

   void ScriptGeneratorP6DOF::CreateAutopilotG_LimitConfigFile(QString& aControlsDir)
   {
   if (mVehiclePtr == nullptr)
   {
      ut::log::error() << "Null vehicle in ScriptGeneratorP6DOF::CreateAutopilotConfigFile().";
      return;
   }

   QFile genFile(aControlsDir + "/autopilot_config_g_limits.txt");
   if (!genFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   QTextStream outStream(&genFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentVehicleFile();
   QJsonObject mainObject = curDoc->object();
   G_LimitSettings settings{G_LimitSettings::PilotsGuidanceName()};
   settings.LoadFromFile(mainObject);

   outStream << "     pitch_gload_min             " << settings.GetPitchG_LoadMin() << "\n";
   outStream << "     pitch_gload_max             " << settings.GetPitchG_LoadMax() << "\n";
   outStream << "     yaw_gload_max               " << settings.GetYawG_LoadMax() << "\n";

   genFile.close();
   }

   void ScriptGeneratorP6DOF::CreateHardwareAutopilotConfigFile(QString& aControlsDir)
   {
   if (mVehiclePtr == nullptr)
   {
      ut::log::error() << "Null vehicle in ScriptGeneratorP6DOF::CreateHardwareAutopilotConfigFile().";
      return;
   }

   QFile genFile(aControlsDir + "/hw_autopilot_config.txt");
   if (!genFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   QTextStream outStream(&genFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "# **************************************************************************************************\n";
   outStream << "# This file defines the configuration of the autopilot, including its PIDs. A PID is a proportional,\n";
   outStream << "# integral, derivative feedback control technique. Multiple PIDs are used to 'define' the autopilot.\n";
   outStream << "# Each control output may be influenced by one or more PIDs.\n";
   outStream << "# **************************************************************************************************\n";
   outStream << "\n";
   outStream << "autopilot_config\n\n";

   QString controlMethodStr = GenerateControlMethodString();

   outStream << "  vertical_middle_loop_rate_factor  4\n";
   outStream << "  vertical_outer_loop_rate_factor   4\n";
   outStream << "  lateral_middle_loop_rate_factor   4\n";
   outStream << "  lateral_outer_loop_rate_factor    4\n";
   outStream << "  speed_middle_loop_rate_factor     200\n";
   outStream << "  speed_outer_loop_rate_factor      4\n";
   outStream << "\n";
   outStream << "  control_method                    " << controlMethodStr << "\n";
   outStream << "\n";
   outStream << "  use_legacy_beta                   false\n";
   outStream << "\n";
   outStream << "  min_taxi_turn_radius              50.0 ft   # This can be manually 'tuned' for improved performance\n";
   outStream << "\n";
   outStream << "  use_simple_yaw_damper             false     # This can be set to true to damp yaw oscillations\n";
   outStream << "\n";
   outStream << "# ------------------------------------------------------------\n";
   outStream << "\n";

   outStream << "  pid_group\n";
   outStream << "\n";
   outStream << "    pid_alpha\n";
   outStream << "      kp 0.001\n";
   outStream << "      ki 0.1\n";
   outStream << "      kd 0.0001\n";
   outStream << "      max_error_accum 0.5\n";
   outStream << "      low_pass_alpha 0.7\n";
   outStream << "    end_pid_alpha\n";
   outStream << "\n";
   outStream << "    pid_altitude\n";
   outStream << "      kp 10\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 100\n";
   outStream << "    end_pid_altitude\n";
   outStream << "\n";
   outStream << "    pid_bank_angle\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 1\n";
   outStream << "    end_pid_bank_angle\n";
   outStream << "\n";
   outStream << "    pid_beta\n";
   outStream << "      kp 0.001\n";
   outStream << "      ki 0.1\n";
   outStream << "      kd 0.0001\n";
   outStream << "      max_error_accum 0.5\n";
   outStream << "      low_pass_alpha 0.7\n";
   outStream << "    end_pid_beta\n";
   outStream << "\n";
   outStream << "    pid_delta_pitch\n";
   outStream << "      kp 0.6\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 10\n";
   outStream << "    end_pid_delta_pitch\n";
   outStream << "\n";
   outStream << "    pid_delta_roll\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 1\n";
   outStream << "    end_pid_delta_roll\n";
   outStream << "\n";
   outStream << "    pid_flightpath_angle\n";
   outStream << "      kp 0.5\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0.0\n";
   outStream << "      max_error_accum 0.5\n";
   outStream << "    end_pid_flightpath_angle\n";
   outStream << "\n";
   outStream << "    pid_forward_accel\n";
   outStream << "      kp 20\n";
   outStream << "      ki 2\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 1\n";
   outStream << "    end_pid_forward_accel\n";
   outStream << "\n";
   outStream << "    pid_pitch_angle\n";
   outStream << "      kp 0.5\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0.0\n";
   outStream << "      max_error_accum 0.5\n";
   outStream << "    end_pid_pitch_angle\n";
   outStream << "\n";
   outStream << "    pid_pitch_rate\n";
   outStream << "      kp 0\n";
   outStream << "      ki 0.2\n";
   outStream << "      kd 0.0001\n";
   outStream << "      max_error_accum 10\n";
   outStream << "    end_pid_pitch_rate\n";
   outStream << "\n";
   outStream << "    pid_roll_heading\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 2\n";
   outStream << "      low_pass_alpha 0.5\n";
   outStream << "    end_pid_roll_heading\n";
   outStream << "\n";
   outStream << "    pid_roll_rate\n";
   outStream << "      kp 0.005\n";
   outStream << "      ki 0.05\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 20\n";
   outStream << "    end_pid_roll_rate\n";
   outStream << "\n";
   outStream << "    pid_speed\n";
   outStream << "      kp 0.5\n";
   outStream << "      ki 0.05\n";
   outStream << "      kd 0.5\n";
   outStream << "      max_error_accum 10\n";
   outStream << "    end_pid_speed\n";
   outStream << "\n";
   outStream << "    pid_taxi_forward_accel\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 1\n";
   outStream << "    end_pid_taxi_forward_accel\n";
   outStream << "\n";
   outStream << "    pid_taxi_heading\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0.01\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 10\n";
   outStream << "    end_pid_taxi_heading\n";
   outStream << "\n";
   outStream << "    pid_taxi_speed\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 1\n";
   outStream << "    end_pid_taxi_speed\n";
   outStream << "\n";
   outStream << "    pid_taxi_yaw_rate\n";
   outStream << "      kp 0.1\n";
   outStream << "      ki 0.2\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 10\n";
   outStream << "    end_pid_taxi_yaw_rate\n";
   outStream << "\n";
   outStream << "    pid_vert_speed\n";
   outStream << "      kp 0.0012\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 60\n";
   outStream << "      ignore_large_error_accum 500\n";
   outStream << "    end_pid_vert_speed\n";
   outStream << "\n";
   outStream << "    pid_yaw_heading\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 1\n";
   outStream << "    end_pid_yaw_heading\n";
   outStream << "\n";
   outStream << "    pid_yaw_rate\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 1\n";
   outStream << "    end_pid_yaw_rate\n";
   outStream << "\n";
   outStream << "\n";

   if (mVehiclePtr->IsAircraft())
   {
      outStream << "    limits_and_settings\n";
      outStream << "     include hw_autopilot_config_g_limits.txt\n";
      outStream << "     afterburner_threshold       1\n";
      outStream << "     speedbrake_threshold        0\n";
      outStream << "     alpha_min                   -16\n";
      outStream << "     alpha_max                   20\n";
      outStream << "     pitch_rate_min              -10\n";
      outStream << "     pitch_rate_max              40\n";
      outStream << "     vert_speed_min              -15000\n";
      outStream << "     vert_speed_max              15000\n";
      outStream << "     beta_max                    10\n";
      outStream << "     yaw_rate_max                5\n";
      outStream << "     roll_rate_max               180\n";
      outStream << "     bank_angle_max              60\n";
      outStream << "     forward_accel_min           -1\n";
      outStream << "     forward_accel_max           2\n";
      outStream << "     taxi_speed_max_fps          10\n";
      outStream << "     taxi_yaw_rate_max           10\n";
      outStream << "     turn_roll_in_multiplier     4.0       # This can be manually 'tuned' for improved performance\n";
      outStream << "     route_allowable_angle_error 5.0 deg   # This can be manually 'tuned' for improved performance\n";
      outStream << "    end_limits_and_settings\n";
   }
   else if (mVehiclePtr->IsWeapon())
   {
      if (mVehiclePtr->UsesBankToTurnControl())
      {
         outStream << "    limits_and_settings\n";
         outStream << "     include hw_autopilot_config_g_limits.txt\n";
         outStream << "     afterburner_threshold       1\n";
         outStream << "     speedbrake_threshold        0\n";
         outStream << "     alpha_min                   -16\n";
         outStream << "     alpha_max                   20\n";
         outStream << "     pitch_rate_min              -10\n";
         outStream << "     pitch_rate_max              40\n";
         outStream << "     vert_speed_min              -15000\n";
         outStream << "     vert_speed_max              15000\n";
         outStream << "     beta_max                    10\n";
         outStream << "     yaw_rate_max                5\n";
         outStream << "     roll_rate_max               180\n";
         outStream << "     bank_angle_max              60\n";
         outStream << "     forward_accel_min           -1\n";
         outStream << "     forward_accel_max           2\n";
         outStream << "     taxi_speed_max_fps          10\n";
         outStream << "     taxi_yaw_rate_max           10\n";
         outStream << "     turn_roll_in_multiplier     4.0       # This can be manually 'tuned' for improved performance\n";
         outStream << "     route_allowable_angle_error 5.0 deg   # This can be manually 'tuned' for improved performance\n";
         outStream << "    end_limits_and_settings\n";
      }
      else
      {
         outStream << "    limits_and_settings\n";
         outStream << "     include hw_autopilot_config_g_limits.txt\n";
         outStream << "     afterburner_threshold       1\n";
         outStream << "     speedbrake_threshold        0\n";
         outStream << "     alpha_min                   -20\n";
         outStream << "     alpha_max                   20\n";
         outStream << "     pitch_rate_min              -60\n";
         outStream << "     pitch_rate_max              60\n";
         outStream << "     vert_speed_min              -60000\n";
         outStream << "     vert_speed_max              60000\n";
         outStream << "     beta_max                    20.0\n";
         outStream << "     yaw_rate_max                60.0\n";
         outStream << "     roll_rate_max               360.0\n";
         outStream << "     bank_angle_max              89.0\n";
         outStream << "     forward_accel_min           -1\n";
         outStream << "     forward_accel_max           2\n";
         outStream << "     taxi_speed_max_fps          10\n";
         outStream << "     taxi_yaw_rate_max           10\n";
         outStream << "     turn_roll_in_multiplier     4.0       # This can be manually 'tuned' for improved performance\n";
         outStream << "     route_allowable_angle_error 5.0 deg   # This can be manually 'tuned' for improved performance\n";
         outStream << "    end_limits_and_settings\n";
      }
   }

   outStream << "\n";
   outStream << "  end_pid_group\n";
   outStream << "\n";
   outStream << "end_autopilot_config\n";
   outStream << "\n";

   genFile.close();
   }

   void ScriptGeneratorP6DOF::CreateHardwareG_LimitsConfigFile(QString& aControlsDir)
   {
   if (mVehiclePtr == nullptr)
   {
      ut::log::error() << "Null vehicle in ScriptGeneratorP6DOF::CreateAutopilotConfigFile().";
      return;
   }

   QFile genFile(aControlsDir + "/hw_autopilot_config_g_limits.txt");
   if (!genFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   QTextStream outStream(&genFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentVehicleFile();
   QJsonObject mainObject = curDoc->object();
   G_LimitSettings settings{G_LimitSettings::HardwareAutopilotName()};
   settings.LoadFromFile(mainObject);

   outStream << "     pitch_gload_min             " << settings.GetPitchG_LoadMin() << "\n";
   outStream << "     pitch_gload_max             " << settings.GetPitchG_LoadMax() << "\n";
   outStream << "     yaw_gload_max               " << settings.GetYawG_LoadMax() << "\n";

   genFile.close();
   }

   void ScriptGeneratorP6DOF::CopyAutopilotSupportTables(const QString& aControlsSourceDir)
   {
   QString fileName = "/autopilot_support_tables.txt";
   QString controlsTargetFile = ControlsFolderPath() + fileName;
   QString controlsSourceFile = aControlsSourceDir + fileName;

   if (QFile::exists(controlsTargetFile))
   {
      QFile::remove(controlsTargetFile);
   }
   QFile::copy(controlsSourceFile, controlsTargetFile);
   }

   ////////////////////////////////////////////////////////////////////
   //          Performance tests file creation
   ////////////////////////////////////////////////////////////////////

   void ScriptGeneratorP6DOF::CreateCommonTestFile(QString& aTestDir)
   {
   QFile testFile(aTestDir + "/common.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return;
   }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << GetEnableControlsScript();
   outStream << GetSetMasterNoLagTestingScript();
   outStream << GetSetMasterFreezeAltitudeScript();
   outStream << GetSetMasterFreezePitchScript();
   outStream << GetSetMasterFreezeRollScript();
   outStream << GetSetMasterFreezeYawScript();
   outStream << GetSetMasterFreezeFuelBurnScript();
   outStream << GetSetMasterNoAlphaTestingScript();
   outStream << GetRangeSinceStart_kmScript();
   outStream << GetFuelRemainingScript();

   testFile.close();
   }

   void ScriptGeneratorP6DOF::CreateEnvelopeCommonTestFile(QString& aTestDir)
   {
   QFile testFile(aTestDir + "/envelope_common.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return;
   }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << GetFlightEnvelopeScripts();

   testFile.close();
   }

   void ScriptGeneratorP6DOF::CreateBallisticRangeTestFile(QString& aTestDir)
   {
   QFile testFile(aTestDir + "/ballistic_range_test.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   { return; }

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
   outStream << "script_variables\n";
   outStream << "   // Note that a non-zero launch altitude is used to avoid\n";
   outStream << "   // collision with the ground during initial acceleration\n";
   outStream << "   double INITIAL_ALTITUDE_FT    = 1000;\n";
   outStream << "   double MIN_LAUNCH_ANGLE_DEG   = 10;\n";
   outStream << "   double MAX_LAUNCH_ANGLE_DEG   = 80;\n";
   outStream << "   double LAUNCH_ANGLE_INCREMENT = 1;\n";
   outStream << "end_script_variables\n";
   outStream << "//**********************************************************\n";
   outStream << "\n";
   outStream << "include_once ../../p6dof_types/environment/p6dof_environment.txt\n";
   outStream << "include_once ../../p6dof_types/" << mVehicleType << "/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << ".txt\n";
   outStream << "\n";
   outStream << "# Set a start time that has a daytime sun position for our test in the Oahu region\n";
   outStream << "start_epoch 2018354.75   # Southern Solstice 2018\n";
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";
   outStream << "script_struct DataStruct\n";
   outStream << "   script_variables\n";
   outStream << "      double maxAltitude_m       = 0;\n";
   outStream << "      double timeMaxAltitude_sec = 0;\n";
   outStream << "      double maxSpeed_mps        = 0;\n";
   outStream << "      double timeMaxSpeed_sec    = 0;\n";
   outStream << "      double maxRange_km         = 0;\n";
   outStream << "      double totalFlightTime_sec = -1;\n";
   outStream << "      double launchAngle_deg     = 0;\n";
   outStream << "   end_script_variables\n";
   outStream << "end_script_struct\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   Map<int, struct> mData = Map<int, struct>();\n";
   outStream << "   \n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_ballistic_range_test.csv\";\n";
   outStream << "end_script_variables\n";
   outStream << GetRangeSinceStart_kmScript();
   outStream << "\n";
   outStream << "platform_type SAM_TEST_PLATFORM WSF_PLATFORM\n";
   outStream << "   spatial_domain        air\n";
   outStream << "   \n";
   outStream << "   mover WSF_P6DOF_MOVER\n";
   outStream << "      p6dof_object_type " << mVehicleName.toUpper() << "\n";
   outStream << "      update_interval   0.01 sec\n";
   outStream << "   end_mover   \n";
   outStream << "         \n";
   outStream << "   p6dof_ned_heading           0 deg\n";
   outStream << "   p6dof_position              21.4  -158.0\n";
   outStream << "   p6dof_set_velocity_ned_fps  0 0 0 \n";
   outStream << "   \n";
   outStream << "   \n";
   outStream << "   enable_controls false\n";
   outStream << "      \n";
   outStream << "   on_update  \n";
   outStream << "      foreach (int key : struct value in mData)\n";
   outStream << "      {\n";
   outStream << "         WsfPlatform platform = WsfSimulation.FindPlatform(key);\n";
   outStream << "         if(!platform.IsNull())\n";
   outStream << "         {\n";
   outStream << "            double time = TIME_NOW;\n";
   outStream << "            \n";
   outStream << "            double altitude = platform.Altitude();\n";
   outStream << "            if(altitude > value->maxAltitude_m)\n";
   outStream << "            {\n";
   outStream << "               value->maxAltitude_m = altitude;\n";
   outStream << "               value->timeMaxAltitude_sec = time;\n";
   outStream << "            }\n";
   outStream << "            double speed = platform.Speed();\n";
   outStream << "            if(speed > value->maxSpeed_mps)\n";
   outStream << "            {\n";
   outStream << "               value->maxSpeed_mps = speed;\n";
   outStream << "               value->timeMaxSpeed_sec = time;\n";
   outStream << "            }\n";
   outStream << "            value->maxRange_km = GetRangeSinceStart_km(platform);\n";
   outStream << "         } \n";
   outStream << "      }\n";
   outStream << "   end_on_update\n";
   outStream << "         \n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "//Create the test platforms\n";
   outStream << "execute at_time .01 sec absolute\n";
   outStream << "   \n";
   outStream << "   double iterations = (MAX_LAUNCH_ANGLE_DEG - MIN_LAUNCH_ANGLE_DEG) / LAUNCH_ANGLE_INCREMENT;  \n";
   outStream << "   for(int i = 0; i <= iterations; i = i + 1)\n";
   outStream << "   {\n";
   outStream << "      double launchAngle = LAUNCH_ANGLE_INCREMENT * i + MIN_LAUNCH_ANGLE_DEG;\n";
   outStream << "      WsfPlatform platform1 = WsfSimulation.CreatePlatform(\"SAM_TEST_PLATFORM\");\n";
   outStream << "      string initString = write_str(\"p6dof_ned_pitch \", launchAngle, \" deg p6dof_alt \", INITIAL_ALTITUDE_FT, \" ft\");\n";
   outStream << "      platform1.ProcessInput(initString);\n";
   outStream << "      WsfPlatform platform2 = WsfSimulation.AddPlatform(platform1, \"\");\n";
   outStream << "      struct temp = struct.New(\"DataStruct\");\n";
   outStream << "      temp->launchAngle_deg = launchAngle;  \n";
   outStream << "      mData.Set(platform2.Index(), temp);\n";
   outStream << "   }  \n";
   outStream << "end_execute\n";
   outStream << "\n";
   outStream << "script void WriteOutput()\n";
   outStream << "   double kmToNm = 0.5399568;\n";
   outStream << "   FileIO output = FileIO();\n";
   outStream << "   if(output.Open(outputFileName, \"out\"))\n";
   outStream << "   {\n";
   outStream << "      output.Writeln(\"Meta,title=Ballistic Range (1000 ft launch altitude),y_label=Range (nm)\");\n";
   outStream << "      output.Writeln(\"Launch Angle (deg), Weapon Range\");\n";
   outStream << "      \n";
   outStream << "      foreach (int key : struct value in mData)\n";
   outStream << "      {\n";
   outStream << "         output.Writeln(write_str(value->launchAngle_deg, \", \", value->maxRange_km * kmToNm));\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void SimulationComplete()\n";
   outStream << "   foreach (int key : struct value in mData)\n";
   outStream << "   {\n";
   outStream << "      //If object flew the entire time.\n";
   outStream << "      if(value->totalFlightTime_sec == -1)\n";
   outStream << "      {\n";
   outStream << "         value->totalFlightTime_sec = TIME_NOW;\n";
   outStream << "         writeln(\"Warning: Platform Id : \", key, \" did not terminate, range data may be inaccurate.\");\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "   WriteOutput();\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void CrashedIntoGround(WsfPlatform aPlatform)\n";
   outStream << "   if(mData.Exists(aPlatform.Index()))\n";
   outStream << "   {\n";
   outStream << "      mData[aPlatform.Index()]->totalFlightTime_sec = TIME_NOW;\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "observer\n";
   outStream << "   enable SIMULATION_COMPLETE SimulationComplete\n";
   outStream << "   enable CRASHED_INTO_GROUND CrashedIntoGround\n";
   outStream << "end_observer\n";

   testFile.close();
   }

   void ScriptGeneratorP6DOF::CreateEnduranceTestFile(QString& aTestDir)
   {
   if (mVehiclePtr == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   QString vehicleType = "";
   if (mVehiclePtr->IsAircraft())
   {
      vehicleType = "aircraft";
   }
   else if (mVehiclePtr->IsWeapon())
   {
      vehicleType = "weapons";
   }

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
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "//**********************************************************\n";
   outStream << "\n";
   outStream << "include_once ../../p6dof_types/environment/p6dof_environment.txt\n";
   outStream << "include_once ../../p6dof_types/" << vehicleType << "/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << ".txt\n";
   outStream << "include_once common.txt\n";
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_endurance_test.csv\";\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM WSF_PLATFORM\n";
   outStream << "   spatial_domain air\n";
   outStream << "   mover WSF_P6DOF_MOVER\n";
   outStream << "      p6dof_object_type " << mVehicleName.toUpper() << "\n";
   outStream << "      update_interval   0.01 sec\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   p6dof_ned_heading          0 deg\n";
   outStream << "   p6dof_ned_pitch            0 deg\n";
   outStream << "   p6dof_ned_roll             0 deg\n";
   outStream << "   p6dof_set_velocity_ned_fps 100 0 0\n";
   outStream << "   p6dof_position             0 0\n";
   outStream << "   p6dof_alt                  5000 m\n";
   outStream << "   \n";
   outStream << "   edit mover\n";
   outStream << "      engines_on true\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   on_initialize\n";
   outStream << "      EnableControls(PLATFORM, false);\n";
   outStream << "      SetMasterNoLagTesting(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeAltitude(PLATFORM, false);\n";
   outStream << "      SetMasterFreezePitch(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeRoll(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeYaw(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeFuelBurn(PLATFORM, false);\n";
   outStream << "      SetMasterNoAlphaTesting(PLATFORM, true);\n";
   outStream << "   end_on_initialize\n";
   outStream << "         \n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "script_struct DataSet\n";
   outStream << "   script_variables\n";
   outStream << "      double altitude_ft  = 0;\n";
   outStream << "      double endurance_hr = 0;\n";
   outStream << "   end_script_variables\n";
   outStream << "end_script_struct\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   Array<double>        mSpeeds = Array<double>();\n";
   outStream << "   Array<Array<struct>> mData   = Array<Array<struct>>();\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "script double IntegrateEndurance(WsfPlatform aPlatform, double aSpeed_ktas, double aAltitude_ft)\n";
   outStream << "   if (!aPlatform.Mover().IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n";
   outStream << "   {\n";
   outStream << "      writeln(\"--ERROR-- platform '\", aPlatform.Name(), \"' does not have a P6DOF mover\");\n";
   outStream << "      return 0.0;\n";
   outStream << "   }\n";
   outStream << "   WsfP6DOF_Mover mover = (WsfP6DOF_Mover)aPlatform.Mover();\n";
   outStream << "   WsfP6DOF_MoverTestObject testObj = mover.__getTestObject();\n";
   outStream << "   \n";
   outStream << "   double emptyMass_lbs = mover.GetEmptyWeight() * MATH.LB_PER_KG();\n";
   outStream << "   double fuelMass_lbs = mover.GetTotalFuelCapacity() * MATH.LB_PER_KG();\n";
   outStream << "   \n";
   outStream << "   double initialMass_lbs = emptyMass_lbs + fuelMass_lbs;\n";
   outStream << "   double finalMass_lbs = initialMass_lbs - (fuelMass_lbs - finalFuelQuantity_lbs);\n";
   outStream << "   double stepMass_lbs = (finalMass_lbs - initialMass_lbs) / 100.0;\n";
   outStream << "   double endurance_hr = 0.0;\n";
   outStream << "   \n";
   outStream << "   double mach = mover.CalcMachFromKtas(aAltitude_ft, aSpeed_ktas);\n";
   outStream << "   \n";
   outStream << "   for (double currentMass_lbs = initialMass_lbs; currentMass_lbs > finalMass_lbs; currentMass_lbs = currentMass_lbs + stepMass_lbs)\n";
   outStream << "   {\n";
   outStream << "      double fuelRate_lbsphr = testObj.GetLevelFlightFuelBurnRate(aAltitude_ft, mach, currentMass_lbs);\n";
   outStream << "      if (fuelRate_lbsphr > 0.0)\n";
   outStream << "      {\n";
   outStream << "         double integrand = -stepMass_lbs / fuelRate_lbsphr;\n";
   outStream << "         endurance_hr = endurance_hr + integrand;\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "   \n";
   outStream << "   return endurance_hr;\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void CollectData(WsfPlatform aPlatform)\n";
   outStream << "   int nSpeedSteps = MATH.Floor((finalVelocity_ktas - initialVelocity_ktas) / velocityStep_ktas) + 1;\n";
   outStream << "   int nAltSteps = MATH.Floor((finalAltitude_ft - initialAltitude_ft) / altitudeStep_ft) + 1;\n";
   outStream << "   for (int speedIdx = 0; speedIdx < nSpeedSteps; speedIdx = speedIdx + 1)\n";
   outStream << "   {\n";
   outStream << "      double speed_ktas = initialVelocity_ktas + velocityStep_ktas * speedIdx;\n";
   outStream << "      mSpeeds.PushBack(speed_ktas);\n";
   outStream << "      \n";
   outStream << "      Array<struct> loopData = Array<struct>();\n";
   outStream << "      for (int altIdx = 0; altIdx < nAltSteps; altIdx = altIdx + 1)\n";
   outStream << "      {\n";
   outStream << "         double alt_ft = initialAltitude_ft + altitudeStep_ft * altIdx;   \n";
   outStream << "         struct data = struct.New(\"DataSet\");\n";
   outStream << "         data->altitude_ft = alt_ft;\n";
   outStream << "         data->endurance_hr = IntegrateEndurance(aPlatform, speed_ktas, alt_ft);\n";
   outStream << "         \n";
   outStream << "         loopData.PushBack(data);         \n";
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
   outStream << "      \n";
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
   outStream << "         output.Writeln(line); \n";
   outStream << "      }\n";
   outStream << "      \n";
   outStream << "      output.Close();\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "platform test_platform TEST_PLATFORM\n";
   outStream << "   on_initialize2\n";
   outStream << "      CollectData(PLATFORM);\n";
   outStream << "      WriteOutput();\n";
   outStream << "   end_on_initialize2\n";
   outStream << "end_platform\n";
   outStream << "\n";

   testFile.close();
   }

   void ScriptGeneratorP6DOF::CreateFlightEnvelopeSimpleTestFile(QString& aTestDir)
   {
   if (mVehiclePtr == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   QString vehicleType = "";
   if (mVehiclePtr->IsAircraft())
   {
      vehicleType = "aircraft";
   }
   else if (mVehiclePtr->IsWeapon())
   {
      vehicleType = "weapons";
   }

   QFile testFile(aTestDir + "/flight_envelope_(simple)_test.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return;
   }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "//TEST_HEADER=====================================================\n";
   outStream << "//\n";
   outStream << "//TITLE Flight Envelope (Simple) Test\n";
   outStream << "//\n";
   outStream << "//TEST_GROUP Aircraft Tests\n";
   outStream << "//\n";
   outStream << "//DESCRIPTION This test generates a flight envelope (dog house plot) for a vehicle\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTION\n";
   outStream << "// This test generates a flight envelope (dog house plot) for a vehicle (Flight envelopes are often referred to by using the slang term, \"dog house plot\".)\n";
   outStream << "//\n";
   outStream << "// A flight envelope shows the minimum and maximum speed that the aircraft can fly under sustained conditions as a function of altitude.";
   outStream << " Flight envelopes may be generated under different conditions, such as maximum (military) power, augmented (afterburner) power, or other";
   outStream << " conditions such as flap settings or customized weapons/fuel load-outs (external fuel tanks and weapons will create drag and all weapons";
   outStream << " and fuel will effect mass properties, such as weight and rotational inertia as well as center-of-gravity). P6DOF models consider all of";
   outStream << " these effects in their kinematic calculations.\n";
   outStream << "//\n";
   outStream << "// This test will generate plots for different conditions, including with afterburner and with only military power (non-AB).\n";
   outStream << "//\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "\n";
   outStream << "include_once ../../p6dof_types/environment/p6dof_environment.txt\n";
   outStream << "include_once ../../p6dof_types/" << vehicleType << "/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << ".txt\n";
   outStream << "include_once common.txt\n";
   outStream << "include_once envelope_common.txt\n";
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   \n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_flight_envelope_(simple)_test.csv\";\n";
   outStream << "   \n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   \n";
   outStream << "   Array<struct> mMilEnvelope = Array<struct>();\n";
   outStream << "   Array<struct> mAB_Envelope = Array<struct>();\n";
   outStream << "   Map<int, Array<struct>> mGEnvelopes = Map<int, Array<struct>>();\n";
   outStream << "   \n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM_TYPE WSF_PLATFORM\n";
   outStream << "   spatial_domain        air\n";
   outStream << "   mover WSF_P6DOF_MOVER\n";
   outStream << "      p6dof_object_type " << mVehicleName.toUpper() << "\n";
   outStream << "      update_interval   0.01 sec\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   \n";
   outStream << "   p6dof_ned_heading          0 deg\n";
   outStream << "   p6dof_ned_pitch            0 deg\n";
   outStream << "   p6dof_ned_roll             0 deg\n";
   outStream << "   p6dof_set_velocity_ned_fps 100 0 0\n";
   outStream << "   p6dof_position             0 0\n";
   outStream << "   p6dof_alt                  1000 ft\n";
   outStream << "   \n";
   outStream << "   edit mover\n";
   outStream << "      engines_on true\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   on_initialize\n";
   outStream << "      EnableControls(PLATFORM, false);\n";
   outStream << "      SetMasterNoLagTesting(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeAltitude(PLATFORM, false);\n";
   outStream << "      SetMasterFreezePitch(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeRoll(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeYaw(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeFuelBurn(PLATFORM, false);\n";
   outStream << "      SetMasterNoAlphaTesting(PLATFORM, true);\n";
   outStream << "   end_on_initialize\n";
   outStream << "         \n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "platform test_platform TEST_PLATFORM_TYPE\n";
   outStream << "   \n";
   outStream << "   script void GenerateMil_Envelope()\n";
   outStream << "      mMilEnvelope = ComputeFlightEnvelope(PLATFORM, 1.0, 0.0, 1);\n";
   outStream << "   end_script\n";
   outStream << "   \n";
   outStream << "   script void GenerateAB_Envelope()\n";
   outStream << "      mAB_Envelope = ComputeFlightEnvelope(PLATFORM, 2.0, 0.0, 1);\n";
   outStream << "   end_script\n";
   outStream << "   \n";
   outStream << "   on_initialize2\n";
   outStream << "      // Generate the Envelope curves\n";
   outStream << "      GenerateMil_Envelope();\n";
   outStream << "      GenerateAB_Envelope();\n";
   outStream << "      \n";
   outStream << "      //Print to File\n";
   outStream << "      WriteOutput(PLATFORM);\n";
   outStream << "    \n";
   outStream << "   end_on_initialize2\n";
   outStream << "   \n";
   outStream << "end_platform\n";
   outStream << "\n";
   outStream << "script double CalcMach(WsfPlatform aPlatform, double aAlt_ft, double aSpeed_fps)\n";
   outStream << "   WsfMover mover = aPlatform.Mover();\n";
   outStream << "   if (mover.IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n";
   outStream << "   {\n";
   outStream << "      WsfP6DOF_Mover p6dofMover = (WsfP6DOF_Mover)mover;\n";
   outStream << "      return p6dofMover.CalcMachFromFps(aAlt_ft, aSpeed_fps);\n";
   outStream << "   }\n";
   outStream << "   else\n";
   outStream << "   {\n";
   outStream << "      writeln(\"--ERROR-- Not a P6DOF Mover.\");\n";
   outStream << "   }\n";
   outStream << "\n";
   outStream << "   return 0.0;\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void WriteOutput(WsfPlatform aPlatform)\n";
   outStream << "   FileIO output = FileIO();\n";
   outStream << "   if(output.Open(outputFileName, \"out\"))\n";
   outStream << "   {\n";
   outStream << "      output.Writeln(\"Meta,title=Flight Envelope (AB and MIL),y_label=Alt (ft)\");\n";
   outStream << "      \n";
   outStream << "      string plotLabels = \"Mach, Afterburner,, Mach, Military Power\";\n";
   outStream << "      \n";
   outStream << "      output.Writeln(plotLabels);\n";
   outStream << "      \n";
   outStream << "      for(int i = 0; i < mAB_Envelope.Size(); i = i + 1)\n";
   outStream << "      {\n";
   outStream << "         double mach = CalcMach(aPlatform, mAB_Envelope[i]->altitude_ft, mAB_Envelope[i]->velocity_fps);\n";
   outStream << "         string lineOut = write_str(mach, \",\", mAB_Envelope[i]->altitude_ft);\n";
   outStream << "         \n";
   outStream << "         if(i < mMilEnvelope.Size())\n";
   outStream << "         {\n";
   outStream << "            double mach = CalcMach(aPlatform, mMilEnvelope[i]->altitude_ft, mMilEnvelope[i]->velocity_fps);\n";
   outStream << "            lineOut = lineOut + write_str(\",,\", mach, \",\", mMilEnvelope[i]->altitude_ft);\n";
   outStream << "         }\n";
   outStream << "         else\n";
   outStream << "         {\n";
   outStream << "            lineOut = lineOut + \",,,\";\n";
   outStream << "         }\n";
   outStream << "         \n";
   outStream << "         output.Writeln(lineOut);\n";
   outStream << "         \n";
   outStream << "      }  \n";
   outStream << "      output.Close();\n";
   outStream << "   }\n";
   outStream << "end_script\n";

   testFile.close();
   }

   void ScriptGeneratorP6DOF::CreateFlightEnvelopeAbTestFile(QString& aTestDir)
   {
   if (mVehiclePtr == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   QString vehicleType = "";
   if (mVehiclePtr->IsAircraft())
   {
      vehicleType = "aircraft";
   }
   else if (mVehiclePtr->IsWeapon())
   {
      vehicleType = "weapons";
   }

   QFile testFile(aTestDir + "/flight_envelope_(ab_with_g-load)_test.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   { return; }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "//TEST_HEADER=====================================================\n";
   outStream << "//\n";
   outStream << "//TITLE Flight Envelope (AB With G-Load) Test\n";
   outStream << "//\n";
   outStream << "//TEST_GROUP Aircraft Tests\n";
   outStream << "//\n";
   outStream << "//DESCRIPTION This test generates a flight envelope (dog house plot) for a vehicle\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTION\n";
   outStream << "// This test generates a flight envelope (dog house plot) for a vehicle (Flight envelopes are often referred to by using the slang term, \"dog house plot\".)\n";
   outStream << "//\n";
   outStream << "// A flight envelope shows the minimum and maximum speed that the aircraft can fly under sustained conditions as a function of altitude.";
   outStream << " Flight envelopes may be generated under different conditions, such as maximum (military) power, augmented (afterburner) power, or other";
   outStream << " conditions such as flap settings or customized weapons/fuel load-outs (external fuel tanks and weapons will create drag and all weapons";
   outStream << " and fuel will effect mass properties, such as weight and rotational inertia as well as center-of-gravity). P6DOF models consider all of";
   outStream << " these effects in their kinematic calculations.\n";
   outStream << "//\n";
   outStream << "// This test will generate plots for different conditions, including afterburner performance";
   outStream << " and multiple g-load conditions.\n";
   outStream << "//\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "\n";
   outStream << "include_once ../../p6dof_types/environment/p6dof_environment.txt\n";
   outStream << "include_once ../../p6dof_types/" << vehicleType << "/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << ".txt\n";
   outStream << "include_once common.txt\n";
   outStream << "include_once envelope_common.txt\n";
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   \n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_flight_envelope_(ab_with_g-load)_test.csv\";\n";
   outStream << "   \n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   \n";
   outStream << "   Array<struct> mMilEnvelope = Array<struct>();\n";
   outStream << "   Array<struct> mAB_Envelope = Array<struct>();\n";
   outStream << "   Map<int, Array<struct>> mGEnvelopes = Map<int, Array<struct>>();\n";
   outStream << "   \n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM_TYPE WSF_PLATFORM\n";
   outStream << "   spatial_domain        air\n";
   outStream << "   mover WSF_P6DOF_MOVER\n";
   outStream << "      p6dof_object_type " << mVehicleName.toUpper() << "\n";
   outStream << "      update_interval   0.01 sec\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   \n";
   outStream << "   p6dof_ned_heading          0 deg\n";
   outStream << "   p6dof_ned_pitch            0 deg\n";
   outStream << "   p6dof_ned_roll             0 deg\n";
   outStream << "   p6dof_set_velocity_ned_fps 100 0 0\n";
   outStream << "   p6dof_position             0 0\n";
   outStream << "   p6dof_alt                  1000 ft\n";
   outStream << "   \n";
   outStream << "   edit mover\n";
   outStream << "      engines_on true\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   on_initialize\n";
   outStream << "      EnableControls(PLATFORM, false);\n";
   outStream << "      SetMasterNoLagTesting(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeAltitude(PLATFORM, false);\n";
   outStream << "      SetMasterFreezePitch(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeRoll(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeYaw(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeFuelBurn(PLATFORM, false);\n";
   outStream << "      SetMasterNoAlphaTesting(PLATFORM, true);\n";
   outStream << "   end_on_initialize\n";
   outStream << "   \n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "platform test_platform TEST_PLATFORM_TYPE\n";
   outStream << "   \n";
   outStream << "   script void GenerateAB_Envelope()\n";
   outStream << "      mAB_Envelope = ComputeFlightEnvelope(PLATFORM, 2.0, 0.0, 1);\n";
   outStream << "   end_script\n";
   outStream << "   \n";
   outStream << "   script void GenerateG_Envelopes()\n";
   outStream << "      WsfP6DOF_Mover p6dofMover = (WsfP6DOF_Mover)PLATFORM.Mover();\n";
   outStream << "      double baseWeight = Math.LB_PER_KG() * p6dofMover.GetTotalWeight();\n";
   outStream << "      writeln(write_str(\"Current Weight: \", baseWeight));\n";
   outStream << "      for(int i = 2; i < 10; i = i + 1)\n";
   outStream << "      {\n";
   outStream << "         double weight = baseWeight * i;\n";
   outStream << "         writeln(write_str(\"Calculated Weight: \", weight));\n";
   outStream << "         p6dofMover.__getTestObject().SetVehicleMass(weight);\n";
   outStream << "         writeln(write_str(\"Current Weight: \", p6dofMover.GetTotalWeight()));\n";
   outStream << "         \n";
   outStream << "         mGEnvelopes[i] = ComputeFlightEnvelope(PLATFORM, 2.0, 0.0, 1);\n";
   outStream << "      }\n";
   outStream << "   end_script\n";
   outStream << "   \n";
   outStream << "   on_initialize2\n";
   outStream << "      // Generate the Envelope curves\n";
   outStream << "      GenerateAB_Envelope();\n";
   outStream << "      GenerateG_Envelopes();\n";
   outStream << "      \n";
   outStream << "      //Print to File\n";
   outStream << "      WriteOutput(PLATFORM);\n";
   outStream << "    \n";
   outStream << "   end_on_initialize2\n";
   outStream << "   \n";
   outStream << "end_platform\n";
   outStream << "\n";
   outStream << "script double CalcMach(WsfPlatform aPlatform, double aAlt_ft, double aSpeed_fps)\n";
   outStream << "   WsfMover mover = aPlatform.Mover();\n";
   outStream << "   if (mover.IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n";
   outStream << "   {\n";
   outStream << "      WsfP6DOF_Mover p6dofMover = (WsfP6DOF_Mover)mover;\n";
   outStream << "      return p6dofMover.CalcMachFromFps(aAlt_ft, aSpeed_fps);\n";
   outStream << "   }\n";
   outStream << "   else\n";
   outStream << "   {\n";
   outStream << "      writeln(\"--ERROR-- Not a P6DOF Mover.\");\n";
   outStream << "   }\n";
   outStream << "\n";
   outStream << "   return 0.0;\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void WriteOutput(WsfPlatform aPlatform)\n";
   outStream << "   FileIO output = FileIO();\n";
   outStream << "   if(output.Open(outputFileName, \"out\"))\n";
   outStream << "   {\n";
   outStream << "      output.Writeln(\"Meta,title=Flight Envelope (AB with G-Load),y_label=Alt (ft)\");\n";
   outStream << "      \n";
   outStream << "      string plotLabels = \"Mach, Afterburner\";\n";
   outStream << "      \n";
   outStream << "      foreach(int key : Array<struct> data in mGEnvelopes)\n";
   outStream << "      {\n";
   outStream << "         if(!data.Empty())\n";
   outStream << "         {\n";
   outStream << "            plotLabels = plotLabels + write_str(\",, Mach, Max G = \", key);\n";
   outStream << "         }\n";
   outStream << "      }\n";
   outStream << "      \n";
   outStream << "      output.Writeln(plotLabels);\n";
   outStream << "      \n";
   outStream << "      for(int i = 0; i < mAB_Envelope.Size(); i = i + 1)\n";
   outStream << "      {\n";
   outStream << "         double mach = CalcMach(aPlatform, mAB_Envelope[i]->altitude_ft, mAB_Envelope[i]->velocity_fps);\n";
   outStream << "         string lineOut = write_str(mach, \",\", mAB_Envelope[i]->altitude_ft);\n";
   outStream << "         \n";
   outStream << "         foreach(int key : Array<struct> data in mGEnvelopes)\n";
   outStream << "         {\n";
   outStream << "            if(i < data.Size())\n";
   outStream << "            {\n";
   outStream << "               double mach = CalcMach(aPlatform, data[i]->altitude_ft, data[i]->velocity_fps);\n";
   outStream << "               lineOut = lineOut + write_str(\",,\", mach, \",\", data[i]->altitude_ft);\n";
   outStream << "            }\n";
   outStream << "         }\n";
   outStream << "         \n";
   outStream << "         output.Writeln(lineOut);\n";
   outStream << "         \n";
   outStream << "      }  \n";
   outStream << "      output.Close();\n";
   outStream << "   }\n";
   outStream << "end_script\n";

   testFile.close();
   }

   void ScriptGeneratorP6DOF::CreateFlightEnvelopeMilTestFile(QString& aTestDir)
   {
   if (mVehiclePtr == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   QString vehicleType = "";
   if (mVehiclePtr->IsAircraft())
   {
      vehicleType = "aircraft";
   }
   else if (mVehiclePtr->IsWeapon())
   {
      vehicleType = "weapons";
   }

   QFile testFile(aTestDir + "/flight_envelope_(mil_with_g-load)_test.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return;
   }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "//TEST_HEADER=====================================================\n";
   outStream << "//\n";
   outStream << "//TITLE Flight Envelope (Mil With G-Load) Test\n";
   outStream << "//\n";
   outStream << "//TEST_GROUP Aircraft Tests\n";
   outStream << "//\n";
   outStream << "//DESCRIPTION This test generates a flight envelope (dog house plot) for a vehicle\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTION\n";
   outStream << "// This test generates a flight envelope (dog house plot) for a vehicle (Flight envelopes are often referred to by using the slang term, \"dog house plot\".)\n";
   outStream << "//\n";
   outStream << "// A flight envelope shows the minimum and maximum speed that the aircraft can fly under sustained conditions as a function of altitude.";
   outStream << " Flight envelopes may be generated under different conditions, such as maximum (military) power, augmented (afterburner) power, or other";
   outStream << " conditions such as flap settings or customized weapons/fuel load-outs (external fuel tanks and weapons will create drag and all weapons";
   outStream << " and fuel will effect mass properties, such as weight and rotational inertia as well as center-of-gravity). P6DOF models consider all of";
   outStream << " these effects in their kinematic calculations.\n";
   outStream << "//\n";
   outStream << "// This test will generate plots for different conditions, including military power (non-AB) performance";
   outStream << " and multiple g-load conditions.\n";
   outStream << "//\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "\n";
   outStream << "include_once ../../p6dof_types/environment/p6dof_environment.txt\n";
   outStream << "include_once ../../p6dof_types/" << vehicleType << "/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << ".txt\n";
   outStream << "include_once common.txt\n";
   outStream << "include_once envelope_common.txt\n";
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   \n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_flight_envelope_(mil_with_g-load)_test.csv\";\n";
   outStream << "   \n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   \n";
   outStream << "   Array<struct> mMilEnvelope = Array<struct>();\n";
   outStream << "   Array<struct> mAB_Envelope = Array<struct>();\n";
   outStream << "   Map<int, Array<struct>> mGEnvelopes = Map<int, Array<struct>>();\n";
   outStream << "   \n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM_TYPE WSF_PLATFORM\n";
   outStream << "   spatial_domain        air\n";
   outStream << "   mover WSF_P6DOF_MOVER\n";
   outStream << "      p6dof_object_type " << mVehicleName.toUpper() << "\n";
   outStream << "      update_interval   0.01 sec\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   \n";
   outStream << "   p6dof_ned_heading          0 deg\n";
   outStream << "   p6dof_ned_pitch            0 deg\n";
   outStream << "   p6dof_ned_roll             0 deg\n";
   outStream << "   p6dof_set_velocity_ned_fps 100 0 0\n";
   outStream << "   p6dof_position             0 0\n";
   outStream << "   p6dof_alt                  1000 ft\n";
   outStream << "   \n";
   outStream << "   edit mover\n";
   outStream << "      engines_on true\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   on_initialize\n";
   outStream << "      EnableControls(PLATFORM, false);\n";
   outStream << "      SetMasterNoLagTesting(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeAltitude(PLATFORM, false);\n";
   outStream << "      SetMasterFreezePitch(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeRoll(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeYaw(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeFuelBurn(PLATFORM, false);\n";
   outStream << "      SetMasterNoAlphaTesting(PLATFORM, true);\n";
   outStream << "   end_on_initialize\n";
   outStream << "         \n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "platform test_platform TEST_PLATFORM_TYPE\n";
   outStream << "   \n";
   outStream << "   script void GenerateMilEnvelope()\n";
   outStream << "      mMilEnvelope = ComputeFlightEnvelope(PLATFORM, 1.0, 0.0, 1);\n";
   outStream << "   end_script\n";
   outStream << "   \n";
   outStream << "   script void GenerateG_Envelopes()\n";
   outStream << "      WsfP6DOF_Mover p6dofMover = (WsfP6DOF_Mover)PLATFORM.Mover();\n";
   outStream << "      double baseWeight = Math.LB_PER_KG() * p6dofMover.GetTotalWeight();\n";
   outStream << "      writeln(write_str(\"Current Weight: \", baseWeight));\n";
   outStream << "      for(int i = 2; i < 10; i = i + 1)\n";
   outStream << "      {\n";
   outStream << "         double weight = baseWeight * i;\n";
   outStream << "         writeln(write_str(\"Calculated Weight: \", weight));\n";
   outStream << "         p6dofMover.__getTestObject().SetVehicleMass(weight);\n";
   outStream << "         writeln(write_str(\"Current Weight: \", p6dofMover.GetTotalWeight()));\n";
   outStream << "         \n";
   outStream << "         mGEnvelopes[i] = ComputeFlightEnvelope(PLATFORM, 1.0, 0.0, 1);\n";
   outStream << "      }\n";
   outStream << "   end_script\n";
   outStream << "   \n";
   outStream << "   on_initialize2\n";
   outStream << "      // Generate the Envelope curves\n";
   outStream << "      GenerateMilEnvelope();\n";
   outStream << "      GenerateG_Envelopes();\n";
   outStream << "      \n";
   outStream << "      //Print to File\n";
   outStream << "      WriteOutput(PLATFORM);\n";
   outStream << "    \n";
   outStream << "   end_on_initialize2\n";
   outStream << "   \n";
   outStream << "end_platform\n";
   outStream << "\n";
   outStream << "script double CalcMach(WsfPlatform aPlatform, double aAlt_ft, double aSpeed_fps)\n";
   outStream << "   WsfMover mover = aPlatform.Mover();\n";
   outStream << "   if (mover.IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n";
   outStream << "   {\n";
   outStream << "      WsfP6DOF_Mover p6dofMover = (WsfP6DOF_Mover)mover;\n";
   outStream << "      return p6dofMover.CalcMachFromFps(aAlt_ft, aSpeed_fps);\n";
   outStream << "   }\n";
   outStream << "   else\n";
   outStream << "   {\n";
   outStream << "      writeln(\"--ERROR-- Not a P6DOF Mover.\");\n";
   outStream << "   }\n";
   outStream << "\n";
   outStream << "   return 0.0;\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void WriteOutput(WsfPlatform aPlatform)\n";
   outStream << "   FileIO output = FileIO();\n";
   outStream << "   if(output.Open(outputFileName, \"out\"))\n";
   outStream << "   {\n";
   outStream << "      output.Writeln(\"Meta,title=Flight Envelope (MIL with G-Load),y_label=Alt (ft)\");\n";
   outStream << "      \n";
   outStream << "      string plotLabels = \"Mach, Military Power\";\n";
   outStream << "      \n";
   outStream << "      foreach(int key : Array<struct> data in mGEnvelopes)\n";
   outStream << "      {\n";
   outStream << "         if(!data.Empty())\n";
   outStream << "         {\n";
   outStream << "            plotLabels = plotLabels + write_str(\",, Mach, Max G = \", key);\n";
   outStream << "         }\n";
   outStream << "      }\n";
   outStream << "      \n";
   outStream << "      output.Writeln(plotLabels);\n";
   outStream << "      \n";
   outStream << "      for(int i = 0; i < mMilEnvelope.Size(); i = i + 1)\n";
   outStream << "      {\n";
   outStream << "         string lineOut;\n";
   outStream << "          \n";
   outStream << "#         if(i < mMilEnvelope.Size())\n";
   outStream << "#         {\n";
   outStream << "#            lineOut = lineOut + write_str(\",,\", mMilEnvelope[i]->velocity_fps, \",\", mMilEnvelope[i]->altitude_ft);\n";
   outStream << "#         }\n";
   outStream << "#         else\n";
   outStream << "#         {\n";
   outStream << "#            lineOut = lineOut + \",,,\";\n";
   outStream << "#         }\n";
   outStream << "         double mach = CalcMach(aPlatform, mMilEnvelope[i]->altitude_ft, mMilEnvelope[i]->velocity_fps);\n";
   outStream << "         lineOut = lineOut + write_str(mach, \",\", mMilEnvelope[i]->altitude_ft);\n";
   outStream << "         \n";
   outStream << "         foreach(int key : Array<struct> data in mGEnvelopes)\n";
   outStream << "         {\n";
   outStream << "            if(i < data.Size())\n";
   outStream << "            {\n";
   outStream << "               double mach = CalcMach(aPlatform, data[i]->altitude_ft, data[i]->velocity_fps);\n";
   outStream << "               lineOut = lineOut + write_str(\",,\", mach, \",\", data[i]->altitude_ft);\n";
   outStream << "            }\n";
   outStream << "         }\n";
   outStream << "      \n";
   outStream << "         output.Writeln(lineOut);\n";
   outStream << "\n";
   outStream << "      }  \n";
   outStream << "      output.Close();\n";
   outStream << "   }\n";
   outStream << "end_script\n";

   testFile.close();
   }

   void ScriptGeneratorP6DOF::CreateGCapabilityTestFile(QString& aTestDir)
   {
   if (mVehiclePtr == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   QString vehicleType = "";
   if (mVehiclePtr->IsAircraft())
   {
      vehicleType = "aircraft";
   }
   else if (mVehiclePtr->IsWeapon())
   {
      vehicleType = "weapons";
   }

   QFile testFile(aTestDir + "/g_capability_test.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   { return; }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "//TEST_HEADER=====================================================\n";
   outStream << "//\n";
   outStream << "//TITLE Instantaneous G Capability Test\n";
   outStream << "//\n";
   outStream << "//TEST_GROUP Weapon Tests\n";
   outStream << "//\n";
   outStream << "//DESCRIPTION This test displays the maximum instantaneous G's that the vehicle can pull under a variety of flight conditions\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTION\n";
   outStream << "// This test displays the maximum instantaneous G's that the vehicle can pull under a variety of flight conditions. It plots a series of data based on speed, that present the maximum g as a function of altitude. This shows the maneuver capability possible if the vehicle operated at the specified speed, although not all speeds shown may be attainable in normal conditions.\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "include_once ../../p6dof_types/environment/p6dof_environment.txt\n";
   outStream << "include_once ../../p6dof_types/" << vehicleType << "/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << ".txt\n";
   outStream << GetSetMasterNoLagTestingScript();
   outStream << GetSetMasterFreezeAltitudeScript();
   outStream << GetSetMasterFreezePitchScript();
   outStream << GetSetMasterFreezeRollScript();
   outStream << GetSetMasterFreezeYawScript();
   outStream << GetSetMasterFreezeFuelBurnScript();
   outStream << GetSetMasterNoAlphaTestingScript();
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   \n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_instantaneous_g_capability_test.csv\";\n";
   outStream << "   \n";
   outStream << "   // Altitude and Velocity settings\n";
   outStream << "   double initialAltitude_ft = 2500;\n";
   outStream << "   double finalAltitude_ft   = 100000;\n";
   outStream << "   double altitudeStep_ft    = 2500;\n";
   outStream << "   double initialMach        = 0.75;\n";
   outStream << "   double finalMach          = 4.0;\n";
   outStream << "   double machStep           = 0.25;\n";
   outStream << "   \n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM_TYPE WSF_PLATFORM\n";
   outStream << "   spatial_domain        air\n";
   outStream << "   mover WSF_P6DOF_MOVER\n";
   outStream << "      p6dof_object_type " << mVehicleName.toUpper() << "\n";
   outStream << "      update_interval   0.01 sec\n";
   outStream << "   end_mover\n";
   outStream << "\n";
   outStream << "   p6dof_ned_heading          0 deg\n";
   outStream << "   p6dof_ned_pitch            0 deg\n";
   outStream << "   p6dof_ned_roll             0 deg\n";
   outStream << "   p6dof_set_velocity_ned_fps 100 0 0\n";
   outStream << "   p6dof_position             0 0\n";
   outStream << "   p6dof_alt                  2500 m\n";
   outStream << "\n";
   outStream << "   edit mover\n";
   outStream << "      engines_on true\n";
   outStream << "   end_mover\n";
   outStream << "\n";
   outStream << "   on_initialize\n";
   outStream << "      SetMasterNoLagTesting(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeAltitude(PLATFORM, false);\n";
   outStream << "      SetMasterFreezePitch(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeRoll(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeYaw(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeFuelBurn(PLATFORM, false);\n";
   outStream << "      SetMasterNoAlphaTesting(PLATFORM, true);\n";
   outStream << "   end_on_initialize\n";
   outStream << "\n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "script_struct DataSet\n";
   outStream << "   script_variables\n";
   outStream << "      double altitude_ft = 0;\n";
   outStream << "      double gLoad       = 0;\n";
   outStream << "   end_script_variables\n";
   outStream << "end_script_struct\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   Array<double> mMachs = Array<double>();\n";
   outStream << "   Array<Array<struct>> mData = Array<Array<struct>>();\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "script double MaxInstantaneousGLoad(WsfPlatform aPlatform, double aAltitude_ft, double aMach, double aCurrentMass_lbs)\n";
   outStream << "   if (!aPlatform.Mover().IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n";
   outStream << "   {\n";
   outStream << "      writeln(\"--ERROR-- platform '\", aPlatform.Name(), \"' does not have a P6DOF mover\");\n";
   outStream << "      return 0.0;\n";
   outStream << "   }\n";
   outStream << "   WsfP6DOF_Mover mover = (WsfP6DOF_Mover)aPlatform.Mover();\n";
   outStream << "   WsfP6DOF_MoverTestObject testObj = mover.__getTestObject();\n";
   outStream << "\n";
   outStream << "   double gLoad = testObj.MaxInstantaneousGLoad(aAltitude_ft, aMach, aCurrentMass_lbs);\n";
   outStream << "\n";
   outStream << "   return gLoad;\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void CollectData(WsfPlatform aPlatform)\n";
   outStream << "   if (!aPlatform.Mover().IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n";
   outStream << "   {\n";
   outStream << "      writeln(\"--ERROR-- platform '\", aPlatform.Name(), \"' does not have a P6DOF mover\");\n";
   outStream << "      return;\n";
   outStream << "   }\n";
   outStream << "   WsfP6DOF_Mover mover = (WsfP6DOF_Mover)aPlatform.Mover();\n";
   outStream << "\n";
   outStream << "   double currentMass_lbs = Math.LB_PER_KG() * 0.5 * (mover.GetCurrentWeight() + mover.GetEmptyWeight());\n";
   outStream << "\n";
   outStream << "   int nSpeedSteps = MATH.Floor((finalMach - initialMach) / machStep) + 1;\n";
   outStream << "   int nAltSteps = MATH.Floor((finalAltitude_ft - initialAltitude_ft) / altitudeStep_ft) + 1;\n";
   outStream << "   for (int machIdx = 0; machIdx < nSpeedSteps; machIdx = machIdx + 1)\n";
   outStream << "   {\n";
   outStream << "      double mach = initialMach + machStep * machIdx;\n";
   outStream << "      mMachs.PushBack(mach);\n";
   outStream << "\n";
   outStream << "      Array<struct> loopData = Array<struct>();\n";
   outStream << "      for (int altIdx = 0; altIdx < nAltSteps; altIdx = altIdx + 1)\n";
   outStream << "      {\n";
   outStream << "         double alt_ft = initialAltitude_ft + altitudeStep_ft * altIdx;\n";
   outStream << "         struct data = struct.New(\"DataSet\");\n";
   outStream << "         data->altitude_ft = alt_ft;\n";
   outStream << "         data->gLoad = MaxInstantaneousGLoad(aPlatform, alt_ft, mach, currentMass_lbs);\n";
   outStream << "\n";
   outStream << "         loopData.PushBack(data);\n";
   outStream << "      }\n";
   outStream << "      mData.PushBack(loopData);\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void GenerateFileHeaders(FileIO aFile)\n";
   outStream << "   aFile.Writeln(\"Meta,title=Instantaneous G-Load Test,y_label=Altitude (ft)\");\n";
   outStream << "   string line = \"\";\n";
   outStream << "   for (int i = 0; i < mMachs.Size(); i = i + 1)\n";
   outStream << "   {\n";
   outStream << "      line = line + \"G-Load,\" + \" Mach \" + (string)mMachs[i];\n";
   outStream << "      if (i != mMachs.Size() - 1)\n";
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
   outStream << "            line = line + write_str(mData[j][i]->gLoad, \",\", mData[j][i]->altitude_ft);\n";
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
   outStream << "platform test_platform TEST_PLATFORM_TYPE\n";
   outStream << "   on_initialize2\n";
   outStream << "      CollectData(PLATFORM);\n";
   outStream << "      WriteOutput();\n";
   outStream << "   end_on_initialize2\n";
   outStream << "end_platform\n";
   outStream << "\n";

   testFile.close();
   }

   void ScriptGeneratorP6DOF::CreateLevelFlightRangeTestFile(QString& aTestDir)
   {
   QFile testFile(aTestDir + "/level_flight_range_test.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   { return; }

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
   outStream << GetFuelRemainingScript();
   outStream << GetRangeSinceStart_kmScript();
   outStream << GetFlyStraightLevelFullThrottleScript();
   outStream << GetSetMasterNoLagTestingScript();
   outStream << GetSetMasterFreezeAltitudeScript();
   outStream << GetSetMasterFreezePitchScript();
   outStream << GetSetMasterFreezeRollScript();
   outStream << GetSetMasterFreezeVerticalSpeedScript();
   outStream << GetSetMasterFreezeYawScript();
   outStream << GetSetMasterFreezeFuelBurnScript();
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM " << mPlatformType << "\n";
   outStream << "   \n";
   outStream << "   weapon_effects WSF_GRADUATED_LETHALITY\n";
   outStream << "   \n";
   outStream << "   aux_data\n";
   outStream << "      double commandedAltitude_m = 0 // meters\n";
   outStream << "   end_aux_data\n";
   outStream << "   \n";
   outStream << "   p6dof_ned_heading           0 deg\n";
   outStream << "   p6dof_position              21.4  -158.0\n";
   outStream << "   p6dof_set_velocity_ned_fps  0 0 0 \n";
   outStream << "   \n";
   outStream << "   enable_controls false  \n";
   outStream << "   \n";
   outStream << "   script_variables\n";
   outStream << "      bool   completed       = false;\n";
   outStream << "      bool   activated       = false;\n";
   outStream << "      double maxSpeed_mach   = 0.0;\n";
   outStream << "   end_script_variables\n";
   outStream << "   \n";
   outStream << "   on_initialize2\n";
   outStream << "      SetMasterNoLagTesting(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeAltitude(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeVerticalSpeed(PLATFORM, true);\n";
   outStream << "      SetMasterFreezePitch(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeRoll(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeYaw(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeFuelBurn(PLATFORM, false);\n";
   outStream << "   end_on_initialize2\n";
   outStream << "   \n";
   outStream << "   on_update\n";
   outStream << "      if (!completed)\n";
   outStream << "      {\n";
   outStream << "         WsfMover mover = PLATFORM.Mover();\n";
   outStream << "         if (mover.IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n";
   outStream << "         {\n";
   outStream << "            WsfP6DOF_Mover p6dofMover = (WsfP6DOF_Mover)mover;\n";
   outStream << "            \n";
   outStream << "            double mach = p6dofMover.GetMach();\n";
   outStream << "            \n";
   outStream << "            if (mach > maxSpeed_mach)\n";
   outStream << "            {\n";
   outStream << "               maxSpeed_mach = mach;\n";
   outStream << "            }\n";
   outStream << "            if (!activated && mach > ACTIVATION_SPEED_MACH)\n";
   outStream << "            {\n";
   outStream << "               activated = true;\n";
   outStream << "            }\n";
   outStream << "            \n";
   outStream << "            if (activated && mach < MIN_TERMINATION_SPEED_MACH)\n";
   outStream << "            {\n";
   outStream << "               completed = true;\n";
   outStream << "               double altitude_m = PLATFORM.AuxDataDouble(\"commandedAltitude_m\");\n";
   outStream << "               double altitude_ft = altitude_m * MATH.FT_PER_M();\n";
   outStream << "               double maxRange_nm = MATH.NM_PER_M() * PLATFORM.GroundRangeTo(INITIAL_LATITUDE, INITIAL_LONGITUDE, altitude_m);\n";
   outStream << "               rangeAltMap.Set(altitude_ft, maxRange_nm);\n";
   outStream << "               writeln(write_str(\"TestCompleted: Alt \", altitude_ft, \" ft Range \", maxRange_nm, \" nm at t=\", TIME_NOW, \" sec\"));\n";
   outStream << "               RegisterCompleted();\n";
   outStream << "            }\n";
   outStream << "            \n";
   outStream << "            FlyStraightLevelFullThrottle(PLATFORM);\n";
   outStream << "         }\n";
   outStream << "      }\n";
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
   outStream << "   FlyStraightLevelFullThrottle(aWeaponEngagement.WeaponPlatform());\n";
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

   testFile.close();
   }

   void ScriptGeneratorP6DOF::CreateMaximumSpeedTestFile(QString& aTestDir)
   {
   QFile testFile(aTestDir + "/maximum_speed_test.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   { return; }

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
   outStream << GetFuelRemainingScript();
   outStream << GetRangeSinceStart_kmScript();
   outStream << GetFlyStraightLevelFullThrottleScript();
   outStream << GetSetMasterNoLagTestingScript();
   outStream << GetSetMasterFreezeAltitudeScript();
   outStream << GetSetMasterFreezePitchScript();
   outStream << GetSetMasterFreezeRollScript();
   outStream << GetSetMasterFreezeVerticalSpeedScript();
   outStream << GetSetMasterFreezeYawScript();
   outStream << GetSetMasterFreezeFuelBurnScript();
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM " << mPlatformType << "\n";
   outStream << "   \n";
   outStream << "   weapon_effects WSF_GRADUATED_LETHALITY\n";
   outStream << "   \n";
   outStream << "   aux_data\n";
   outStream << "      double commandedAltitude_m = 0 // meters\n";
   outStream << "   end_aux_data\n";
   outStream << "   \n";
   outStream << "   p6dof_ned_heading           0 deg\n";
   outStream << "   p6dof_position              21.4  -158.0\n";
   outStream << "   p6dof_set_velocity_ned_fps  0 0 0 \n";
   outStream << "   \n";
   outStream << "   enable_controls false  \n";
   outStream << "   \n";
   outStream << "   script_variables\n";
   outStream << "      bool   completed       = false;\n";
   outStream << "      bool   activated       = false;\n";
   outStream << "      double maxSpeed_mach   = 0.0;\n";
   outStream << "   end_script_variables\n";
   outStream << "   \n";
   outStream << "   on_initialize2\n";
   outStream << "      SetMasterNoLagTesting(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeAltitude(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeVerticalSpeed(PLATFORM, true);\n";
   outStream << "      SetMasterFreezePitch(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeRoll(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeYaw(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeFuelBurn(PLATFORM, false);\n";
   outStream << "   end_on_initialize2\n";
   outStream << "   \n";
   outStream << "   on_update\n";
   outStream << "      if (!completed)\n";
   outStream << "      {\n";
   outStream << "         WsfMover mover = PLATFORM.Mover();\n";
   outStream << "         if (mover.IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n";
   outStream << "         {\n";
   outStream << "            WsfP6DOF_Mover p6dofMover = (WsfP6DOF_Mover)mover;\n";
   outStream << "            \n";
   outStream << "            double mach = p6dofMover.GetMach();\n";
   outStream << "            \n";
   outStream << "            if (mach > maxSpeed_mach)\n";
   outStream << "            {\n";
   outStream << "               maxSpeed_mach = mach;\n";
   outStream << "            }\n";
   outStream << "            if (!activated && mach > ACTIVATION_SPEED_MACH)\n";
   outStream << "            {\n";
   outStream << "               activated = true;\n";
   outStream << "            }\n";
   outStream << "            \n";
   outStream << "            if (activated && mach < MIN_TERMINATION_SPEED_MACH)\n";
   outStream << "            {\n";
   outStream << "               completed = true;\n";
   outStream << "               double altitude_m = PLATFORM.AuxDataDouble(\"commandedAltitude_m\");\n";
   outStream << "               double altitude_ft = altitude_m * MATH.FT_PER_M();\n";
   outStream << "               double maxRange_nm = MATH.NM_PER_M() * PLATFORM.GroundRangeTo(INITIAL_LATITUDE, INITIAL_LONGITUDE, altitude_m);\n";
   outStream << "               speedAltMap.Set(altitude_ft, maxSpeed_mach);\n";
   outStream << "               writeln(write_str(\"TestCompleted: Alt \", altitude_ft, \" ft Max Mach \", maxSpeed_mach, \" at t=\", TIME_NOW, \" sec\"));\n";
   outStream << "               RegisterCompleted();\n";
   outStream << "            }\n";
   outStream << "            \n";
   outStream << "            FlyStraightLevelFullThrottle(PLATFORM);\n";
   outStream << "         }\n";
   outStream << "      }\n";
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
   outStream << "   FlyStraightLevelFullThrottle(aWeaponEngagement.WeaponPlatform());\n";
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

   testFile.close();
   }

   void ScriptGeneratorP6DOF::CreateRangeTestFile(QString& aTestDir)
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
   outStream << "   // Altitude and Velocity settings  \n";
   outStream << "   double initialAltitude_ft   = 0;\n";
   outStream << "   double finalAltitude_ft     = 50000;\n";
   outStream << "   double altitudeStep_ft      = 1000;\n";
   outStream << "   double initialVelocity_ktas = 100;\n";
   outStream << "   double finalVelocity_ktas   = 700;\n";
   outStream << "   double velocityStep_ktas    = 50;\n";
   outStream << "   \n";
   outStream << "   // Fuel at which the aircraft is considered out of fuel, and it has reach maximum range.\n";
   outStream << "   double finalFuelQuantity_lbs = 100; \n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "//**********************************************************\n";
   outStream << "\n";
   outStream << "include_once ../../p6dof_types/environment/p6dof_environment.txt\n";
   outStream << "include_once ../../p6dof_types/" << mVehicleType << "/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << ".txt\n";
   outStream << "include_once common.txt\n";
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_range_test.csv\";\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM WSF_PLATFORM\n";
   outStream << "   spatial_domain air\n";
   outStream << "   mover WSF_P6DOF_MOVER\n";
   outStream << "      p6dof_object_type " << mVehicleName.toUpper() << "\n";
   outStream << "      update_interval   0.01 sec\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   p6dof_ned_heading          0 deg\n";
   outStream << "   p6dof_ned_pitch            0 deg\n";
   outStream << "   p6dof_ned_roll             0 deg\n";
   outStream << "   p6dof_set_velocity_ned_fps 100 0 0\n";
   outStream << "   p6dof_position             0 0\n";
   outStream << "   p6dof_alt                  5000 m\n";
   outStream << "   \n";
   outStream << "   edit mover\n";
   outStream << "      engines_on true\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   on_initialize\n";
   outStream << "      EnableControls(PLATFORM, false);\n";
   outStream << "      SetMasterNoLagTesting(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeAltitude(PLATFORM, false);\n";
   outStream << "      SetMasterFreezePitch(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeRoll(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeYaw(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeFuelBurn(PLATFORM, false);\n";
   outStream << "      SetMasterNoAlphaTesting(PLATFORM, true);\n";
   outStream << "   end_on_initialize\n";
   outStream << "         \n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "script_struct DataSet\n";
   outStream << "   script_variables\n";
   outStream << "      double altitude_ft = 0;\n";
   outStream << "      double range_nm    = 0;\n";
   outStream << "   end_script_variables\n";
   outStream << "end_script_struct\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   Array<double>        mSpeeds = Array<double>();\n";
   outStream << "   Array<Array<struct>> mData   = Array<Array<struct>>();\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "script double IntegrateRange(WsfPlatform aPlatform, double aSpeed_ktas, double aAltitude_ft)\n";
   outStream << "   if (!aPlatform.Mover().IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n";
   outStream << "   {\n";
   outStream << "      writeln(\"--ERROR-- platform '\", aPlatform.Name(), \"' does not have a P6DOF mover\");\n";
   outStream << "      return 0.0;\n";
   outStream << "   }\n";
   outStream << "   WsfP6DOF_Mover mover = (WsfP6DOF_Mover)aPlatform.Mover();\n";
   outStream << "   WsfP6DOF_MoverTestObject testObj = mover.__getTestObject();\n";
   outStream << "   \n";
   outStream << "   double emptyMass_lbs = mover.GetEmptyWeight() * MATH.LB_PER_KG();\n";
   outStream << "   double fuelMass_lbs = mover.GetTotalFuelCapacity() * MATH.LB_PER_KG();\n";
   outStream << "   \n";
   outStream << "   double initialMass_lbs = emptyMass_lbs + fuelMass_lbs;\n";
   outStream << "   double finalMass_lbs = initialMass_lbs - (fuelMass_lbs - finalFuelQuantity_lbs);\n";
   outStream << "   double stepMass_lbs = (finalMass_lbs - initialMass_lbs) / 100.0;\n";
   outStream << "   double range_nm = 0.0;\n";
   outStream << "   \n";
   outStream << "   double mach = mover.CalcMachFromKtas(aAltitude_ft, aSpeed_ktas);\n";
   outStream << "   \n";
   outStream << "   for (double currentMass_lbs = initialMass_lbs; currentMass_lbs > finalMass_lbs; currentMass_lbs = currentMass_lbs + stepMass_lbs)\n";
   outStream << "   {\n";
   outStream << "      double fuelRate_lbsphr = testObj.GetLevelFlightFuelBurnRate(aAltitude_ft, mach, currentMass_lbs);\n";
   outStream << "      if (fuelRate_lbsphr > 0.0)\n";
   outStream << "      {\n";
   outStream << "         double integrand = -(aSpeed_ktas / fuelRate_lbsphr) * stepMass_lbs;\n";
   outStream << "         range_nm = range_nm + integrand;\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "   \n";
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
   outStream << "      \n";
   outStream << "      Array<struct> loopData = Array<struct>();\n";
   outStream << "      for (int altIdx = 0; altIdx < nAltSteps; altIdx = altIdx + 1)\n";
   outStream << "      {\n";
   outStream << "         double alt_ft = initialAltitude_ft + altitudeStep_ft * altIdx;   \n";
   outStream << "         struct data = struct.New(\"DataSet\");\n";
   outStream << "         data->altitude_ft = alt_ft;\n";
   outStream << "         data->range_nm = IntegrateRange(aPlatform, speed_ktas, alt_ft);\n";
   outStream << "         \n";
   outStream << "         loopData.PushBack(data);         \n";
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
   outStream << "      \n";
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
   outStream << "         output.Writeln(line); \n";
   outStream << "      }\n";
   outStream << "      \n";
   outStream << "      output.Close();\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "platform test_platform TEST_PLATFORM\n";
   outStream << "   on_initialize2\n";
   outStream << "      CollectData(PLATFORM);\n";
   outStream << "      WriteOutput();\n";
   outStream << "   end_on_initialize2\n";
   outStream << "end_platform\n";
   outStream << "\n";

   testFile.close();
   }

   void ScriptGeneratorP6DOF::CreateTurnPerformanceTestFile(QString& aTestDir, int aAltitudeFt, bool aUseAB)
   {
   std::stringstream testNameBuilder{};
   if (aUseAB)
   {
      testNameBuilder << "Turn Performance (AB Power at " << aAltitudeFt << "ft)";
   }
   else
   {
      testNameBuilder << "Turn Performance (MIL Power at " << aAltitudeFt << "ft)";
   }
   testNameBuilder << " Test";
   QString testName = QString::fromStdString(testNameBuilder.str());
   QFile testFile(aTestDir + "/" + testName.toLower().replace(" ", "_") + ".txt");

   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   { return; }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "//TEST_HEADER=====================================================\n";
   outStream << "//\n";
   outStream << "//TITLE " << testName << "\n";
   outStream << "//\n";
   outStream << "//TEST_GROUP Aircraft Tests\n";
   outStream << "//\n";
   outStream << "//DESCRIPTION This test generates a turn performance plot (also called an Energy-Management plot, or specific energy plot) for the aircraft\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTION\n";
   outStream << "// This test generates a turn performance plot (also called an Energy-Management plot, or specific energy plot) for the aircraft. It shows the ability of the aircraft to perform at various specific energy (Ps) values.\n";
   outStream << "//\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "\n";
   outStream << "//Changes the altitude that the Turn Performance Plot is generated at\n";
   outStream << "$define TEST_ALT_FT " << aAltitudeFt << "\n";
   outStream << "\n";
   outStream << "//Variables to control the output values\n";
   outStream << "script_variables\n";
   outStream << "\n";
   outStream << "   double MIN_VELOCITY_FPS  = 200;\n";
   outStream << "   double MAX_VELOCITY_FPS  = 2000;\n";
   outStream << "   double VELOCITY_STEP_FPS = 10;\n";
   outStream << "    \n";
   outStream << "   // Valid power settings are \"idle\", \"mil\", or \"ab\"\n";

   if (aUseAB)
   {
      outStream << "   string POWER_SETTING = \"ab\";\n";
   }
   else
   {
      outStream << "   string POWER_SETTING = \"mil\";\n";
   }

   outStream << "    \n";
   outStream << "   // Valid speed units are \"fps\", \"ktas\", \"kcas\", or \"mach\"\n";
   outStream << "   // NOTE: PlotIt will not show background curves for g \n";
   outStream << "   //       and turn radius when using mach.\n";
   outStream << "   string SPEED_UNITS = \"kcas\";\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "//**********************************************************\n";
   outStream << "\n";
   outStream << "include_once ../../p6dof_types/environment/p6dof_environment.txt\n";
   outStream << "include_once ../../p6dof_types/" << mVehicleType << "/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << ".txt\n";
   outStream << "include_once common.txt\n";
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";
   outStream << "script_struct Data\n";
   outStream << "   script_variables\n";
   outStream << "      double vel = 0;\n";
   outStream << "      double tr  = 0;\n";
   outStream << "   end_script_variables\n";
   outStream << "end_script_struct\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_" << testName.toLower().replace(" ", "_") << ".csv\";\n";
   outStream << "\n";
   outStream << "   Map<int, Array<struct>> mDataMap = Map<int, Array<struct>>();\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM_TYPE WSF_PLATFORM\n";
   outStream << "   spatial_domain        air\n";
   outStream << "   mover WSF_P6DOF_MOVER\n";
   outStream << "      p6dof_object_type " << mVehicleName.toUpper() << "\n";
   outStream << "      update_interval   0.01 sec\n";
   outStream << "   end_mover   \n";
   outStream << "   \n";
   outStream << "   p6dof_ned_heading          0 deg\n";
   outStream << "   p6dof_ned_pitch            0 deg\n";
   outStream << "   p6dof_ned_roll             0 deg\n";
   outStream << "   p6dof_set_velocity_ned_fps 100 0 0\n";
   outStream << "   p6dof_position             0 0\n";
   outStream << "   p6dof_alt                  $<TEST_ALT_FT:15000>$ ft\n";
   outStream << "   \n";
   outStream << "   edit mover\n";
   outStream << "      engines_on true\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   on_initialize\n";
   outStream << "      EnableControls(PLATFORM, false);\n";
   outStream << "      SetMasterNoLagTesting(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeAltitude(PLATFORM, false);\n";
   outStream << "      SetMasterFreezePitch(PLATFORM, false);\n";
   outStream << "      SetMasterFreezeRoll(PLATFORM, false);\n";
   outStream << "      SetMasterFreezeYaw(PLATFORM, false);\n";
   outStream << "      SetMasterFreezeFuelBurn(PLATFORM, true);\n";
   outStream << "      SetMasterNoAlphaTesting(PLATFORM, false);   \n";
   outStream << "   end_on_initialize\n";
   outStream << "         \n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "platform test_platform TEST_PLATFORM_TYPE\n";
   outStream << "\n";
   outStream << "   script void GenerateTurnPerformanceData()\n";
   outStream << "      if(PLATFORM.Mover().IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n";
   outStream << "      {\n";
   outStream << "         WsfP6DOF_Mover p6dofMover = (WsfP6DOF_Mover)PLATFORM.Mover();\n";
   outStream << "         \n";
   outStream << "         // Valid power settings are \"idle\", \"mil\", or \"ab\"\n";
   outStream << "         double throttleSetting = 0.0;  \n";
   outStream << "         if(POWER_SETTING == \"mil\")       { throttleSetting = 1.0; }\n";
   outStream << "         else if(POWER_SETTING == \"ab\")   { throttleSetting = 2.0; }\n";
   outStream << "         else if(POWER_SETTING == \"idle\") { throttleSetting = 0.0; }\n";
   outStream << "         else \n";
   outStream << "         { \n";
   outStream << "            writeln(\"Error: Unknown POWER_SETTING of \", POWER_SETTING);\n";
   outStream << "         }\n";
   outStream << "\n";
   outStream << "         double currentMass_lbs = MATH.LB_PER_KG() * (p6dofMover.GetEmptyWeight() + 0.5 * p6dofMover.GetTotalFuelCapacity());\n";
   outStream << "         double maxG = p6dofMover.__getTestObject().GetMaxGLoad();\n";
   outStream << "\n";
   outStream << "         // Set-up specific excess power array\n";
   outStream << "         Array<double> SpecificExcessPower = Array<double>();\n";
   outStream << "         SpecificExcessPower.PushBack(-800);\n";
   outStream << "         SpecificExcessPower.PushBack(-400);\n";
   outStream << "         SpecificExcessPower.PushBack(0);\n";
   outStream << "         SpecificExcessPower.PushBack(200);\n";
   outStream << "         SpecificExcessPower.PushBack(400);\n";
   outStream << "         SpecificExcessPower.PushBack(600);\n";
   outStream << "         \n";
   outStream << "         ArrayIterator iter = SpecificExcessPower.GetIterator();\n";
   outStream << "         while (iter.HasNext())\n";
   outStream << "         {\n";
   outStream << "            iter.Next();\n";
   outStream << "            double specificExcessPower_fps = (double)iter.Data();       \n";
   outStream << "            mDataMap[specificExcessPower_fps] = Array<struct>();        \n";
   outStream << "            for(double velocity_fps = MIN_VELOCITY_FPS;\n";
   outStream << "                velocity_fps <= MAX_VELOCITY_FPS;\n";
   outStream << "                velocity_fps += VELOCITY_STEP_FPS)\n";
   outStream << "            {   \n";
   outStream << "               double turnRate = p6dofMover.__getTestObject().TurnRateWithConditions(specificExcessPower_fps, currentMass_lbs, maxG, $<TEST_ALT_FT:15000>$, velocity_fps, throttleSetting);\n";
   outStream << "               struct temp = struct.New(\"Data\"); \n";
   outStream << "               temp->vel = velocity_fps; \n";
   outStream << "               temp->tr = turnRate;\n";
   outStream << "               mDataMap[specificExcessPower_fps].PushBack(temp);\n";
   outStream << "            }\n";
   outStream << "         }\n";
   outStream << "      }\n";
   outStream << "      else\n";
   outStream << "      {\n";
   outStream << "         writeln(\"Error: TurnPerformanceTest only works on platforms with P6DOF Movers on them.\");\n";
   outStream << "      }\n";
   outStream << "   end_script\n";
   outStream << "   \n";
   outStream << "   script void WriteOutData(double alt_ft)\n";
   outStream << "      FileIO output = FileIO();\n";
   outStream << "      output.Open(outputFileName, \"out\");\n";
   outStream << "   \n";
   outStream << "      //Output a nicer looking string than MIL or A/B for plot titles\n";
   outStream << "      string powerStr = \"\";\n";
   outStream << "      // Power settings are \"idle\", \"mil\", or \"ab\"\n";
   outStream << "      if(POWER_SETTING == \"mil\")      { powerStr = \"Military\"; }\n";
   outStream << "      else if(POWER_SETTING == \"ab\")  { powerStr = \"Afterburner\"; }\n";
   outStream << "      else                            { powerStr = \"Idle\"; }\n";
   outStream << "      \n";
   outStream << "      //No need to check this, as it was already checked above.\n";
   outStream << "      WsfP6DOF_Mover p6dofMover = (WsfP6DOF_Mover)PLATFORM.Mover(); \n";
   outStream << "      \n";
   outStream << "      //ktas and kcas is not recognized by AFSIM so convert to knots which is recognized.\n";
   outStream << "      double speedMultiplier = 1.0;\n";
   outStream << "      if(SPEED_UNITS == \"fps\")\n";
   outStream << "      {\n";
   outStream << "         speedMultiplier = 1.0;\n";
   outStream << "      }\n";
   outStream << "      else if(SPEED_UNITS == \"ktas\")\n";
   outStream << "      {\n";
   outStream << "         speedMultiplier = p6dofMover.CalcKtasFromFps(1.0);\n";
   outStream << "      }\n";
   outStream << "      else if(SPEED_UNITS == \"kcas\")\n";
   outStream << "      {\n";
   outStream << "         speedMultiplier = p6dofMover.CalcKcasFromFps(alt_ft, 1.0);\n";
   outStream << "      }\n";
   outStream << "      else if(SPEED_UNITS == \"mach\")\n";
   outStream << "      {\n";
   outStream << "         speedMultiplier = p6dofMover.CalcMachFromFps(alt_ft, 1.0);\n";
   outStream << "      }\n";
   outStream << "      else\n";
   outStream << "      {\n";
   outStream << "         writeln(\"Error: Unknown SPEED_UNITS of \", SPEED_UNITS);\n";
   outStream << "      }\n";
   outStream << "      \n";
   outStream << "      //This Meta data row outputs values used by PlotIt to format things correctly\n";
   outStream << "      output.Writeln(write_str(\"Meta,title=Turn Performance @ \", alt_ft, \"ft Power=\", powerStr,\",y_label=Turn Rate (deg/s),tp_multiplier=\", speedMultiplier));\n";
   outStream << "   \n";
   outStream << "      //Create header for the data\n";
   outStream << "      string header = write_str(\"Speed (\", SPEED_UNITS, \"),\");    \n";
   outStream << "      int maxSize = 0;\n";
   outStream << "      foreach (int key : Array<struct> data in mDataMap)\n";
   outStream << "      {\n";
   outStream << "         header += write_str(\"Ps=\", key,\",\");\n";
   outStream << "         if(data.Size() > maxSize)\n";
   outStream << "         {\n";
   outStream << "            maxSize = data.Size();\n";
   outStream << "         }\n";
   outStream << "      }\n";
   outStream << "      output.Writeln(header);\n";
   outStream << "    \n";
   outStream << "      for(int i = 0; i < maxSize; i += 1)\n";
   outStream << "      {\n";
   outStream << "         //This assumes the following lines use a valid index\n";
   outStream << "         int firstIndex = (int)mDataMap.ElementKeyAtIndex(0);\n";
   outStream << "         double vel_fps = mDataMap[firstIndex].Get(i)->vel;\n";
   outStream << "\n";
   outStream << "         string line = \"\";\n";
   outStream << "         if(SPEED_UNITS == \"ktas\")\n";
   outStream << "         {\n";
   outStream << "            line = write_str(p6dofMover.CalcKtasFromFps(vel_fps), \",\");\n";
   outStream << "         }\n";
   outStream << "         else if(SPEED_UNITS == \"kcas\")\n";
   outStream << "         {\n";
   outStream << "            line = write_str(p6dofMover.CalcKcasFromFps(alt_ft,vel_fps), \",\");\n";
   outStream << "         }\n";
   outStream << "         else if(SPEED_UNITS == \"fps\")\n";
   outStream << "         {\n";
   outStream << "            line = write_str(vel_fps, \",\");\n";
   outStream << "         }\n";
   outStream << "         else if(SPEED_UNITS == \"mach\")\n";
   outStream << "         {\n";
   outStream << "            line = write_str(p6dofMover.CalcMachFromFps(alt_ft,vel_fps), \",\");\n";
   outStream << "            writeln(\"Warning: Mach support is limited within PlotIt\");\n";
   outStream << "         }\n";
   outStream << "         else\n";
   outStream << "         {\n";
   outStream << "            writeln(\"Error: Unknown Unit String:\", SPEED_UNITS, \" Invalid Output File.\");\n";
   outStream << "         }\n";
   outStream << "      \n";
   outStream << "         foreach (int key : Array<struct> data in mDataMap)\n";
   outStream << "         {\n";
   outStream << "            line = line + write_str(data.Get(i)->tr, \",\");\n";
   outStream << "         }\n";
   outStream << "         output.Writeln(line);\n";
   outStream << "      }\n";
   outStream << "   \n";
   outStream << "      output.Close(); \n";
   outStream << "   end_script\n";
   outStream << "\n";
   outStream << "   on_initialize2\n";
   outStream << "      //Execute Turn Performance Test\n";
   outStream << "      writeln(\"Generating Turn Performance\");\n";
   outStream << "      GenerateTurnPerformanceData();\n";
   outStream << "      writeln(\"Writing out results\");\n";
   outStream << "      WriteOutData($<TEST_ALT_FT:15000>$);\n";
   outStream << "   end_on_initialize2\n";
   outStream << "   \n";
   outStream << "end_platform\n";

   testFile.close();
   }

   void ScriptGeneratorP6DOF::CreatePitchingMomentTest(QString& aTestDir, double aAltitude_ft, double aMach)
   {
   std::stringstream testNameBuilder{};
   testNameBuilder.precision(3);
   testNameBuilder << "Pitching Moment (" << "Mach " << aMach << ")";
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
   outStream << "//DESCRIPTION This test determines the pitching moment as a function of angle of attack\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTIONS\n";
   outStream << "// This test determines the pitching moment as a function of angle of attack. It generates three plot series -- zero control pitching moment, full aft control pitching moment, and full forward control pitching moment.\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "include_once ../../p6dof_types/environment/p6dof_environment.txt\n";
   outStream << "include_once ../../p6dof_types/" << mVehicleType << "/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << ".txt\n";
   outStream << "include_once common.txt\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   double gTestAltitude_ft = " << aAltitude_ft << ";\n";
   outStream << "   double gTestMach        = " << aMach << ";\n";
   outStream << "   \n";
   outStream << "   double gInitialAlpha = -20.0;\n";
   outStream << "   double gFinalAlpha   = 75.0;\n";
   outStream << "   double gStepAlpha    = 1.0;\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_" << testName.toLower().replace(" ", "_") << ".csv\";\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM WSF_PLATFORM\n";
   outStream << "   spatial_domain air\n";
   outStream << "   mover WSF_P6DOF_MOVER\n";
   outStream << "      p6dof_object_type " << mVehicleName.toUpper() << "\n";
   outStream << "      update_interval   0.01 sec\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   p6dof_ned_heading          0 deg\n";
   outStream << "   p6dof_ned_pitch            0 deg\n";
   outStream << "   p6dof_ned_roll             0 deg\n";
   outStream << "   p6dof_set_velocity_ned_fps 100 0 0\n";
   outStream << "   p6dof_position             0 0\n";
   outStream << "   p6dof_alt                  5000 m\n";
   outStream << "   \n";
   outStream << "   edit mover\n";
   outStream << "      engines_on true\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   on_initialize\n";
   outStream << "      EnableControls(PLATFORM, false);\n";
   outStream << "      SetMasterNoLagTesting(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeAltitude(PLATFORM, false);\n";
   outStream << "      SetMasterFreezePitch(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeRoll(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeYaw(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeFuelBurn(PLATFORM, false);\n";
   outStream << "      SetMasterNoAlphaTesting(PLATFORM, true);\n";
   outStream << "   end_on_initialize\n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   Array<double> mAlpha        = Array<double>();\n";
   outStream << "   Array<double> mZeroStick    = Array<double>();\n";
   outStream << "   Array<double> mBackStick    = Array<double>();\n";
   outStream << "   Array<double> mForwardStick = Array<double>();\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "script void CollectData(WsfPlatform aPlatform)\n";
   outStream << "   if (!aPlatform.Mover().IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n";
   outStream << "   {\n";
   outStream << "      return;\n";
   outStream << "   }\n";
   outStream << "\n";
   outStream << "   WsfP6DOF_Mover mover = (WsfP6DOF_Mover)aPlatform.Mover();\n";
   outStream << "   WsfP6DOF_MoverTestObject testObj = mover.__getTestObject();\n";
   outStream << "   \n";
   outStream << "   for (double alpha = gInitialAlpha; alpha <= gFinalAlpha; alpha = alpha + gStepAlpha)\n";
   outStream << "   {\n";
   outStream << "      mAlpha.PushBack(alpha);\n";
   outStream << "      mZeroStick.PushBack(testObj.GetPitchingMoment(gTestAltitude_ft, gTestMach, alpha, 0.0));\n";
   outStream << "      mBackStick.PushBack(testObj.GetPitchingMoment(gTestAltitude_ft, gTestMach, alpha, 1.0));\n";
   outStream << "      mForwardStick.PushBack(testObj.GetPitchingMoment(gTestAltitude_ft, gTestMach, alpha, -1.0));\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void WriteOutput()\n";
   outStream << "   FileIO output = FileIO();\n";
   outStream << "   if (output.Open(outputFileName, \"out\"))\n";
   outStream << "   {\n";
   outStream << "      output.Writeln(\"Meta,title=" << testHeading << ",y_label=Cm (ft-lbs)\");\n";
   outStream << "      output.Writeln(\"Alpha,Zero Stick,Stick Back,Stick Forward\");\n";
   outStream << "      for (int i = 0; i < mAlpha.Size(); i = i + 1)\n";
   outStream << "      {\n";
   outStream << "         output.Writeln(write_str(mAlpha[i], \",\", mZeroStick[i], \",\", mBackStick[i], \",\", mForwardStick[i]));\n";
   outStream << "      }\n";
   outStream << "      output.Close();\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "platform test_platform TEST_PLATFORM\n";
   outStream << "   on_initialize2\n";
   outStream << "      CollectData(PLATFORM);\n";
   outStream << "      WriteOutput();\n";
   outStream << "   end_on_initialize2\n";
   outStream << "end_platform\n";
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";

   testFile.close();
   }

   void ScriptGeneratorP6DOF::CreateThrustDragTest(QString& aTestDir, double aAltitude_ft)
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
   outStream << "// This test compares the drag and thrust curves at a specified altitude. It plots, as a function of Mach, the drag, afterburner thrust, and military power thrust.\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   double gTestAltitude_ft = " << aAltitude_ft << ";\n";
   outStream << "   double gInitialMach = 0.0;\n";
   outStream << "   double gFinalMach   = 3.0;\n";
   outStream << "   double gStepMach    = 0.1;\n"; 
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_" << testName.toLower().replace(" ", "_") << ".csv\";\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "include_once ../../p6dof_types/environment/p6dof_environment.txt\n";
   outStream << "include_once ../../p6dof_types/" << mVehicleType << "/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << ".txt\n";
   outStream << "include_once common.txt\n";
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM WSF_PLATFORM\n";
   outStream << "   spatial_domain air\n";
   outStream << "   mover WSF_P6DOF_MOVER\n";
   outStream << "      p6dof_object_type " << mVehicleName.toUpper() << "\n";
   outStream << "      update_interval   0.01 sec\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   p6dof_ned_heading          0 deg\n";
   outStream << "   p6dof_ned_pitch            0 deg\n";
   outStream << "   p6dof_ned_roll             0 deg\n";
   outStream << "   p6dof_set_velocity_ned_fps 100 0 0\n";
   outStream << "   p6dof_position             0 0\n";
   outStream << "   p6dof_alt                  5000 m\n";
   outStream << "   \n";
   outStream << "   edit mover\n";
   outStream << "      engines_on true\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   on_initialize\n";
   outStream << "      EnableControls(PLATFORM, false);\n";
   outStream << "      SetMasterNoLagTesting(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeAltitude(PLATFORM, false);\n";
   outStream << "      SetMasterFreezePitch(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeRoll(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeYaw(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeFuelBurn(PLATFORM, false);\n";
   outStream << "      SetMasterNoAlphaTesting(PLATFORM, true);\n";
   outStream << "   end_on_initialize\n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "script_struct DataSet\n";
   outStream << "   script_variables\n";
   outStream << "      double mach          = 0;\n";
   outStream << "      double thrustMil_lbs = 0;\n";
   outStream << "      double thrustAB_lbs  = 0;\n";
   outStream << "      double drag_lbs      = 0;\n";
   outStream << "   end_script_variables\n";
   outStream << "end_script_struct\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   Array<struct> mData          = Array<struct>();\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "script double GetTotalThrust(WsfPlatform aPlatform)\n";
   outStream << "   WsfMover mover = aPlatform.Mover();\n";
   outStream << "   if (mover.IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n";
   outStream << "   {\n";
   outStream << "      WsfP6DOF_Mover p6dofMover = (WsfP6DOF_Mover)mover;\n";
   outStream << "      return p6dofMover.GetTotalThrust();\n";
   outStream << "   }\n";
   outStream << "   else\n";
   outStream << "   {\n";
   outStream << "      writeln(\"--ERROR-- Not a P6DOF Mover.\");\n";
   outStream << "      return 0.0;\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void CollectData(WsfPlatform aPlatform)\n";
   outStream << "   WsfMover mover = aPlatform.Mover();\n";
   outStream << "   if (mover.IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n";
   outStream << "   {\n";
   outStream << "      WsfP6DOF_Mover           p6dofMover = (WsfP6DOF_Mover)mover;\n";
   outStream << "      WsfP6DOF_MoverTestObject testObj    = p6dofMover.__getTestObject();\n";
   outStream << "      \n";
   outStream << "      double testWeight = p6dofMover.GetEmptyWeight() + 0.5 * p6dofMover.GetTotalFuelCapacity();\n";
   outStream << "      \n";
   outStream << "      int machSteps = MATH.Floor((gFinalMach - gInitialMach) / gStepMach) + 1;\n";
   outStream << "      for (int machIdx = 0; machIdx <= machSteps; machIdx = machIdx + 1)\n";
   outStream << "      {\n";
   outStream << "         double mach = gInitialMach + machIdx * gStepMach;\n";
   outStream << "         struct data = struct.New(\"DataSet\");\n";
   outStream << "         data->mach = mach;\n";
   outStream << "         \n";
   outStream << "         data->thrustMil_lbs = testObj.GetLevelFlightHorizontalThrust(gTestAltitude_ft, mach, 1, testWeight);   // 1 is military power\n";
   outStream << "         data->thrustAB_lbs  = testObj.GetLevelFlightHorizontalThrust(gTestAltitude_ft, mach, 2, testWeight);   // 2 is afterburner\n";
   outStream << "         data->drag_lbs      = testObj.GetLevelFlightDrag(gTestAltitude_ft, mach, testWeight);\n";
   outStream << "         \n";
   outStream << "         mData.PushBack(data);\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void WriteOutput()\n";
   outStream << "   FileIO output = FileIO();\n";
   outStream << "   if (output.Open(outputFileName, \"out\"))\n";
   outStream << "   {\n";
   outStream << "      output.Writeln(\"Meta,title=" << testHeading << ",y_label=Forces (lbs)\");\n";
   outStream << "      output.Writeln(\"Mach,Drag,,Mach,Thrust - Military,,Mach,Thrust - Afterburner\");\n";
   outStream << "      \n";
   outStream << "      for (int i = 0; i < mData.Size(); i = i + 1)\n";
   outStream << "      {\n";
   outStream << "         double mach   = mData[i]->mach;\n";
   outStream << "         double thrMIL = mData[i]->thrustMil_lbs;\n";
   outStream << "         double thrAB  = mData[i]->thrustAB_lbs; \n";
   outStream << "         double drag   = mData[i]->drag_lbs;\n";
   outStream << "         output.Writeln(write_str(mach, \",\", drag, \",,\", mach, \",\", thrMIL, \",,\", mach, \",\", thrAB));\n";
   outStream << "      }\n";
   outStream << "      \n";
   outStream << "      output.Close();\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "platform test_platform TEST_PLATFORM\n";
   outStream << "   on_initialize2\n";
   outStream << "      CollectData(PLATFORM);\n";
   outStream << "      WriteOutput();\n";
   outStream << "   end_on_initialize2\n";
   outStream << "end_platform\n";
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";

   testFile.close();
   }

   QString ScriptGeneratorP6DOF::GetFlyStraightLevelFullThrottleScript()
   {
   QString scriptStr = "\n";
   scriptStr.append("// This uses maximum throttle (value=2.0, afterburner). If the vehicle\n");
   scriptStr.append("// does not have an afterburner, it will use full throttle \n");
   scriptStr.append("// (value=1.0, military power) instead.\n");
   scriptStr.append("script void FlyStraightLevelFullThrottle(WsfPlatform aPlatform)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover();\n");
   scriptStr.append("   if (mover.IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfP6DOF_Mover p6dofMover = (WsfP6DOF_Mover)mover;\n");
   scriptStr.append("      p6dofMover.__getTestObject().FlyStraightLevelFullThrottle();\n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\");\n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
   }

   QString ScriptGeneratorP6DOF::GetRangeSinceStart_kmScript()
   {
   QString scriptStr = "\n";
   scriptStr.append("script double GetRangeSinceStart_km(WsfPlatform aPlatform)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover(); \n");
   scriptStr.append("   if (mover.IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfP6DOF_Mover p6dofMover = (WsfP6DOF_Mover)mover; \n");
   scriptStr.append("      return p6dofMover.__getTestObject().GetRangeSinceStart_km(); \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\"); \n");
   scriptStr.append("      return false; \n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
   }

   QString ScriptGeneratorP6DOF::GetSetMasterNoLagTestingScript()
   {
   QString scriptStr = "\n";
   scriptStr.append("script void SetMasterNoLagTesting(WsfPlatform aPlatform, bool aEnable)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover(); \n");
   scriptStr.append("   if (mover.IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfP6DOF_Mover p6dofMover = (WsfP6DOF_Mover)mover; \n");
   scriptStr.append("      p6dofMover.__getTestObject().SetMasterNoLagTesting(aEnable); \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\"); \n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
   }

   QString ScriptGeneratorP6DOF::GetSetMasterFreezeAltitudeScript()
   {
   QString scriptStr = "\n";
   scriptStr.append("script void SetMasterFreezeAltitude(WsfPlatform aPlatform, bool aEnable)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover(); \n");
   scriptStr.append("   if (mover.IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfP6DOF_Mover p6dofMover = (WsfP6DOF_Mover)mover; \n");
   scriptStr.append("      p6dofMover.__getTestObject().SetMasterFreezeAltitude(aEnable); \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\"); \n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
   }

   QString ScriptGeneratorP6DOF::GetSetMasterFreezePitchScript()
   {
   QString scriptStr = "\n";
   scriptStr.append("script void SetMasterFreezePitch(WsfPlatform aPlatform, bool aEnable)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover(); \n");
   scriptStr.append("   if (mover.IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfP6DOF_Mover p6dofMover = (WsfP6DOF_Mover)mover; \n");
   scriptStr.append("      p6dofMover.__getTestObject().SetMasterFreezePitch(aEnable); \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\"); \n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
   }

   QString ScriptGeneratorP6DOF::GetSetMasterFreezeRollScript()
   {
   QString scriptStr = "\n";
   scriptStr.append("script void SetMasterFreezeRoll(WsfPlatform aPlatform, bool aEnable)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover(); \n");
   scriptStr.append("   if (mover.IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfP6DOF_Mover p6dofMover = (WsfP6DOF_Mover)mover; \n");
   scriptStr.append("      p6dofMover.__getTestObject().SetMasterFreezeRoll(aEnable); \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\"); \n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
   }

   QString ScriptGeneratorP6DOF::GetSetMasterFreezeVerticalSpeedScript()
   {
   QString scriptStr = "\n";
   scriptStr.append("script void SetMasterFreezeVerticalSpeed(WsfPlatform aPlatform, bool aEnable)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover(); \n");
   scriptStr.append("   if (mover.IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfP6DOF_Mover p6dofMover = (WsfP6DOF_Mover)mover; \n");
   scriptStr.append("      p6dofMover.__getTestObject().SetMasterFreezeVerticalSpeed(aEnable); \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\"); \n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
   }

   QString ScriptGeneratorP6DOF::GetSetMasterFreezeYawScript()
   {
   QString scriptStr = "\n";
   scriptStr.append("script void SetMasterFreezeYaw(WsfPlatform aPlatform, bool aEnable)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover(); \n");
   scriptStr.append("   if (mover.IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfP6DOF_Mover p6dofMover = (WsfP6DOF_Mover)mover; \n");
   scriptStr.append("      p6dofMover.__getTestObject().SetMasterFreezeYaw(aEnable); \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\"); \n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
   }

   QString ScriptGeneratorP6DOF::GetSetMasterFreezeFuelBurnScript()
   {
   QString scriptStr = "\n";
   scriptStr.append("script void SetMasterFreezeFuelBurn(WsfPlatform aPlatform, bool aEnable)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover(); \n");
   scriptStr.append("   if (mover.IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfP6DOF_Mover p6dofMover = (WsfP6DOF_Mover)mover; \n");
   scriptStr.append("      p6dofMover.__getTestObject().SetMasterFreezeFuelBurn(aEnable); \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\"); \n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
   }

   QString ScriptGeneratorP6DOF::GetSetMasterNoAlphaTestingScript()
   {
   QString scriptStr = "\n";
   scriptStr.append("script void SetMasterNoAlphaTesting(WsfPlatform aPlatform, bool aEnable)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover(); \n");
   scriptStr.append("   if(mover.IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfP6DOF_Mover p6dofMover = (WsfP6DOF_Mover)mover; \n");
   scriptStr.append("      p6dofMover.__getTestObject().SetMasterNoAlphaTesting(aEnable); \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\"); \n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
   }

   QString ScriptGeneratorP6DOF::GetEnableControlsScript()
   {
   QString scriptStr = "\n";
   scriptStr.append("script void EnableControls(WsfPlatform aPlatform, bool aEnable)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover(); \n");
   scriptStr.append("   if (mover.IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfP6DOF_Mover p6dofMover = (WsfP6DOF_Mover)mover; \n");
   scriptStr.append("      p6dofMover.EnableControls(aEnable); \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\"); \n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
   }

   QString ScriptGeneratorP6DOF::GetFuelRemainingScript()
   {
   QString scriptStr = "\n";
   scriptStr.append("script double GetFuelRemaining(WsfPlatform aPlatform)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover(); \n");
   scriptStr.append("   if (mover.IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfP6DOF_Mover p6dofMover = (WsfP6DOF_Mover)mover; \n");
   scriptStr.append("      return p6dofMover.GetTotalFuelRemaining(); \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\"); \n");
   scriptStr.append("      return 0.0; \n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
   }

   QString ScriptGeneratorP6DOF::GetFlightEnvelopeScripts()
   {
   QString scriptStr = "\n";

   scriptStr.append("script_struct DataPair\n");
   scriptStr.append("   script_variables\n");
   scriptStr.append("      double velocity_fps = 0;\n");
   scriptStr.append("      double altitude_ft  = 0;\n");
   scriptStr.append("   end_script_variables\n");
   scriptStr.append("end_script_struct\n");
   scriptStr.append("\n");
   scriptStr.append("script_variables\n");
   scriptStr.append("   //******************************************\n");
   scriptStr.append("   //These are input variables to control the generation\n");
   scriptStr.append("   // of AB and Mil flight envelopes\n");
   scriptStr.append("   double feInitialAltitude_ft  = 2000;\n");
   scriptStr.append("   double feFinalAltitude_ft    = 70000;\n");
   scriptStr.append("   double feAltitudeStep_ft     = 2000;\n");
   scriptStr.append("   double feAltitudePrecision   = 10.0;\n");
   scriptStr.append("   \n");
   scriptStr.append("   double feInitialVelocity_fps = 100;\n");
   scriptStr.append("   double feFinalVelocity_fps   = 3000;\n");
   scriptStr.append("   double feVelocityStep_fps    = 25;\n");
   scriptStr.append("   double feVelocityPrecision   = 1.0;\n");
   scriptStr.append("   \n");
   scriptStr.append("   //******************************************\n");
   scriptStr.append("   //Do not change the remaining variables here.\n");
   scriptStr.append("   //These are not input variables\n");
   scriptStr.append("   Array<struct> bottomSide = Array<struct>();\n");
   scriptStr.append("   Array<struct> topSide = Array<struct>();\n");
   scriptStr.append("   \n");
   scriptStr.append("   double throttleSetting; //Do not initialize here, as it is set prior to use, and it might cause confusion\n");
   scriptStr.append("   double flapSettings; //Do not initialize here, as it is set prior to use, and it might cause confusion\n");
   scriptStr.append("   int gLoad; //Do not initialize here, as it is set prior to use, and it might cause confusion\n");
   scriptStr.append("   \n");
   scriptStr.append("   double minCanFlyVelocity = 0.0;\n");
   scriptStr.append("   double minCanFlyAltitude = 0.0;\n");
   scriptStr.append("   //******************************************\n");
   scriptStr.append("end_script_variables\n");
   scriptStr.append("\n");
   scriptStr.append("script int CanFlyWithG_Load(WsfP6DOF_Mover aPlatform, double aAltitude, double aVelocity, double aThrottleSetting, double aFlapsSetting, int aGLoad)\n");
   scriptStr.append("   int output = aPlatform.__getTestObject().CanFlyAt(aAltitude, aVelocity, aThrottleSetting, aFlapsSetting);\n");
   scriptStr.append("\n");
   scriptStr.append("   if(aPlatform.MaxPotentialManeuverGLoad() >= aGLoad )\n");
   scriptStr.append("   {\n");
   scriptStr.append("      return output;\n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("   return 0;\n");
   scriptStr.append("   }\n");
   scriptStr.append("        \n");
   scriptStr.append("end_script\n");
   scriptStr.append("\n");
   scriptStr.append("script bool ComputeMinFlightSpeed(WsfP6DOF_Mover aPlatform, double aAltitude, double aMinSpeed, double aMaxSpeed, double aSpeedStep)\n");
   scriptStr.append("   double velocity = aMinSpeed;\n");
   scriptStr.append("   while(velocity <= aMaxSpeed)\n");
   scriptStr.append("   {\n");
   scriptStr.append("      if(CanFlyWithG_Load(aPlatform, aAltitude, velocity, throttleSetting, flapSettings, gLoad) == 2) // 2 == CAN_FLY\n");
   scriptStr.append("      {\n");
   scriptStr.append("         if(aSpeedStep <= feVelocityPrecision ||  // If step is small enough (accuracy is acheived, return result)\n");
   scriptStr.append("            aMinSpeed == velocity) // if Can fly at min velocity, return result\n");
   scriptStr.append("         {\n");
   scriptStr.append("            struct temp = struct.New(\"DataPair\");\n");
   scriptStr.append("            temp->altitude_ft = aAltitude;  \n");
   scriptStr.append("            temp->velocity_fps = velocity;  \n");
   scriptStr.append("            topSide.PushBack(temp);\n");
   scriptStr.append("            return true;\n");
   scriptStr.append("         }\n");
   scriptStr.append("         else\n");
   scriptStr.append("         {\n");
   scriptStr.append("            return ComputeMinFlightSpeed(aPlatform, aAltitude, velocity - aSpeedStep, velocity, aSpeedStep / 2);\n");
   scriptStr.append("         }\n");
   scriptStr.append("      }\n");
   scriptStr.append("      velocity = velocity + aSpeedStep;\n");
   scriptStr.append("   }\n");
   scriptStr.append("   return false;\n");
   scriptStr.append("end_script\n");
   scriptStr.append("\n");
   scriptStr.append("script bool ComputeMaxFlightSpeed(WsfP6DOF_Mover aPlatform, double aAltitude, double aMinSpeed, double aMaxSpeed, double aSpeedStep)\n");
   scriptStr.append("   double velocity = aMinSpeed;\n");
   scriptStr.append("   while(velocity <= aMaxSpeed)\n");
   scriptStr.append("   {\n");
   scriptStr.append("      if(CanFlyWithG_Load(aPlatform, aAltitude, velocity, throttleSetting, flapSettings, gLoad) != 2) // 2 == CAN_FLY\n");
   scriptStr.append("      {\n");
   scriptStr.append("         if(aSpeedStep <= feVelocityPrecision)\n");
   scriptStr.append("         {\n");
   scriptStr.append("            struct temp = struct.New(\"DataPair\");\n");
   scriptStr.append("            temp->altitude_ft = aAltitude;  \n");
   scriptStr.append("            temp->velocity_fps = velocity - aSpeedStep;   \n");
   scriptStr.append("            bottomSide.PushBack(temp);\n");
   scriptStr.append("            return true;\n");
   scriptStr.append("         }\n");
   scriptStr.append("         else\n");
   scriptStr.append("         {\n");
   scriptStr.append("            return ComputeMaxFlightSpeed(aPlatform, aAltitude, velocity - aSpeedStep, velocity, aSpeedStep / 2);\n");
   scriptStr.append("         }\n");
   scriptStr.append("      }\n");
   scriptStr.append("      velocity = velocity + aSpeedStep;\n");
   scriptStr.append("   }\n");
   scriptStr.append("   return false;\n");
   scriptStr.append("end_script\n");
   scriptStr.append("\n");
   scriptStr.append("script void ComputeStep1(WsfP6DOF_Mover aPlatform)\n");
   scriptStr.append("   if(ComputeMinFlightSpeed(aPlatform, feInitialAltitude_ft, feInitialVelocity_fps, feFinalVelocity_fps, feVelocityStep_fps))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      minCanFlyVelocity = topSide[0]->velocity_fps;\n");
   scriptStr.append("      ComputeMaxFlightSpeed(aPlatform, feInitialAltitude_ft, minCanFlyVelocity, feFinalVelocity_fps, feVelocityStep_fps);\n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   scriptStr.append("\n");
   scriptStr.append("script double ComputeMinFlightAltitude(WsfP6DOF_Mover aPlatform, double aVelocity, double aStartAltitude, double aEndAltitude, double aAltitudeStep)\n");
   scriptStr.append("\n");
   scriptStr.append("   double alt_ft = aStartAltitude;\n");
   scriptStr.append("   while(alt_ft <= aEndAltitude)\n");
   scriptStr.append("   {     \n");
   scriptStr.append("      if(CanFlyWithG_Load(aPlatform, alt_ft, aVelocity, throttleSetting, flapSettings, gLoad) == 2) // 2 == CAN_FLY\n");
   scriptStr.append("      {\n");
   scriptStr.append("         if(aAltitudeStep <= feAltitudePrecision || alt_ft <= aStartAltitude)\n");
   scriptStr.append("         {\n");
   scriptStr.append("            minCanFlyAltitude = alt_ft;\n");
   scriptStr.append("            \n");
   scriptStr.append("            if(alt_ft != feInitialAltitude_ft)\n");
   scriptStr.append("            {\n");
   scriptStr.append("               struct temp = struct.New(\"DataPair\");\n");
   scriptStr.append("               temp->altitude_ft = alt_ft;  \n");
   scriptStr.append("               temp->velocity_fps = aVelocity;  \n");
   scriptStr.append("               bottomSide.PushBack(temp);\n");
   scriptStr.append("            }\n");
   scriptStr.append("            \n");
   scriptStr.append("            return true;\n");
   scriptStr.append("         }\n");
   scriptStr.append("         else if(alt_ft < aAltitudeStep)\n");
   scriptStr.append("         {\n");
   scriptStr.append("            minCanFlyAltitude = feInitialAltitude_ft;\n");
   scriptStr.append("            return true;\n");
   scriptStr.append("         }\n");
   scriptStr.append("         else\n");
   scriptStr.append("         {\n");
   scriptStr.append("            return ComputeMinFlightAltitude(aPlatform, aVelocity, alt_ft - aAltitudeStep, alt_ft, aAltitudeStep / 2);\n");
   scriptStr.append("         }\n");
   scriptStr.append("      }\n");
   scriptStr.append("      alt_ft += aAltitudeStep;\n");
   scriptStr.append("   }\n");
   scriptStr.append("   \n");
   scriptStr.append("   minCanFlyAltitude = 0.0;\n");
   scriptStr.append("   return false;\n");
   scriptStr.append("end_script\n");
   scriptStr.append("\n");
   scriptStr.append("script bool ComputeMaxFlightAltitude(WsfP6DOF_Mover aPlatform, double aVelocity, double aStartAltitude, double aEndAltitude, double aAltitudeStep)\n");
   scriptStr.append("   \n");
   scriptStr.append("   double alt_ft = aStartAltitude;\n");
   scriptStr.append("   while(alt_ft <= aEndAltitude)\n");
   scriptStr.append("   {\n");
   scriptStr.append("      if(CanFlyWithG_Load(aPlatform, alt_ft, aVelocity, throttleSetting, flapSettings, gLoad) != 2) // 2 == CAN_FLY\n");
   scriptStr.append("      {\n");
   scriptStr.append("         if(aAltitudeStep <= feAltitudePrecision)\n");
   scriptStr.append("         {\n");
   scriptStr.append("            struct temp = struct.New(\"DataPair\");\n");
   scriptStr.append("            temp->altitude_ft = alt_ft;  \n");
   scriptStr.append("            temp->velocity_fps = aVelocity;  \n");
   scriptStr.append("            topSide.PushBack(temp);\n");
   scriptStr.append("            return true;\n");
   scriptStr.append("         }\n");
   scriptStr.append("         else\n");
   scriptStr.append("         {\n");
   scriptStr.append("            return ComputeMaxFlightAltitude(aPlatform, aVelocity, alt_ft - aAltitudeStep, alt_ft, aAltitudeStep / 2);\n");
   scriptStr.append("         }        \n");
   scriptStr.append("      }\n");
   scriptStr.append("     \n");
   scriptStr.append("      alt_ft = alt_ft + aAltitudeStep;\n");
   scriptStr.append("   }\n");
   scriptStr.append("   \n");
   scriptStr.append("   if(CanFlyWithG_Load(aPlatform, alt_ft, aVelocity, throttleSetting, flapSettings, gLoad) == 2)  // 2 == CAN_FLY\n");
   scriptStr.append("   {\n");
   scriptStr.append("      struct temp = struct.New(\"DataPair\");\n");
   scriptStr.append("      temp->altitude_ft = aEndAltitude;  \n");
   scriptStr.append("      temp->velocity_fps = aVelocity;   \n");
   scriptStr.append("      topSide.PushBack(temp);\n");
   scriptStr.append("      return true;\n");
   scriptStr.append("   }\n");
   scriptStr.append("   \n");
   scriptStr.append("   return false;\n");
   scriptStr.append("   \n");
   scriptStr.append("end_script\n");
   scriptStr.append("\n");
   scriptStr.append("script void ComputeStep2(WsfP6DOF_Mover aPlatform)\n");
   scriptStr.append("   double currentVelocity = (minCanFlyVelocity + feVelocityStep_fps) / feVelocityStep_fps;\n");
   scriptStr.append("   currentVelocity = Math.Floor(currentVelocity) * feVelocityStep_fps;\n");
   scriptStr.append("   \n");
   scriptStr.append("   bool completed = false;\n");
   scriptStr.append("   while(currentVelocity <= feFinalVelocity_fps && !completed)\n");
   scriptStr.append("   {\n");
   scriptStr.append("      write(\".\");\n");
   scriptStr.append("      \n");
   scriptStr.append("      if(ComputeMinFlightAltitude(aPlatform, currentVelocity, feInitialAltitude_ft, feFinalAltitude_ft, feAltitudeStep_ft))\n");
   scriptStr.append("      {\n");
   scriptStr.append("         ComputeMaxFlightAltitude(aPlatform, currentVelocity, minCanFlyAltitude, feFinalAltitude_ft, feAltitudeStep_ft);\n");
   scriptStr.append("      }\n");
   scriptStr.append("      else //There is no altitude at which the platform can fly for the given speed\n");
   scriptStr.append("      {\n");
   scriptStr.append("         completed = true;\n");
   scriptStr.append("      }\n");
   scriptStr.append("      \n");
   scriptStr.append("      currentVelocity = currentVelocity + feVelocityStep_fps;\n");
   scriptStr.append("   }\n");
   scriptStr.append("   writeln(\".\");\n");
   scriptStr.append("      \n");
   scriptStr.append("end_script\n");
   scriptStr.append("\n");
   scriptStr.append("// This computes the flight envelope for a platform at the specified throttle and flaps settings.\n");
   scriptStr.append("// The throttle input assumes that 0 is idle, 1 is full power without augmentation/afterburner\n");
   scriptStr.append("// (military power), and 2 is full power with full augmentation/afterburner (full afterburner).\n");
   scriptStr.append("script Array<struct> ComputeFlightEnvelope(WsfPlatform aPlatform, double aThrottle, double aFlaps, int aGLoad)\n");
   scriptStr.append("   Array<struct> returnVal = Array<struct>();\n");
   scriptStr.append("   \n");
   scriptStr.append("   throttleSetting = aThrottle;\n");
   scriptStr.append("   flapSettings = aFlaps;\n");
   scriptStr.append("   gLoad = aGLoad;\n");
   scriptStr.append("   \n");
   scriptStr.append("   bottomSide.Clear();\n");
   scriptStr.append("   topSide.Clear();\n");
   scriptStr.append("   \n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover();\n");
   scriptStr.append("   if(mover.IsA_TypeOf(\"WSF_P6DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfP6DOF_Mover           p6dofMover = (WsfP6DOF_Mover)mover;\n");
   scriptStr.append("      WsfP6DOF_MoverTestObject testObj    = p6dofMover.__getTestObject();\n");
   scriptStr.append("      testObj.SetMasterNoLagTesting(true);\n");
   scriptStr.append("      testObj.SetMasterFreezeAltitude(true);\n");
   scriptStr.append("      testObj.SetMasterFreezeFuelBurn(true);\n");
   scriptStr.append("      testObj.SetMasterFreezeLocation(true);\n");
   scriptStr.append("      \n");
   scriptStr.append("      write(\".\");\n");
   scriptStr.append("      ComputeStep1(p6dofMover);\n");
   scriptStr.append("      ComputeStep2(p6dofMover);\n");
   scriptStr.append("      \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\");\n");
   scriptStr.append("   }\n");
   scriptStr.append("   \n");
   scriptStr.append("   for(int i = 0; i < topSide.Size(); i = i + 1)\n");
   scriptStr.append("   {     \n");
   scriptStr.append("      returnVal.PushBack(topSide[i]);\n");
   scriptStr.append("   } \n");
   scriptStr.append("   for(int i = bottomSide.Size() - 1; i >= 0; i = i - 1)\n");
   scriptStr.append("   {     \n");
   scriptStr.append("      returnVal.PushBack(bottomSide[i]);\n");
   scriptStr.append("   } \n");
   scriptStr.append("   \n");
   scriptStr.append("   return returnVal;\n");
   scriptStr.append("end_script\n");
   return scriptStr;
   }

   double ScriptGeneratorP6DOF::CalcSolidPropellantMass_lbs()
   {
   double propellantMass_lbs = 0.0;

   // Loop through the engines, looking for solid-propellant rockets
   for (auto curEngine : mEngineList)
   {
      if (curEngine->GetEngineType() == GeometryEngine::EngineType::cSOLIDROCKET)
      {
         auto engine = dynamic_cast<AeroSolidRocketEngine*>(curEngine);
         propellantMass_lbs += engine->GetPropellantMass_lbs();
      }
   }

   return propellantMass_lbs;
   }

} // namespace Designer
