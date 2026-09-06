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


#include "DisUnderwaterAcoustic.hpp"

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisUnderwaterAcoustic::DisUnderwaterAcoustic()
   : DisPdu()
   , mEmitterId()
   , mEventId()
   , mStateUpdate(0)
   , mIndex(0)
   , mPropulsionPlantConfig(0)
   , mShaftCount(0)
   , mAPACount(0)
   , mSystemCount(0)
{
   SetPduType(DisEnum::Pdu::Type::UnderwaterAcoustic);
   SetProtocolFamily(DisEnum::Pdu::Family::DistributedEmissionRegeneration);
   SetLength(32); // 256 bits / 8 bits = 32 bytes
}

DisUnderwaterAcoustic::DisUnderwaterAcoustic(const DisUnderwaterAcoustic& aSrc)
   : DisPdu(aSrc)
   , mEmitterId(aSrc.mEmitterId)
   , mEventId(aSrc.mEventId)
   , mStateUpdate(aSrc.mStateUpdate)
   , mIndex(aSrc.mIndex)
   , mPropulsionPlantConfig(aSrc.mPropulsionPlantConfig)
   , mShaftCount(aSrc.mShaftCount)
   , mAPACount(aSrc.mAPACount)
   , mSystemCount(aSrc.mSystemCount)
{
}

DisUnderwaterAcoustic::DisUnderwaterAcoustic(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mEmitterId()
   , mEventId()
   , mStateUpdate(0)
   , mIndex(0)
   , mPropulsionPlantConfig(0)
   , mShaftCount(0)
   , mAPACount(0)
   , mSystemCount(0)
{
   GetMemberData(aGenI);
}

DisUnderwaterAcoustic::DisUnderwaterAcoustic(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisUnderwaterAcoustic::~DisUnderwaterAcoustic()
{
   // The destructor for "mSystemList" will take of itself and the
   // pointers it owns.
}

// virtual
DisPdu* DisUnderwaterAcoustic::Clone() const
{
   return new DisUnderwaterAcoustic(*this);
}

// virtual
const DisEntityId& DisUnderwaterAcoustic::GetOriginatingEntity() const
{
   return mEmitterId;
}

// Virtual
int DisUnderwaterAcoustic::GetClass() const
{
   return (int)DisEnum::Pdu::Type::UnderwaterAcoustic;
}

void DisUnderwaterAcoustic::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   DisUnderwaterAcoustic::GetMemberData(aGenI);
}

DisUint16 DisUnderwaterAcoustic::GetLength()
{
   // Update count
   UpdateShaftCount();
   UpdateAPACount();
   UpdateSystemCount();

   DisUint16 octetCount = 32; // 256 bits / 8 bits = 32 bytes

   std::vector<DisShaft*>::const_iterator shaftIter(mShaftList.GetBegin());
   while (shaftIter != mShaftList.GetEnd())
   {
      octetCount += (*shaftIter++)->GetLength();
   }

   std::vector<DisAPA*>::const_iterator apaIter(mAPAList.GetBegin());
   while (apaIter != mAPAList.GetEnd())
   {
      octetCount += (*apaIter++)->GetLength();
   }

   std::vector<DisUASystem*>::const_iterator systemIter(mSystemList.GetBegin());
   while (systemIter != mSystemList.GetEnd())
   {
      octetCount += (*systemIter++)->GetLength();
   }

   SetLength(octetCount);
   return octetCount;
}

void DisUnderwaterAcoustic::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();
   DisUint8  pad8;

   RemoveAllShafts();
   RemoveAllAPAs();
   RemoveAllSystems();

   aGenI >> mEmitterId;
   aGenI >> mEventId;
   aGenI >> mStateUpdate;
   aGenI >> mIndex;
   aGenI >> pad8;
   aGenI >> mPropulsionPlantConfig;
   aGenI >> mShaftCount;
   aGenI >> mAPACount;
   aGenI >> mSystemCount;
   lengthToRead -= 32; // Length of fixed portion (256 bits)

   int i;
   if (mShaftCount != 0)
   {
      mShaftList.Reserve(mShaftCount);
      for (i = 0; i < mShaftCount; ++i)
      {
         DisShaft* shaft = new DisShaft();
         aGenI >> *shaft;
         mShaftList.Add(shaft); // List takes ownership
         lengthToRead -= shaft->GetLengthRead();
      }
   }

   if (mAPACount != 0)
   {
      mAPAList.Reserve(mAPACount);
      for (i = 0; i < mAPACount; ++i)
      {
         DisAPA* apa = new DisAPA();
         aGenI >> *apa;
         mAPAList.Add(apa); // List takes ownership
         lengthToRead -= apa->GetLengthRead();
      }
   }

   if (mSystemCount != 0)
   {
      mSystemList.Reserve(mSystemCount);
      for (i = 0; i < mSystemCount; ++i)
      {
         DisUASystem* system = new DisUASystem();
         aGenI >> *system;
         mSystemList.Add(system); // List takes ownership
         lengthToRead -= system->GetLengthRead();
      }
   }

   // Skip any 'extra' data
   ReadExtraData(aGenI, lengthToRead);
}

void DisUnderwaterAcoustic::Put(GenO& aGenO)
{
   DisUint8 pad8(0);

   GetLength(); // Ensure the length is current

   DisPdu::Put(aGenO);
   aGenO << mEmitterId;
   aGenO << mEventId;
   aGenO << mStateUpdate;
   aGenO << pad8;
   aGenO << mIndex;
   aGenO << mPropulsionPlantConfig;
   aGenO << mShaftCount;
   aGenO << mAPACount;
   aGenO << mSystemCount;

   std::vector<DisShaft*>::const_iterator shaftIter(mShaftList.GetBegin());
   while (shaftIter != mShaftList.GetEnd())
   {
      aGenO << **shaftIter++;
   }

   std::vector<DisAPA*>::const_iterator apaIter(mAPAList.GetBegin());
   while (apaIter != mAPAList.GetEnd())
   {
      aGenO << **apaIter++;
   }

   std::vector<DisUASystem*>::const_iterator systemIter(mSystemList.GetBegin());
   while (systemIter != mSystemList.GetEnd())
   {
      aGenO << **systemIter++;
   }
}

bool DisUnderwaterAcoustic::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!DisPdu::IsValid())
      isValid = false;
   if (!mEmitterId.IsValid())
      isValid = false;
   if (!mEventId.IsValid())
      isValid = false;

   std::vector<DisShaft*>::const_iterator shaftIter(mShaftList.GetBegin());
   while (shaftIter != mShaftList.GetEnd())
   {
      if (!(*shaftIter++)->IsValid())
         isValid = false;
   }

   std::vector<DisAPA*>::const_iterator apaIter(mAPAList.GetBegin());
   while (apaIter != mAPAList.GetEnd())
   {
      if (!(*apaIter++)->IsValid())
         isValid = false;
   }

   std::vector<DisUASystem*>::const_iterator systemIter(mSystemList.GetBegin());
   while (systemIter != mSystemList.GetEnd())
   {
      if (!(*systemIter++)->IsValid())
         isValid = false;
   }

   return isValid;
}
