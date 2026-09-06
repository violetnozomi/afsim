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

#include "DisDesignator.hpp"

#include "DisDeadReckoningEnums.hpp"
#include "DisPduEnums.hpp"
#include "DisValidationUtils.hpp"
#include "GenIO.hpp"

DisDesignator::DisDesignator()
   : DisPdu()
   , mDesignatingEntity()
   , mCodeName(0)
   , mDesignatedEntity()
   , mCode(0)
   , mPower(0.0F)
   , mWavelength(0.0F)
   , mDeadReckoningAlgorithm(DisEnum::DeadReckoning::cOTHER)
{
   SetPduType(DisEnum::Pdu::Type::Designator);
   SetProtocolFamily(DisEnum::Pdu::Family::DistributedEmissionRegeneration);
   mOffset[0]   = 0.0F;
   mOffset[1]   = 0.0F;
   mOffset[2]   = 0.0F;
   mLocation[0] = 0.0;
   mLocation[1] = 0.0;
   mLocation[2] = 0.0;
   mAcc[0]      = 0.0F;
   mAcc[1]      = 0.0F;
   mAcc[2]      = 0.0F;
   SetLength(88); // 704 bits
}

DisDesignator::DisDesignator(const DisDesignator& aSrc)
   : DisPdu(aSrc)
   , mDesignatingEntity(aSrc.mDesignatingEntity)
   , mCodeName(aSrc.mCodeName)
   , mDesignatedEntity(aSrc.mDesignatedEntity)
   , mCode(aSrc.mCode)
   , mPower(aSrc.mPower)
   , mWavelength(aSrc.mWavelength)
   , mDeadReckoningAlgorithm(aSrc.mDeadReckoningAlgorithm)
{
   mOffset[0]   = aSrc.mOffset[0];
   mOffset[1]   = aSrc.mOffset[1];
   mOffset[2]   = aSrc.mOffset[2];
   mLocation[0] = aSrc.mLocation[0];
   mLocation[1] = aSrc.mLocation[1];
   mLocation[2] = aSrc.mLocation[2];
   mAcc[0]      = aSrc.mAcc[0];
   mAcc[1]      = aSrc.mAcc[1];
   mAcc[2]      = aSrc.mAcc[2];
}

DisDesignator::DisDesignator(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisDesignator::DisDesignator(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisDesignator::~DisDesignator() {}

// virtual
DisPdu* DisDesignator::Clone() const
{
   return new DisDesignator(*this);
}

// virtual
const DisEntityId& DisDesignator::GetOriginatingEntity() const
{
   return mDesignatingEntity;
}

// virtual
int DisDesignator::GetClass() const
{
   return (int)DisEnum::Pdu::Type::Designator;
}

// virtual
void DisDesignator::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

// virtual
DisUint16 DisDesignator::GetLength()
{
   DisUint16 octetLength = 88;
   SetLength(octetLength);
   return octetLength;
}

void DisDesignator::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();
   DisUint16 pad16;
   DisUint8  pad8;

   aGenI >> mDesignatingEntity;
   aGenI >> mCodeName;
   aGenI >> mDesignatedEntity;
   aGenI >> mCode;
   aGenI >> mPower;
   aGenI >> mWavelength;
   aGenI >> mOffset[0] >> mOffset[1] >> mOffset[2];
   aGenI >> mLocation[0] >> mLocation[1] >> mLocation[2];
   aGenI >> mDeadReckoningAlgorithm;
   aGenI >> pad16 >> pad8;
   aGenI >> mAcc[0] >> mAcc[1] >> mAcc[2];

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

// virtual
void DisDesignator::Put(GenO& aGenO)
{
   DisUint16 pad16(0);
   DisUint8  pad8(0);

   DisPdu::Put(aGenO);
   aGenO << mDesignatingEntity;
   aGenO << mCodeName;
   aGenO << mDesignatedEntity;
   aGenO << mCode;
   aGenO << mPower;
   aGenO << mWavelength;
   aGenO << mOffset[0] << mOffset[1] << mOffset[2];
   aGenO << mLocation[0] << mLocation[1] << mLocation[2];
   aGenO << mDeadReckoningAlgorithm;
   aGenO << pad16 << pad8;
   aGenO << mAcc[0] << mAcc[1] << mAcc[2];
}

bool DisDesignator::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!DisPdu::IsValid())
      isValid = false;
   if (!mDesignatingEntity.IsValid())
      isValid = false;
   if (!mDesignatedEntity.IsValid())
      isValid = false;
   if (!ValidateScalar(mPower))
      isValid = false;
   if (!ValidateScalar(mWavelength))
      isValid = false;
   if (!ValidateScalar(mOffset[0]))
      isValid = false;
   if (!ValidateScalar(mOffset[1]))
      isValid = false;
   if (!ValidateScalar(mOffset[2]))
      isValid = false;
   if (!ValidateScalar(mLocation[0]))
      isValid = false;
   if (!ValidateScalar(mLocation[1]))
      isValid = false;
   if (!ValidateScalar(mLocation[2]))
      isValid = false;
   if (!ValidateScalar(mAcc[0]))
      isValid = false;
   if (!ValidateScalar(mAcc[1]))
      isValid = false;
   if (!ValidateScalar(mAcc[2]))
      isValid = false;
   return isValid;
}
