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
#include "RvAuxDataUpdater.hpp"

#include "RvEventPipeUtil.hpp"
#include "RvResultPlatform.hpp"
#include "UtAirspeed.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtLatPos.hpp"
#include "UtLonPos.hpp"
#include "UtMath.hpp"
#include "WkfEnvironment.hpp"
#include "WkfUnitsObject.hpp"

RvAuxData::Updater::Updater(const std::string& aPlatformName, const std::string& aDatumName)
   : rv::PlatformUpdater(cUPDATER_NAME, aPlatformName)
   , mDatumName(QString::fromStdString(aDatumName))
{
}

QString RvAuxData::Updater::GetValueString() const
{
   return mValue.toString();
}

const QString RvAuxData::Updater::GetDisplayName() const
{
   return mDatumName;
}

bool RvAuxData::Updater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const auto msgPtr = aPlatform->FindFirstBefore<rv::MsgAuxData>(aSimTime);
   if (msgPtr != nullptr)
   {
      for (const auto& data : msgPtr->auxData())
      {
         if (QString::fromStdString(data.name()) == mDatumName)
         {
            switch (data.type())
            {
            case rv::AuxDataType::Boolean:
               mValue = data.boolean();
               break;
            case rv::AuxDataType::Integer:
               mValue = data.integer();
               break;
            case rv::AuxDataType::Real:
               mValue = data.real();
               break;
            case rv::AuxDataType::Text:
               mValue = QString::fromStdString(data.text());
               break;
            default:
               mValue = "(unknown type)";
            }
         }
      }
   }
   return msgPtr != nullptr;
}
