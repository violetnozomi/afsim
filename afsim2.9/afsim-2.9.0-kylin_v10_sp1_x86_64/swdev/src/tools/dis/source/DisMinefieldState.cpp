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

#include "DisMinefieldState.hpp"

#include "DisPduEnums.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisMinefieldState::DisMinefieldState()
   : DisPdu()
{
   SetPduType(DisEnum::Pdu::Type::MinefieldState);
   SetProtocolFamily(DisEnum::Pdu::Family::Minefield);
   SetLength(DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS);
}

DisMinefieldState::DisMinefieldState(const DisMinefieldState& aSrc)
   : DisPdu(aSrc)
   , mMinefieldId(aSrc.mMinefieldId)
   , mMinefieldSequenceNumber(aSrc.mMinefieldSequenceNumber)
   , mForceId(aSrc.mForceId)
   , mPerimeterPointCount(aSrc.mPerimeterPointCount)
   , mMinefieldType(aSrc.mMinefieldType)
   , mMineTypeCount(aSrc.mMineTypeCount)
   , mMinefieldLocation(aSrc.mMinefieldLocation)
   , mMinefieldOrientation(aSrc.mMinefieldOrientation)
   , mAppearance(aSrc.mAppearance)
   , mProtocolMode(aSrc.mProtocolMode)
   , mPerimeterPointCoordinates(aSrc.mPerimeterPointCoordinates)
   , mMineTypes(aSrc.mMineTypes)
{
   GetLength();
}

DisMinefieldState::DisMinefieldState(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisMinefieldState::DisMinefieldState(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisMinefieldState* DisMinefieldState::Clone() const
{
   return new DisMinefieldState(*this);
}

int DisMinefieldState::GetClass() const
{
   return static_cast<int>(DisEnum::Pdu::Type::MinefieldState);
}

const DisEntityId& DisMinefieldState::GetOriginatingEntity() const
{
   return mMinefieldId;
}
void DisMinefieldState::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisMinefieldState::GetLength()
{
   DisUint16 octets = DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS + 64 * (mPerimeterPointCount + mMineTypeCount);

   SetLength(octets);

   return octets;
}

void DisMinefieldState::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);

   aGenO << mMinefieldId;
   aGenO << mMinefieldSequenceNumber;
   aGenO << mForceId;
   aGenO << mPerimeterPointCount;
   aGenO << mMinefieldType;
   aGenO << mMineTypeCount;
   aGenO << mMinefieldLocation[0] << mMinefieldLocation[1] << mMinefieldLocation[2];
   aGenO << mMinefieldOrientation[0] << mMinefieldOrientation[1] << mMinefieldOrientation[2];
   aGenO << mAppearance;
   aGenO << mProtocolMode;

   for (auto& it : mPerimeterPointCoordinates)
   {
      aGenO << it[0] << it[1];
   }

   for (auto& it : mMineTypes)
   {
      aGenO << it;
   }
}

void DisMinefieldState::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   aGenI >> mMinefieldId;
   aGenI >> mMinefieldSequenceNumber;
   aGenI >> mForceId;
   aGenI >> mPerimeterPointCount;
   mPerimeterPointCoordinates.reserve(mPerimeterPointCount);
   aGenI >> mMinefieldType;
   aGenI >> mMineTypeCount;
   mMineTypes.reserve(mMineTypeCount);
   aGenI >> mMinefieldLocation[0] >> mMinefieldLocation[1] >> mMinefieldLocation[2];
   aGenI >> mMinefieldOrientation[0] >> mMinefieldOrientation[1] >> mMinefieldOrientation[2];
   aGenI >> mAppearance;
   aGenI >> mProtocolMode;
   for (int i = 0; i < mPerimeterPointCount; i++)
   {
      DisFloat32 x, y;
      aGenI >> x >> y;
      mPerimeterPointCoordinates.emplace_back(x, y);
   }
   for (int i = 0; i < mMineTypeCount; i++)
   {
      DisEntityType type;
      aGenI >> type;

      mMineTypes.push_back(std::move(type));
   }
   ////// Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

bool DisMinefieldState::IsValid() const
{
   bool isValid = DisPdu::IsValid() && mMinefieldId.IsValid() && mMinefieldType.IsValid() &&
                  ValidateScalar(mMinefieldLocation[0]) && ValidateScalar(mMinefieldLocation[1]) &&
                  ValidateScalar(mMinefieldLocation[2]) && ValidateScalar(mMinefieldOrientation[0]) &&
                  ValidateScalar(mMinefieldOrientation[1]) && ValidateScalar(mMinefieldOrientation[2]);

   for (const auto& it : mPerimeterPointCoordinates)
   {
      if (!ValidateScalar(it[0]))
      {
         isValid = false;
      }
      if (!ValidateScalar(it[1]))
      {
         isValid = false;
      }
   }

   for (const auto& it : mMineTypes)
   {
      if (!it.IsValid())
      {
         isValid = false;
      }
   }
   return isValid;
}
