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

#include "AeroLiquidRocketEngine.hpp"

#include <QDate>
#include <QJsonArray>

#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorWidget.hpp"
#include "UtTableOperations.hpp"

Designer::AeroLiquidRocketEngine::AeroLiquidRocketEngine(const std::string& aModelName)
   : AeroEngine(aModelName)
{
   mEngineType = GeometryEngine::EngineType::cLIQUIDROCKET;

   mPlotNameMap.clear();
   mPlotLabelMap.clear();

   mPlotNameMap = std::map<AeroEngine::DataKey, AeroEngine::DataLabels>
   {
      { AeroEngine::DataKey::cTHRUST_ALT, { "Thrust vs Altitude", "Altitude", "Thrust" } },
      { AeroEngine::DataKey::cISP_ALT, { "Specific Impulse vs Altitude", "Altitude", "Specific Impulse" } }
   };

   mPlotLabelMap = std::map<AeroEngine::DataKey, AeroEngine::DataLabels>
   {
      { AeroEngine::DataKey::cTHRUST_ALT, { "Thrust", "Altitude (ft)", "Thrust (lbs)" } },
      { AeroEngine::DataKey::cISP_ALT, { "Specific Impulse", "Altitude (ft)", "Specific Impulse (s)" } }
   };

   LoadFromFile();
}

double Designer::AeroLiquidRocketEngine::GetThrust(double aAltitude) const
{
   std::vector<std::pair<double, double>> thrustVec;
   GetEngineTableData(AeroEngine::DataKey::cTHRUST_ALT, thrustVec);
   std::map<double, double> thrustMap(thrustVec.begin(), thrustVec.end());

   return util::LookupValue(thrustMap, aAltitude, util::ExtrapolationMethod::cCLAMP, util::ExtrapolationMethod::cZERO);
}

double Designer::AeroLiquidRocketEngine::GetFuelFlow(double aAltitude) const
{
   return GetThrust(aAltitude) / GetSpecificImpulse(aAltitude);
}

void Designer::AeroLiquidRocketEngine::LoadFromFile()
{
   mRawData.clear();
   mEngineDataMultiplier.clear();
   mEngineDataOffset.clear();

   QString engineBasePath = MoverCreatorMainWindow::GetDataPath() + "/Engines/";
   // Change the current JSON doc
   QString docName = engineBasePath;
   docName.append("LiquidRocket/");
   docName.append(QString::fromStdString(mModelName));
   docName.append(".amc");
   MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->SetCurrentEngineFile(docName);

   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentEngineFile();
   if (!curDoc)
   {
      return;
   }
   QJsonObject mainObj = curDoc->object();

   mThrustOffset_ft = mainObj["ThrustOffset"].toDouble();
   mDiameter_ft = mainObj["Diameter"].toDouble();
   mLength_ft = mainObj["Length"].toDouble();
   mMass_lbs = mainObj["Mass"].toDouble();

   mRefArea_sqft = mainObj["RefArea"].toDouble();
   mSpinUpRate = mainObj["SpinUpRate"].toDouble();
   mSpinDownRate = mainObj["SpinDownRate"].toDouble();
   mThrustSeaLevel_lbs = mainObj["ThrustSeaLevel"].toDouble();
   mThrustVacuum_lbs   = mainObj["ThrustVacuum"].toDouble();
   mSpecificImpulseSeaLevel_sec = mainObj["SpecificImpulseSeaLevel"].toDouble();
   mSpecificImpulseVacuum_sec   = mainObj["SpecificImpulseVacuum"].toDouble();

   QJsonObject plotObj = mainObj["plots"].toObject();

   for (const auto& p : mPlotNameMap)
   {
      QString plotName = QString::fromStdString(p.second.title);
      QString xVar = QString::fromStdString(p.second.xLabel);
      QString yVar = QString::fromStdString(p.second.yLabel);

      QJsonObject plot = plotObj[plotName].toObject();
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

   mEngineDataMultiplier.clear();
   mEngineDataOffset.clear();

   mEngineDataMultiplier.emplace(AeroEngine::DataKey::cISP_ALT, mSpecificImpulseVacuum_sec - mSpecificImpulseSeaLevel_sec);
   mEngineDataMultiplier.emplace(AeroEngine::DataKey::cTHRUST_ALT, mThrustVacuum_lbs - mThrustSeaLevel_lbs);

   mEngineDataOffset.emplace(AeroEngine::DataKey::cISP_ALT, mSpecificImpulseSeaLevel_sec);
   mEngineDataOffset.emplace(AeroEngine::DataKey::cTHRUST_ALT, mThrustSeaLevel_lbs);

   mMaxIndepVar.emplace(DataKey::cISP_ALT, 0);
   mMaxIndepVar.emplace(DataKey::cTHRUST_ALT, 0);

   GenerateEngineData();
}

void Designer::AeroLiquidRocketEngine::SaveToFile() const
{
   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentEngineFile();
   if (!curDoc)
   {
      QMessageBox::critical(nullptr, "Error", "No Model has been selected to be saved.  Please select a model or create a new one if one does not exist!");
      return;
   }
   QJsonObject mainObj = curDoc->object();
   QJsonObject plotsObj;

   mainObj["model_name"] = QString::fromStdString(mModelName);
   mainObj["Length"] = mLength_ft;
   mainObj["Diameter"] = mDiameter_ft;
   mainObj["Mass"] = mMass_lbs;
   mainObj["ThrustOffset"] = mThrustOffset_ft;

   mainObj["RefArea"] = mRefArea_sqft;
   mainObj["SpinUpRate"] = mSpinUpRate;
   mainObj["SpinDownRate"] = mSpinDownRate;
   mainObj["ThrustSeaLevel"] = mThrustSeaLevel_lbs;
   mainObj["ThrustVacuum"] = mThrustVacuum_lbs;
   mainObj["SpecificImpulseSeaLevel"] = mSpecificImpulseSeaLevel_sec;
   mainObj["SpecificImpulseVacuum"] = mSpecificImpulseVacuum_sec;

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

void Designer::AeroLiquidRocketEngine::Export(const std::string& aFileName, ExportMoverType aExportMoverType) const
{
   QString filename = QString::fromStdString(aFileName);

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
      stream << endl << endl;
      if (aExportMoverType == ExportMoverType::cP6DOF)
      {
         stream << "p6dof_object_types" << endl << endl;
         stream << "   liquid_propellant_rocket_type   " << QString::fromStdString(mModelName).toUpper() << "   P6DOF_LIQUID_PROPELLANT_ROCKET" << endl << endl;
         stream << "      //-------------------------------------------------------------------------" << endl << endl;
         stream << "      thrust_offset    " << mThrustOffset_ft << " ft" << endl << endl;
         stream << "      //-------------------------------------------------------------------------" << endl << endl;
         stream << "      inop_ref_area     " << mRefArea_sqft << " ft^2" << endl << endl;
         stream << "      //-------------------------------------------------------------------------" << endl << endl;
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
          stream << "      liquid_propellant_rocket" << endl << endl;
      }
      else
      {
         stream << "six_dof_object_types" << endl << endl;
         stream << "   rigid_body_engine_type   " << QString::fromStdString(mModelName).toUpper() << "   BASE_TYPE" << endl << endl;
         stream << "      //-------------------------------------------------------------------------" << endl << endl;
         stream << "      thrust_offset    " << mThrustOffset_ft << " ft" << endl << endl;
         stream << "      //-------------------------------------------------------------------------" << endl << endl;
         stream << "      inop_ref_area     " << mRefArea_sqft << " ft^2" << endl << endl;
         stream << "      liquid_propellant_rocket" << endl << endl;
      }
      stream << "      max_thrust_sealevel     " << mThrustSeaLevel_lbs << " lbf" << endl;
      stream << "      //-------------------------------------------------------------------------" << endl << endl;
      stream << "      normalized_spinup     " << mSpinUpRate << endl;
      stream << "      //-------------------------------------------------------------------------" << endl << endl;
      stream << "      normalized_spindown     " << mSpinDownRate << endl;
      stream << "      //-------------------------------------------------------------------------" << endl << endl;
      stream << "      isp_vs_alt" << endl;
      stream << "      //alt          Isp" << endl;

      std::vector<std::pair<double, double>> ispVsAltitude;
      std::vector<std::pair<double, double>> thrustVsAltitude;
      GetEngineTableData(DataKey::cISP_ALT, ispVsAltitude);
      GetEngineTableData(DataKey::cTHRUST_ALT, thrustVsAltitude);

      for (auto& i : ispVsAltitude)
      {
         stream << "         ";
         OutputData(stream, i.first, 8, 1, alignment);
         stream << "    ";
         OutputData(stream, i.second, 13, 6, alignment);
         stream << endl;
      }

      stream << "      end_isp_vs_alt" << endl << endl;
      stream << "      //-------------------------------------------------------------------------" << endl << endl;
      stream << "      normalized_thrust_vs_alt" << endl;
      stream << "      //alt          normalized thrust" << endl;

      for (auto& i : thrustVsAltitude)
      {
         stream << "         ";
         OutputData(stream, i.first, 8, 1, alignment);
         stream << "    ";
         OutputData(stream, i.second / mThrustSeaLevel_lbs, 10, 7, alignment);
         stream << endl;
      }

      stream << "      end_normalized_thrust_vs_alt" << endl << endl;
      stream << "      //-------------------------------------------------------------------------" << endl << endl;
      if (aExportMoverType == ExportMoverType::cP6DOF)
      {
         stream << "   end_liquid_propellant_rocket_type" << endl;
         stream << endl;
         stream << "end_p6dof_object_types" << endl;
      }
      else if (aExportMoverType == ExportMoverType::cPOINT_MASS)
      {
          stream << "      end_liquid_propellant_rocket" << endl << endl;
          stream << "   end_point_mass_engine_type" << endl << endl;
          stream << "end_six_dof_object_types" << endl;
      }
      else 
      {
         stream << "      end_liquid_propellant_rocket" << endl << endl;
         stream << "   end_rigid_body_engine_type" << endl << endl;
         stream << "end_six_dof_object_types" << endl;
      }

      file.close();
   }
}

Designer::AeroEngine::PropulsionParameters Designer::AeroLiquidRocketEngine::GetPropulsionData(double aMaxMach) const
{
   PropulsionParameters params;

   params.mFuel_mass = 0.0;
   params.mIsp_sea_level = mSpecificImpulseSeaLevel_sec;
   params.mIsp_vacuum = mSpecificImpulseVacuum_sec;

   params.mMass_flow_rate = 3600 * mThrustSeaLevel_lbs / mSpecificImpulseSeaLevel_sec;

   double maxBurnTime = 14400; // 4 hrs (arbitrary time in the future after we run out of fuel)

   params.mThrustVsTime.emplace_back(0.0, mThrustSeaLevel_lbs);
   params.mThrustVsTime.emplace_back(maxBurnTime, mThrustSeaLevel_lbs);

   params.mThrottleVsMach.emplace_back(0.0, 1.0);
   params.mThrottleVsMach.emplace_back(aMaxMach, 1.0);

   return params;
}

std::vector<double> Designer::AeroLiquidRocketEngine::GetEngineSpecs() const
{
   std::vector<double> specs = AeroEngine::GetEngineSpecs();

   std::vector<double> solidRocketSpecs = std::vector<double>({ mRefArea_sqft,
                                                                mSpinUpRate,
                                                                mSpinDownRate,
                                                                mThrustSeaLevel_lbs,
                                                                mThrustVacuum_lbs,
                                                                mSpecificImpulseSeaLevel_sec,
                                                                mSpecificImpulseVacuum_sec,
                                                              });

   specs.insert(specs.end(), solidRocketSpecs.begin(), solidRocketSpecs.end());

   return specs;
}

void Designer::AeroLiquidRocketEngine::SetEngineSpecs(const std::vector<double>& aEngineSpecs)
{
   AeroEngine::SetEngineSpecs(aEngineSpecs);

   mRefArea_sqft                = aEngineSpecs[mNumBaseSpecs + 0];
   mSpinUpRate                  = aEngineSpecs[mNumBaseSpecs + 1];
   mSpinDownRate                = aEngineSpecs[mNumBaseSpecs + 2];
   mThrustSeaLevel_lbs          = aEngineSpecs[mNumBaseSpecs + 3];
   mThrustVacuum_lbs            = aEngineSpecs[mNumBaseSpecs + 4];
   mSpecificImpulseSeaLevel_sec = aEngineSpecs[mNumBaseSpecs + 5];
   mSpecificImpulseVacuum_sec   = aEngineSpecs[mNumBaseSpecs + 6];

   mEngineDataMultiplier[DataKey::cISP_ALT] = mSpecificImpulseVacuum_sec - mSpecificImpulseSeaLevel_sec;
   mEngineDataOffset[DataKey::cISP_ALT] = mSpecificImpulseSeaLevel_sec;

   mEngineDataMultiplier[DataKey::cTHRUST_ALT] = mThrustVacuum_lbs - mThrustSeaLevel_lbs;
   mEngineDataOffset[DataKey::cTHRUST_ALT] = mThrustSeaLevel_lbs;
}

double Designer::AeroLiquidRocketEngine::GetSpecificImpulse(double aAltitude) const
{
   std::vector<std::pair<double, double>> ispVec;
   GetEngineTableData(AeroEngine::DataKey::cISP_ALT, ispVec);
   std::map<double, double> ispMap(ispVec.begin(), ispVec.end());

   return util::LookupValue(ispMap, aAltitude, util::ExtrapolationMethod::cCLAMP, util::ExtrapolationMethod::cCLAMP);
}
