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

#include "dis/WsfDisDataR.hpp"

#include <cassert>

#include "Dis.hpp"
#include "DisActionEnums.hpp"
#include "DisClockTime.hpp"
#include "DisDatumEnums.hpp"
#include "DisPduEnums.hpp"
#include "DisTransactionEnums.hpp"
#include "UtLog.hpp"
#include "dis/WsfDisDataQueryR.hpp"
#include "dis/WsfDisRecordQueryR.hpp"
#include "dis/WsfDisSetDataR.hpp"
#include "dis/WsfDisSetRecordR.hpp"
#include "dis/WsfDisUtil.hpp"

//! Constructor
//! Local Origination
//! Generate WsfDisDataR when NOT responding to an externally received DisDataQueryR or DisSetDataR PDU.
//! @param aInterfacePtr A pointer to the DIS interface.
WsfDisDataR::WsfDisDataR(WsfDisInterface* aInterfacePtr)
   : DisDataR()
   , mInterfacePtr(aInterfacePtr)
{
   SetOriginatingEntity(DisEntityId(Dis::GetSite(), Dis::GetApplication(), 0));
   // Note:  By default, this PDU is given the same Reliability setting
   // as is specified by the DIS interface specification, but since this
   // message is a respondent, not an initiator, some applications of it
   // will force the reliability indicator back to Unacknowledged.
   SetReliabilityService(WsfDisUtil::GetReliabilityService(mInterfacePtr));
}

//! Constructor
//! Remote Origination
//! Generate WsfDisDataR in response to an externally received DisDataQueryR or DisSetDataR PDU or
//! Generate WsfDisDataR while processing an externally received DisDataR PDU.
//! @param aPdu          A reference to the incoming Protocol Data Unit (PDU).
//! @param aGenI         A GenI helper reference used to retrieve PDU information from the buffer.
//! @param aInterfacePtr A pointer to the DIS interface.
WsfDisDataR::WsfDisDataR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisDataR(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisDataR::~WsfDisDataR() {}

// virtual
int WsfDisDataR::Process()
{
   const int DELETE_PDU = 1;
   return DELETE_PDU;
}

bool WsfDisDataR::ProcessResponseTo(DisPdu* aInitiatorPduPtr)
{
   bool processed = false;

   assert(IsRespondent());

   auto out = ut::log::debug();
   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      out << "WsfDisDataR: WSF does not implement the SM function and does not use returned data.";
   }

   // This received DataR pdu is in response to one of two initiating requests:
   WsfDisSetRecordR* dsrrPduPtr = dynamic_cast<WsfDisSetRecordR*>(aInitiatorPduPtr);
   if (dsrrPduPtr != nullptr)
   {
      processed = true;
      if (WsfDisUtil::DebugEnabled(mInterfacePtr))
      {
         out.AddNote() << "Requested from a WsfDisSetRecordR request.";
      }
   }

   if (!processed)
   {
      WsfDisDataQueryR* drqrPduPtr = dynamic_cast<WsfDisDataQueryR*>(aInitiatorPduPtr);
      if (drqrPduPtr != nullptr)
      {
         processed = true;
         if (WsfDisUtil::DebugEnabled(mInterfacePtr))
         {
            out.AddNote() << "Requested from a WsfDisDataQueryR request.";
         }
      }
   }

   if (!processed)
   {
      WsfDisRecordQueryR* drqrPduPtr = dynamic_cast<WsfDisRecordQueryR*>(aInitiatorPduPtr);
      if (drqrPduPtr != nullptr)
      {
         processed = true;
         if (WsfDisUtil::DebugEnabled(mInterfacePtr))
         {
            out.AddNote() << "Requested from a WsfDisRecordQueryR request.";
         }
      }
   }

   if (!processed)
   {
      bool print = WsfDisUtil::DebugEnabled(mInterfacePtr);

      WsfDisSetDataR* dsdrPduPtr = dynamic_cast<WsfDisSetDataR*>(aInitiatorPduPtr);
      if (dsdrPduPtr != nullptr)
      {
         processed = true;

         unsigned int i        = 0;
         unsigned int supplied = 0;
         unsigned int parsed   = 0;

         for (i = 0; i != GetData().GetNumFixedDatums(); ++i)
         {
            ++supplied;

            DisEnum32 id = GetData().GetFixedDatums()[i].GetId();

            if (id == DisEnum::DatumTag::APPLICATION_RATE)
            {
               ++parsed;
               if (print)
               {
                  out.AddNote() << "Fixed Datum: APPLICATION_RATE";
               }
            }
            else if (id == DisEnum::DatumTag::SIMULATION_RATE)
            {
               ++parsed;
               if (print)
               {
                  out.AddNote() << "Fixed Datum: SIMULATION_RATE";
               }
            }
            else if (id == DisEnum::DatumTag::FEEDBACK_TIME)
            {
               ++parsed;
               if (print)
               {
                  out.AddNote() << "Fixed Datum: FEEDBACK_TIME";
               }
            }
            else if (id == DisEnum::DatumTag::TIME_SCHEME)
            {
               ++parsed;
               if (print)
               {
                  out.AddNote() << "Fixed Datum: TIME_SCHEME";
               }
            }
            else
            {
               ++parsed;
               if (print)
               {
                  // Okay. This should print before 'out' stream.
                  auto warn = ut::log::warning() << "WsfDisDataR received unknown data reply.";
                  warn.AddNote() << "ID: " << id;
               }
            }
         }

         for (i = 0; i != GetData().GetNumVariableDatums(); ++i)
         {
            ++supplied;

            DisEnum32 id = GetData().GetVariableDatums()[i].GetId();

            if (id == DisEnum::DatumTag::EXERCISE_NAME)
            {
               ++parsed;
               if (print)
               {
                  out.AddNote() << "Variable Datum: EXERCISE_NAME";
               }
            }
            else if (id == DisEnum::DatumTag::SIMULATION_TIME_STEP)
            {
               ++parsed;
               if (print)
               {
                  out.AddNote() << "Variable Datum: SIMULATION_TIME_STEP";
               }
            }
            else if (id == DisEnum::DatumTag::APPLICATION_TIME_STEP)
            {
               ++parsed;
               if (print)
               {
                  out.AddNote() << "Variable Datum: APPLICATION_TIME_STEP";
               }
            }
            else if (id == DisEnum::DatumTag::SIMULATION_TIME)
            {
               ++parsed;
               if (print)
               {
                  out.AddNote() << "Variable Datum: SIMULATION_TIME";
               }
            }
            else
            {
               ++parsed;
               if (print)
               {
                  // Okay. This should print before 'out' stream.
                  auto warn = ut::log::warning() << "WsfDisDataR: Received unparsed variable.";
                  warn.AddNote() << "ID: " << id;
               }
            }
         }

         if (WsfDisUtil::DebugEnabled(mInterfacePtr) && (supplied != parsed))
         {
            // Okay. This should print before 'out' stream.
            ut::log::warning() << "WsfDisDataR: Cannot interpret all returned values.";
         }
      }
   }

   if (!processed)
   {
      processed = true;
      if (WsfDisUtil::DebugEnabled(mInterfacePtr))
      {
         // Okay. This should print before 'out' stream.
         ut::log::warning() << "WsfDisDataR: Does not know what type of initiating PDU was received.";
      }
   }

   return processed;
}

bool WsfDisDataR::ProcessTimeAdvanceReport()
{
   ut::log::warning() << "WsfDisDataR: Unhandled method ProcessTimeAdvanceReport().";
   return true;
}
