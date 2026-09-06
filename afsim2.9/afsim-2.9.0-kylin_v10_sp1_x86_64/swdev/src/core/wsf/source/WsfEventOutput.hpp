// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFEVENTOUTPUT_HPP
#define WSFEVENTOUTPUT_HPP

#include "wsf_export.h"

#include "WsfEventOutputBase.hpp"

struct WsfEventOutputData : wsf::event::output::Data
{
   bool ProcessInput(UtInput& aInput) override;

   bool mPrintCGMissDist{false};
   bool mPrintFailedMessageDeliveryAttempts{true};
   bool mPrintFailedSensorDetectionAttempts{true};
};

//! A simulation observer to write an event output file.
class WSF_EXPORT WsfEventOutput : public wsf::event::output::SimulationExtension
{
public:
   static WsfEventOutput* Find(const WsfSimulation& aSimulation);

   WsfEventOutput(WsfEventOutputData aData)
      : wsf::event::output::SimulationExtension(mEVT_Data)
      , mEVT_Data(std::move(aData))
   {
   }

private:
   void PrintEvent(const wsf::event::Result& aResult) const override;

   WsfEventOutputData mEVT_Data;
};

#endif
