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

#include "DisReceiver.hpp"

#include "DisPduEnums.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisReceiver::DisReceiver()
   : DisPdu()
   , mEntityId(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRadioId(0)
   , mReceiverState(0)
   , mReceivedPower(0.0F)
   , mTransmitterEntityId()
   , mTransmitterRadioId(0)
{
   SetPduType(DisEnum::Pdu::Type::Receiver);
   SetProtocolFamily(DisEnum::Pdu::Family::RadioCommunication);
   SetLength(36); // 288 bits
}

DisReceiver::DisReceiver(const DisReceiver& aSrc)
   : DisPdu(aSrc)
   , mEntityId(aSrc.mEntityId)
   , mRadioId(aSrc.mRadioId)
   , mReceiverState(aSrc.mReceiverState)
   , mReceivedPower(aSrc.mReceivedPower)
   , mTransmitterEntityId(aSrc.mTransmitterEntityId)
   , mTransmitterRadioId(aSrc.mTransmitterRadioId)
{
   SetPduType(DisEnum::Pdu::Type::Receiver);
   SetProtocolFamily(DisEnum::Pdu::Family::RadioCommunication);
   SetLength(36); // 288 bits
}

DisReceiver::DisReceiver(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisReceiver::DisReceiver(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

// virtual
DisPdu* DisReceiver::Clone() const
{
   return new DisReceiver(*this);
}

// virtual
int DisReceiver::GetClass() const
{
   return (int)DisEnum::Pdu::Type::Receiver;
}

// virtual
const DisEntityId& DisReceiver::GetOriginatingEntity() const
{
   return mEntityId;
}

// virtual
void DisReceiver::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   DisReceiver::GetMemberData(aGenI);
}

// virtual
DisUint16 DisReceiver::GetLength()
{
   DisUint16 octetCount = 36; // 288 bits
   SetLength(octetCount);
   return octetCount;
}

// private
void DisReceiver::GetMemberData(GenI& aGenI)
{
   DisUint16 pad16(0);

   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   aGenI >> mEntityId;
   aGenI >> mRadioId;
   aGenI >> mReceiverState;
   aGenI >> pad16;
   aGenI >> mReceivedPower;
   aGenI >> mTransmitterEntityId;
   aGenI >> mTransmitterRadioId;

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

// virtual
void DisReceiver::Put(GenO& aGenO)
{
   DisUint16 pad16(0);

   GetLength(); // Ensure length is up-to-date
   DisPdu::Put(aGenO);
   aGenO << mEntityId;
   aGenO << mRadioId;
   aGenO << mReceiverState;
   aGenO << pad16;
   aGenO << mReceivedPower;
   aGenO << mTransmitterEntityId;
   aGenO << mTransmitterRadioId;
}

bool DisReceiver::IsValid() const
{
   bool isValid = true;
   if (!DisPdu::IsValid())
   {
      isValid = false;
   }
   if (!mEntityId.IsValid())
   {
      isValid = false;
   }
   if (!ValidateScalar(mReceivedPower))
   {
      isValid = false;
   }
   if (!mTransmitterEntityId.IsValid())
   {
      isValid = false;
   }
   return isValid;
}

void DisReceiver::Stream(std::ostream& aStream) const
{
   aStream << ToString();
}

std::string DisReceiver::ToString() const
{
   using namespace std;
   std::ostringstream ss("");

   ss << setiosflags(ios::fixed) << DisPdu::ToString() << '\n'
      << "Orig. Entity:    " << GetOriginatingEntity().ToString() << '\n'
      << "Entity Id:       " << GetEntityId().ToString() << '\n'
      << "Radio Id:        " << static_cast<unsigned int>(GetRadioId()) << '\n'
      << "Receiver State:  " << static_cast<unsigned int>(GetReceiverState()) << " ";

   switch (static_cast<unsigned int>(GetReceiverState()))
   {
   case DisReceiver::RsOff:
      ss << "(Off)\n";
      break;
   case DisReceiver::RsOnAndNotReceiving:
      ss << "(On and not receiveing)\n";
      break;
   case DisReceiver::RsOnAndReceiving:
      ss << "(On and receiving)\n";
      break;
   default:
      ss << "(Unknown)\n";
      break;
   }

   locale tmp = ss.getloc();
   ss.imbue(locale(""));
   float power_dBm = GetReceivedPower();
   float power_W   = (float)pow(10.0, ((power_dBm - 30.0) / 10.0));
   ss << "Received Power:  " << fixed << setprecision(3) << power_dBm << " dBm (" << setprecision(1) << power_W << " W)\n"
      << "Xmiter Id:       " << GetTransmitterEntityId().ToString() << '\n'
      << "Xmiter Radio Id: " << GetTransmitterRadioId();
   ss.imbue(tmp);
   return ss.str();
}
