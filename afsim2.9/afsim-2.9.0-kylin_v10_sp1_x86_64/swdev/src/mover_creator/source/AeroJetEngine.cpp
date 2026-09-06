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

#include "AeroJetEngine.hpp"

#include <QDate>
#include <QDir>
#include <QJsonArray>
#include <QMessageBox>

#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorWidget.hpp"
#include "UtTableOperations.hpp"

Designer::AeroJetEngine::AeroJetEngine(const std::string& aModelName)
   : AeroEngine(aModelName)
{
   mEngineType = GeometryEngine::EngineType::cJET;
   LoadFromFile();
}

void Designer::AeroJetEngine::LoadFromFile()
{
   mRawData.clear();
   mEngineDataMultiplier.clear();
   mEngineDataOffset.clear();
   mPlotNameMap.clear();
   mPlotLabelMap.clear();

   QString engineBasePath = MoverCreatorMainWindow::GetDataPath() + "/Engines/";

   // Change the current JSON doc
   QString docName = engineBasePath;
   docName.append("Jet/");
   docName.append(QString::fromStdString(mModelName));
   docName.append(".amc");
   MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->SetCurrentEngineFile(docName);

   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentEngineFile();
   if (!curDoc)
   {
      return;
   }
   QJsonObject mainObj = curDoc->object();

   mThrustOffset_ft    = mainObj["ThrustOffset"].toDouble();
   mDiameter_ft        = mainObj["Diameter"].toDouble();
   mLength_ft          = mainObj["Length"].toDouble();
   mMass_lbs           = mainObj["Mass"].toDouble();
   mRefArea_sqft       = mainObj["RefArea"].toDouble();

   mIncludeAfterburner = mainObj["IncludeAfterburner"].toBool();

   double maxAltitude = mainObj["MaxAltitude"].toDouble();
   double maxMach = mainObj["MaxMach"].toDouble();

   mIdleTsfc = mainObj["IdleTsfc"].toDouble();
   mMaxIndepVar.emplace(DataKey::cIDLE_THRUST_ALT, maxAltitude);
   mMaxIndepVar.emplace(DataKey::cIDLE_THRUST_MACH, maxMach);

   mEngineDataMultiplier.emplace(DataKey::cIDLE_THRUST_ALT, mainObj["IdleThrust"].toDouble());
   mEngineDataMultiplier.emplace(DataKey::cIDLE_THRUST_MACH, mainObj["IdleThrust"].toDouble());
   mEngineDataOffset.emplace(DataKey::cIDLE_THRUST_ALT, 0);
   mEngineDataOffset.emplace(DataKey::cIDLE_THRUST_MACH, 0);
   mPlotNameMap.emplace(AeroEngine::DataKey::cIDLE_THRUST_ALT,   AeroEngine::DataLabels("Idle Thrust vs Altitude", "Altitude", "Thrust"));
   mPlotNameMap.emplace(AeroEngine::DataKey::cIDLE_THRUST_MACH,  AeroEngine::DataLabels("Idle Thrust vs Mach", "Mach", "Thrust"));
   mPlotLabelMap.emplace(AeroEngine::DataKey::cIDLE_THRUST_ALT,  AeroEngine::DataLabels("Idle Thrust Profile", "Altitude (ft)", "Thrust (lbs)"));
   mPlotLabelMap.emplace(AeroEngine::DataKey::cIDLE_THRUST_MACH, AeroEngine::DataLabels("Idle Thrust Profile", "Mach", "Thrust (lbs)"));

   mMilTsfc = mainObj["MilTsfc"].toDouble();
   mMaxIndepVar.emplace(DataKey::cMIL_THRUST_ALT, maxAltitude);
   mMaxIndepVar.emplace(DataKey::cMIL_THRUST_MACH, maxMach);
   mMaxIndepVar.emplace(DataKey::cMIL_SPIN_UP, 0);
   mMaxIndepVar.emplace(DataKey::cMIL_SPIN_DOWN, 0);
   mEngineDataMultiplier.emplace(DataKey::cMIL_THRUST_ALT, mainObj["MilThrust"].toDouble());
   mEngineDataMultiplier.emplace(DataKey::cMIL_THRUST_MACH, mainObj["MilThrust"].toDouble());
   mEngineDataMultiplier.emplace(DataKey::cMIL_SPIN_UP, 1);
   mEngineDataMultiplier.emplace(DataKey::cMIL_SPIN_DOWN, 1);
   mEngineDataOffset.emplace(DataKey::cMIL_THRUST_ALT, 0);
   mEngineDataOffset.emplace(DataKey::cMIL_THRUST_MACH, 0);
   mEngineDataOffset.emplace(DataKey::cMIL_SPIN_UP, 0);
   mEngineDataOffset.emplace(DataKey::cMIL_SPIN_DOWN, 0);
   mPlotNameMap.emplace(AeroEngine::DataKey::cMIL_THRUST_ALT,   AeroEngine::DataLabels("Military Thrust vs Altitude", "Altitude", "Thrust"));
   mPlotNameMap.emplace(AeroEngine::DataKey::cMIL_THRUST_MACH,  AeroEngine::DataLabels("Military Thrust vs Mach", "Mach", "Thrust"));
   mPlotNameMap.emplace(AeroEngine::DataKey::cMIL_SPIN_UP,      AeroEngine::DataLabels("Military Spin-Up Rate", "Percent Throttle", "Spin-Up Rate"));
   mPlotNameMap.emplace(AeroEngine::DataKey::cMIL_SPIN_DOWN,    AeroEngine::DataLabels("Military Spin-Down Rate", "Percent Throttle", "Spin-Down Rate"));
   mPlotLabelMap.emplace(AeroEngine::DataKey::cMIL_THRUST_ALT,  AeroEngine::DataLabels("Military Thrust Profile", "Altitude (ft)", "Thrust (lbs)"));
   mPlotLabelMap.emplace(AeroEngine::DataKey::cMIL_THRUST_MACH, AeroEngine::DataLabels("Military Thrust Profile", "Mach", "Thrust (lbs)"));
   mPlotLabelMap.emplace(AeroEngine::DataKey::cMIL_SPIN_UP,     AeroEngine::DataLabels("Military Spin-up", "Percent Throttle", "Spin-up Rate (% throttle/sec)"));
   mPlotLabelMap.emplace(AeroEngine::DataKey::cMIL_SPIN_DOWN,   AeroEngine::DataLabels("Military Spin-down", "Percent Throttle", "Spin-down Rate (% throttle/sec)"));

   mMaxIndepVar.emplace(DataKey::cAB_THRUST_ALT, maxAltitude);
   mMaxIndepVar.emplace(DataKey::cAB_THRUST_MACH, maxMach);
   mMaxIndepVar.emplace(DataKey::cAB_SPIN_UP, 0);
   mMaxIndepVar.emplace(DataKey::cAB_SPIN_DOWN, 0);
   mEngineDataMultiplier.emplace(DataKey::cAB_THRUST_ALT, 1);
   mEngineDataMultiplier.emplace(DataKey::cAB_THRUST_MACH, 1);
   mEngineDataMultiplier.emplace(DataKey::cAB_SPIN_UP, 1);
   mEngineDataMultiplier.emplace(DataKey::cAB_SPIN_DOWN, 1);
   mEngineDataOffset.emplace(DataKey::cAB_THRUST_ALT, 0);
   mEngineDataOffset.emplace(DataKey::cAB_THRUST_MACH, 0);
   mEngineDataOffset.emplace(DataKey::cAB_SPIN_UP, 0);
   mEngineDataOffset.emplace(DataKey::cAB_SPIN_DOWN, 0);

   if (mIncludeAfterburner)
   {
      mABTsfc = mainObj["ABTsfc"].toDouble();
      mEngineDataMultiplier[DataKey::cAB_THRUST_ALT] = mainObj["ABThrust"].toDouble();
      mEngineDataMultiplier[DataKey::cAB_THRUST_MACH] = mainObj["ABThrust"].toDouble();

      mPlotNameMap.emplace(AeroEngine::DataKey::cAB_THRUST_ALT,   AeroEngine::DataLabels("Afterburner Thrust vs Altitude", "Altitude", "Thrust"));
      mPlotNameMap.emplace(AeroEngine::DataKey::cAB_THRUST_MACH,  AeroEngine::DataLabels("Afterburner Thrust vs Mach", "Mach", "Thrust"));
      mPlotNameMap.emplace(AeroEngine::DataKey::cAB_SPIN_UP,      AeroEngine::DataLabels("Afterburner Spin-Up Rate", "Percent Throttle", "Spin-Up Rate"));
      mPlotNameMap.emplace(AeroEngine::DataKey::cAB_SPIN_DOWN,    AeroEngine::DataLabels("Afterburner Spin-Down Rate", "Percent Throttle", "Spin-Down Rate"));
      mPlotLabelMap.emplace(AeroEngine::DataKey::cAB_THRUST_ALT,  AeroEngine::DataLabels("Afterburner Thrust Profile", "Altitude (ft)", "Thrust (lbs)"));
      mPlotLabelMap.emplace(AeroEngine::DataKey::cAB_THRUST_MACH, AeroEngine::DataLabels("Afterburner Thrust Profile", "Mach", "Thrust (lbs)"));
      mPlotLabelMap.emplace(AeroEngine::DataKey::cAB_SPIN_UP,     AeroEngine::DataLabels("Afterburner Spin-up", "Percent Throttle", "Spin-up Rate (% throttle/sec)"));
      mPlotLabelMap.emplace(AeroEngine::DataKey::cAB_SPIN_DOWN,   AeroEngine::DataLabels("Afterburner Spin-down", "Percent Throttle", "Spin-down Rate (% throttle/sec)"));
   }
   QJsonObject plotObj = mainObj["plots"].toObject();

   for (const auto& p : mPlotNameMap)
   {
      QString plotName = QString::fromStdString(p.second.title);
      QString xVar     = QString::fromStdString(p.second.xLabel);
      QString yVar     = QString::fromStdString(p.second.yLabel);

      QJsonObject plot  = plotObj[plotName].toObject();
      QJsonArray xArray = plot[xVar].toArray();
      QJsonArray yArray = plot[yVar].toArray();

      if (xArray.size() != yArray.size())
      {
         QMessageBox::critical(nullptr, "Error", "Number of X and Y plot parameters do not match for " + plotName);
         return;
      }

      for (int i = 0; i < xArray.size(); ++i)
      {
         double x = xArray.at(i).toDouble();
         double y = yArray.at(i).toDouble();

         mRawData[p.first].emplace_back(x, y);
      }

      if (plot.contains("modifier"))
      {
         QJsonObject modifierObj = plot["modifier"].toObject();
         // Now figure out if using a gain or an offset
         if (modifierObj.contains("gain"))
         {
            // Finally figure out if gain is an object or just a value. If it's an object, then this uses the modifier line, if just a value then this uses a global gain
            if (modifierObj["gain"].isObject())
            {
               QJsonObject gainObj = modifierObj["gain"].toObject();
               std::vector<std::pair<double, double>> modifierLine;
               QJsonArray xVals = gainObj["x_values"].toArray();
               QJsonArray yVals = gainObj["y_values"].toArray();
               QJsonArray::iterator xIter = xVals.begin();
               QJsonArray::iterator yIter = yVals.begin();
               while (xIter != xVals.end())
               {
                  double curX = xIter->toDouble();
                  double curY = yIter->toDouble();
                  modifierLine.emplace_back(std::pair<double, double>(curX, curY));
                  ++xIter;
                  ++yIter;
               }
               mModifierMultiplierLines.emplace(p.first, modifierLine);
               mModifierTypes.emplace(p.first, ModifierType::cMULTIPLIER_LINE);
            }
            if (modifierObj["gain"].isDouble())
            {
               double gainValue = modifierObj["gain"].toDouble();
               mModifierMultipliers.emplace(p.first, gainValue);
               mModifierTypes.emplace(p.first, ModifierType::cMULTIPLIER_VALUE);
            }
         }
         else if (modifierObj.contains("offset"))
         {
            // Finally figure out if gain is an object or just a value. If it's an object, then this uses the modifier line, if just a value then this uses a global gain
            if (modifierObj["offset"].isObject())
            {
               QJsonObject gainObj = modifierObj["offset"].toObject();
               std::vector<std::pair<double, double>> modifierLine;
               QJsonArray xVals = gainObj["x_values"].toArray();
               QJsonArray yVals = gainObj["y_values"].toArray();
               QJsonArray::iterator xIter = xVals.begin();
               QJsonArray::iterator yIter = yVals.begin();
               while (xIter != xVals.end())
               {
                  double curX = xIter->toDouble();
                  double curY = yIter->toDouble();
                  modifierLine.emplace_back(std::pair<double, double>(curX, curY));
                  ++xIter;
                  ++yIter;
               }
               mModifierOffsetLines.emplace(p.first, modifierLine);
               mModifierTypes.emplace(p.first, ModifierType::cOFFSET_LINE);
            }
            if (modifierObj["offset"].isDouble())
            {
               double offsetValue = modifierObj["offset"].toDouble();
               mModifierOffsets.emplace(p.first, offsetValue);
               mModifierTypes.emplace(p.first, ModifierType::cOFFSET_VALUE);
            }
         }
      }

   }

   GenerateEngineData();
}

void Designer::AeroJetEngine::SaveToFile() const
{
   std::shared_ptr<QJsonDocument> curDoc =
      MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentEngineFile();

   if (!curDoc)
   {
      QMessageBox::critical(nullptr, "Error", "No Model has been selected to be saved.  Please select a model or create a new one if one does not exist!");
      return;
   }
   QJsonObject mainObj = curDoc->object();
   QJsonObject plotsObj;

   mainObj["model_name"]   = QString::fromStdString(mModelName);
   mainObj["Length"]       = mLength_ft;
   mainObj["Diameter"]     = mDiameter_ft;
   mainObj["Mass"]         = mMass_lbs;
   mainObj["ThrustOffset"] = mThrustOffset_ft;

   mainObj["RefArea"]            = mRefArea_sqft;
   mainObj["IncludeAfterburner"] = mIncludeAfterburner;
   mainObj["IdleThrust"]         = mEngineDataMultiplier.at(AeroEngine::DataKey::cIDLE_THRUST_ALT);
   mainObj["MilThrust"]          = mEngineDataMultiplier.at(AeroEngine::DataKey::cMIL_THRUST_ALT);
   mainObj["ABThrust"]           = mEngineDataMultiplier.at(AeroEngine::DataKey::cAB_THRUST_ALT);
   mainObj["IdleTsfc"]           = mIdleTsfc;
   mainObj["MilTsfc"]            = mMilTsfc;
   mainObj["ABTsfc"]             = mABTsfc;
   mainObj["MaxAltitude"]        = mMaxIndepVar.at(AeroEngine::DataKey::cIDLE_THRUST_ALT);
   mainObj["MaxMach"]            = mMaxIndepVar.at(AeroEngine::DataKey::cIDLE_THRUST_MACH);

   QJsonObject oldPlotsObj = mainObj["plots"].toObject();

   for (const auto& i : mRawData)
   {
      QJsonObject curPlot;
      QJsonObject indepVar;
      QJsonObject depVar;

      QJsonArray x;
      QJsonArray y;

      for (const auto& j : i.second)
      {
         x.append(j.first);
         y.append(j.second);
      }

      QString xlabel = QString::fromStdString(GetPlotName(i.first).xLabel);
      QString ylabel = QString::fromStdString(GetPlotName(i.first).yLabel);
      QString title = QString::fromStdString(GetPlotName(i.first).title);

      curPlot[QString::fromStdString(GetPlotName(i.first).xLabel)] = x;
      curPlot[QString::fromStdString(GetPlotName(i.first).yLabel)] = y;

      // Save off modifications made to plots
      if (oldPlotsObj.contains(title))
      {
         QJsonObject prevPlotDataObj = oldPlotsObj[title].toObject();
         if (prevPlotDataObj.contains("modifier"))
         {
            QJsonObject modifierObj = prevPlotDataObj["modifier"].toObject();
            curPlot["modifier"] = modifierObj;
         }
      }
      plotsObj[QString::fromStdString(GetPlotName(i.first).title)] = curPlot;
   }
   mainObj["plots"] = plotsObj;

   curDoc->setObject(mainObj);
   MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->SaveCurrentEngineFile();
}

double Designer::AeroJetEngine::GetThrust(double aAltitude, double aMach, int aThrottle) const
{
   double seaLevelThrust = 0;

   switch (aThrottle)
   {
      case 0:
         seaLevelThrust = mEngineDataMultiplier.at(DataKey::cIDLE_THRUST_ALT);
         break;
      case 1:
         seaLevelThrust = mEngineDataMultiplier.at(DataKey::cMIL_THRUST_ALT);
         break;
      case 2:
         seaLevelThrust = mEngineDataMultiplier.at(DataKey::cAB_THRUST_ALT);
         break;
      default:
         break;
   }

   if (seaLevelThrust > Designer::cEpsilon)
   {
      // We need to multiply the thrust at aAltitude at Mach 0 by machFactor to account for Machs greater than 0.
      // To get the machFactor, divide the thrust at aMach at sea level by thrust at mach 0 at sea level.
      double machFactor = GetModifiedThrustFromMach(aMach, aThrottle) / seaLevelThrust;
      return GetModifiedThrustFromAltitude(aAltitude, aThrottle) * machFactor;
   }
   return 0.0;
}

double Designer::AeroJetEngine::GetFuelFlow(double aAltitude, double aMach, int aThrottle) const
{
   double TSFC = 0.0;

   switch (aThrottle)
   {
      case 0:
      {
         TSFC = mIdleTsfc;
         break;
      }
      case 1:
      {
         TSFC = mMilTsfc;
         break;
      }
      case 2:
      {
         TSFC = mABTsfc;
         break;
      }
      default:
      {
         TSFC = mIdleTsfc;
         break;
      }
   }

   return TSFC * GetThrust(aAltitude, aMach, aThrottle);
}

void Designer::AeroJetEngine::Export(const std::string& aFileName, ExportMoverType aExportMoverType) const
{
   QString filename = QString::fromStdString(aFileName);
   QDir directory(filename);
   directory.cdUp();
   directory.mkdir(QString::fromStdString("data"));
   directory.setPath(directory.path() + QString::fromStdString("/data"));

   // Get the vehicle
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Vehicle* vehicle = mw.GetMoverCreatorWidget()->GetVehicle();
   if (vehicle == nullptr)
   {
      mw.CreateErrorBox("No vehicle is available!");
      return;
   }

   QFile file(filename);
   file.remove();
   if (file.open(QIODevice::ReadWrite))
   {
      QTextStream stream(&file);

      stream.setRealNumberNotation(QTextStream::FixedNotation);
      QTextStream::FieldAlignment alignment = QTextStream::FieldAlignment::AlignRight;

      QDate Today = QDate::currentDate();
      stream << "# ##################################################################################################" << endl;
      stream << "# " << endl;
      VersionInfo::OutputVersionStringInOutputStream(stream);
      stream << "# " << endl;
      stream << "# This file defines a P6DOF model of " << QString::fromStdString(mModelName).toUpper() << "." << endl;
      stream << "# " << endl;
      stream << "# Creation Date:  " << Today.toString(Qt::RFC2822Date) << endl;
      stream << "# " << endl;
      stream << "# ##################################################################################################" << endl;
      stream << endl;
      stream << endl;
      if (aExportMoverType == ExportMoverType::cP6DOF)
      {
         stream << "p6dof_object_types" << endl;
         stream << endl;
         stream << "   jet_engine_type   " << QString::fromStdString(mModelName).toUpper() << "   P6DOF_JET_ENGINE" << endl;
         stream << endl;
         stream << "      thrust_offset       ";
         OutputData(stream, mThrustOffset_ft, 12, 4, alignment);
         stream << " ft" << endl;
         stream << endl;
         stream << "      //-------------------------------------------------------------------------" << endl;
         stream << endl;
         stream << "      inop_ref_area       ";
         OutputData(stream, mRefArea_sqft, 12, 4, alignment);
         stream << " ft^2" << endl;
         stream << endl;
      }
      else if (aExportMoverType == ExportMoverType::cPOINT_MASS)
      {
          stream << "six_dof_object_types" << endl << endl;
          stream << "   point_mass_engine_type   " << QString::fromStdString(mModelName).toUpper() << "   BASE_TYPE" << endl << endl;
          stream << "      //-------------------------------------------------------------------------" << endl << endl;
          stream << "      thrust_offset    " << mThrustOffset_ft << " ft" << endl << endl;
          stream << "      //-------------------------------------------------------------------------" << endl << endl;
          stream << "      inop_ref_area     " << mRefArea_sqft << " ft^2" << endl << endl;
          stream << "      //-------------------------------------------------------------------------" << endl << endl;
          stream << "      jet" << endl;
      }
      else
      {
         stream << "six_dof_object_types" << endl;
         stream << endl;
         stream << "   rigid_body_engine_type   " << QString::fromStdString(mModelName).toUpper() << "   BASE_TYPE" << endl;
         stream << endl;
         stream << "      thrust_offset       ";
         OutputData(stream, mThrustOffset_ft, 12, 4, alignment);
         stream << " ft" << endl;
         stream << endl;
         stream << "      //-------------------------------------------------------------------------" << endl;
         stream << endl;
         stream << "      inop_ref_area       ";
         OutputData(stream, mRefArea_sqft, 12, 4, alignment);
         stream << " ft^2" << endl;
         stream << endl;
         stream << "      jet" << endl;
      }
      stream << endl;
      stream << "      tsfc_idle_pph       ";
      OutputData(stream, mIdleTsfc, 12, 4, alignment);
      stream << "  # lb/lb/hr" << endl;
      stream << "      tsfc_mil_pph        ";
      OutputData(stream, mMilTsfc, 12, 4, alignment);
      stream << "  # lb/lb/hr" << endl;

      if (mIncludeAfterburner)
      {
         stream << "      tsfc_ab_pph         ";
         OutputData(stream, mABTsfc, 12, 4, alignment);
         stream << "  # lb/lb/hr" << endl;
      }

      stream << endl;
      stream << "      //-------------------------------------------------------------------------" << endl;
      stream << endl;
      stream << "      rated_thrust_idle   ";
      OutputData(stream, mEngineDataMultiplier.at(DataKey::cIDLE_THRUST_ALT), 12, 4, alignment);
      stream << " lbf" << endl;

      stream << "      rated_thrust_mil    ";
      OutputData(stream, mEngineDataMultiplier.at(DataKey::cMIL_THRUST_ALT), 12, 4, alignment);
      stream << " lbf" << endl;

      if (mIncludeAfterburner)
      {
         stream << "      rated_thrust_ab     ";
         OutputData(stream, mEngineDataMultiplier.at(DataKey::cAB_THRUST_ALT), 12, 4, alignment);
         stream << " lbf" << endl;
      }

      stream << "      //-------------------------------------------------------------------------" << endl;
      stream << endl;
      stream << "      spin_up_table_mil_per_sec" << endl;
      stream << "         // throttle_level  spin_up_per_sec" << endl;

      std::vector<std::pair<double, double>> spinUpTableData;
      GetEngineTableData(DataKey::cMIL_SPIN_UP, spinUpTableData);
      for (const auto& i : spinUpTableData)
      {
         stream << "              ";
         OutputData(stream, i.first, 12, 4, alignment);
         stream << "         ";
         OutputData(stream, i.second, 12, 4, alignment);
         stream << endl;
      }
      stream << "      end_spin_up_table_mil_per_sec" << endl;
      stream << endl;

      stream << "      spin_down_table_mil_per_sec" << endl;
      stream << "         // throttle_level  spin_down_per_sec" << endl;

      std::vector<std::pair<double, double>> spinDownTableData;
      GetEngineTableData(DataKey::cMIL_SPIN_DOWN, spinDownTableData);
      for (const auto& i : spinDownTableData)
      {
         stream << "              ";
         OutputData(stream, i.first, 12, 4, alignment);
         stream << "         ";
         OutputData(stream, i.second, 12, 4, alignment);
         stream << endl;
      }
      stream << "      end_spin_down_table_mil_per_sec" << endl;
      stream << endl;

      if (mIncludeAfterburner)
      {
         stream << "      spin_up_table_ab_per_sec" << endl;
         stream << "         // throttle_level  spin_up_per_sec" << endl;

         std::vector<std::pair<double, double>> ABSpinUpTableData;
         GetEngineTableData(DataKey::cAB_SPIN_UP, ABSpinUpTableData);
         for (const auto& i : ABSpinUpTableData)
         {
            stream << "              ";
            OutputData(stream, i.first, 12, 4, alignment);
            stream << "         ";
            OutputData(stream, i.second, 12, 4, alignment);
            stream << endl;
         }
         stream << "      end_spin_up_table_ab_per_sec" << endl;
         stream << endl;

         stream << "      spin_down_table_ab_per_sec" << endl;
         stream << "         // throttle_level  spin_up_per_sec" << endl;

         std::vector<std::pair<double, double>> ABSpinDownTableData;
         GetEngineTableData(DataKey::cAB_SPIN_DOWN, ABSpinDownTableData);
         for (const auto& i : ABSpinDownTableData)
         {
            stream << "              ";
            OutputData(stream, i.first, 12, 4, alignment);
            stream << "         ";
            OutputData(stream, i.second, 12, 4, alignment);
            stream << endl;
         }
         stream << "      end_spin_down_table_ab_per_sec" << endl;
      }

      stream << "      //-------------------------------------------------------------------------" << endl;
      stream << endl;
      stream << "      thrust_idle_alt_mach_table" << endl;
      stream << "         file " << "data/thrust_idle_alt_mach.txt" << endl;

      QFile idleFile(directory.path() + "/thrust_idle_alt_mach.txt");
      idleFile.remove();
      idleFile.open(QIODevice::ReadWrite);
      QTextStream idleStream(&idleFile);

      idleStream.setRealNumberNotation(QTextStream::FixedNotation);

      idleStream << "irregular_table" << endl;
      idleStream << "   independent_variable alt units ft" << endl;
      idleStream << "   independent_variable mach precision float" << endl;
      idleStream << "   dependent_variable precision float" << endl;

      std::vector<std::pair<double, double>> thrustAltIdle;
      std::vector<std::pair<double, double>> thrustMachIdle;
      std::vector<std::pair<double, double>> thrustAltMil;
      std::vector<std::pair<double, double>> thrustMachMil;
      std::vector<std::pair<double, double>> thrustAltAB;
      std::vector<std::pair<double, double>> thrustMachAB;
      GetEngineTableData(DataKey::cIDLE_THRUST_ALT, thrustAltIdle);
      GetEngineTableData(DataKey::cIDLE_THRUST_MACH, thrustMachIdle);
      GetEngineTableData(DataKey::cMIL_THRUST_ALT, thrustAltMil);
      GetEngineTableData(DataKey::cMIL_THRUST_MACH, thrustMachMil);

      if (mIncludeAfterburner)
      {
         GetEngineTableData(DataKey::cAB_THRUST_ALT, thrustAltAB);
         GetEngineTableData(DataKey::cAB_THRUST_MACH, thrustMachAB);
      }

      for (const auto& i : thrustAltIdle)
      {
         idleStream << "   alt  ";
         OutputData(idleStream, i.first, 8, 1, alignment);
         idleStream << endl;

         idleStream << "      mach  ";
         for (const auto& j : thrustMachIdle)
         {
            idleStream << "    ";
            OutputData(idleStream, j.first, 5, 2, alignment);
         }
         idleStream << endl;

         idleStream << "      values";

         for (const auto& j : thrustMachIdle)
         {
            idleStream << "    ";
            OutputData(idleStream, GetThrust(i.first, j.first, 0), 12, 4, alignment);
         }
         idleStream << endl;
      }

      idleStream << "end_irregular_table" << endl;
      idleFile.close();

      stream << "      end_thrust_idle_alt_mach_table" << endl;
      stream << endl;
      stream << "      //-------------------------------------------------------------------------" << endl;
      stream << endl;
      stream << "      thrust_mil_alt_mach_table" << endl;
      stream << "         file " << "data/thrust_mil_alt_mach.txt" << endl;

      QFile milFile(directory.path() + "/thrust_mil_alt_mach.txt");
      milFile.remove();
      milFile.open(QIODevice::ReadWrite);
      QTextStream milStream(&milFile);

      milStream.setRealNumberNotation(QTextStream::FixedNotation);

      milStream << "irregular_table" << endl;
      milStream << "   independent_variable alt units ft" << endl;
      milStream << "   independent_variable mach precision float" << endl;
      milStream << "   dependent_variable precision float" << endl;

      for (const auto& i : thrustAltMil)
      {
         milStream << "   alt  ";
         OutputData(milStream, i.first, 8, 1, alignment);
         milStream << endl;

         milStream << "      mach  ";
         for (const auto& j : thrustMachMil)
         {
            milStream << "    ";
            OutputData(milStream, j.first, 5, 2, alignment);
         }
         milStream << endl;

         milStream << "      values";

         for (const auto& j : thrustMachMil)
         {
            milStream << "    ";
            OutputData(milStream, GetThrust(i.first, j.first, 1), 12, 4, alignment);
         }

         milStream << endl;
      }

      milStream << "end_irregular_table" << endl;
      milFile.close();

      stream << "      end_thrust_mil_alt_mach_table" << endl;
      stream << endl;
      stream << "      //-------------------------------------------------------------------------" << endl;
      stream << endl;

      if (mIncludeAfterburner)
      {
         stream << "      thrust_ab_alt_mach_table" << endl;
         stream << "         file " << "data/thrust_ab_alt_mach.txt" << endl;

         QFile abFile(directory.path() + "/thrust_ab_alt_mach.txt");
         abFile.remove();
         abFile.open(QIODevice::ReadWrite);
         QTextStream abStream(&abFile);

         abStream.setRealNumberNotation(QTextStream::FixedNotation);

         abStream << "irregular_table" << endl;
         abStream << "   independent_variable alt units ft" << endl;
         abStream << "   independent_variable mach precision float" << endl;
         abStream << "   dependent_variable precision float" << endl;

         for (const auto& i : thrustAltAB)
         {
            abStream << "   alt  ";
            OutputData(abStream, i.first, 8, 1, alignment);
            abStream << endl;

            abStream << "      mach  ";
            for (const auto& j : thrustMachAB)
            {
               abStream << "    ";
               OutputData(abStream, j.first, 5, 2, alignment);
            }
            abStream << endl;

            abStream << "      values";
            for (const auto& j : thrustMachAB)
            {
               abStream << "    ";
               OutputData(abStream, GetThrust(i.first, j.first, 2), 12, 4, alignment);
            }

            abStream << endl;
         }

         abStream << "end_irregular_table" << endl;
         abFile.close();

         stream << "      end_thrust_ab_alt_mach_table" << endl;
         stream << endl;

         if (aExportMoverType == ExportMoverType::cP6DOF)
         {
            stream << "      //-------------------------------------------------------------------------" << endl;
            stream << endl;
            stream << "      throttle_setting_ab     ThrottleAfterburner" << endl;
            stream << endl;
         }
      }

      if (aExportMoverType == ExportMoverType::cP6DOF)
      {
         stream << "      throttle_setting_mil    ThrottleMilitary" << endl;
         stream << "   end_jet_engine_type" << endl;
         stream << endl;
         stream << "end_p6dof_object_types" << endl;
      }
      else if (aExportMoverType == ExportMoverType::cPOINT_MASS)
      {
          stream << "      end_jet" << endl;
          stream << "   end_point_mass_engine_type" << endl;
          stream << "end_six_dof_object_types" << endl;
      }
      else
      {
         stream << "      end_jet" << endl;
         if (mIncludeAfterburner)
         {
            stream << "      throttle_setting_ab     ThrottleAfterburner" << endl;
         }
         stream << "      throttle_setting_mil    ThrottleMilitary" << endl;
         stream << "   end_rigid_body_engine_type" << endl;
         stream << endl;
         stream << "end_six_dof_object_types" << endl;
      }

      stream << endl;

      file.close();
   }
}

Designer::AeroEngine::PropulsionParameters Designer::AeroJetEngine::GetPropulsionData(double aMaxMach) const
{
   PropulsionParameters params;

   params.mFuel_mass = 0.0;
   params.mIsp_sea_level = 3600 * 1.0 / mMilTsfc;

   double sea_level_thrust = 0;
   GetEngineTable(AeroEngine::DataKey::cMIL_THRUST_ALT)->GetInterpolatedDependentValueModified(0, sea_level_thrust);

   params.mMass_flow_rate = sea_level_thrust * mMilTsfc;

   double maxBurnTime = 14400; // 4 hrs (arbitrary time in the future after we run out of fuel)

   params.mThrustVsTime.emplace_back(0.0, sea_level_thrust);
   params.mThrustVsTime.emplace_back(maxBurnTime, sea_level_thrust);

   std::vector<std::pair<double, double>> thrust_vs_mach;
   GetEngineTableData(AeroEngine::DataKey::cMIL_THRUST_MACH, thrust_vs_mach);

   // Throttle table will be normalized thrust vs mach.
   for (const auto& thrust : thrust_vs_mach)
   {
      if (thrust.first <= aMaxMach)
      {
         params.mThrottleVsMach.emplace_back(thrust.first, thrust.second / sea_level_thrust);
      }
   }

   // Calculate what the vacuum specific impulse should be. 
   // Theoretically, to get the desired altitude effect, this should be zero. 
   // However, GM and P6DOF differ in how they compute thrust.
   // To compute the thrust at a given altitude, GM uses air pressure, whereas P6DOF uses air density. 
   // By shifting the vacuum isp (and therefore the vacuum thrust), 
   // we can get the thrust for GM and P6DOF to line up at the testing altitude.
   params.mIsp_vacuum = 0.01;

   // Get the vehicle
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Vehicle* vehicle = mw.GetMoverCreatorWidget()->GetVehicle();
   if (vehicle != nullptr)
   {
      double testingAltitude_ft = vehicle->GetVehicleAltitude_ft();
      double localPressure_psf = vehicle->GetAtmosphere().Pressure(testingAltitude_ft * UtMath::cM_PER_FT) / UtMath::cPASCAL_PER_PSF;
      double seaLevelPressure_psf = vehicle->GetAtmosphere().Pressure(0.0)  / UtMath::cPASCAL_PER_PSF;

      double localThrust = 0;
      GetEngineTable(AeroEngine::DataKey::cMIL_THRUST_ALT)->GetInterpolatedDependentValueModified(testingAltitude_ft, localThrust);

      // Guided Mover uses the following equation to compute thrust:
      //
      // thrust = V -  (V - S) * p / P
      // 
      // where: 
      //        'thrust' is the local thrust
      //        'V' is the vacuum thrust
      //        'S' is the sea level thrust
      //        'p' is the local pressure
      //        'P' is the pressure at sea level
      //
      // Rearranging this equation yields:
      //
      // V = (P * thrust - S * p) / (P - p)
      //
      double vacuumThrust = (seaLevelPressure_psf * localThrust - sea_level_thrust * localPressure_psf)
                          / (seaLevelPressure_psf - localPressure_psf);

      // We can compute the vacuum isp from (isp_v / isp_sl = thrust_v / thrust_sl)
      params.mIsp_vacuum = vacuumThrust * params.mIsp_sea_level / sea_level_thrust;
   }

   return params;
}

std::vector<double> Designer::AeroJetEngine::GetEngineSpecs() const
{
   std::vector<double> specs = AeroEngine::GetEngineSpecs();

   std::vector<double> jetSpecs = std::vector<double>({ mRefArea_sqft,
                                                        mEngineDataMultiplier.at(DataKey::cIDLE_THRUST_ALT),
                                                        mEngineDataMultiplier.at(DataKey::cMIL_THRUST_ALT),
                                                        mEngineDataMultiplier.at(DataKey::cAB_THRUST_ALT),
                                                        mIdleTsfc,
                                                        mMilTsfc,
                                                        mABTsfc,
                                                        mMaxIndepVar.at(DataKey::cIDLE_THRUST_ALT),
                                                        mMaxIndepVar.at(DataKey::cIDLE_THRUST_MACH)
                                                      });

   specs.insert(specs.end(), jetSpecs.begin(), jetSpecs.end());

   return specs;
}

void Designer::AeroJetEngine::SetEngineSpecs(const std::vector<double>& aEngineSpecs)
{
   AeroEngine::SetEngineSpecs(aEngineSpecs);

   mRefArea_sqft                                     = aEngineSpecs.at(mNumBaseSpecs);
   mEngineDataMultiplier[DataKey::cIDLE_THRUST_ALT]  = aEngineSpecs.at(mNumBaseSpecs + 1);
   mEngineDataMultiplier[DataKey::cIDLE_THRUST_MACH] = aEngineSpecs.at(mNumBaseSpecs + 1);
   mEngineDataMultiplier[DataKey::cMIL_THRUST_ALT]   = aEngineSpecs.at(mNumBaseSpecs + 2);
   mEngineDataMultiplier[DataKey::cMIL_THRUST_MACH]  = aEngineSpecs.at(mNumBaseSpecs + 2);
   mEngineDataMultiplier[DataKey::cAB_THRUST_ALT]    = aEngineSpecs.at(mNumBaseSpecs + 3);
   mEngineDataMultiplier[DataKey::cAB_THRUST_MACH]   = aEngineSpecs.at(mNumBaseSpecs + 3);

   mIdleTsfc = aEngineSpecs.at(mNumBaseSpecs + 4);
   mMilTsfc  = aEngineSpecs.at(mNumBaseSpecs + 5);
   mABTsfc   = aEngineSpecs.at(mNumBaseSpecs + 6);

   mMaxIndepVar[DataKey::cIDLE_THRUST_ALT]  = aEngineSpecs.at(mNumBaseSpecs + 7);
   mMaxIndepVar[DataKey::cMIL_THRUST_ALT]   = aEngineSpecs.at(mNumBaseSpecs + 7);
   mMaxIndepVar[DataKey::cAB_THRUST_ALT]    = aEngineSpecs.at(mNumBaseSpecs + 7);
   mMaxIndepVar[DataKey::cIDLE_THRUST_MACH] = aEngineSpecs.at(mNumBaseSpecs + 8);
   mMaxIndepVar[DataKey::cMIL_THRUST_MACH]  = aEngineSpecs.at(mNumBaseSpecs + 8);
   mMaxIndepVar[DataKey::cAB_THRUST_MACH]   = aEngineSpecs.at(mNumBaseSpecs + 8);
}

double Designer::AeroJetEngine::GetModifiedThrustFromAltitude(double aAltitude, int aThrottle) const
{
   std::vector<std::pair<double, double>> thrustVec;

   switch (aThrottle)
   {
      case 0:
         GetEngineTableData(AeroEngine::DataKey::cIDLE_THRUST_ALT, thrustVec);
         break;
      case 1:
         GetEngineTableData(AeroEngine::DataKey::cMIL_THRUST_ALT, thrustVec);
         break;
      case 2:
         GetEngineTableData(AeroEngine::DataKey::cAB_THRUST_ALT, thrustVec);
         break;

      default:
         break;
   }

   std::map<double, double> altitudeMap(thrustVec.begin(), thrustVec.end());

   return util::LookupValue(altitudeMap, aAltitude, util::ExtrapolationMethod::cZERO, util::ExtrapolationMethod::cCLAMP);
}

double Designer::AeroJetEngine::GetModifiedThrustFromMach(double aMach, int aThrottle) const
{
   std::vector<std::pair<double, double>> thrustVec;

   switch (aThrottle)
   {
      case 0:
         GetEngineTableData(AeroEngine::DataKey::cIDLE_THRUST_MACH, thrustVec);
         break;
      case 1:
         GetEngineTableData(AeroEngine::DataKey::cMIL_THRUST_MACH, thrustVec);
         break;
      case 2:
         GetEngineTableData(AeroEngine::DataKey::cAB_THRUST_MACH, thrustVec);
         break;

      default:
         break;
   }

   std::map<double, double> machMap(thrustVec.begin(), thrustVec.end());

   return util::LookupValue(machMap, aMach, util::ExtrapolationMethod::cZERO, util::ExtrapolationMethod::cCLAMP);
}
