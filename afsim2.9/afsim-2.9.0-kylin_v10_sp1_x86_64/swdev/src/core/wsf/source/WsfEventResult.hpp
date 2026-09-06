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

#ifndef WSFEVENTRESULT_HPP
#define WSFEVENTRESULT_HPP


#include "wsf_export.h"

#include <functional>
#include <map>
#include <ostream>
#include <string.h>

#include "UtAngle.hpp"
#include "UtTime.hpp"

class WsfMessage;
#include "WsfStringId.hpp"

namespace wsf
{
namespace event
{

class WSF_EXPORT Settings
{
public:
   using MessagePrintFunction = std::function<void(double, const WsfMessage&)>;
   using MessagePrintMap      = std::map<WsfStringId, MessagePrintFunction>;

#undef GetTimeFormat // Avoid conflict with Windows macro
   int  GetTimeFormat() const { return mTimeFormat; }
   void SetTimeFormat(int aTimeFormat) { mTimeFormat = aTimeFormat; }
   int  GetLatLonFormat() const { return mLatLonFormat; }
   void SetLatLonFormat(int aLatLonFormat) { mLatLonFormat = aLatLonFormat; }

   bool PrintLLA_Locations() const { return mPrintLLA_Locations; }
   void SetPrintLLA_Locations(bool aPrintLLA_Locations) { mPrintLLA_Locations = aPrintLLA_Locations; }
   bool PrintSingleLinePerEvent() const { return mPrintSingleLinePerEvent; }
   void SetPrintSingleLinePerEvent(bool aPrintSingleLinePerEvent)
   {
      mPrintSingleLinePerEvent = aPrintSingleLinePerEvent;
   }
   bool PrintTrackInMessage() const { return mPrintTrackInMessage; }
   void SetPrintTrackInMessage(bool aPrintTrackInMessage) { mPrintTrackInMessage = aPrintTrackInMessage; }
   bool PrintTrackCovariance() const { return mPrintTrackCovariance; }
   void SetPrintTrackCovariance(bool aPrintTrackCovariance) { mPrintTrackCovariance = aPrintTrackCovariance; }
   bool PrintTrackResidualCovariance() const { return mPrintTrackResidualCovariance; }
   void SetPrintTrackResidualCovariance(bool aPrintTrackResidualCovariance)
   {
      mPrintTrackResidualCovariance = aPrintTrackResidualCovariance;
   }

   // These allow the labels to be tweaked so they match 1.10 output.
   std::string GetSNR_InterferenceLabel() const { return mSNR_InterferenceLabel; }
   void        SetSNR_InterferenceLabel(const std::string& aLabel) { mSNR_InterferenceLabel = aLabel; }
   std::string GetInterferencePowerLabel() const { return mInterferencePowerLabel; }
   void        SetInterferencePowerLabel(const std::string& aLabel) { mInterferencePowerLabel = aLabel; }


   const MessagePrintMap& GetMessagePrinters() const { return mMessagePrinters; }
   void                   RegisterMessagePrinter(WsfStringId aMessageType, const MessagePrintFunction& aFunction)
   {
      mMessagePrinters[aMessageType] = aFunction;
   }

protected:
   // Common
   int mTimeFormat{UtTime::FmtS + 5 + UtTime::FmtShowPoint + UtTime::FmtNoSuffix}; // ASCII format
   int mLatLonFormat{UtAngle::FmtDMS + 2};                                         // ASCII format

   // WsfEventOutput
   bool mPrintLLA_Locations{true};
   bool mPrintSingleLinePerEvent{false};
   bool mPrintTrackInMessage{false};
   bool mPrintTrackCovariance{false};
   bool mPrintTrackResidualCovariance{false};

   // These allow the labels to be tweaked so they match 1.10 output.
   std::string mSNR_InterferenceLabel{"S/(N+C+I)"};
   std::string mInterferencePowerLabel{"Interference_Power"};

   MessagePrintMap mMessagePrinters;
};

class WSF_EXPORT ResultBase
{
public:
   ResultBase(double aSimTime, std::string aEventName = "UNSET")
      : mSimTime(aSimTime)
      , mEventName(std::move(aEventName))
   {
   }

   ResultBase(const ResultBase&) = default;
   ResultBase& operator=(const ResultBase&) = default;
   virtual ~ResultBase()                    = default;

   double             GetSimTime() const { return mSimTime; }
   const std::string& GetName() const { return mEventName; }

protected:
   double      mSimTime;
   std::string mEventName;
};

class WSF_EXPORT Result : public ResultBase
{
public:
   Result(double aSimTime, Settings aSettings = Settings{}, std::string aEventName = "UNSET")
      : ResultBase(aSimTime, std::move(aEventName))
      , mSettings(std::move(aSettings))
   {
   }

   virtual void Print(std::ostream& aStream) const = 0;
   virtual void Print(std::ostream& aStream, Settings aSettings)
   {
      // TODO: Remove member Settings and use dependency injection
      mSettings = std::move(aSettings);
      Print(aStream);
   }
   virtual void PrintCSV(std::ostream& aStream) const = 0;
   virtual void PrintCSV(std::ostream& aStream, Settings aSettings)
   {
      // TODO: Remove member Settings and use dependency injection
      mSettings = std::move(aSettings);
      PrintCSV(aStream);
   }

protected:
   Settings mSettings;
};
} // namespace event
} // namespace wsf

#endif
