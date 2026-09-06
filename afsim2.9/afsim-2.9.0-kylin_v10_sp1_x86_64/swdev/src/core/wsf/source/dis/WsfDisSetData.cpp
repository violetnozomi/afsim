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

#include "dis/WsfDisSetData.hpp"

#include "Dis.hpp"
#include "DisClockTime.hpp"
#include "DisDatumEnums.hpp"
#include "DisIO.hpp"
#include "DisTime.hpp"
#include "GenI.hpp"
#include "GenIConvertBigEndian.hpp"
#include "GenOConvertBigEndian.hpp"
#include "UtLog.hpp"
#include "WsfDisObserver.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisUtil.hpp"

//! Constructor
//! Local Origination
//! Generate WsfDisSetData to be sent to one or more external simulations.
//! @param aInterfacePtr A pointer to the DIS interface.
WsfDisSetData::WsfDisSetData(WsfDisInterface* aInterfacePtr)
   : DisSetData()
   , mInterfacePtr(aInterfacePtr)
{
   SetRequestId(WsfDisUtil::GetNextRequestNumber(mInterfacePtr)); // Serialized identifier handle.
   SetOriginatingEntity(DisEntityId(Dis::GetSite(), Dis::GetApplication(), 0));
}

//! Constructor
//! Remote Origination
//! Generate WsfDisSetDataR while processing an externally received DisSetDataR PDU.
//! @param aPdu          A reference to the incoming Protocol Data Unit (PDU).
//! @param aGenI         A GenI helper reference used to retrieve PDU information from the buffer.
//! @param aInterfacePtr A pointer to the DIS interface.
WsfDisSetData::WsfDisSetData(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisSetData(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisSetData::~WsfDisSetData() {}

// virtual
int WsfDisSetData::Process()
{
   // Allow other applications to handle SetData PDU
   WsfObserver::DisSetDataReceived (&WsfDisUtil::GetSimulation(mInterfacePtr))(mInterfacePtr, *this);

   double newSimTime    = -1.0;
   double newSimRate    = 0.0;
   double newTimeStep   = 0.0;
   int    newTimeScheme = 0;

   unsigned int i;
   for (i = 0; i != GetData().GetNumFixedDatums(); ++i)
   {
      const GenIConvertBigEndian converter;
      const DisFixedDatum&       fixedRef = GetData().GetFixedDatums()[i];
      unsigned char              buffer[4];
      DisUint32                  id = fixedRef.GetId();
      fixedRef.GetData(buffer);

      switch (id)
      {
      case DisEnum::DatumTag::SIMULATION_RATE:
      case DisEnum::DatumTag::APPLICATION_RATE:
      {
         float floatValue; // get value as a float, then convert to double
         converter.GetFromBuf(buffer, floatValue);
         newSimRate = static_cast<double>(floatValue);
         break;
      }
      case DisEnum::DatumTag::TIME_SCHEME:
         converter.GetFromBuf(buffer, newTimeScheme);
      default:
         break;
      }
   }

   for (i = 0; i != GetData().GetNumVariableDatums(); ++i)
   {
      const DisVariableDatum& varRef = GetData().GetVariableDatums()[i];
      DisUint32               id     = varRef.GetId();
      DisClockTime            disTime;
      unsigned int            absolute;

      switch (id)
      {
      case DisEnum::DatumTag::SIMULATION_TIME:
      case DisEnum::DatumTag::APPLICATION_TIME:
         varRef.GetClockTimeFromBuffer(disTime);
         newSimTime = 3600 * disTime.GetHour() + DisTime::GetSecsAfterHour(disTime.GetTimePastHour(), absolute);
         break;
      case DisEnum::DatumTag::SIMULATION_TIME_STEP:
      case DisEnum::DatumTag::APPLICATION_TIME_STEP:
         varRef.GetClockTimeFromBuffer(disTime);
         newTimeStep = 3600 * disTime.GetHour() + DisTime::GetSecsAfterHour(disTime.GetTimePastHour(), absolute);
         break;
      default:
         break;
      }
   }

   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      double simTime = WsfDisUtil::GetTimeFromPdu(mInterfacePtr, *this);

      auto out = ut::log::debug() << "SetData received.";
      out.AddNote() << "T = " << simTime;
      out.AddNote() << "Sim Rate: " << newSimRate;

      if (newSimTime != -1.0)
      {
         out.AddNote() << "Run to Sim Time: " << newSimTime;
      }
      if (newTimeStep != 0.0)
      {
         out.AddNote() << "Time Step: " << newTimeStep;
      }
      if (newTimeScheme != 0)
      {
         out.AddNote() << "Time Scheme: " << newTimeScheme;
      }
   }

   if ((newSimTime != -1.0) && (newTimeStep != 0.0))
   {
      const bool enableTimeAdvance = true;
      WsfDisUtil::GetSimulation(mInterfacePtr)
         .SetTimeParameters(newTimeScheme, newSimTime, newSimRate, newTimeStep, enableTimeAdvance);
   }
   else if (newSimRate != 0.0)
   {
      WsfDisUtil::GetSimulation(mInterfacePtr).SetClockRate(newSimRate);
   }

   return 1;
}
