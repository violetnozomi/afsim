// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "RvAuxDataPlotUpdater.hpp"

#include <cstdlib>

#include "RvEnvironment.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "WkfPlatformGrouping.hpp"
#include "WkfUnitsObject.hpp"

RvAuxData::PlotUpdater::PlotUpdater(const QString& aX, const QString& aY, const wkf::PlatformGrouping& aGroup)
   : mInitialXAxis(aX)
   , mInitialYAxis(aY)
{
   if (aGroup.Type() == wkf::PlatformGrouping::eIndividual)
   {
      mInitialSelection.push_back(QString::fromStdString(aGroup.Name()));
   }
   else
   {
      // build the selection list
      rv::ResultData* dataPtr = rvEnv.GetData();
      if (dataPtr != nullptr)
      {
         rv::ResultDb* dbPtr = dataPtr->GetDb();
         if (dbPtr != nullptr)
         {
            const std::map<wkf::PlatformGrouping, std::set<int>>& catmap = wkfEnv.GetGroupings();
            auto                                                  it     = catmap.find(aGroup);
            if (it != catmap.end())
            {
               for (auto&& jt : it->second)
               {
                  mInitialSelection.push_back(QString::fromStdString(dbPtr->GetPlatformName(jt)));
               }
            }
         }
      }
   }
}

QStringList RvAuxData::PlotUpdater::GetPlatforms() const
{
   QStringList     list;
   rv::ResultData* dataPtr = rvEnv.GetData();

   // initialize the plot window's information
   if (dataPtr != nullptr)
   {
      rv::ResultDb* db = dataPtr->GetDb();
      if (db != nullptr)
      {
         for (unsigned int i = 0; i < db->GetPlatformCount(); i++)
         {
            list.push_back(QString::fromStdString(db->GetPlatformName(i)));
         }
      }
   }
   return list;
}

QStringList RvAuxData::PlotUpdater::GetAxes() const
{
   QStringList axisVariableList;
   axisVariableList.push_back("Time");

   rv::ResultData* data = rvEnv.GetData();

   if (data != nullptr)
   {
      rv::ResultDb* db = data->GetDb();
      if (db != nullptr)
      {
         for (unsigned int i = 0; i < db->GetPlatformCount(); i++)
         {
            rv::ResultPlatform* platform = db->FindPlatform(i);
            if (platform)
            {
               DiscoverAuxDataKeys(*platform, axisVariableList);
            }
         }
      }
   }

   return axisVariableList;
}

const QStringList& RvAuxData::PlotUpdater::GetInitialSelection() const
{
   return mInitialSelection;
}

const QString& RvAuxData::PlotUpdater::GetInitialXAxis() const
{
   return mInitialXAxis;
}

const QString& RvAuxData::PlotUpdater::GetInitialYAxis() const
{
   return mInitialYAxis;
}

std::vector<std::pair<double, double>> RvAuxData::PlotUpdater::GetSeries(const QString& aItem,
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
            const auto msgArray = platformPtr->GetArray<rv::MsgAuxData>();
            if (msgArray)
            {
               for (const auto& msg : *msgArray)
               {
                  const auto x = GetData(aXAxis, static_cast<rv::MsgAuxData*>(msg));
                  const auto y = GetData(aYAxis, static_cast<rv::MsgAuxData*>(msg));

                  if (x && y)
                  {
                     series.emplace_back(*x, *y);
                  }

                  mLastUpdate = msg->simTime();
               }
            }
         }
      }
   }
   return series;
}

void RvAuxData::PlotUpdater::DiscoverAuxDataKeys(rv::ResultPlatform& aPlatform, QStringList& aKeyList) const
{
   const auto msgArray = aPlatform.GetArray<rv::MsgAuxData>();
   if (msgArray)
   {
      for (const auto* msg : *msgArray)
      {
         auto ptr = static_cast<const rv::MsgAuxData*>(msg);
         for (const auto& value : ptr->auxData())
         {
            auto name    = QString::fromStdString(value.name());
            auto type    = value.type();
            bool numeric = (type == rv::AuxDataType::Integer) || (type == rv::AuxDataType::Real);
            if (!aKeyList.contains(name) && numeric)
            {
               aKeyList.append(name);
            }
         }
      }
   }
}

ut::optional<double> RvAuxData::PlotUpdater::GetData(const QString& aDesc, const rv::MsgAuxData* aState) const
{
   if (aDesc == "Time")
   {
      UtUnitTime::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetTimeUnit();
      return UtUnitTime::ConvertFromStandard(aState->simTime(), u);
   }

   for (const auto& value : aState->auxData())
   {
      if (value.name() == aDesc.toStdString())
      {
         if (value.integerValid())
         {
            return value.integer();
         }
         else if (value.realValid())
         {
            return value.real();
         }
         return ut::nullopt;
      }
   }

   return ut::nullopt;
}
