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

#include "DisMinefieldQuery.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisMinefieldQuery::DisMinefieldQuery()
   : DisPdu()
{
   SetPduType(DisEnum::Pdu::Type::MinefieldQuery);
   SetProtocolFamily(DisEnum::Pdu::Family::Minefield);
   SetLength(DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS);
}

DisMinefieldQuery::DisMinefieldQuery(const DisMinefieldQuery& aSrc)
   : DisPdu(aSrc)
   , mMinefieldId(aSrc.mMinefieldId)
   , mRequestingSimulationId(aSrc.mRequestingSimulationId)
   , mMinefieldRequestID(aSrc.mMinefieldRequestID)
   , mPerimeterPointCount(aSrc.mPerimeterPointCount)
   , mSensorTypeCount(aSrc.mSensorTypeCount)
   , mDataFilter(aSrc.mDataFilter)
   , mRequestedMineType(aSrc.mRequestedMineType)
   , mPerimeterPointCoordinates(aSrc.mPerimeterPointCoordinates)
   , mSensorTypes(aSrc.mSensorTypes)
{
   GetLength();
}

DisMinefieldQuery::DisMinefieldQuery(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisMinefieldQuery::DisMinefieldQuery(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisMinefieldQuery* DisMinefieldQuery::Clone() const
{
   return new DisMinefieldQuery(*this);
}

void DisMinefieldQuery::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisMinefieldQuery::GetLength()
{
   DisUint16 octets = DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS;
   int       N      = mPerimeterPointCount;
   int       M      = mSensorTypeCount;
   int       P      = static_cast<int>((2 * (ceil(M / 2.0) * 2 - M)));

   octets += (8 * N + 2 * M + P);

   SetLength(octets);

   return octets;
}

void DisMinefieldQuery::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);

   DisUint8 pad8(0);

   aGenO << mMinefieldId;
   aGenO << mRequestingSimulationId;
   aGenO << mMinefieldRequestID;
   aGenO << mPerimeterPointCount;
   aGenO << pad8;
   aGenO << mSensorTypeCount;
   aGenO << mDataFilter;
   aGenO << mRequestedMineType;

   for (auto& it : mPerimeterPointCoordinates)
   {
      aGenO << it.first << it.second;
   }

   for (auto& it : mSensorTypes)
   {
      aGenO << it;
   }

   DisUint16 M = mSensorTypeCount;
   int       P = static_cast<int>((2 * (ceil(M / 2.0) * 2 - M)));

   for (int i = 0; i < P; i++)
   {
      aGenO << pad8;
   }
}

void DisMinefieldQuery::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();
   DisUint8  pad8(0);

   aGenI >> mMinefieldId;
   aGenI >> mRequestingSimulationId;
   aGenI >> mMinefieldRequestID;
   aGenI >> mPerimeterPointCount;
   mPerimeterPointCoordinates.reserve(mPerimeterPointCount);
   aGenI >> pad8;
   aGenI >> mSensorTypeCount;
   mSensorTypes.reserve(mSensorTypeCount);
   aGenI >> mDataFilter;
   aGenI >> mRequestedMineType;

   for (int i = 0; i < mPerimeterPointCount; i++)
   {
      DisFloat32 x, y;
      aGenI >> x >> y;
      mPerimeterPointCoordinates.emplace_back(x, y);
   }
   for (int i = 0; i < mSensorTypeCount; i++)
   {
      DisUint16 type;
      aGenI >> type;

      mSensorTypes.push_back(type);
   }

   DisUint16 M = mSensorTypeCount;
   DisUint16 P = (DisUint16)(2 * (ceil(M / 2.0) * 2 - M));
   for (int i = 0; i < P; i++)
   {
      DisUint8 temp;
      aGenI >> temp;
   }

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

bool DisMinefieldQuery::IsValid() const
{
   bool isValid =
      DisPdu::IsValid() && mMinefieldId.IsValid() && mRequestingSimulationId.IsValid() && mRequestedMineType.IsValid();

   for (const auto& it : mPerimeterPointCoordinates)
   {
      if (!ValidateScalar(it.first))
      {
         isValid = false;
      }
      if (!ValidateScalar(it.second))
      {
         isValid = false;
      }
   }

   return isValid;
}
