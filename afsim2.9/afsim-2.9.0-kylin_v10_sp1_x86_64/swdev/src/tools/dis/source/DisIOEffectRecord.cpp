//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "DisIOEffectRecord.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisIOEffectRecord::DisIOEffectRecord()
   : DisIOBaseRecord(DisIORecordTypeEnum::IO_EFFECT)
{
   SetRecordLength(cRECORD_LENGTH);
}

DisIOEffectRecord::DisIOEffectRecord(const DisIOEffectRecord& aSrc)
   : DisIOBaseRecord(DisIORecordTypeEnum::IO_EFFECT)
   , mIOStatus(aSrc.mIOStatus)
   , mIOLinkType(aSrc.mIOLinkType)
   , mIOEffect(aSrc.mIOEffect)
   , mIOEffectDutyCycle(aSrc.mIOEffectDutyCycle)
   , mIOEffectDuration(aSrc.mIOEffectDuration)
   , mIOProcess(aSrc.mIOProcess)
{
   SetRecordLength(cRECORD_LENGTH);
}

DisIOEffectRecord::DisIOEffectRecord(const DisIOBaseRecord& aBaseRecord, GenI& aGenI)
   : DisIOBaseRecord(aBaseRecord)
{
   Get(aGenI);
}

DisIOEffectRecord::DisIOEffectRecord(GenI& aGenI)
   : DisIOBaseRecord(DisIORecordTypeEnum::IO_EFFECT)
{
   GetMemberData(aGenI);
}

DisIOEffectRecord* DisIOEffectRecord::Clone() const
{
   return new DisIOEffectRecord(*this);
}

DisIORecordTypeEnum DisIOEffectRecord::GetIORecordType() const
{
   return DisIORecordTypeEnum::IO_EFFECT;
}

const char* DisIOEffectRecord::GetScriptClassName() const
{
   return "DisIoEffectRecord";
}

void DisIOEffectRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

void DisIOEffectRecord::Put(GenO& aGenO) const
{
   DisUint16 pad16(0);

   DisIOBaseRecord::Put(aGenO);

   aGenO << mIOStatus;
   aGenO << mIOLinkType;
   aGenO << mIOEffect;
   aGenO << mIOEffectDutyCycle;
   aGenO << mIOEffectDuration;
   aGenO << mIOProcess;
   aGenO << pad16;
}

DisUint16 DisIOEffectRecord::GetLength()
{
   return GetRecordLength();
}

std::string DisIOEffectRecord::ToString() const
{
   std::string returnString = DisIOBaseRecord::ToString();
   returnString += "IO EFFECT RECORD\n";
   returnString += "IO Status:               " + std::to_string(mIOStatus) + "\n";
   returnString += "IO Link Type:            " + std::to_string(mIOLinkType) + "\n";
   returnString += "IO Effect:               " + std::to_string(mIOEffect) + "\n";
   returnString += "IO Effect Duty Cycle:    " + std::to_string(mIOEffectDutyCycle) + "\n";
   returnString += "IO Effect Duration:      " + std::to_string(mIOEffectDuration) + "\n";
   returnString += "IO Process:              " + std::to_string(mIOProcess) + "\n";
   returnString += "End IO EFFECT RECORD\n";
   return returnString;
}

void DisIOEffectRecord::GetMemberData(GenI& aGenI)
{
   DisUint16 pad16(0);
   DisIOBaseRecord::Get(aGenI);
   aGenI >> mIOStatus;
   aGenI >> mIOLinkType;
   aGenI >> mIOEffect;
   aGenI >> mIOEffectDutyCycle;
   aGenI >> mIOEffectDuration;
   aGenI >> mIOProcess;
   aGenI >> pad16;
}

bool DisIOEffectRecord::IsValid() const
{
   return DisRecordHeader::IsValid();
}

void DisIOEffectRecord::SetIOStatus(const DisEnum8 aIOStatus)
{
   mIOStatus = aIOStatus;
}

void DisIOEffectRecord::SetIOLinkType(const DisEnum8 aIOLinkType)
{
   mIOLinkType = aIOLinkType;
}

void DisIOEffectRecord::SetIOEffect(const DisEnum8 aIOEffect)
{
   mIOEffect = aIOEffect;
}

void DisIOEffectRecord::SetIOEffectDutyCycle(const DisUint8 aIOEffectDutyCycle)
{
   mIOEffectDutyCycle = aIOEffectDutyCycle;
}

void DisIOEffectRecord::SetIOEffectDuration(const DisUint16 aIOEffectDuration)
{
   mIOEffectDuration = aIOEffectDuration;
}

void DisIOEffectRecord::SetIOProcess(const DisEnum16 aIOProcess)
{
   mIOProcess = aIOProcess;
}

DisEnum8 DisIOEffectRecord::GetIOStatus() const
{
   return mIOStatus;
}

DisEnum8 DisIOEffectRecord::GetIOLinkType() const
{
   return mIOLinkType;
}

DisEnum8 DisIOEffectRecord::GetIOEffect() const
{
   return mIOEffect;
}

DisUint8 DisIOEffectRecord::GetIOEffectDutyCycle() const
{
   return mIOEffectDutyCycle;
}

DisUint16 DisIOEffectRecord::GetIOEffectDuration() const
{
   return mIOEffectDuration;
}

DisEnum16 DisIOEffectRecord::GetIOProcess() const
{
   return mIOProcess;
}

bool DisIOEffectRecord::operator==(const DisRecordHeader& aOther) const
{
   const DisIOEffectRecord* other = dynamic_cast<const DisIOEffectRecord*>(&aOther);

   if (!other)
   {
      return false;
   }

   return mIOStatus == other->mIOStatus && mIOLinkType == other->mIOLinkType && mIOEffect == other->mIOEffect &&
          mIOEffectDutyCycle == other->mIOEffectDutyCycle && mIOEffectDuration == other->mIOEffectDuration &&
          mIOProcess == other->mIOProcess;
}

void DisIOEffectRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << "\n";
}
