// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFCSV_EVENTOUTPUT_HPP
#define WSFCSV_EVENTOUTPUT_HPP

#include "wsf_export.h"

#include "WsfEventOutputBase.hpp"

struct WSF_EXPORT WsfCSV_EventOutputData : wsf::event::output::Data
{
   WsfCSV_EventOutputData();

   bool ProcessInput(UtInput& aInput) override;

   static void AddDataTags(const std::string& aEventName, const std::vector<std::string>& aDataTags);
   static void AppendDataTags(std::vector<std::string>& aLHS, const std::vector<std::string>& aRHS);

   // Header information
   bool        mInsertDataTags{true};
   std::string mLocationDataStr{"lat<lat>, lon<lon>, alt<double>"};

   static std::mutex sCriticalSection;

   static std::map<std::string, std::vector<std::string>> mDataTags;
   static const std::vector<std::string>                  mEM_InteractionTags;
   static const std::vector<std::string>                  mMessageDataTags;
   static const std::vector<std::string>                  mTrackDataBriefTags;
   static const std::vector<std::string>                  mTrackDataTags;
};

//! A simulation observer to write a comma separated values (csv) event output file.
class WSF_EXPORT WsfCSV_EventOutput : public wsf::event::output::SimulationExtension
{
public:
   static WsfCSV_EventOutput* Find(const WsfSimulation& aSimulation);

   WsfCSV_EventOutput(WsfCSV_EventOutputData aData)
      : wsf::event::output::SimulationExtension(mCSV_Data)
      , mCSV_Data(std::move(aData))
   {
   }

   const std::string& GetLocationDataStr() const { return mCSV_Data.mLocationDataStr; }

private:
   void EnableOrDisableEventP(const std::string& aEventName, bool aEnable) override;

   void WriteHeader(const std::string& aEventName);

   void PrintEvent(const wsf::event::Result& aResult) const override;

   WsfCSV_EventOutputData mCSV_Data;
};

#endif
