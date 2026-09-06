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

#include "AeroRamjetEngine.hpp"

#include <QDate>
#include <QJsonArray>

#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorWidget.hpp"
#include "UtTableOperations.hpp"

Designer::AeroRamjetEngine::AeroRamjetEngine(const std::string& aModelName)
   : AeroEngine(aModelName)
{
   mEngineType = GeometryEngine::EngineType::cRAMJET;

   mPlotNameMap.clear();
   mPlotLabelMap.clear();

   mPlotNameMap = std::map<AeroEngine::DataKey, AeroEngine::DataLabels>
   {
      { AeroEngine::DataKey::cTHRUST_ALT, { "Thrust vs Altitude", "Altitude", "Thrust" } },
      { AeroEngine::DataKey::cTHRUST_MACH, { "Thrust vs Mach", "Mach", "Thrust" } }
   };

   mPlotLabelMap = std::map<AeroEngine::DataKey, AeroEngine::DataLabels>
   {
      { AeroEngine::DataKey::cTHRUST_ALT, { "Thrust", "Altitude (ft)", "Thrust (lbs)" } },
      { AeroEngine::DataKey::cTHRUST_MACH, { "Thrust", "Mach", "Thrust (lbs)" } }
   };

   LoadFromFile();
}

double Designer::AeroRamjetEngine::GetThrust(double aAltitude, double aMach) const
{
   double thrustDesignMach = GetModifiedThrustFromMach(mDesignMach);
   double thrustDesignAlt = GetModifiedThrustFromAltitude(mDesignAltitude_ft);
   if (thrustDesignMach > Designer::cEpsilon)
   {
      double machMult = GetModifiedThrustFromMach(aMach) / thrustDesignMach;
      return GetModifiedThrustFromAltitude(aAltitude) * machMult;
   }
   else if (thrustDesignAlt > Designer::cEpsilon)
   {
      double altMult = GetModifiedThrustFromAltitude(aAltitude) / thrustDesignAlt;
      return GetModifiedThrustFromMach(aMach) * altMult;
   }
   return 0.0;
}

double Designer::AeroRamjetEngine::GetFuelFlow(double aAltitude, double aMach) const
{
   return mTsfc_lbs_per_hr * GetThrust(aAltitude, aMach);
}

void Designer::AeroRamjetEngine::Export(const std::string& aFileName, ExportMoverType aExportMoverType) const
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
      stream << endl;
      stream << endl;
      if (aExportMoverType == ExportMoverType::cP6DOF)
      {
         stream << "p6dof_object_types" << endl;
         stream << endl;
         stream << "   ramjet_engine_type   " << QString::fromStdString(mModelName).toUpper() << "   P6DOF_RAMJET_ENGINE" << endl;
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
          stream << "      ramjet" << endl;
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
         stream << "      ramjet" << endl;
      }
      stream << endl;
      stream << "      //-------------------------------------------------------------------------" << endl;
      stream << endl;
      OutputData(stream, "latch_fuel_injection", 28, 6, QTextStream::FieldAlignment::AlignLeft);
      OutputData(stream, QVariant(mLatchFuelInjection).toString(), 12, 0, alignment);
      stream << endl << endl;
      stream << "      //-------------------------------------------------------------------------" << endl;
      stream << endl;
      OutputData(stream, "use_proportional_throttle", 28, 6, QTextStream::FieldAlignment::AlignLeft);
      OutputData(stream, QVariant(mUseProportionalThrottle).toString(), 12, 0, alignment);
      stream << endl << endl;
      stream << "      //-------------------------------------------------------------------------" << endl;
      stream << endl;
      OutputData(stream, "minimum_proportional_thrust", 28, 6, QTextStream::FieldAlignment::AlignLeft);
      OutputData(stream, mMinProportionalThrottle, 12, 4, alignment);
      stream << endl << endl;
      stream << "      //-------------------------------------------------------------------------" << endl;
      stream << endl;
      stream << "      tsfc_alt_mach_table" << endl;
      stream << "         irregular_table" << endl;
      stream << "            independent_variable alt units ft" << endl;
      stream << "            independent_variable mach precision float" << endl;
      stream << "            dependent_variable precision float" << endl;
      stream << endl;
      stream << "            alt 0.0" << endl;
      stream << "               mach     0.0     20.0" << endl;
      stream << "               values   ";
      OutputData(stream, mTsfc_lbs_per_hr, 12, 4, alignment);
      stream << "     ";
      OutputData(stream, mTsfc_lbs_per_hr, 12, 4, alignment);
      stream << endl;
      stream << "            alt 100000.0" << endl;
      stream << "               mach     0.0     20.0" << endl;
      stream << "               values   ";
      OutputData(stream, mTsfc_lbs_per_hr, 12, 4, alignment);
      stream << "     ";
      OutputData(stream, mTsfc_lbs_per_hr, 12, 4, alignment);
      stream << endl;
      stream << "          end_irregular_table" << endl;
      stream << "       end_tsfc_alt_mach_table" << endl;
      stream << endl;
      stream << "      //-------------------------------------------------------------------------" << endl;
      stream << endl;
      stream << "      thrust_alt_mach_table" << endl;
      stream << "         irregular_table" << endl;
      stream << "            independent_variable alt units ft" << endl;
      stream << "            independent_variable mach precision float" << endl;
      stream << "            dependent_variable precision float" << endl;

      std::vector<std::pair<double, double>> thrustVsAltitude;
      std::vector<std::pair<double, double>> thrustVsMach;
      GetEngineTableData(DataKey::cTHRUST_ALT, thrustVsAltitude);
      GetEngineTableData(DataKey::cTHRUST_MACH, thrustVsMach);

      for (auto& i : thrustVsAltitude)
      {
         stream << "   alt  ";
         OutputData(stream, i.first, 8, 1, alignment);
         stream << endl;

         stream << "      mach";
         for (auto& j : thrustVsMach)
         {
            stream << "     ";
            OutputData(stream, j.first, 5, 2, alignment);
         }
         stream << endl;

         stream << "      values";
         for (auto& j : thrustVsMach)
         {
            stream << "     ";
            OutputData(stream, GetThrust(i.first, j.first), 12, 4, alignment);
         }
         stream << endl;
      }

      stream << "         end_irregular_table" << endl;
      stream << "      end_thrust_alt_mach_table" << endl;
      stream << endl;
      if (aExportMoverType == ExportMoverType::cP6DOF)
      {
         stream << "   end_ramjet_engine_type" << endl;
         stream << endl;
         stream << "end_p6dof_object_types" << endl;
      }
      else if (aExportMoverType == ExportMoverType::cPOINT_MASS)
      {
          stream << "      end_ramjet" << endl << endl;
          stream << "   end_point_mass_engine_type" << endl << endl;
          stream << "end_six_dof_object_types" << endl;
      }
      else
      {
         stream << "      end_ramjet" << endl;
         stream << endl;
         stream << "   end_rigid_body_engine_type" << endl;
         stream << endl;
         stream << "end_six_dof_object_types" << endl;
      }
      stream << endl;

      file.close();
   }
}

Designer::AeroEngine::PropulsionParameters Designer::AeroRamjetEngine::GetPropulsionData(double aMaxMach) const
{
   PropulsionParameters params;

   params.mFuel_mass = 0.0;
   params.mIsp_sea_level = 3600 * 1.0 / mTsfc_lbs_per_hr;
   
   double sea_level_thrust = 0;
   GetEngineTable(AeroEngine::DataKey::cTHRUST_ALT)->GetInterpolatedDependentValueModified(0, sea_level_thrust);

   params.mMass_flow_rate = sea_level_thrust * mTsfc_lbs_per_hr;

   double maxBurnTime = 14400; // 4 hrs (arbitrary time in the future after we run out of fuel)

   params.mThrustVsTime.emplace_back(0.0, sea_level_thrust);
   params.mThrustVsTime.emplace_back(maxBurnTime, sea_level_thrust);

   std::vector<std::pair<double, double>> thrust_vs_mach;
   GetEngineTableData(AeroEngine::DataKey::cTHRUST_MACH, thrust_vs_mach);

   // Throttle table will be normalized thrust vs mach.
   for (const auto& thrust : thrust_vs_mach)
   {
      if (thrust.first <= aMaxMach)
      {
         params.mThrottleVsMach.emplace_back(thrust.first, thrust.second / mDesignThrust_lbs);
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
      double seaLevelPressure_psf = vehicle->GetAtmosphere().Pressure(0.0) / UtMath::cPASCAL_PER_PSF;

      double localThrust = 0;
      GetEngineTable(AeroEngine::DataKey::cTHRUST_ALT)->GetInterpolatedDependentValueModified(testingAltitude_ft, localThrust);

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

void Designer::AeroRamjetEngine::LoadFromFile()
{
   mRawData.clear();
   mEngineDataMultiplier.clear();
   mEngineDataOffset.clear();

   QString engineBasePath = MoverCreatorMainWindow::GetDataPath() + "/Engines/";
   // Change the current JSON doc
   QString docName = engineBasePath;
   docName.append("Ramjet/");
   docName.append(QString::fromStdString(mModelName));
   docName.append(".amc");
   MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->SetCurrentEngineFile(docName);

   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentEngineFile();
   if (!curDoc)
   {
      return;
   }
   QJsonObject mainObj = curDoc->object();

   mThrustOffset_ft   = mainObj["ThrustOffset"].toDouble();
   mDiameter_ft       = mainObj["Diameter"].toDouble();
   mLength_ft         = mainObj["Length"].toDouble();
   mMass_lbs          = mainObj["Mass"].toDouble();

   mRefArea_sqft      = mainObj["RefArea"].toDouble();
   mDesignAltitude_ft = mainObj["DesignAltitude"].toDouble();
   mDesignMach        = mainObj["DesignMach"].toDouble();
   mDesignThrust_lbs  = mainObj["DesignThrust"].toDouble();
   mTsfc_lbs_per_hr   = mainObj["Tsfc"].toDouble();

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

   double maxAltitude = mainObj["MaxAltitude"].toDouble();
   double maxMach = mainObj["MaxMach"].toDouble();

   mMaxIndepVar.emplace(DataKey::cTHRUST_ALT, maxAltitude);
   mMaxIndepVar.emplace(DataKey::cTHRUST_MACH, maxMach);

   mEngineDataMultiplier.emplace(DataKey::cTHRUST_ALT, mDesignThrust_lbs / GetNormalizedThrustFromAltitude(mDesignAltitude_ft));
   mEngineDataMultiplier.emplace(DataKey::cTHRUST_MACH, mDesignThrust_lbs / GetNormalizedThrustFromMach(mDesignMach));

   mEngineDataOffset.emplace(DataKey::cTHRUST_ALT, 0);
   mEngineDataOffset.emplace(DataKey::cTHRUST_MACH, 0);

   GenerateEngineData();
}

void Designer::AeroRamjetEngine::SaveToFile() const
{
   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentEngineFile();
   if (!curDoc)
   {
      QMessageBox::critical(nullptr, "Error", "No Model has been selected to be saved.  Please select a model or create a new one if one does not exist!");
      return;
   }
   QJsonObject mainObj = curDoc->object();
   QJsonObject plotsObj;

   mainObj["model_name"]     = QString::fromStdString(mModelName);
   mainObj["Length"]         = mLength_ft;
   mainObj["Diameter"]       = mDiameter_ft;
   mainObj["Mass"]           = mMass_lbs;
   mainObj["ThrustOffset"]   = mThrustOffset_ft;

   mainObj["RefArea"]        = mRefArea_sqft;
   mainObj["DesignAltitude"] = mDesignAltitude_ft;
   mainObj["DesignMach"]     = mDesignMach;
   mainObj["DesignThrust"]   = mDesignThrust_lbs;
   mainObj["Tsfc"]           = mTsfc_lbs_per_hr;
   mainObj["MaxAltitude"]    = mMaxIndepVar.at(AeroEngine::DataKey::cTHRUST_ALT);
   mainObj["MaxMach"]        = mMaxIndepVar.at(AeroEngine::DataKey::cTHRUST_MACH);

   QJsonObject oldPlotsObj   = mainObj["plots"].toObject();

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
      QString title  = QString::fromStdString(GetPlotName(i.first).title);

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

std::vector<double> Designer::AeroRamjetEngine::GetEngineSpecs() const
{
   std::vector<double> specs = AeroEngine::GetEngineSpecs();

   std::vector<double> jetSpecs = std::vector<double>({ mRefArea_sqft,
                                                        mDesignAltitude_ft,
                                                        mDesignMach,
                                                        mDesignThrust_lbs,
                                                        mTsfc_lbs_per_hr,
                                                        mMaxIndepVar.at(DataKey::cTHRUST_ALT),
                                                        mMaxIndepVar.at(DataKey::cTHRUST_MACH),
                                                      });

   specs.insert(specs.end(), jetSpecs.begin(), jetSpecs.end());

   return specs;
}

void Designer::AeroRamjetEngine::SetEngineSpecs(const std::vector<double>& aEngineSpecs)
{
   AeroEngine::SetEngineSpecs(aEngineSpecs);

   mRefArea_sqft = aEngineSpecs.at(mNumBaseSpecs + 0);
   mDesignAltitude_ft = aEngineSpecs.at(mNumBaseSpecs + 1);
   mDesignMach = aEngineSpecs.at(mNumBaseSpecs + 2);
   mDesignThrust_lbs = aEngineSpecs.at(mNumBaseSpecs + 3);
   mTsfc_lbs_per_hr = aEngineSpecs.at(mNumBaseSpecs + 4);

   mEngineDataMultiplier[DataKey::cTHRUST_ALT] = mDesignThrust_lbs / GetNormalizedThrustFromAltitude(mDesignAltitude_ft);
   mEngineDataMultiplier[DataKey::cTHRUST_MACH] = mDesignThrust_lbs / GetNormalizedThrustFromMach(mDesignMach);

   mMaxIndepVar[DataKey::cTHRUST_ALT] = aEngineSpecs.at(mNumBaseSpecs + 5);
   mMaxIndepVar[DataKey::cTHRUST_MACH] = aEngineSpecs.at(mNumBaseSpecs + 6);

}

void Designer::AeroRamjetEngine::SetData(const std::string&         aName,
                                         GeometryEngine::EngineType aEngineType,
                                         UtVec3dX                   aRefPoint,
                                         double                     aYaw_deg,
                                         double                     aPitch_deg,
                                         double                     aRoll_deg,
                                         double                     aLength_ft,
                                         double                     aDiameter_ft,
                                         double                     aMass_lbs,
                                         double                     aThrustOffset_ft,
                                         double                     aIgnitionDelay_sec,
                                         bool                       aLatchFuelInjection,
                                         bool                       aUseProportionalThrottle,
                                         double                     aMinProportionalThrottle)
{
   mLatchFuelInjection = aLatchFuelInjection;
   mUseProportionalThrottle = aUseProportionalThrottle;
   mMinProportionalThrottle = aMinProportionalThrottle;

   AeroEngine::SetData(aName, 
                       aEngineType, 
                       aRefPoint, 
                       aYaw_deg, 
                       aPitch_deg, 
                       aRoll_deg, 
                       aLength_ft, 
                       aDiameter_ft, 
                       aMass_lbs, 
                       aThrustOffset_ft, 
                       aIgnitionDelay_sec);
}

double Designer::AeroRamjetEngine::GetNormalizedThrustFromAltitude(double aAltitude) const
{
   std::vector<std::pair<double, double>> thrustVec = mRawData.at(AeroEngine::DataKey::cTHRUST_ALT);
   std::map<double, double> altitudeMap(thrustVec.begin(), thrustVec.end());

   return util::LookupValue(altitudeMap, aAltitude, util::ExtrapolationMethod::cZERO, util::ExtrapolationMethod::cCLAMP);
}

double Designer::AeroRamjetEngine::GetNormalizedThrustFromMach(double aMach) const
{
   std::vector<std::pair<double, double>> thrustVec = mRawData.at(AeroEngine::DataKey::cTHRUST_MACH);
   std::map<double, double> machMap(thrustVec.begin(), thrustVec.end());

   return util::LookupValue(machMap, aMach, util::ExtrapolationMethod::cZERO, util::ExtrapolationMethod::cZERO);
}

double Designer::AeroRamjetEngine::GetModifiedThrustFromAltitude(double aAltitude) const
{
   std::vector<std::pair<double, double>> thrustVec;
   GetEngineTableData(AeroEngine::DataKey::cTHRUST_ALT, thrustVec);
   std::map<double, double> altitudeMap(thrustVec.begin(), thrustVec.end());

   return util::LookupValue(altitudeMap, aAltitude, util::ExtrapolationMethod::cZERO, util::ExtrapolationMethod::cCLAMP);
}

double Designer::AeroRamjetEngine::GetModifiedThrustFromMach(double aMach) const
{
   std::vector<std::pair<double, double>> thrustVec;
   GetEngineTableData(AeroEngine::DataKey::cTHRUST_MACH, thrustVec);
   std::map<double, double> machMap(thrustVec.begin(), thrustVec.end());

   return util::LookupValue(machMap, aMach, util::ExtrapolationMethod::cZERO, util::ExtrapolationMethod::cZERO);
}
