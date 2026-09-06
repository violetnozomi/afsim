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

#include "RvScriptDataUpdater.hpp"

#include "RvEnvironment.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"

RvScriptDataFeed::Updater::Updater(const QString& aPlatformName, const QString& aDatum)
   : wkf::UnitlessValueUpdater(aDatum, aPlatformName.toStdString())
{
}

QString RvScriptDataFeed::Updater::GetValueString() const
{
   rv::ResultData* results = rvEnv.GetData();
   if (nullptr != results)
   {
      rv::ResultDb* db = results->GetDb();
      if (nullptr != db)
      {
         rv::ResultPlatform* platformPtr = db->FindPlatformByName(GetPlatformOfInterest());
         if (platformPtr)
         {
            std::string name = GetUpdaterName().toStdString();
            auto        msg  = platformPtr->FindFirstBefore<rv::MsgScriptData>(results->GetSimTime(), name);
            if (msg)
            {
               switch (msg->value().GetFieldType())
               {
               case (rv::MsgScriptData_Value::cFT_floating):
                  return QString::number(msg->value().floating());
               case (rv::MsgScriptData_Value::cFT_integer):
                  return QString::number(msg->value().integer());
               case (rv::MsgScriptData_Value::cFT_boolean):
                  return msg->value().boolean() ? "true" : "false";
               case (rv::MsgScriptData_Value::cFT_text):
                  return QString::fromStdString(msg->value().text());
               case (rv::MsgScriptData_Value::cFT_NONE):
               default:
                  return "";
               }
            }
         }
      }
   }
   return "";
}
