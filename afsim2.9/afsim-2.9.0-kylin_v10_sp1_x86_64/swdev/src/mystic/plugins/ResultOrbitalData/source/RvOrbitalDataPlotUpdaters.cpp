//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#include "RvOrbitalDataPlotUpdaters.hpp"

#include "RvEnvironment.hpp"
#include "RvEventPipeClasses.hpp"
#include "RvOrbitalDataInterface.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "UtUnitTypes.hpp"
#include "WkfEnvironment.hpp"
#include "WkfPlatformGrouping.hpp"
#include "WkfUnitsObject.hpp"

RvOrbitalData::PlotUpdater::PlotUpdater(const QString& aX, const QString& aY, const wkf::PlatformGrouping& aGroup)
   : rv::PlotUpdater()
   , mInitialXAxis(aX)
   , mInitialYAxis(aY)
{
   if (aGroup.Type() == wkf::PlatformGrouping::eIndividual)
   {
      mInitialSelection.push_back(aGroup.Name().c_str());
   }
   else
   {
      // build the selection list
      rv::ResultData* dataPtr = rvEnv.GetData();
      if (dataPtr)
      {
         rv::ResultDb* dbPtr = dataPtr->GetDb();
         if (dbPtr)
         {
            const auto& catmap = wkfEnv.GetGroupings();
            auto        it     = catmap.find(aGroup);
            if (it != catmap.end())
            {
               for (auto&& jt : it->second)
               {
                  mInitialSelection.push_back(dbPtr->GetPlatformName(jt).c_str());
               }
            }
         }
      }
   }
}

QStringList RvOrbitalData::PlotUpdater::GetPlatforms() const
{
   QStringList     list;
   rv::ResultData* dataPtr = rvEnv.GetData();

   // initialize the plot window's information
   if (dataPtr)
   {
      rv::ResultDb* dbPtr = dataPtr->GetDb();
      if (dbPtr)
      {
         for (unsigned int i = 0; i < dbPtr->GetPlatformCount(); i++)
         {
            list.push_back(dbPtr->GetPlatformName(i).c_str());
         }
      }
   }
   return list;
}

/** Helper function to get the available axes. */
QStringList RvOrbitalData::PlotUpdater::GetAxes() const
{
   QStringList axisVariableList;
   axisVariableList.push_back("Time");
   axisVariableList.push_back("Eccentricity");
   axisVariableList.push_back("Semi-major Axis");
   axisVariableList.push_back("Inclination");
   axisVariableList.push_back("RAAN");
   axisVariableList.push_back("Argument of Periapsis");
   axisVariableList.push_back("True Anomaly");
   return axisVariableList;
}

QString RvOrbitalData::PlotUpdater::GetUnits(const QString& aAxis) const
{
   if (aAxis == "Time")
   {
      return WKF_UNIT_NAME(Time);
   }
   else if (aAxis == "Semi-major Axis")
   {
      return WKF_UNIT_NAME(Length);
   }
   else if (aAxis == "Inclination" || aAxis == "RAAN" || aAxis == "Argument of Periapsis" || aAxis == "True Anomaly")
   {
      return WKF_UNIT_NAME(Angle);
   }
   // Eccentricity is unitless.
   return {};
}

/** Get a series(plot) of X vs. Y. */
std::vector<std::pair<double, double>> RvOrbitalData::PlotUpdater::GetSeries(const QString& aItem,
                                                                             const QString& aXAxis,
                                                                             const QString& aYAxis) const
{
   std::vector<std::pair<double, double>> series;

   rv::ResultData* results = rvEnv.GetData();
   if (results)
   {
      rv::ResultDb* db = results->GetDb();
      if (db)
      {
         rv::ResultPlatform* platformPtr = db->FindPlatformByName(aItem);
         if (platformPtr)
         {
            const auto dataArray = platformPtr->GetArray<rv::MsgEntityState>();
            if (dataArray)
            {
               for (auto&& kt : *dataArray)
               {
                  if ((kt->simTime() - mLastUpdate) > GetSampleRate())
                  {
                     auto msg = platformPtr->FindFirstBefore<rv::MsgEntityState>(kt->simTime());
                     series.emplace_back(GetOrbitalData(aXAxis, msg), GetOrbitalData(aYAxis, msg));
                     mLastUpdate = kt->simTime();
                  }
               }
            }
         }
      }
   }
   return series;
}

double RvOrbitalData::PlotUpdater::GetOrbitalData(const QString& aDesc, const rv::MsgEntityState* aState) const
{
   if (aDesc == "Time")
   {
      UtUnitTime::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetTimeUnit();
      return UtUnitTime::ConvertFromStandard(aState->simTime(), u);
   }

   // If axis is not time, need to generate orbital data
   auto data = std::move(RvOrbitalData::Interface::GenerateOrbitalData(aState->state(), aState->simTime()));
   if (aDesc == "Eccentricity")
   {
      return data.mEccentricity;
   }
   else if (aDesc == "Semi-major Axis")
   {
      UtUnitLength::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLengthUnit();
      return UtUnitLength::ConvertFromStandard(data.mSemiMajorAxis, u);
   }
   else if (aDesc == "Inclination")
   {
      UtUnitAngle::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit();
      return UtUnitAngle::ConvertFromStandard(data.mInclination, u);
   }
   else if (aDesc == "RAAN")
   {
      UtUnitAngle::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit();
      return UtUnitAngle::ConvertFromStandard(data.mRAAN, u);
   }
   else if (aDesc == "Argument of Periapsis")
   {
      UtUnitAngle::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit();
      return UtUnitAngle::ConvertFromStandard(data.mArgOfPeriapsis, u);
   }
   else if (aDesc == "True Anomaly")
   {
      UtUnitAngle::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit();
      return UtUnitAngle::ConvertFromStandard(data.mTrueAnomaly, u);
   }
   return 0;
}
