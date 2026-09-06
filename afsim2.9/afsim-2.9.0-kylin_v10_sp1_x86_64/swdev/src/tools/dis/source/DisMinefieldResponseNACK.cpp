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

#include "DisMinefieldResponseNACK.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisMinefieldResponseNACK::DisMinefieldResponseNACK()
   : DisPdu()
{
   SetPduType(DisEnum::Pdu::Type::MinefieldResponseNACK);
   SetProtocolFamily(DisEnum::Pdu::Family::Minefield);
   SetLength(DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS);
}

DisMinefieldResponseNACK::DisMinefieldResponseNACK(const DisMinefieldResponseNACK& aSrc)
   : DisPdu(aSrc)
   , mMinefieldId(aSrc.mMinefieldId)
   , mRequestingSimulationId(aSrc.mRequestingSimulationId)
   , mMinefieldRequestID(aSrc.mMinefieldRequestID)
   , mMissingPDUNumber(aSrc.mMissingPDUNumber)
   , mMissingPDUSequences(aSrc.mMissingPDUSequences)
{
   GetLength();
}

DisMinefieldResponseNACK::DisMinefieldResponseNACK(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisMinefieldResponseNACK::DisMinefieldResponseNACK(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisMinefieldResponseNACK* DisMinefieldResponseNACK::Clone() const
{
   return new DisMinefieldResponseNACK(*this);
}

void DisMinefieldResponseNACK::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisMinefieldResponseNACK::GetLength()
{
   DisUint16 octets = DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS;
   int       N      = mMissingPDUNumber;
   int       P      = static_cast<int>(((ceil((N + 2) / 4.0) * 4 - (N + 2))));
   octets += (N + P);

   SetLength(octets);
   return octets;
}

void DisMinefieldResponseNACK::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);

   DisUint8 pad8(0);

   aGenO << mMinefieldId;
   aGenO << mRequestingSimulationId;
   aGenO << mMinefieldRequestID;
   aGenO << mMissingPDUNumber;

   for (auto& it : mMissingPDUSequences)
   {
      aGenO << it;
   }

   int N = mMissingPDUNumber;
   int P = static_cast<int>(((ceil((N + 2) / 4.0) * 4 - (N + 2))));

   for (int i = 0; i < P; i++)
   {
      aGenO << pad8;
   }
}

void DisMinefieldResponseNACK::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   aGenI >> mMinefieldId;
   aGenI >> mRequestingSimulationId;
   aGenI >> mMinefieldRequestID;
   aGenI >> mMissingPDUNumber;
   mMissingPDUSequences.reserve(mMissingPDUNumber);

   for (int i = 0; i < mMissingPDUNumber; i++)
   {
      DisUint8 num = 0;
      aGenI >> num;

      mMissingPDUSequences.push_back(num);
   }

   int N = mMissingPDUNumber;
   int P = static_cast<int>((ceil((N + 2) / 4.0) * 4 - (N + 2)));

   for (int i = 0; i < P; i++)
   {
      DisUint8 temp = 0;
      aGenI >> temp;
   }

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

bool DisMinefieldResponseNACK::IsValid() const
{
   return DisPdu::IsValid() && mMinefieldId.IsValid() && mRequestingSimulationId.IsValid();
}
