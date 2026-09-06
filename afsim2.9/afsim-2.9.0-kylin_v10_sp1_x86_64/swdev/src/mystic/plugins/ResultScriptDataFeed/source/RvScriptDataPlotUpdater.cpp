// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "RvScriptDataPlotUpdater.hpp"

#include "RvEnvironment.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "WkfUnitsObject.hpp"

RvScriptDataFeed::PlotUpdater::PlotUpdater(const QString& aX, const QString& aY, const wkf::PlatformGrouping& aGroup)
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
                  mInitialSelection.push_back(dbPtr->GetPlatformName(jt).c_str());
               }
            }
         }
      }
   }
}

QStringList RvScriptDataFeed::PlotUpdater::GetPlatforms() const
{
   QStringList     list;
   rv::ResultData* dataPtr = rvEnv.GetData();

   // initialize the plot window's information
   if (dataPtr != nullptr)
   {
      rv::ResultDb* dbPtr = dataPtr->GetDb();
      if (dbPtr != nullptr)
      {
         for (unsigned int i = 0; i < dbPtr->GetPlatformCount(); i++)
         {
            list.push_back(dbPtr->GetPlatformName(i).c_str());
         }
      }
   }
   return list;
}

QStringList RvScriptDataFeed::PlotUpdater::GetAxesX() const
{
   QStringList axisVariableList;
   axisVariableList.push_back("Time");
   return axisVariableList;
}

QStringList RvScriptDataFeed::PlotUpdater::GetAxesY() const
{
   QStringList       axisVariableList;
   std::set<QString> nameset;

   rv::ResultData* results = rvEnv.GetData();
   if (nullptr != results)
   {
      rv::ResultDb* db = results->GetDb();
      if (nullptr != db)
      {
         for (auto&& it : mInitialSelection)
         {
            rv::ResultPlatform* platformPtr = db->FindPlatformByName(it);
            if (platformPtr)
            {
               auto scriptDataMap = platformPtr->GetStringKeyedMap<rv::MsgScriptData>();
               if (scriptDataMap)
               {
                  for (auto& series : *scriptDataMap)
                  {
                     nameset.insert(QString::fromStdString(series.first));
                  }
               }
            }
         }
      }
   }
   for (auto&& it : nameset)
   {
      axisVariableList.push_back(it);
   }
   return axisVariableList;
}

QString RvScriptDataFeed::PlotUpdater::GetUnitsX(const QString& aAxis) const
{
   return "seconds";
}

QString RvScriptDataFeed::PlotUpdater::GetUnitsY(const QString& aAxis) const
{
   // all Y-axis values are unitless
   return {};
}

const QStringList& RvScriptDataFeed::PlotUpdater::GetInitialSelection() const
{
   return mInitialSelection;
}

const QString& RvScriptDataFeed::PlotUpdater::GetInitialXAxis() const
{
   return mInitialXAxis;
}

const QString& RvScriptDataFeed::PlotUpdater::GetInitialYAxis() const
{
   return mInitialYAxis;
}

std::vector<std::pair<double, double>> RvScriptDataFeed::PlotUpdater::GetSeries(const QString& aItem,
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
         std::string name = aItem.toStdString();

         rv::ResultPlatform* platformPtr = db->FindPlatformByName(name);
         if (platformPtr)
         {
            auto valueArray = platformPtr->GetArray<rv::MsgScriptData>(aYAxis.toStdString());
            if (valueArray)
            {
               for (auto&& kt : *valueArray)
               {
                  rv::MsgScriptData* ds = static_cast<rv::MsgScriptData*>(kt);
                  if ((ds->simTime() - GetLastUpdate()) > GetSampleRate())
                  {
                     if (ds->value().GetFieldType() == rv::MsgScriptData_Value::cFT_floating)
                     {
                        double x = ds->simTime();
                        series.emplace_back(x, ds->value().floating());
                        mLastUpdate = ds->simTime();
                     }
                     else if (ds->value().GetFieldType() == rv::MsgScriptData_Value::cFT_integer)
                     {
                        double x = ds->simTime();
                        series.emplace_back(x, ds->value().integer());
                        mLastUpdate = ds->simTime();
                     }
                  }
               }
            }
         }
      }
   }
   return series;
}
