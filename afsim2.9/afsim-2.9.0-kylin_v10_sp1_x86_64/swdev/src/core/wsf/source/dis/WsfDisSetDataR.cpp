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

#include "dis/WsfDisSetDataR.hpp"

#include <cassert>

#include "Dis.hpp"
#include "DisActionEnums.hpp"
#include "DisClockTime.hpp"
#include "DisDatumEnums.hpp"
#include "DisFixedDatum.hpp"
#include "DisIO.hpp"
#include "DisPduEnums.hpp"
#include "DisTime.hpp"
#include "GenIConvertBigEndian.hpp"
#include "GenOConvertBigEndian.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfClockSource.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisDataR.hpp"
#include "dis/WsfDisUtil.hpp"

//! Constructor
//! Local Origination
//! Generate WsfDisSetDataR to be sent to one or more external simulations.
//! @param aInterfacePtr A pointer to the DIS interface.
WsfDisSetDataR::WsfDisSetDataR(WsfDisInterface* aInterfacePtr)
   : DisSetDataR()
   , mInterfacePtr(aInterfacePtr)
{
   SetReliabilityService(WsfDisUtil::GetReliabilityService(mInterfacePtr));
   SetRequestId(WsfDisUtil::GetNextRequestNumber(mInterfacePtr)); // Serialized identifier handle.
   SetOriginatingEntity(DisEntityId(Dis::GetSite(), Dis::GetApplication(), 0));
}

//! Constructor
//! Remote Origination
//! Generate WsfDisSetDataR while processing an externally received DisSetDataR PDU.
//! @param aPdu          A reference to the incoming Protocol Data Unit (PDU).
//! @param aGenI         A GenI helper reference used to retrieve PDU information from the buffer.
//! @param aInterfacePtr A pointer to the DIS interface.
WsfDisSetDataR::WsfDisSetDataR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisSetDataR(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisSetDataR::~WsfDisSetDataR() {}


bool WsfDisSetDataR::PopulateResponse(WsfDisDataR* aReplyPduPtr) const
{
   unsigned int requested = 0;
   unsigned int set       = 0;

   using namespace DisEnum::DatumTag;

   unsigned int i;
   for (i = 0; i != GetData().GetNumFixedDatums(); ++i)
   {
      ++requested;
      const std::vector<DisFixedDatum>& array = GetData().GetFixedDatums();
      const DisFixedDatum&              ref   = array[i];
      DisEnum32                         id    = ref.GetId();

      if ((id == SIMULATION_RATE) || (id == APPLICATION_RATE))
      {
         ++set;
         double               clockRate = WsfDisUtil::GetSimulation(mInterfacePtr).GetClockSource()->GetClockRate();
         unsigned char        buffer[4];
         GenOConvertBigEndian converter;
         converter.PutToBuf(buffer, static_cast<DisFloat32>(clockRate));
         aReplyPduPtr->GetData().AddFixedDatum(DisFixedDatum(id, buffer));
      }
      else if (id == TIME_SCHEME)
      {
         // don't actually need to send back time scheme
         ++set;
      }
      else
      {
         if (WsfDisUtil::DebugEnabled(mInterfacePtr))
         {
            auto out = ut::log::debug() << "WsfDisSetDataR did not populate unknown fixed datum." << id;
            out.AddNote() << "ID: " << id;
         }
      }
   }

   for (i = 0; i != GetData().GetNumVariableDatums(); ++i)
   {
      ++requested;
      const std::vector<DisVariableDatum>& array = GetData().GetVariableDatums();
      const DisVariableDatum&              ref   = array[i];
      int                                  id    = ref.GetId();

      if (id == SIMULATION_TIME)
      {
         ++set;
         double       simTime = WsfDisUtil::GetSimTime(mInterfacePtr);
         DisClockTime disTime;
         WsfDisUtil::SimTimeToDisClockTime(simTime, disTime);
         aReplyPduPtr->GetData().AddVariableDatum(DisVariableDatum(id, disTime));
      }
      else if (id == SIMULATION_TIME_STEP)
      {
         ++set;
         double       simTimeStep = WsfDisUtil::GetSimulation(mInterfacePtr).GetTimestep();
         DisClockTime disTimeStep;
         WsfDisUtil::SimTimeToDisClockTime(simTimeStep, disTimeStep);
         aReplyPduPtr->GetData().AddVariableDatum(DisVariableDatum(id, disTimeStep));
      }
      else
      {
         if (WsfDisUtil::DebugEnabled(mInterfacePtr))
         {
            auto out = ut::log::debug() << "WsfDisSetDataR did not populate unknown variable.";
            out.AddNote() << "ID: " << id;
         }
      }
   }

   if (WsfDisUtil::DebugEnabled(mInterfacePtr) && (requested != set))
   {
      ut::log::warning() << "WsfDisSetDataR: Could not fully respond to data query.";
      requested = set;
   }

   return requested == set;
}

WsfDisDataR* WsfDisSetDataR::ProduceResponse(bool& aIsDone)
{
   WsfDisDataR* responsePtr = new WsfDisDataR(mInterfacePtr);

   responsePtr->SetRequestId(GetRequestId());
   responsePtr->SetReceivingEntity(GetOriginatingEntity());

   // Per the DIS Standard, the response shall NOT require acknowledgment.
   responsePtr->SetReliabilityService(DisEnum::Pdu::Reliability::Unacknowledged);

   aIsDone = PopulateResponse(responsePtr);

   return responsePtr;
}

// virtual
void WsfDisSetDataR::TimedOut()
{
   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      ut::log::warning() << "WsfDisSetDataR: Unhandled virtual TimedOut() call.";
   }
}

void WsfDisSetDataR::Make_STP_or_TAG(double aSimTime, double aSimTimeStep, double aSimRate, int aTimeScheme)
{
   if (aSimTime != 0.0)
   {
      DisClockTime disTime;
      // WsfDisInterface::SimTimeToDisClockTime(aSimTime, disTime);
      GetData().AddVariableDatum(DisVariableDatum(DisEnum::DatumTag::SIMULATION_TIME, disTime));
   }

   if (aSimTimeStep != 0.0)
   {
      DisClockTime disTime;
      // WsfDisInterface::SimTimeToDisClockTime(aSimTimeStep, disTime);
      GetData().AddVariableDatum(DisVariableDatum(DisEnum::DatumTag::SIMULATION_TIME_STEP, disTime));
   }

   if (aSimRate > 0.0)
   {
      unsigned char        dataBuffer[4];
      GenOConvertBigEndian converter;
      converter.PutToBuf(dataBuffer, static_cast<float>(aSimRate)); // 32-bit float
      GetData().AddFixedDatum(DisFixedDatum(DisEnum::DatumTag::SIMULATION_RATE, dataBuffer));
   }

   if (aTimeScheme != 0)
   {
      unsigned char        dataBuffer[4];
      GenOConvertBigEndian converter;
      converter.PutToBuf(dataBuffer, static_cast<DisUint32>(aTimeScheme)); // 32-bit uint
      GetData().AddFixedDatum(DisFixedDatum(DisEnum::DatumTag::TIME_SCHEME, dataBuffer));
   }
}

bool WsfDisSetDataR::ParseTimeParameters(double& aSimTime, double& aSimTimeStep, double& aSimRate, int& aTimeScheme) const
{
   unsigned int supplied = 0;
   unsigned int parsed   = 0;

   unsigned int absoluteFlag;
   DisClockTime disTime;

   unsigned int i;
   for (i = 0; i != GetData().GetNumFixedDatums(); ++i)
   {
      ++supplied;

      const DisFixedDatum& fixedRef = GetData().GetFixedDatums()[i];
      DisUint32            id       = fixedRef.GetId();

      if (id == DisEnum::DatumTag::SIMULATION_RATE)
      {
         GenIConvertBigEndian converter;
         unsigned char        buffer[8];
         float                floatValue;

         fixedRef.GetData(buffer);
         converter.GetFromBuf(buffer, floatValue);
         aSimRate = static_cast<double>(floatValue);
         ++parsed;
      }
      else if (id == DisEnum::DatumTag::TIME_SCHEME)
      {
         GenIConvertBigEndian converter;
         unsigned char        buffer[8];
         DisUint32            uintValue;

         fixedRef.GetData(buffer);
         converter.GetFromBuf(buffer, uintValue);
         aTimeScheme = static_cast<unsigned int>(uintValue);
         ++parsed;
      }
      else if (WsfDisUtil::DebugEnabled(mInterfacePtr))
      {
         auto out = ut::log::debug() << "WsfDisSetDataR received a request to set an unknown fixed datum.";
         out.AddNote() << "ID: " << id;
      }
   }

   for (i = 0; i != GetData().GetNumVariableDatums(); ++i)
   {
      ++supplied;

      const DisVariableDatum& varRef = GetData().GetVariableDatums()[i];
      DisUint32               id     = varRef.GetId();

      if (id == DisEnum::DatumTag::SIMULATION_TIME)
      {
         varRef.GetClockTimeFromBuffer(disTime);
         aSimTime = 3600 * disTime.GetHour() + DisTime::GetSecsAfterHour(disTime.GetTimePastHour(), absoluteFlag);
         assert(absoluteFlag == 0);
         ++parsed;
      }
      else if (id == DisEnum::DatumTag::SIMULATION_TIME_STEP)
      {
         varRef.GetClockTimeFromBuffer(disTime);
         aSimTimeStep = 3600 * disTime.GetHour() + DisTime::GetSecsAfterHour(disTime.GetTimePastHour(), absoluteFlag);
         assert(absoluteFlag == 0);
         ++parsed;
      }
      else if (WsfDisUtil::DebugEnabled(mInterfacePtr))
      {
         auto out = ut::log::debug() << "WsfDisSetDataR received a request to set an unknown variable datum.";
         out.AddNote() << "ID: " << id;
      }
   }

   return parsed == supplied;
}

bool WsfDisSetDataR::ProcessSetDataRequest()
{
   unsigned int paramsParsed   = 0;
   unsigned int paramsSupplied = 0;

   double newSimTime    = -1.0;
   double newSimRate    = 0.0;
   double newTimeStep   = 1.0;
   int    newTimeScheme = 0;

   unsigned int i;
   for (i = 0; i != GetData().GetNumFixedDatums(); ++i)
   {
      ++paramsSupplied;

      const DisFixedDatum& fixedRef = GetData().GetFixedDatums()[i];
      DisUint32            id       = fixedRef.GetId();

      if ((id == DisEnum::DatumTag::SIMULATION_RATE) || (id == DisEnum::DatumTag::APPLICATION_RATE))
      {
         GenIConvertBigEndian converter;
         unsigned char        buffer[8];
         float                floatValue;

         fixedRef.GetData(buffer);
         converter.GetFromBuf(buffer, floatValue);
         newSimRate = static_cast<double>(floatValue);
         ++paramsParsed;
      }
      else if (id == DisEnum::DatumTag::TIME_SCHEME)
      {
         GenIConvertBigEndian converter;
         unsigned char        buffer[8];
         DisUint32            uintValue;

         fixedRef.GetData(buffer);
         converter.GetFromBuf(buffer, uintValue);
         newTimeScheme = static_cast<int>(uintValue);
         ++paramsParsed;
      }
      else
      {
         if (WsfDisUtil::DebugEnabled(mInterfacePtr))
         {
            auto out = ut::log::warning() << "WsfDisSetDataR: Unsupported fixed datum.";
            out.AddNote() << "ID: " << id;
         }
      }
   }

   for (i = 0; i != GetData().GetNumVariableDatums(); ++i)
   {
      ++paramsSupplied;

      const DisVariableDatum& varRef = GetData().GetVariableDatums()[i];
      DisUint32               id     = varRef.GetId();
      DisClockTime            disTime;
      unsigned int            absolute;

      if (id == DisEnum::DatumTag::SIMULATION_TIME)
      {
         varRef.GetClockTimeFromBuffer(disTime);
         newSimTime = 3600 * disTime.GetHour() + DisTime::GetSecsAfterHour(disTime.GetTimePastHour(), absolute);
         ++paramsParsed;
      }
      else if (id == DisEnum::DatumTag::SIMULATION_TIME_STEP)
      {
         varRef.GetClockTimeFromBuffer(disTime);
         newTimeStep = 3600 * disTime.GetHour() + DisTime::GetSecsAfterHour(disTime.GetTimePastHour(), absolute);
         ++paramsParsed;
      }
      else
      {
         if (WsfDisUtil::DebugEnabled(mInterfacePtr))
         {
            auto out = ut::log::warning() << "WsfDisSetDataR: Unsupported variable datum.";
            out.AddNote() << "ID: " << id;
         }
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

   return paramsParsed == paramsSupplied;
}
