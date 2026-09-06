// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "RelativeGeometryPlotUpdater.hpp"

#include "RvEnvironment.hpp"
#include "RvEventPipeUtil.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "UtRelativeGeometry.hpp"
#include "UtUnitTypes.hpp"
#include "WkfEnvironment.hpp"
#include "WkfUnitsObject.hpp"

RelativeGeometry::PlotUpdater::PlotUpdater(const QString& aX_Axis,
                                           const QString& aY_Axis,
                                           const QString& aFromPlatform,
                                           const QString& aToPlatform)
   : rv::PlotUpdater()
{
   mFromPlatformName = aFromPlatform;
   mInitialX_Axis    = aX_Axis;
   mInitialY_Axis    = aY_Axis;
   mInitialSelection.push_back(aToPlatform);
   // build up mInitialSelection from aGrouping
}

QStringList RelativeGeometry::PlotUpdater::GetPlatforms() const
{
   return mInitialSelection;
}

const QStringList& RelativeGeometry::PlotUpdater::GetInitialSelection() const
{
   return mInitialSelection;
}

const QString& RelativeGeometry::PlotUpdater::GetInitialXAxis() const
{
   return mInitialX_Axis;
}

const QString& RelativeGeometry::PlotUpdater::GetInitialYAxis() const
{
   return mInitialY_Axis;
}

std::vector<std::pair<double, double>> RelativeGeometry::PlotUpdater::GetSeries(const QString& aItem,
                                                                                const QString& aXAxis,
                                                                                const QString& aYAxis) const
{
   std::vector<std::pair<double, double>> series;

   rv::ResultData* results = rvEnv.GetData();
   if (nullptr != results)
   {
      rv::ResultDb* db = results->GetDb();
      if (nullptr != db)
      {
         rv::ResultPlatform* fromPtr = db->FindPlatformByName(mFromPlatformName);
         rv::ResultPlatform* toPtr   = db->FindPlatformByName(aItem);
         if (fromPtr && toPtr)
         {
            UtEntity from;
            UtEntity to;
            auto     stateArrayF = fromPtr->GetArray<rv::MsgEntityState>();
            auto     stateArrayT = toPtr->GetArray<rv::MsgEntityState>();
            if ((stateArrayF != nullptr) && (stateArrayT != nullptr))
            {
               auto fit = stateArrayF->begin();
               auto tit = stateArrayT->begin();
               // first, advance things so that both platforms exist
               if ((*fit)->simTime() > (*tit)->simTime())
               {
                  while ((*fit)->simTime() > (*tit)->simTime())
                  {
                     ++tit;
                  }
               }
               else if ((*tit)->simTime() > (*fit)->simTime())
               {
                  while ((*tit)->simTime() > (*fit)->simTime())
                  {
                     ++fit;
                  }
               }
               double simTime = 0.0;
               while ((fit != stateArrayF->end()) && (tit != stateArrayT->end()))
               {
                  auto fromStatePtr = dynamic_cast<rv::MsgEntityState*>(*fit);
                  auto toStatePtr   = dynamic_cast<rv::MsgEntityState*>(*tit);

                  if (fromStatePtr->simTime() < toStatePtr->simTime())
                  {
                     simTime = fromStatePtr->simTime();
                     if ((simTime - GetLastUpdate()) > GetSampleRate())
                     {
                        rv::BuildUtEntity(simTime, *fromStatePtr, from);
                        rv::BuildUtEntity(simTime, *toStatePtr, to);
                        double x = GetData(aXAxis, simTime, from, to);
                        double y = GetData(aYAxis, simTime, from, to);
                        series.emplace_back(x, y);

                        mLastUpdate = simTime;
                     }
                     ++fit;
                  }
                  else
                  {
                     simTime = toStatePtr->simTime();
                     if ((simTime - GetLastUpdate()) > GetSampleRate())
                     {
                        rv::BuildUtEntity(simTime, *fromStatePtr, from);
                        rv::BuildUtEntity(simTime, *toStatePtr, to);
                        double x = GetData(aXAxis, simTime, from, to);
                        double y = GetData(aYAxis, simTime, from, to);
                        series.emplace_back(x, y);

                        mLastUpdate = simTime;
                     }
                     ++tit;
                  }
               }
            }
         }
      }
   }
   return series;
}

double RelativeGeometry::PlotUpdater::GetData(const QString&  aDatum,
                                              double          aSimTime,
                                              const UtEntity& aFrom,
                                              const UtEntity& aTo) const
{
   if (aDatum == "Time")
   {
      UtUnitTime::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetTimeUnit();
      return UtUnitTime::ConvertFromStandard(aSimTime, u);
   }
   else if (aDatum == "Range (linear)")
   {
      UtUnitLength::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLengthUnit();
      return UtUnitLength::ConvertFromStandard(UtRelativeGeometry::CalculateLinearRange(aFrom, aTo), u);
   }
   else if (aDatum == "Radial")
   {
      UtUnitLength::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLengthUnit();
      return UtUnitLength::ConvertFromStandard(UtRelativeGeometry::CalculateRadial(aFrom, aTo), u);
   }
   else if (aDatum == "In-Track")
   {
      UtUnitLength::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLengthUnit();
      return UtUnitLength::ConvertFromStandard(UtRelativeGeometry::CalculateInTrack(aFrom, aTo), u);
   }
   else if (aDatum == "Cross-Track")
   {
      UtUnitLength::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLengthUnit();
      return UtUnitLength::ConvertFromStandard(UtRelativeGeometry::CalculateCrossTrack(aFrom, aTo), u);
   }
   else if (aDatum == "Bearing")
   {
      UtUnitAngle::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit();
      return UtUnitAngle::ConvertFromStandard(UtRelativeGeometry::CalculateBearing(aFrom, aTo), u);
   }
   else if (aDatum == "Elevation")
   {
      UtUnitAngle::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit();
      return UtUnitAngle::ConvertFromStandard(UtRelativeGeometry::CalculateElevation(aFrom, aTo), u);
   }
   else if (aDatum == "Down Range")
   {
      UtUnitLength::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLengthUnit();
      return UtUnitLength::ConvertFromStandard(UtRelativeGeometry::CalculateDownRange(aFrom, aTo), u);
   }
   else if (aDatum == "Cross Range")
   {
      UtUnitLength::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLengthUnit();
      return UtUnitLength::ConvertFromStandard(UtRelativeGeometry::CalculateCrossRange(aFrom, aTo), u);
   }
   else if (aDatum == "Range Rate")
   {
      UtUnitSpeed::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetSpeedUnit();
      return UtUnitSpeed::ConvertFromStandard(UtRelativeGeometry::CalculateRangeRate(aFrom, aTo), u);
   }
   return 1.0;
}

QStringList RelativeGeometry::PlotUpdater::GetAxes() const
{
   QStringList axisVariableList;
   axisVariableList.push_back("Time");
   axisVariableList.push_back("Range (linear)");
   axisVariableList.push_back("Radial");
   axisVariableList.push_back("In-Track");
   axisVariableList.push_back("Cross-Track");
   axisVariableList.push_back("Bearing");
   axisVariableList.push_back("Elevation");
   axisVariableList.push_back("Down Range");
   axisVariableList.push_back("Cross Range");
   axisVariableList.push_back("Range Rate");
   return axisVariableList;
}

QString RelativeGeometry::PlotUpdater::GetUnits(const QString& aAxis) const
{
   if (aAxis == "Time")
   {
      return WKF_UNIT_NAME(Time);
   }
   else if (QStringList{"Range (linear)", "Radial", "In-Track", "Cross-Track", "Down Range", "Cross Range"}.contains(aAxis))
   {
      return WKF_UNIT_NAME(Length);
   }
   else if (aAxis == "Bearing" || aAxis == "Elevation")
   {
      return WKF_UNIT_NAME(Angle);
   }
   else if (aAxis == "Range Rate")
   {
      return WKF_UNIT_NAME(Speed);
   }
   return {};
}
