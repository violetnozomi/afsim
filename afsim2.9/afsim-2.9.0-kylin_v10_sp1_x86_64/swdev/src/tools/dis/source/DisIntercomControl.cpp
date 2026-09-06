// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// *****************************************************************************

#include "DisIntercomControl.hpp"

#include "DisGroupAssignmentRecord.hpp"
#include "DisGroupDestinationRecord.hpp"
#include "DisPduEnums.hpp"
#include "DisSpecificDestinationRecord.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "UtCast.hpp"

DisIntercomControl::DisIntercomControl()
   : DisPdu()
   , mSourceIntercomReferenceId()
   , mMasterIntercomReferenceId()
   , mIntercomParameters()
{
   SetPduType(DisEnum::Pdu::Type::IntercomControl);
   SetProtocolFamily(DisEnum::Pdu::Family::RadioCommunication);
   SetLength(DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS);
}

DisIntercomControl::DisIntercomControl(const DisIntercomControl& aSrc)
   : DisPdu(aSrc)
   , mControlType(aSrc.mControlType)
   , mCommunicationsChannelType(aSrc.mCommunicationsChannelType)
   , mSourceIntercomReferenceId(aSrc.mSourceIntercomReferenceId)
   , mSourceIntercomNumber(aSrc.mSourceIntercomNumber)
   , mSourceLineId(aSrc.mSourceLineId)
   , mTransmitPriority(aSrc.mTransmitPriority)
   , mTransmitLineState(aSrc.mTransmitLineState)
   , mCommand(aSrc.mCommand)
   , mMasterIntercomReferenceId(aSrc.mMasterIntercomReferenceId)
   , mMasterIntercomNumber(aSrc.mMasterIntercomNumber)
   , mMasterChannelId(aSrc.mMasterChannelId)
   , mIntercomParametersLength(aSrc.mIntercomParametersLength)
{
   DisIntercomCommunicationsParametersRecord intercomCommunicationsParametersRecord = aSrc.mIntercomParameters;
   SetIntercomParametersRecord(intercomCommunicationsParametersRecord);
   GetLength();
}

DisIntercomControl::DisIntercomControl(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mSourceIntercomReferenceId()
   , mMasterIntercomReferenceId()
   , mIntercomParameters()
{
   GetMemberData(aGenI);
}

DisIntercomControl::DisIntercomControl(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisPdu* DisIntercomControl::Clone() const
{
   return new DisIntercomControl(*this);
}

int DisIntercomControl::GetClass() const
{
   return static_cast<int>(DisEnum::Pdu::Type::IntercomControl);
}

int DisIntercomControl::GetFamily() const
{
   return static_cast<int>(DisEnum::Pdu::Family::RadioCommunication);
}

DisEnum8 DisIntercomControl::GetControlType() const
{
   return mControlType;
}

DisUint8 DisIntercomControl::GetCommunicationsChannelType() const
{
   return mCommunicationsChannelType;
}

const DisEntityId& DisIntercomControl::GetSourceIntercomReferenceId() const
{
   return mSourceIntercomReferenceId;
}

DisUint16 DisIntercomControl::GetMasterIntercomNumber() const
{
   return mMasterIntercomNumber;
}

DisUint16 DisIntercomControl::GetSourceIntercomNumber() const
{
   return mSourceIntercomNumber;
}

DisUint8 DisIntercomControl::GetSourceLineId() const
{
   return mSourceLineId;
}

DisUint8 DisIntercomControl::GetTransmitPriority() const
{
   return mTransmitPriority;
}

DisEnum8 DisIntercomControl::GetTransmitLineState() const
{
   return mTransmitLineState;
}

DisEnum8 DisIntercomControl::GetCommand() const
{
   return mCommand;
}

const DisEntityId& DisIntercomControl::GetMasterIntercomReferenceId() const
{
   return mMasterIntercomReferenceId;
}

DisUint16 DisIntercomControl::GetMasterChannelId() const
{
   return mMasterChannelId;
}

DisUint32 DisIntercomControl::GetIntercomParametersLength() const
{
   return mIntercomParametersLength;
}

const DisIntercomCommunicationsParametersRecord& DisIntercomControl::GetIntercomParametersRecord() const
{
   return mIntercomParameters;
}

void DisIntercomControl::SetControlType(DisEnum8 aControlType)
{
   mControlType = aControlType;
}

void DisIntercomControl::SetCommunicationsChannelType(DisUint8 aCommunicationsChannelType)
{
   mCommunicationsChannelType = aCommunicationsChannelType;
}

void DisIntercomControl::SetSourceIntercomReferenceId(const DisEntityId& aSourceIntercomReferenceId)
{
   mSourceIntercomReferenceId = aSourceIntercomReferenceId;
}

void DisIntercomControl::SetMasterIntercomNumber(DisUint16 aMasterIntercomNumber)
{
   mMasterIntercomNumber = aMasterIntercomNumber;
}

void DisIntercomControl::SetSourceIntercomNumber(DisUint16 aSourceIntercomNumber)
{
   mSourceIntercomNumber = aSourceIntercomNumber;
}

void DisIntercomControl::SetSourceLineId(DisUint8 aSourceLineId)
{
   mSourceLineId = aSourceLineId;
}

void DisIntercomControl::SetTransmitPriority(DisUint8 aTransmitPriority)
{
   mTransmitPriority = aTransmitPriority;
}

void DisIntercomControl::SetTransmitLineState(DisEnum8 aTransmitLineState)
{
   mTransmitLineState = aTransmitLineState;
}

void DisIntercomControl::SetCommand(DisEnum8 aCommand)
{
   mCommand = aCommand;
}

void DisIntercomControl::SetMasterIntercomReferenceId(const DisEntityId& aMasterIntercomReferenceId)
{
   mMasterIntercomReferenceId = aMasterIntercomReferenceId;
}

void DisIntercomControl::SetMasterChannelId(DisUint16 aMasterChannelId)
{
   mMasterChannelId = aMasterChannelId;
}

void DisIntercomControl::SetIntercomParametersLength(DisUint32 aIntercomParametersLength)
{
   mIntercomParametersLength = aIntercomParametersLength;
}

void DisIntercomControl::SetIntercomParametersRecord(DisIntercomCommunicationsParametersRecord& aIntercomParametersRecord)
{
   mIntercomParameters = aIntercomParametersRecord;
}

void DisIntercomControl::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisIntercomControl::GetLength()
{
   DisUint16 length = DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS + mIntercomParameters.GetLength();
   SetLength(length);
   return length;
}

void DisIntercomControl::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);
   aGenO << mControlType;
   aGenO << mCommunicationsChannelType;
   mSourceIntercomReferenceId.Put(aGenO);
   aGenO << mSourceIntercomNumber;
   aGenO << mSourceLineId;
   aGenO << mTransmitPriority;
   aGenO << mTransmitLineState;
   aGenO << mCommand;
   aGenO << mMasterIntercomReferenceId;
   aGenO << mMasterIntercomNumber;
   aGenO << mMasterChannelId;
   aGenO << mIntercomParametersLength;
   mIntercomParameters.Put(aGenO);
}

std::string DisIntercomControl::ToString() const
{
   std::string returnString = DisPdu::ToString();
   returnString += "===== DisIntercomControl =====\n";
   returnString += "Control Type" + std::to_string(mControlType) + '\n';
   returnString += "Communications Channel Type:  " + std::to_string(mCommunicationsChannelType) + '\n';
   returnString += "Source Intercom Reference Id: " + mSourceIntercomReferenceId.ToString() + '\n';
   returnString += "Source Intercom Number:       " + std::to_string(mSourceIntercomNumber) + '\n';
   returnString += "Source Line Id:               " + std::to_string(mSourceLineId) + '\n';
   returnString += "Transmit Priority:            " + std::to_string(mTransmitPriority) + '\n';
   returnString += "Transmit Line State:          " + std::to_string(mTransmitLineState) + '\n';
   returnString += "Command:                      " + std::to_string(mCommand) + '\n';
   returnString += "Master Intercom Reference Id: " + mMasterIntercomReferenceId.ToString() + '\n';
   returnString += "Master Intercom Number:       " + std::to_string(mMasterIntercomNumber) + '\n';
   returnString += "Master Channel Id:            " + std::to_string(mMasterChannelId) + '\n';
   returnString += "Intercom Parameter:           " + mIntercomParameters.GetRecordSpecificField().ToString();
   returnString += "=== End DisIntercomControl ===\n";
   return returnString;
}

void DisIntercomControl::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

bool DisIntercomControl::IsValid() const
{
   return DisPdu::IsValid() && mSourceIntercomReferenceId.IsValid() && mIntercomParameters.IsValid();
}

void DisIntercomControl::GetMemberData(GenI& aGenI)
{
   aGenI >> mControlType;
   aGenI >> mCommunicationsChannelType;
   mSourceIntercomReferenceId.Get(aGenI);
   aGenI >> mSourceIntercomNumber;
   aGenI >> mSourceLineId;
   aGenI >> mTransmitPriority;
   aGenI >> mTransmitLineState;
   aGenI >> mCommand;
   aGenI >> mMasterIntercomReferenceId;
   aGenI >> mMasterIntercomNumber;
   aGenI >> mMasterChannelId;
   aGenI >> mIntercomParametersLength;
   mIntercomParameters.Get(aGenI);
   GetLength();
}
