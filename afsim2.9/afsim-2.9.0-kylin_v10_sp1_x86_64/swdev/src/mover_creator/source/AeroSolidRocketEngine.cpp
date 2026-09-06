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

#include "AeroSolidRocketEngine.hpp"

#include <QDate>
#include <QJsonArray>
#include <QMessageBox>

#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorWidget.hpp"
#include "UtTableOperations.hpp"

Designer::AeroSolidRocketEngine::AeroSolidRocketEngine(const std::string& aModelName)
   : AeroEngine(aModelName)
{
   mEngineType = GeometryEngine::EngineType::cSOLIDROCKET;

   mPlotNameMap.clear();
   mPlotLabelMap.clear();

   mPlotNameMap = std::map<AeroEngine::DataKey, AeroEngine::DataLabels>
   {
      { AeroEngine::DataKey::cTHRUST_TIME, { "Thrust vs Time", "Time", "Thrust" } },
      { AeroEngine::DataKey::cISP_ALT, { "Specific Impulse vs Altitude", "Altitude", "Specific Impulse" } }
   };

   mPlotLabelMap = std::map<AeroEngine::DataKey, AeroEngine::DataLabels>
   {
      { AeroEngine::DataKey::cTHRUST_TIME, { "Thrust Profile", "Time (s)", "Thrust (lbs)" } },
      { AeroEngine::DataKey::cISP_ALT, { "Specific Impulse", "Altitude (ft)", "Specific Impulse (s)" } }
   };

   LoadFromFile();
}

double Designer::AeroSolidRocketEngine::GetThrust(double aTime, double aAltitude) const
{
   std::vector<std::pair<double, double>> thrustVec;
   GetEngineTableData(AeroEngine::DataKey::cTHRUST_TIME, thrustVec);
   std::map<double, double> thrustMap(thrustVec.begin(), thrustVec.end());

   return util::LookupValue(thrustMap, aTime, util::ExtrapolationMethod::cZERO, util::ExtrapolationMethod::cZERO);
}

double Designer::AeroSolidRocketEngine::GetFuelFlow(double aTime, double aAltitude) const
{
   double isp    = GetSpecificImpulse(aAltitude);
   double thrust = GetThrust(aTime, aAltitude);

   return thrust / isp;
}

void Designer::AeroSolidRocketEngine::LoadFromFile()
{
   mRawData.clear();
   mEngineDataMultiplier.clear();

   QString engineBasePath = MoverCreatorMainWindow::GetDataPath() + "/Engines/";
   // Change the current JSON doc
   QString docName = engineBasePath;
   docName.append("SolidRocket/");
   docName.append(QString::fromStdString(mModelName));
   docName.append(".amc");
   MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->SetCurrentEngineFile(docName);

   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentEngineFile();
   if (!curDoc)
   {
      return;
   }
   QJsonObject mainObj = curDoc->object();

   // Specific Impulse
   mSpecificImpulseSeaLevel_sec = mainObj["SpecificImpulseSeaLevel"].toDouble();
   mSpecificImpulseVacuum_sec = mainObj["SpecificImpulseVacuum"].toDouble();

   // Altitude Specification. May be either Sea Level or Vacuum
   mAltitudeSpecification = mainObj["AltitudeCondition"].toString();

   mEngineDataMultiplier.clear();
   mEngineDataOffset.clear();

   mEngineDataMultiplier.emplace(DataKey::cISP_ALT, mSpecificImpulseVacuum_sec - mSpecificImpulseSeaLevel_sec);
   mEngineDataMultiplier.emplace(DataKey::cTHRUST_TIME, 1);

   mEngineDataOffset.emplace(DataKey::cISP_ALT, mSpecificImpulseSeaLevel_sec);
   mEngineDataOffset.emplace(DataKey::cTHRUST_TIME, 0);

   mMaxIndepVar.emplace(DataKey::cISP_ALT, 0);
   mMaxIndepVar.emplace(DataKey::cTHRUST_TIME, 0);

   // Plots - Specific Impulse
   QJsonObject plotObj = mainObj["plots"].toObject();
   QString plotName = QString::fromStdString("Specific Impulse vs Altitude");
   QString xVar = QString::fromStdString("Altitude");
   QString yVar = QString::fromStdString("Specific Impulse");

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

      mRawData[DataKey::cISP_ALT].emplace_back(x, y);
   }

   if (plot.contains("modifier"))
   {
      QJsonObject modifierObj = plot["modifier"].toObject();
      // Now figure out if using a gain or an offset
      if (modifierObj.contains("gain"))
      {
         // Finally figure out if gain is an object or just a value.
         // If it's an object, then this uses the modifier line,
         // if just a value then this uses a global gain
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
            mModifierMultiplierLines.emplace(DataKey::cISP_ALT, modifierLine);
            mModifierTypes.emplace(DataKey::cISP_ALT, ModifierType::cMULTIPLIER_LINE);
         }
         if (modifierObj["gain"].isDouble())
         {
            double gainValue = modifierObj["gain"].toDouble();
            mModifierMultipliers.emplace(DataKey::cISP_ALT, gainValue);
            mModifierTypes.emplace(DataKey::cISP_ALT, ModifierType::cMULTIPLIER_VALUE);
         }
      }
      else if (modifierObj.contains("offset"))
      {
         // Finally figure out if gain is an object or just a value.
         // If it's an object, then this uses the modifier line,
         // if just a value then this uses a global gain
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
            mModifierOffsetLines.emplace(DataKey::cISP_ALT, modifierLine);
            mModifierTypes.emplace(DataKey::cISP_ALT, ModifierType::cOFFSET_LINE);
         }
         if (modifierObj["offset"].isDouble())
         {
            double offsetValue = modifierObj["offset"].toDouble();
            mModifierOffsets.emplace(DataKey::cISP_ALT, offsetValue);
            mModifierTypes.emplace(DataKey::cISP_ALT, ModifierType::cOFFSET_VALUE);
         }
      }
   }

   // Base engine parameters
   mThrustOffset_ft = mainObj["ThrustOffset"].toDouble();
   mDiameter_ft = mainObj["Diameter"].toDouble();
   mLength_ft = mainObj["Length"].toDouble();

   // Specified Variables. User may specify any 2 of: Thrust, Burn Time, Propellant Mass
   QString var1 = mainObj["SpecifiedCharacteristic1"].toString();
   QString var2 = mainObj["SpecifiedCharacteristic2"].toString();
   QString vars = var1 + " " + var2;

   if (QString::compare(vars, "Thrust BurnTime") == 0 ||
       QString::compare(vars, "BurnTime Thrust") == 0)
   {
      mSpecifiedVariables = "Thrust and Burn Time";
   }
   else if (QString::compare(vars, "Thrust PropellantMass") == 0 ||
            QString::compare(vars, "PropellantMass Thrust") == 0)
   {
      mSpecifiedVariables = "Thrust and Propellant Mass";
   }
   else
   {
      mSpecifiedVariables = "Burn Time and Propellant Mass";
   }

   // Ramp up/down time
   mRampUpTime_sec = mainObj["RampUpTime"].toDouble();
   mRampDownTime_sec = mainObj["RampDownTime"].toDouble();

   // Mass. User may specify either empty or total
   double mass = mainObj["Mass"].toDouble();
   mMassSpecification = mainObj["MassType"].toString();

   // Thrust, Burn Time, Propellant Mass

   // Propellant Mass Specification
   mPropellantMassSpecification = mainObj["PropellantMassType"].toString();

   if (QString::compare(mSpecifiedVariables, "Thrust and Burn Time") == 0)
   {
      mRampUpThrust_lbs = mainObj["RatedThrustInitial"].toDouble();
      mRampDownThrust_lbs = mainObj["RatedThrustFinal"].toDouble();
      mBurnTime_sec = mainObj["BurnTime"].toDouble();

      // Calculate Propellant Mass based on Thrust and Burn Time
      CalcPropellantMass();

      if (QString::compare(mMassSpecification, "Empty") == 0)
      {
         mEmptyMass_lbs = mass;
         mTotalMass_lbs = mEmptyMass_lbs + mPropellantMass_lbs;
      }
      else // "Total"
      {
         mTotalMass_lbs = mass;
         mEmptyMass_lbs = mTotalMass_lbs - mPropellantMass_lbs;
      }
      CalcPropellantMassFraction();
   }
   else if (QString::compare(mSpecifiedVariables, "Thrust and Propellant Mass") == 0)
   {
      mRampUpThrust_lbs = mainObj["RatedThrustInitial"].toDouble();
      mRampDownThrust_lbs = mainObj["RatedThrustFinal"].toDouble();

      if (QString::compare(mPropellantMassSpecification, "Absolute") == 0)
      {
         mPropellantMass_lbs = mainObj["PropellantMass"].toDouble();
         if (QString::compare(mMassSpecification, "Empty") == 0)
         {
            mEmptyMass_lbs = mass;
            mTotalMass_lbs = mEmptyMass_lbs + mPropellantMass_lbs;
            mPropellantMassFraction = mPropellantMass_lbs / mTotalMass_lbs;
         }
         else // "Total"
         {
            mTotalMass_lbs = mass;
            mEmptyMass_lbs = mTotalMass_lbs - mPropellantMass_lbs;
            mPropellantMassFraction = mPropellantMass_lbs / mTotalMass_lbs;
         }

      }
      else // "Fraction"
      {
         mPropellantMassFraction = mainObj["PropellantMass"].toDouble();
         if (QString::compare(mMassSpecification, "Empty") == 0)
         {
            mEmptyMass_lbs = mass;
            mPropellantMass_lbs = mEmptyMass_lbs * mPropellantMassFraction / (1 - mPropellantMassFraction);
            mTotalMass_lbs = mEmptyMass_lbs + mPropellantMass_lbs;
         }
         else // "Total"
         {
            mTotalMass_lbs = mass;
            mPropellantMass_lbs = mTotalMass_lbs * mPropellantMassFraction;
            mEmptyMass_lbs = mTotalMass_lbs - mPropellantMass_lbs;
         }
      }

      // Calculate Burn Time based on Thrust and Propellant Mass
      CalcBurnTime();

   }
   else if (QString::compare(mSpecifiedVariables, "Burn Time and Propellant Mass") == 0)
   {
      mBurnTime_sec = mainObj["BurnTime"].toDouble();

      if (QString::compare(mPropellantMassSpecification, "Absolute") == 0)
      {
         mPropellantMass_lbs = mainObj["PropellantMass"].toDouble();
         if (QString::compare(mMassSpecification, "Empty") == 0)
         {
            mEmptyMass_lbs = mass;
            mTotalMass_lbs = mEmptyMass_lbs + mPropellantMass_lbs;
            mPropellantMassFraction = mPropellantMass_lbs / mTotalMass_lbs;
         }
         else // "Total"
         {
            mTotalMass_lbs = mass;
            mEmptyMass_lbs = mTotalMass_lbs - mPropellantMass_lbs;
            mPropellantMassFraction = mPropellantMass_lbs / mTotalMass_lbs;
         }

      }
      else // "Fraction"
      {
         mPropellantMassFraction = mainObj["PropellantMass"].toDouble();
         if (QString::compare(mMassSpecification, "Empty") == 0)
         {
            mEmptyMass_lbs = mass;
            mPropellantMass_lbs = mEmptyMass_lbs * mPropellantMassFraction / (1 - mPropellantMassFraction);
            mTotalMass_lbs = mEmptyMass_lbs + mPropellantMass_lbs;
         }
         else // "Total"
         {
            mTotalMass_lbs = mass;
            mPropellantMass_lbs = mTotalMass_lbs * mPropellantMassFraction;
            mEmptyMass_lbs = mTotalMass_lbs - mPropellantMass_lbs;
         }
      }
      // Calculate Thrust based on Burn Time and Propellant Mass
      CalcThrust();
   }

   mMass_lbs = mTotalMass_lbs;
   GenerateEngineData();
}

void Designer::AeroSolidRocketEngine::SaveToFile() const
{
   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentEngineFile();
   if (!curDoc)
   {
      QMessageBox::critical(nullptr, "Error", "No Model has been selected to be saved.  Please select a model or create a new one if one does not exist!");
      return;
   }
   QJsonObject mainObj = curDoc->object();
   QJsonObject plotsObj;

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

   QString specVars = mSpecifiedVariables;
   QStringList specVarsList = specVars.split(" and ");

   QString massSpec = mMassSpecification;
   QString altSpec = mAltitudeSpecification;
   QString propMassSpec = mPropellantMassSpecification;

   mainObj["SpecifiedCharacteristic1"] = specVarsList[0];
   mainObj["SpecifiedCharacteristic2"] = specVarsList[1];
   mainObj["MassType"] = massSpec;
   mainObj["AltitudeCondition"] = altSpec;

   mainObj["model_name"] = QString::fromStdString(mModelName);
   mainObj["Length"] = mLength_ft;
   mainObj["Diameter"] = mDiameter_ft;
   mainObj["ThrustOffset"] = mThrustOffset_ft;

   mainObj["SpecificImpulseSeaLevel"] = mSpecificImpulseSeaLevel_sec;
   mainObj["SpecificImpulseVacuum"] = mSpecificImpulseVacuum_sec;
   mainObj["RampUpTime"] = mRampUpTime_sec;
   mainObj["RampDownTime"] = mRampDownTime_sec;

   mainObj["RatedThrustInitial"] = mRampUpThrust_lbs;
   mainObj["RatedThrustFinal"] = mRampDownThrust_lbs;

   mainObj["BurnTime"] = mBurnTime_sec;

   if (QString::compare(propMassSpec, "Absolute") == 0)
   {
      mainObj["PropellantMass"] = mPropellantMass_lbs;
   }
   else // "Fraction"
   {
      mainObj["PropellantMass"] = mPropellantMassFraction;
   }
   mainObj["PropellantMassType"] = propMassSpec;

   if (QString::compare(massSpec, "Total") == 0)
   {
      mainObj["Mass"] = mTotalMass_lbs;
   }
   else
   {
      mainObj["Mass"] = mEmptyMass_lbs;
   }

   curDoc->setObject(mainObj);
   MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->SaveCurrentEngineFile();
}

void Designer::AeroSolidRocketEngine::CalcEngineData()
{
   AeroEngine::CalcEngineData();

   // For solid rockets, we need to update the Thrust vs Time table, separately.
   // A different method is used to update this table.
   AeroTable2d* thrustTable = mEngineTables.at(DataKey::cTHRUST_TIME);
   thrustTable->SetDirty();
   thrustTable->ClearBaseData();
   thrustTable->AddData(0, 0);
   thrustTable->AddData(mRampUpTime_sec, mRampUpThrust_lbs);
   thrustTable->AddData(mBurnTime_sec - mRampDownTime_sec, mRampDownThrust_lbs);
   thrustTable->AddData(mBurnTime_sec, 0);
   thrustTable->UpdateModifiedData();
}

void Designer::AeroSolidRocketEngine::GenerateEngineData()
{
   AeroEngine::GenerateEngineData();

   // For solid rockets, we need an additional table for Thrust vs Time,
   // because this plot is not in the amc file. A different method is used
   // to construct this table.
   AeroTable2d* thrustTable = new AeroTable2d(mPlotNameMap[DataKey::cTHRUST_TIME].xLabel,
                                              mPlotNameMap[DataKey::cTHRUST_TIME].yLabel);
   thrustTable->AddData(0, 0);
   thrustTable->AddData(mRampUpTime_sec, mRampUpThrust_lbs);
   thrustTable->AddData(mBurnTime_sec - mRampDownTime_sec, mRampDownThrust_lbs);
   thrustTable->AddData(mBurnTime_sec, 0);
   thrustTable->UpdateModifiedData();
   mEngineTables.emplace(DataKey::cTHRUST_TIME, thrustTable);
}

void Designer::AeroSolidRocketEngine::Export(const std::string& aFileName, ExportMoverType aExportMoverType) const
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
         stream << "   solid_propellant_rocket_type   " << QString::fromStdString(mModelName).toUpper() <<
                "   P6DOF_SOLID_PROPELLANT_ROCKET" << endl;
         stream << "      thrust_offset       ";
         OutputData(stream, mThrustOffset_ft, 12, 4, alignment);
         stream << " ft" << endl << endl;
      }
      else if (aExportMoverType == ExportMoverType::cPOINT_MASS)
      {
          stream << "six_dof_object_types" << endl << endl;
          stream << "   point_mass_engine_type   " << QString::fromStdString(mModelName).toUpper() << "   BASE_TYPE" << endl << endl;
          stream << "      //-------------------------------------------------------------------------" << endl << endl;
          stream << "      thrust_offset    " << mThrustOffset_ft << " ft" << endl << endl;
          stream << "      //-------------------------------------------------------------------------" << endl << endl;
          stream << "      solid_propellant_rocket" << endl;
      }
      else
      {
         stream << "six_dof_object_types" << endl << endl;
         stream << "   rigid_body_engine_type   " << QString::fromStdString(mModelName).toUpper() << "   BASE_TYPE" << endl << endl;
         stream << "      thrust_offset       ";
         OutputData(stream, mThrustOffset_ft, 12, 4, alignment);
         stream << " ft" << endl << endl;
         stream << "      solid_propellant_rocket" << endl;
      }
      stream << endl;

      stream << "      //-------------------------------------------------------------------------" << endl;
      stream << "      " << endl;

      if (QString::compare(mAltitudeSpecification, "Vacuum") == 0)
      {
         stream << "      thrust_vs_time_vacuum" << endl;
      }
      else
      {
         stream << "      thrust_vs_time_sealevel" << endl;
      }

      stream << "         // time    thrust_lbs" << endl;
      std::vector<std::pair<double, double>> thrustVsTime;
      GetEngineTableData(AeroEngine::DataKey::cTHRUST_TIME, thrustVsTime);
      for (const auto& i : thrustVsTime)
      {
         stream << "         ";
         OutputData(stream, i.first, 10, 3, alignment);
         stream << "    ";
         OutputData(stream, i.second, 12, 3, alignment);
         stream << endl;
      }

      if (QString::compare(mAltitudeSpecification, "Vacuum") == 0)
      {
         stream << "      end_thrust_vs_time_vacuum" << endl;
      }
      else
      {
         stream << "      end_thrust_vs_time_sealevel" << endl;
      }
      stream << "      " << endl;
      stream << "      //-------------------------------------------------------------------------" << endl;
      stream << "      " << endl;
      stream << "      isp_vs_alt" << endl;
      stream << "         // alt          Isp" << endl;

      std::vector<std::pair<double, double>> ispVsAlt;
      GetEngineTableData(AeroEngine::DataKey::cISP_ALT, ispVsAlt);
      for (const auto& i : ispVsAlt)
      {
         stream << "         ";
         OutputData(stream, i.first, 8, 1, alignment);
         stream << "   ";
         OutputData(stream, i.second, 13, 6, alignment);
         stream << endl;
      }

      stream << "      end_isp_vs_alt" << endl;
      stream << endl;

      double ratedThrust_lbs = std::max(mRampUpThrust_lbs, mRampDownThrust_lbs);

      stream << "      //-------------------------------------------------------------------------" << endl;
      stream << endl;
      stream << "      rated_thrust      ";
      OutputData(stream, ratedThrust_lbs, 10, 1, alignment);
      stream << " lbf" << endl;
      stream << endl;
      if (aExportMoverType == ExportMoverType::cP6DOF)
      {
         stream << "      //-------------------------------------------------------------------------" << endl;
         stream << endl;
         stream << "      propellant_mass   ";
         OutputData(stream, mPropellantMass_lbs, 10, 1, alignment);
         stream << " lbs" << endl;
         stream << endl;
         stream << "   end_solid_propellant_rocket_type" << endl;
         stream << endl;
         stream << "end_p6dof_object_types" << endl;
      }
      else if (aExportMoverType == ExportMoverType::cPOINT_MASS)
      {
          stream << "      end_solid_propellant_rocket" << endl << endl;
          stream << "   end_point_mass_engine_type" << endl << endl;
          stream << "end_six_dof_object_types" << endl;
      }
      else
      {
         stream << "      end_solid_propellant_rocket" << endl << endl;
         stream << "   end_rigid_body_engine_type" << endl << endl;
         stream << "end_six_dof_object_types" << endl;
      }
      stream << endl;

      file.close();
   }
}

Designer::AeroEngine::PropulsionParameters Designer::AeroSolidRocketEngine::GetPropulsionData(double aMaxMach) const
{
   PropulsionParameters params;

   params.mIsp_sea_level = mSpecificImpulseSeaLevel_sec;
   params.mIsp_vacuum = mSpecificImpulseVacuum_sec;

   double ratio = params.mIsp_sea_level / params.mIsp_vacuum;

   GetEngineTableData(AeroEngine::DataKey::cTHRUST_TIME, params.mThrustVsTime);
   if (mAltitudeSpecification == "Vacuum")
   {
      for (auto& thrust : params.mThrustVsTime)
      {
         thrust.second *= ratio;
      }
   }

   params.mThrottleVsMach.emplace_back(0, 1.0);
   params.mThrottleVsMach.emplace_back(aMaxMach, 1.0);

   params.mFuel_mass = mPropellantMass_lbs;

   params.mMass_flow_rate = 3600 * mPropellantMass_lbs / mBurnTime_sec;

   return params;
}

std::vector<double> Designer::AeroSolidRocketEngine::GetEngineSpecs() const
{
   std::vector<double> specs = AeroEngine::GetEngineSpecs();

   std::vector<double> solidRocketSpecs = std::vector<double>({ mSpecificImpulseSeaLevel_sec,
                                                                mSpecificImpulseVacuum_sec,
                                                                mRampUpTime_sec,
                                                                mRampDownTime_sec,
                                                                mRampUpThrust_lbs,
                                                                mRampDownThrust_lbs,
                                                                mBurnTime_sec,
                                                                mPropellantMass_lbs,
                                                                mPropellantMassFraction,
                                                                mEmptyMass_lbs,
                                                                mTotalMass_lbs
                                                              });


   specs.insert(specs.end(), solidRocketSpecs.begin(), solidRocketSpecs.end());

   return specs;
}

void Designer::AeroSolidRocketEngine::SetEngineSpecs(const std::vector<double>& aEngineSpecs)
{
   AeroEngine::SetEngineSpecs(aEngineSpecs);

   mSpecificImpulseSeaLevel_sec = aEngineSpecs[mNumBaseSpecs + 0];
   mSpecificImpulseVacuum_sec   = aEngineSpecs[mNumBaseSpecs + 1];
   mRampUpTime_sec              = aEngineSpecs[mNumBaseSpecs + 2];
   mRampDownTime_sec            = aEngineSpecs[mNumBaseSpecs + 3];
   mRampUpThrust_lbs            = aEngineSpecs[mNumBaseSpecs + 4];
   mRampDownThrust_lbs          = aEngineSpecs[mNumBaseSpecs + 5];
   mBurnTime_sec                = aEngineSpecs[mNumBaseSpecs + 6];
   mPropellantMass_lbs          = aEngineSpecs[mNumBaseSpecs + 7];
   mPropellantMassFraction      = aEngineSpecs[mNumBaseSpecs + 8];
   mEmptyMass_lbs               = aEngineSpecs[mNumBaseSpecs + 9];
   mTotalMass_lbs               = aEngineSpecs[mNumBaseSpecs + 10];

   mEngineDataMultiplier[DataKey::cISP_ALT] = mSpecificImpulseVacuum_sec - mSpecificImpulseSeaLevel_sec;
   mEngineDataOffset[DataKey::cISP_ALT]     = mSpecificImpulseSeaLevel_sec;

   if (QString::compare(mSpecifiedVariables, "Thrust and Burn Time") == 0)
   {
      CalcPropellantMass();
      CalcPropellantMassFraction();
   }
   else
   {
      if (QString::compare(mSpecifiedVariables, "Thrust and Propellant Mass") == 0)
      {
         CalcBurnTime();
      }
      else if (QString::compare(mSpecifiedVariables, "Burn Time and Propellant Mass") == 0)
      {
         CalcThrust();
      }
   }
}

std::vector<QString> Designer::AeroSolidRocketEngine::GetEngineSpecsOptions() const
{
   std::vector<QString> specsOptions = AeroEngine::GetEngineSpecsOptions();

   std::vector<QString> solidRocketSpecsOptions = std::vector<QString>({ mSpecifiedVariables,
                                                                         mMassSpecification,
                                                                         mAltitudeSpecification,
                                                                         mPropellantMassSpecification
                                                                       });


   specsOptions.insert(specsOptions.end(), solidRocketSpecsOptions.begin(), solidRocketSpecsOptions.end());

   return specsOptions;
}

void Designer::AeroSolidRocketEngine::SetEngineSpecsOptions(const std::vector<QString>& aEngineSpecsOptions)
{
   mSpecifiedVariables          = aEngineSpecsOptions[mNumBaseSpecsOptions + 0];
   mMassSpecification           = aEngineSpecsOptions[mNumBaseSpecsOptions + 1];
   mAltitudeSpecification       = aEngineSpecsOptions[mNumBaseSpecsOptions + 2];
   mPropellantMassSpecification = aEngineSpecsOptions[mNumBaseSpecsOptions + 3];

   if (QString::compare(mSpecifiedVariables, "Thrust and Burn Time") == 0)
   {
      CalcPropellantMass();
      CalcPropellantMassFraction();
   }
   else
   {
      if (QString::compare(mSpecifiedVariables, "Thrust and Propellant Mass") == 0)
      {
         CalcBurnTime();
      }
      else if (QString::compare(mSpecifiedVariables, "Burn Time and Propellant Mass") == 0)
      {
         CalcThrust();
      }
   }
}

double Designer::AeroSolidRocketEngine::GetSpecificImpulse(double aAltitude_ft) const
{
   std::vector<std::pair<double, double>> thrustVec;
   GetEngineTableData(AeroEngine::DataKey::cISP_ALT, thrustVec);
   std::map<double, double> thrustMap(thrustVec.begin(), thrustVec.end());

   return util::LookupValue(thrustMap, aAltitude_ft, util::ExtrapolationMethod::cCLAMP, util::ExtrapolationMethod::cCLAMP);
}

// Calculate Thrust based on Burn Time and Propellant Mass
void Designer::AeroSolidRocketEngine::CalcThrust()
{
   // Find the propellant mass that is currently valid, and calculate the other variant
   if (QString::compare(mPropellantMassSpecification, "Absolute") == 0)
   {
      mPropellantMassFraction = mPropellantMass_lbs / mTotalMass_lbs;
   }
   else // "Fraction"
   {
      mPropellantMass_lbs = mPropellantMassFraction * mTotalMass_lbs;
   }

   double isp = mSpecificImpulseSeaLevel_sec;
   if (QString::compare(mAltitudeSpecification, "Vacuum") == 0)
   {
      isp = mSpecificImpulseVacuum_sec;
   }

   double massFlow     = mPropellantMass_lbs / (mBurnTime_sec - 0.5 * mRampUpTime_sec - 0.5 * mRampDownTime_sec);
   mRampUpThrust_lbs   = massFlow * isp;
   mRampDownThrust_lbs = mRampUpThrust_lbs;
}

// Calculate Burn Time based on Thrust and Propellant Mass
void Designer::AeroSolidRocketEngine::CalcBurnTime()
{
   // Find the propellant mass that is currently valid, and calculate the other variant
   if (QString::compare(mPropellantMassSpecification, "Absolute") == 0)
   {
      mPropellantMassFraction = mPropellantMass_lbs / mTotalMass_lbs;
   }
   else // "Fraction"
   {
      mPropellantMass_lbs = mPropellantMassFraction * mTotalMass_lbs;
   }

   double isp = mSpecificImpulseSeaLevel_sec;
   if (QString::compare(mAltitudeSpecification, "Vacuum") == 0)
   {
      isp = mSpecificImpulseVacuum_sec;
   }

   double massFlowRampUp   = mRampUpThrust_lbs / isp;
   double massFlowRampDown = mRampDownThrust_lbs / isp;

   mBurnTime_sec = (mPropellantMass_lbs + 0.5 * (massFlowRampUp * mRampDownTime_sec + massFlowRampDown *
                                                 mRampUpTime_sec)) / (0.5 * (massFlowRampUp + massFlowRampDown));
}

// Calculate Propellant Mass based on Burn Time and Thrust
void Designer::AeroSolidRocketEngine::CalcPropellantMass()
{
   double isp = mSpecificImpulseSeaLevel_sec;
   if (QString::compare(mAltitudeSpecification, "Vacuum") == 0)
   {
      isp = mSpecificImpulseVacuum_sec;
   }

   double massFlowRampUp = mRampUpThrust_lbs / isp;
   double massFlowRampDown = mRampDownThrust_lbs / isp;

   mPropellantMass_lbs = 0.5 * ((massFlowRampUp + massFlowRampDown) *
                                mBurnTime_sec - (massFlowRampUp * mRampDownTime_sec + massFlowRampDown * mRampUpTime_sec));

   // Find the mass that is currently valid, and calculate the other variant
   if (QString::compare(mMassSpecification, "Empty") == 0)
   {
      mTotalMass_lbs = mEmptyMass_lbs + mPropellantMass_lbs;
   }
   else // "Total"
   {
      mEmptyMass_lbs = mTotalMass_lbs - mPropellantMass_lbs;
   }
}

// Calculate Propellant Mass Fraction based on Propellant Mass and Total Mass
void Designer::AeroSolidRocketEngine::CalcPropellantMassFraction()
{
   mPropellantMassFraction = mPropellantMass_lbs / mTotalMass_lbs;
}
