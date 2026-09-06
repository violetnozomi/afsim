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

// References:
//
// 1) "Direct-Detection LADAR Systems"; SPIE Press Tutorial Text in Optical Engineering,
//    Volume TT85; Richard D. Richmond and Stephen C. Cain.
// 2) "Laser Communications in Space", Stephen G. Lambert & William L. Casey, Artech House, 1995.

#include "WsfLASER_CommXmtrComponent.hpp"

#include <cmath>
#include <iomanip>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"

// =================================================================================================
// Definition for the nested class WsfLASER_CommXmtrComponent
// =================================================================================================

// =================================================================================================
WsfLASER_CommXmtrComponent::WsfLASER_CommXmtrComponent()
   : WsfLASER_XmtrComponent()
   , mPointingTransmissionFactor(1.0)
   , mAveragePower(0.0)
   , mModulationPtr(ut::make_unique<Modulation>())
{
}

WsfLASER_CommXmtrComponent::WsfLASER_CommXmtrComponent(const WsfLASER_CommXmtrComponent& aSrc)
   : WsfLASER_XmtrComponent(aSrc)
   , mPointingTransmissionFactor(aSrc.mPointingTransmissionFactor)
   , mAveragePower(aSrc.mAveragePower)
   , mModulationPtr(ut::make_unique<Modulation>(*aSrc.mModulationPtr))
{
}

// =================================================================================================
WsfComponent* WsfLASER_CommXmtrComponent::CloneComponent() const
{
   return new WsfLASER_CommXmtrComponent(*this);
}

// =================================================================================================
bool WsfLASER_CommXmtrComponent::PreInitialize(double aSimTime)
{
   return mModulationPtr->Initialize(*GetComponentParent());
}

// =================================================================================================
// virtual
bool WsfLASER_CommXmtrComponent::Initialize(double aSimTime)
{
   bool ok = WsfLASER_XmtrComponent::Initialize(aSimTime);

   if (ok)
   {
      WsfEM_Xmtr& xmtr = *GetComponentParent();
      if (mAveragePower > 0.0)
      {
         xmtr.SetPower(mAveragePower / mModulationPtr->GetDutyCycle()); // peak power
      }
      xmtr.SetDutyCycle(mModulationPtr->GetDutyCycle());
   }
   return ok;
}

// =================================================================================================
bool WsfLASER_CommXmtrComponent::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "average_power")
   {
      aInput.ReadValueOfType(mAveragePower, UtInput::cPOWER);
      aInput.ValueGreater(mAveragePower, 0.0);
   }
   else if (command == "pointing_transmission_factor")
   {
      aInput.ReadValue(mPointingTransmissionFactor);
      aInput.ValueInClosedRange(mPointingTransmissionFactor, 0.0, 1.0);
   }
   else if (command == "pointing_loss")
   {
      aInput.ReadValueOfType(mPointingTransmissionFactor, UtInput::cRATIO);
      aInput.ValueInClosedRange(mPointingTransmissionFactor, 0.0, 1.0);
   }
   else if (mModulationPtr->ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = WsfLASER_XmtrComponent::ProcessInput(aInput);
   }
   return myCommand;
}

// static
const std::map<std::string, WsfLASER_CommXmtrComponent::Modulation::Type> WsfLASER_CommXmtrComponent::Modulation::sTypesMap = {
   std::make_pair<std::string, WsfLASER_CommXmtrComponent::Modulation::Type>("ook",
                                                                             WsfLASER_CommXmtrComponent::Modulation::cOOK),
   std::make_pair<std::string, WsfLASER_CommXmtrComponent::Modulation::Type>("ppm",
                                                                             WsfLASER_CommXmtrComponent::Modulation::cPPM),
   std::make_pair<std::string, WsfLASER_CommXmtrComponent::Modulation::Type>("dpsk",
                                                                             WsfLASER_CommXmtrComponent::Modulation::cDPSK)};

WsfLASER_CommXmtrComponent::Modulation::Modulation()
   : mType(cPPM)
   , mTypeName("ppm")
   , mSlotDuration(0.0)
   , mDataRate(0.0)
   , mDutyCycle(1.0)
   , mPPM_Order(16)
{
}

//! Process input keywords.
//!@param aInput The input source.
bool WsfLASER_CommXmtrComponent::Modulation::ProcessInput(UtInput& aInput)
{
   std::string command   = aInput.GetCommand();
   bool        myCommand = false;
   if (command == "modulation_type")
   {
      aInput.ReadCommand(mTypeName);
      auto iter = sTypesMap.find(mTypeName);
      if (iter != sTypesMap.end())
      {
         mType     = iter->second;
         myCommand = true;
      }
      else
      {
         auto out = ut::log::error() << "Comm::WsfLASER_CommXmtrComponent::Modulation: Type not supported.";
         out.AddNote() << "Type: " << mTypeName;
      }
   }
   else if (command == "ppm_order")
   {
      aInput.ReadValue(mPPM_Order);
      aInput.ValueGreaterOrEqual(mPPM_Order, 2U);
      myCommand = true;
   }
   else if (command == "slot_rate")
   {
      double slotRate = 0.0;
      aInput.ReadValueOfType(slotRate, UtInput::cFREQUENCY);
      aInput.ValueGreater(slotRate, 0.0);
      mSlotDuration = 1.0 / slotRate;
      myCommand     = true;
   }
   else if (command == "slot_width")
   {
      aInput.ReadValueOfType(mSlotDuration, UtInput::cTIME);
      aInput.ValueGreater(mSlotDuration, 0.0);
      myCommand = true;
   }
   return myCommand;
}

//! Initialize for use with a WsfEM_Xmtr.  Data rate and duty cycle are calculated, and
//! transmitter values for pulse width and pulse repetition frequency are reconciled with
//! modulation slot width and duty cycle.
//!@param aXmtr The associated WsfEM_Xmtr object.
bool WsfLASER_CommXmtrComponent::Modulation::Initialize(WsfEM_Xmtr& aXmtr)
{
   bool ok = true;

   if (mType == cPPM)
   {
      mDutyCycle = 1.0 / mPPM_Order;
   }
   else // ook or dpsk
   {
      mDutyCycle = 0.5;
   }

   if (mSlotDuration != 0.0)
   {
      // Preferentially use the slot_duration input over any transmitter pulse width input
      // todo if both set issue a warning message.
      aXmtr.SetPulseWidth(mSlotDuration);
   }
   else
   {
      mSlotDuration = aXmtr.GetPulseWidth();
   }

   // At this point, if slot duration is nonzero, we have enough information to successfully initialize.
   if (mSlotDuration > 0.0)
   {
      if (aXmtr.GetPulseRepetitionFrequency() > 0.0)
      {
         auto out = ut::log::warning()
                    << "Laser transmitter 'pulse_repetition_frequency' will be set based on modulation type and either "
                       "'pulse_width' or 'slot_duration' inputs (not the provided 'pulse_repetition_frequency' input).";
         out.AddNote() << "Xmtr Platform: " << aXmtr.GetPlatform();
      }
      aXmtr.SetPulseRepetitionFrequency(mDutyCycle / mSlotDuration);

      if (mType == cPPM)
      {
         mDataRate = log2(mPPM_Order) / mPPM_Order / mSlotDuration;
      }
      else // ook or dpsk
      {
         mDataRate = 1.0 / mSlotDuration;
      }
   }
   else
   {
      auto out = ut::log::error() << "Comm::WsfLASER_CommXmtrComponent: You must specify a nonzero 'slot_width', or "
                                     "nonzero 'pulse_width' in the transmitter...end_transmiiter block.";
      out.AddNote() << "Xmtr Platform: " << aXmtr.GetPlatform();
      ok = false;
   }

   return ok;
}
