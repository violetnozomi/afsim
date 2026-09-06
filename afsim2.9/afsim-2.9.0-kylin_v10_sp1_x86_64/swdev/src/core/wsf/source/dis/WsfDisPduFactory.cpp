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

#include "dis/WsfDisPduFactory.hpp"

#include "DisOther.hpp"
#include "DisPdu.hpp"
#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "dis/WsfDisAcknowledgeR.hpp"
#include "dis/WsfDisActionRequest.hpp"
#include "dis/WsfDisActionRequestR.hpp"
#include "dis/WsfDisActionResponse.hpp"
#include "dis/WsfDisActionResponseR.hpp"
#include "dis/WsfDisComment.hpp"
#include "dis/WsfDisData.hpp"
#include "dis/WsfDisDataQueryR.hpp"
#include "dis/WsfDisDataR.hpp"
#include "dis/WsfDisDesignator.hpp"
#include "dis/WsfDisEmission.hpp"
#include "dis/WsfDisEntityState.hpp"
#include "dis/WsfDisEventReport.hpp"
#include "dis/WsfDisReceiver.hpp"
#include "dis/WsfDisRecordQueryR.hpp"
#include "dis/WsfDisRecordR.hpp"
#include "dis/WsfDisRemoveEntity.hpp"
#include "dis/WsfDisRepairComplete.hpp"
#include "dis/WsfDisRepairResponse.hpp"
#include "dis/WsfDisResupplyCancel.hpp"
#include "dis/WsfDisResupplyOffer.hpp"
#include "dis/WsfDisResupplyReceived.hpp"
#include "dis/WsfDisServiceRequest.hpp"
#include "dis/WsfDisSetData.hpp"
#include "dis/WsfDisSetDataR.hpp"
#include "dis/WsfDisSetRecordR.hpp"
#include "dis/WsfDisSignal.hpp"
#include "dis/WsfDisStartResume.hpp"
#include "dis/WsfDisStartResumeR.hpp"
#include "dis/WsfDisStopFreeze.hpp"
#include "dis/WsfDisStopFreezeR.hpp"
#include "dis/WsfDisTransferOwnership.hpp"
#include "dis/WsfDisTransmitter.hpp"

WsfDisPduFactory::WsfDisPduFactory(WsfDisInterface* aInterfacePtr)
   : DisPduFactory()
   , mInterfacePtr(aInterfacePtr)
{
   mCreateFunctions.resize(255);
   SetPduCreateFunction(DisEnum::Pdu::Type::AcknowledgeR, new CreateFunctionT<WsfDisAcknowledgeR>);
   SetPduCreateFunction(DisEnum::Pdu::Type::ActionRequest, new CreateFunctionT<WsfDisActionRequest>);
   SetPduCreateFunction(DisEnum::Pdu::Type::ActionRequestR, new CreateFunctionT<WsfDisActionRequestR>);
   SetPduCreateFunction(DisEnum::Pdu::Type::ActionResponse, new CreateFunctionT<WsfDisActionResponse>);
   SetPduCreateFunction(DisEnum::Pdu::Type::ActionResponseR, new CreateFunctionT<WsfDisActionResponseR>);
   SetPduCreateFunction(DisEnum::Pdu::Type::Comment, new CreateFunctionT<WsfDisComment>);
   SetPduCreateFunction(DisEnum::Pdu::Type::DataQueryR, new CreateFunctionT<WsfDisDataQueryR>);
   SetPduCreateFunction(DisEnum::Pdu::Type::Data, new CreateFunctionT<WsfDisData>);
   SetPduCreateFunction(DisEnum::Pdu::Type::EventReport, new CreateFunctionT<WsfDisEventReport>);
   SetPduCreateFunction(DisEnum::Pdu::Type::DataR, new CreateFunctionT<WsfDisDataR>);
   SetPduCreateFunction(DisEnum::Pdu::Type::Designator, new CreateFunctionT<WsfDisDesignator>);
   SetPduCreateFunction(DisEnum::Pdu::Type::Emission, new CreateFunctionT<WsfDisEmission>);
   SetPduCreateFunction(DisEnum::Pdu::Type::EntityState, new CreateFunctionT<WsfDisEntityState>);

   SetPduCreateFunction(DisEnum::Pdu::Type::Receiver, new CreateFunctionT<WsfDisReceiver>);
   SetPduCreateFunction(DisEnum::Pdu::Type::RecordQueryR, new CreateFunctionT<WsfDisRecordQueryR>);
   SetPduCreateFunction(DisEnum::Pdu::Type::RecordR, new CreateFunctionT<WsfDisRecordR>);
   SetPduCreateFunction(DisEnum::Pdu::Type::RemoveEntity, new CreateFunctionT<WsfDisRemoveEntity>);
   SetPduCreateFunction(DisEnum::Pdu::Type::RepairComplete, new CreateFunctionT<WsfDisRepairComplete>);
   SetPduCreateFunction(DisEnum::Pdu::Type::RepairResponse, new CreateFunctionT<WsfDisRepairResponse>);
   SetPduCreateFunction(DisEnum::Pdu::Type::ResupplyCancel, new CreateFunctionT<WsfDisResupplyCancel>);
   SetPduCreateFunction(DisEnum::Pdu::Type::ResupplyOffer, new CreateFunctionT<WsfDisResupplyOffer>);
   SetPduCreateFunction(DisEnum::Pdu::Type::ResupplyReceived, new CreateFunctionT<WsfDisResupplyReceived>);
   SetPduCreateFunction(DisEnum::Pdu::Type::ServiceRequest, new CreateFunctionT<WsfDisServiceRequest>);
   SetPduCreateFunction(DisEnum::Pdu::Type::SetData, new CreateFunctionT<WsfDisSetData>);
   SetPduCreateFunction(DisEnum::Pdu::Type::SetDataR, new CreateFunctionT<WsfDisSetDataR>);
   SetPduCreateFunction(DisEnum::Pdu::Type::SetRecordR, new CreateFunctionT<WsfDisSetRecordR>);
   SetPduCreateFunction(DisEnum::Pdu::Type::Signal, new CreateFunctionT<WsfDisSignal>);
   SetPduCreateFunction(DisEnum::Pdu::Type::StartResume, new CreateFunctionT<WsfDisStartResume>);
   SetPduCreateFunction(DisEnum::Pdu::Type::StartResumeR, new CreateFunctionT<WsfDisStartResumeR>);
   SetPduCreateFunction(DisEnum::Pdu::Type::StopFreeze, new CreateFunctionT<WsfDisStopFreeze>);
   SetPduCreateFunction(DisEnum::Pdu::Type::StopFreezeR, new CreateFunctionT<WsfDisStopFreezeR>);
   SetPduCreateFunction(DisEnum::Pdu::Type::Transmitter, new CreateFunctionT<WsfDisTransmitter>);
   SetPduCreateFunction(DisEnum::Pdu::Type::TransferOwnership, new CreateFunctionT<WsfDisTransferOwnership>);
}

// virtual
WsfDisPduFactory::~WsfDisPduFactory()
{
   for (std::vector<CreateFunction*>::iterator it = mCreateFunctions.begin(); it != mCreateFunctions.end(); ++it)
   {
      delete *it;
   }
   mCreateFunctions.clear();
}

// User overrides this method to create their own DIS classes
// virtual
DisPdu* WsfDisPduFactory::UserCreate(DisPdu& aDisPdu, GenI& aGenI)
{
   DisPdu*         pduPtr  = nullptr;
   CreateFunction* funcPtr = mCreateFunctions[aDisPdu.GetPduType()];
   if (funcPtr != nullptr)
   {
      return funcPtr->Create(aDisPdu, aGenI, mInterfacePtr);
   }
   else
   {
      // Any PDU that we don't explicitly process is categorized as 'other'.
      // It is faster than allowing the base class to create a PDU of the
      // true type.
      pduPtr = new DisOther(aDisPdu, aGenI);
   }
   return pduPtr;
}

void WsfDisPduFactory::SetPduCreateFunction(int aPduType, CreateFunction* aFunctionPtr)
{
   if (mCreateFunctions[aPduType])
   {
      delete mCreateFunctions[aPduType];
   }
   mCreateFunctions[aPduType] = aFunctionPtr;
}
