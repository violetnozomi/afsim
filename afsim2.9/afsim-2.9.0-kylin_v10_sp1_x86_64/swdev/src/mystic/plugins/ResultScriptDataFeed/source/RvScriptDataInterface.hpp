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
#ifndef RVSCRIPTDATAINTERFACE_HPP
#define RVSCRIPTDATAINTERFACE_HPP

#include <set>

#include <QMutex>
#include <QObject>
#include <RvEventPipeClasses.hpp>

namespace rv
{
class ResultData;
}

namespace RvScriptDataFeed
{
class Interface : public QObject
{
   Q_OBJECT

public:
   Interface(QObject* parent);
   ~Interface() override = default;

   void SetPlatformOfInterest(const std::string& aPlatformName);

   void AdvanceTimeRead(const rv::ResultData& aData);

   using PlatformData = std::map<std::string, rv::MsgScriptData_Value>;

   const PlatformData& GetPlatformData() const;

   bool CheckReset();

private:
   enum
   {
      LOOKUP_REQUIRED = 0,
      SCENARIO        = -1
   };
   mutable QMutex mMutex;
   std::string    mName;
   PlatformData   mPlatformData;
   int            mIndex;
   // reset lets the using class know that the platform has changed, and old data is out of date
   bool mReset;
};
} // namespace RvScriptDataFeed
#endif // RVPLATFORMDATAINTERFACE_HPP
