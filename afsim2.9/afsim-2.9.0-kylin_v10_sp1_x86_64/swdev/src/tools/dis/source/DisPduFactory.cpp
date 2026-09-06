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

#include "DisPduFactory.hpp"

#include <iostream>

#include "UtLog.hpp"

//  This class inherits from GenIFactory

#include "Dis.hpp"
#include "DisAcknowledge.hpp"
#include "DisAcknowledgeR.hpp"
#include "DisActionRequest.hpp"
#include "DisActionRequestR.hpp"
#include "DisActionResponse.hpp"
#include "DisActionResponseR.hpp"
#include "DisAttribute.hpp"
#include "DisComment.hpp"
#include "DisCommentR.hpp"
#include "DisCreateEntity.hpp"
#include "DisCreateEntityR.hpp"
#include "DisData.hpp"
#include "DisDataQuery.hpp"
#include "DisDataQueryR.hpp"
#include "DisDataR.hpp"
#include "DisDesignator.hpp"
#include "DisDetonation.hpp"
#include "DisDirectedEnergyFire.hpp"
#include "DisEmission.hpp"
#include "DisEntityDamageStatus.hpp"
#include "DisEntityState.hpp"
#include "DisEventReport.hpp"
#include "DisEventReportR.hpp"
#include "DisFire.hpp"
#include "DisIO.hpp"
#include "DisIff.hpp"
#include "DisOther.hpp"
#include "DisPdu.hpp"
#include "DisPduEnums.hpp"
#include "DisReceiver.hpp"
#include "DisRecordQueryR.hpp"
#include "DisRecordR.hpp"
#include "DisRemoveEntity.hpp"
#include "DisRemoveEntityR.hpp"
#include "DisRepairComplete.hpp"
#include "DisRepairResponse.hpp"
#include "DisResupplyCancel.hpp"
#include "DisResupplyOffer.hpp"
#include "DisResupplyReceived.hpp"
#include "DisServiceRequest.hpp"
#include "DisSetData.hpp"
#include "DisSetDataR.hpp"
#include "DisSetRecordR.hpp"
#include "DisSignal.hpp"
#include "DisSpotted.hpp"
#include "DisStartResume.hpp"
#include "DisStartResumeR.hpp"
#include "DisStopFreeze.hpp"
#include "DisStopFreezeR.hpp"
#include "DisTransferOwnership.hpp"
#include "DisTransmitter.hpp"
#include "DisUnderwaterAcoustic.hpp"

DisPduFactory::DisPduFactory()
   : mLastGoodDisEntityIdValid(0)
   , mFilteredPduCount(0)
{
}

DisPduFactory::~DisPduFactory() {}

// virtual
DisPdu* DisPduFactory::Create(GenI& aGenI)
{
   long startPos = aGenI.GetGetOffset(GenBuf::FromBeg);

   DisPdu disPdu(aGenI);

   if (aGenI.GetInputStatus() != GenBuf::NoError || !disPdu.IsValid())
   {
      if (Dis::ShowError(Dis::cIO_ERRORS))
      {
         long errorPos = aGenI.GetGetOffset(GenBuf::FromBeg) - startPos;

         { // RAII block
            auto out = ut::log::error();
            if (aGenI.GetInputStatus() != GenBuf::NoError)
            {
               out << "Error reading DisPdu header of PDU.";
               out.AddNote() << "Position: " << errorPos << " bytes from start";
            }
            else
            {
               out << "Invalid DisPdu header found.";
            }

            disPdu.HandleReadError(out);
         }

         LogError(startPos);
      }

      // No need to go any further
      // Buffer clean up will be done by GenIFactory
      return nullptr;
   }

   int          pduType          = 0;
   unsigned int disEntityIdValid = 1;

   // Give the user a chance to create a class which would
   // not be created in the switch statement below
   DisPdu* pduPtr = UserCreate(disPdu, aGenI);

   // If user didn't create a DIS class, then we will
   if (pduPtr == nullptr)
   {
      pduType = disPdu.GetPduType();
      switch (pduType)
      {
      case DisEnum::Pdu::Type::Acknowledge:
         pduPtr = new DisAcknowledge(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::AcknowledgeR:
         pduPtr = new DisAcknowledgeR(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::ActionRequest:
         pduPtr = new DisActionRequest(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::ActionRequestR:
         pduPtr = new DisActionRequestR(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::ActionResponse:
         pduPtr = new DisActionResponse(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::ActionResponseR:
         pduPtr = new DisActionResponseR(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::Attribute:
         pduPtr = new DisAttribute(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::Comment:
         pduPtr = new DisComment(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::CommentR:
         pduPtr = new DisCommentR(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::CreateEntity:
         pduPtr = new DisCreateEntity(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::CreateEntityR:
         pduPtr = new DisCreateEntityR(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::Data:
         pduPtr = new DisData(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::DataR:
         pduPtr = new DisDataR(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::DataQuery:
         pduPtr = new DisDataQuery(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::DataQueryR:
         pduPtr = new DisDataQueryR(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::Designator:
         pduPtr = new DisDesignator(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::Detonation:
         pduPtr = new DisDetonation(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::DirectedEnergyFire:
         pduPtr = new DisDirectedEnergyFire(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::DirectedEnergyDamageStatus:
         pduPtr = new DisEntityDamageStatus(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::Emission:
         pduPtr = new DisEmission(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::EntityState:
         pduPtr = new DisEntityState(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::EventReport:
         pduPtr = new DisEventReport(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::EventReportR:
         pduPtr = new DisEventReportR(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::Fire:
         pduPtr = new DisFire(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::IFF:
         pduPtr = new DisIff(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::Receiver:
         pduPtr = new DisReceiver(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::RecordR:
         pduPtr = new DisRecordR(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::RecordQueryR:
         pduPtr = new DisRecordQueryR(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::RemoveEntity:
         pduPtr = new DisRemoveEntity(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::RemoveEntityR:
         pduPtr = new DisRemoveEntityR(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::RepairComplete:
         pduPtr = new DisRepairComplete(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::RepairResponse:
         pduPtr = new DisRepairResponse(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::ResupplyCancel:
         pduPtr = new DisResupplyCancel(disPdu, aGenI);
         // Note:  Unlike almost all other PDU's this one may originate from
         // the entity who desires services or is providing them, so the
         // originator of the PDU cannot be determined with certainty.
         disEntityIdValid = 0;
         break;
      case DisEnum::Pdu::Type::ResupplyOffer:
         pduPtr = new DisResupplyOffer(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::ResupplyReceived:
         pduPtr = new DisResupplyReceived(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::ServiceRequest:
         pduPtr = new DisServiceRequest(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::SetData:
         pduPtr = new DisSetData(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::SetDataR:
         pduPtr = new DisSetDataR(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::SetRecordR:
         pduPtr = new DisSetRecordR(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::Signal:
         pduPtr = new DisSignal(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::Spotted:
         pduPtr = new DisSpotted(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::StartResume:
         pduPtr = new DisStartResume(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::StartResumeR:
         pduPtr = new DisStartResumeR(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::StopFreeze:
         pduPtr = new DisStopFreeze(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::StopFreezeR:
         pduPtr = new DisStopFreezeR(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::TransferOwnership:
         pduPtr = new DisTransferOwnership(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::Transmitter:
         pduPtr = new DisTransmitter(disPdu, aGenI);
         break;
      case DisEnum::Pdu::Type::UnderwaterAcoustic:
         pduPtr = new DisUnderwaterAcoustic(disPdu, aGenI);
         break;
      default:
         pduPtr           = new DisOther(disPdu, aGenI);
         disEntityIdValid = 0;
         break;
      }
   }

   if (pduPtr != nullptr)
   {
      DisEntityId disEntityId = pduPtr->GetOriginatingEntity();

      if (aGenI.GetInputStatus() != GenBuf::NoError)
      {
         if (Dis::ShowError(Dis::cIO_ERRORS))
         {
            long errorPos = aGenI.GetGetOffset(GenBuf::FromBeg) - startPos;

            { // RAII block
               auto out = ut::log::error() << "Error reading DisPdu type of PDU.";
               out.AddNote() << "Position: " << errorPos << "bytes from start";

               // Give class a chance to write out any more info
               pduPtr->HandleReadError(out);

               if (disEntityIdValid)
               {
                  out.AddNote() << "DisEntityId(originating): " << disEntityId;
               }
            }

            // No need to go any further with this
            delete pduPtr;
            pduPtr = nullptr;

            LogError(startPos);

            // Buffer clean up will be done by GenIFactory
         }
      }
      else
      {
         mLastGoodDisPdu           = disPdu;
         mLastGoodDisEntityId      = disEntityId;
         mLastGoodDisEntityIdValid = disEntityIdValid;
      }
   }

   return pduPtr;
}
// Increments the number of filtered pdus.
void DisPduFactory::IncrementFilteredPduCount()
{
   ++mFilteredPduCount;
}

// virtual
// User overrides this method to create their own DIS classes
DisPdu* DisPduFactory::UserCreate(DisPdu& /* aDisPdu */, GenI& /* aGenI */)
{
   return nullptr;
}

// Used for error messages
void DisPduFactory::LogError(long aStartPos)
{
   if (aStartPos != 0)
   {
      auto out = ut::log::error() << "Last good PDU Header was:";
      mLastGoodDisPdu.HandleReadError(out);
      if (mLastGoodDisEntityIdValid)
      {
         out.AddNote() << "Originating DisEntityId: " << mLastGoodDisEntityId;
      }
   }
   else
   {
      ut::log::error() << "This was the first PDU in the receive buffer.";
   }
}
