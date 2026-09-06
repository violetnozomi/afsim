// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "DisAggregateState.hpp"

#include "DisPduEnums.hpp"

DisAggregateState::DisAggregateState()
   : DisPdu()
{
   SetPduType(DisEnum::Pdu::Type::AggregateState);
   SetProtocolFamily(DisEnum::Pdu::Family::EntityManagement);

   for (int i = 0; i < 32; i++)
   {
      mAggregateMarking[i] = 0;
   }

   for (int i = 0; i < 3; ++i)
   {
      mDimensions[i]   = 0;
      mOrientation[i]  = 0;
      mCenterOfMass[i] = 0;
      mVelocity[i]     = 0;
   }
   SetLength(DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS);
}

DisAggregateState::DisAggregateState(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisAggregateState::DisAggregateState(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisAggregateState::DisAggregateState(const DisAggregateState& aSrc)
   : DisPdu(aSrc)
   , mAggregateId(aSrc.mAggregateId)
   , mForceId(aSrc.mForceId)
   , mAggregateState(aSrc.mAggregateState)
   , mAggregateType(aSrc.mAggregateType)
   , mFormation(aSrc.mFormation)
   , mNumberOfAggregateIds(aSrc.mNumberOfAggregateIds)
   , mNumberOfEntityIds(aSrc.mNumberOfEntityIds)
   , mNumberofSilentAggregateSystems(aSrc.mNumberofSilentAggregateSystems)
   , mNumberOfSilentEntitySystems(aSrc.mNumberOfSilentEntitySystems)
   , mAggregateIds(aSrc.mAggregateIds)
   , mEntityIds(aSrc.mEntityIds)
   , mSilentAggregateSystems(aSrc.mSilentAggregateSystems)
   , mSilentEntitySystems(aSrc.mSilentEntitySystems)
   , mNumberOfVariableDatumRecords(aSrc.mNumberOfVariableDatumRecords)
   , mVariableDatumRecords(aSrc.mVariableDatumRecords)
{
   SetPduType(DisEnum::Pdu::Type::AggregateState);
   SetProtocolFamily(DisEnum::Pdu::Family::EntityManagement);
   for (int i = 0; i < 32; i++)
   {
      mAggregateMarking[i] = aSrc.mAggregateMarking[i];
   }

   for (int i = 0; i < 3; ++i)
   {
      mDimensions[i]   = aSrc.mDimensions[i];
      mOrientation[i]  = aSrc.mOrientation[i];
      mCenterOfMass[i] = aSrc.mCenterOfMass[i];
      mVelocity[i]     = aSrc.mVelocity[i];
   }
   GetLength();
}

DisAggregateState::~DisAggregateState()
{
   mSilentAggregateSystems.RemoveAll();
   mSilentEntitySystems.RemoveAll();
   mVariableDatumRecords.RemoveAll();
}

DisAggregateState* DisAggregateState::Clone() const
{
   return new DisAggregateState(*this);
}

int DisAggregateState::GetClass() const
{
   return DisEnum::Pdu::Type::AggregateState;
}

int DisAggregateState::GetFamily() const
{
   return DisEnum::Pdu::Family::EntityManagement;
}

void DisAggregateState::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisAggregateState::GetLength()
{
   // The length of this pdu is the sum of the header, the non-record
   // values, and the records themselves
   DisUint16 length = DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS;
   length += 6 * mNumberOfAggregateIds; // aggregate id size
   length += 6 * mNumberOfEntityIds;    // entity id size
   length += (DisUint16)(2 * std::ceil((mNumberOfAggregateIds + mNumberOfEntityIds) / 4) * 4 -
                         (mNumberOfAggregateIds + mNumberOfEntityIds)); // padding size
   length += 12 * mNumberofSilentAggregateSystems;                      // length of silent aggregate systems

   // length of silent entity systems
   for (auto it = mSilentEntitySystems.GetBegin(); it != mSilentEntitySystems.GetEnd(); it++)
   {
      length += (**it++).GetLength();
   }

   // length of variable datum records
   for (auto it = mVariableDatumRecords.GetBegin(); it != mVariableDatumRecords.GetEnd(); it++)
   {
      length += (**it++).GetLength();
   }
   SetLength(length);
   return length;
}

void DisAggregateState::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);
   // mAggregateId.Put(aGenO);
   aGenO << mAggregateId;
   aGenO << mForceId;
   aGenO << mAggregateState;
   mAggregateType.Put(aGenO);
   aGenO << mFormation;

   for (int i = 0; i < 32; i++)
   {
      aGenO << mAggregateMarking[i];
   }

   aGenO << mDimensions[0] << mDimensions[1] << mDimensions[2];
   aGenO << mOrientation[0] << mOrientation[1] << mOrientation[2];
   aGenO << mCenterOfMass[0] << mCenterOfMass[1] << mCenterOfMass[2];
   aGenO << mVelocity[0] << mVelocity[1] << mVelocity[2];
   aGenO << mNumberOfAggregateIds;
   aGenO << mNumberOfEntityIds;
   aGenO << mNumberofSilentAggregateSystems;
   aGenO << mNumberOfSilentEntitySystems;

   for (DisEntityId currentEntity : mAggregateIds)
   {
      currentEntity.Put(aGenO);
   }

   for (DisEntityId currentEntity : mEntityIds)
   {
      currentEntity.Put(aGenO);
   }

   // Padding size is calculated from the equation given on page 427 of the IEEE 1278.1-2012 spec
   // number of padding octets: ceiling((W + J)/4) * 4 - (W + J) (octets)
   int numberOfPaddingOctets = static_cast<int>(
      (std::ceil((mNumberOfAggregateIds + mNumberOfEntityIds) / 4) * 4 - (mNumberOfAggregateIds + mNumberOfEntityIds)));
   DisUint8 padding = 0;

   // Pad up to 32 bits
   for (int i = 0; i < numberOfPaddingOctets; i++)
   {
      aGenO << padding; // padding size
   }

   PutRecordData(aGenO);
}

bool DisAggregateState::IsValid() const
{
   return DisPdu::IsValid() && mAggregateId.IsValid() && mAggregateType.IsValid() && mSilentAggregateSystems.IsValid() &&
          mSilentEntitySystems.IsValid() && mVariableDatumRecords.IsValid() && ValidateScalarArray(mDimensions, 3) &&
          ValidateScalarArray(mOrientation, 3) && ValidateScalarArray(mCenterOfMass, 3) &&
          ValidateScalarArray(mVelocity, 3) && ValidateVector(mAggregateIds) && ValidateVector(mEntityIds);
}

std::string DisAggregateState::GetStringId() const
{
   return "DisAggregateState";
}

std::string DisAggregateState::ToString() const
{
   std::string returnString = "-------Aggregate State PDU-------\n";
   returnString += "Aggregate Id:                           " + mAggregateId.ToString() + "\n";
   returnString += "Force Id:                               " + std::to_string(mForceId) + "\n";
   returnString += "Aggregate State:                        " + std::to_string(mAggregateState) + "\n";
   returnString += "Aggregate Type:                         \n";
   returnString += mAggregateType.ToString();
   returnString += "Formation:           " + std::to_string(mFormation) + "\n";
   returnString += "-------Aggregate Marking-------\n";

   for (int i = 0; i < 32; i++)
   {
      returnString += "Element " + std::to_string(i) + ": " + std::to_string(mAggregateMarking[i]) + "\n";
   }

   returnString += "-----End Aggregate Marking-----\n\n";

   returnString += "-------Dimensions-------\n";
   for (int i = 0; i < 3; i++)
   {
      returnString += "Element " + std::to_string(i) + ": " + std::to_string(mDimensions[i]) + "\n";
   }
   returnString += "-----End Dimensions-----\n";

   returnString += "-------Orientation-------\n";
   for (int i = 0; i < 3; i++)
   {
      returnString += "Element " + std::to_string(i) + ": " + std::to_string(mOrientation[i]) + "\n";
   }
   returnString += "-----End Orientation-----\n";

   returnString += "-------Center of Mass-------\n";
   for (int i = 0; i < 3; i++)
   {
      returnString += "Element " + std::to_string(i) + ": " + std::to_string(mCenterOfMass[i]) + "\n";
   }
   returnString += "-----End Center of Mass-----\n";

   returnString += "-------Velocity-------\n";
   for (int i = 0; i < 3; i++)
   {
      returnString += "Element " + std::to_string(i) + ": " + std::to_string(mVelocity[i]) + "\n";
   }
   returnString += "-----End Velocity-----\n";

   returnString += "Number of Aggregate Ids:                " + std::to_string(mNumberOfAggregateIds) + "\n";
   returnString += "Number of Entity Ids:                   " + std::to_string(mNumberOfEntityIds) + "\n";
   returnString += "Number of Silent Aggregate Systems:     " + std::to_string(mNumberofSilentAggregateSystems) + "\n";
   returnString += "Number of Silent Entity Systems:        " + std::to_string(mNumberOfSilentEntitySystems) + "\n";

   returnString += "-------Aggregate IDs-------\n";
   for (int i = 0; i < 32; i++)
   {
      returnString += "Element " + std::to_string(i) + ": \n";
      returnString += mAggregateIds.at(i).ToString();
   }
   returnString += "-----End Aggregate IDs-----\n";

   returnString += "-------Entity IDs-------\n";
   for (int i = 0; i < 32; i++)
   {
      returnString += "Element " + std::to_string(i) + ": \n";
      returnString += mEntityIds.at(i).ToString();
   }
   returnString += "-----End Entity IDs-----\n";

   returnString += "-------Silent Aggregate Systems-------\n";
   std::vector<DisSilentAggregateSystem*>::const_iterator silentAggregateSystemIterator(mSilentAggregateSystems.GetBegin());
   while (silentAggregateSystemIterator != mSilentAggregateSystems.GetEnd())
   {
      returnString += (*silentAggregateSystemIterator++)->ToString();
   }
   returnString += "-----End Silent Aggregate Systems-----\n";

   returnString += "-------Silent Entity Systems-------\n";
   std::vector<DisSilentEntitySystemRecord*>::const_iterator silentEntitySystemIterator(mSilentEntitySystems.GetBegin());
   while (silentEntitySystemIterator != mSilentEntitySystems.GetEnd())
   {
      returnString += (*silentEntitySystemIterator++)->ToString();
   }
   returnString += "-----End Silent Entity Systems-----\n";

   returnString += "Silent Entity System:     " + std::to_string(mNumberOfVariableDatumRecords) + "\n";

   returnString += "-------Silent Entity Systems-------\n";
   std::vector<DisVariableDatum*>::const_iterator variableDatumRecordsIterator(mVariableDatumRecords.GetBegin());
   while (variableDatumRecordsIterator != mVariableDatumRecords.GetEnd())
   {
      returnString += (*variableDatumRecordsIterator++)->ToString();
   }
   returnString += "-----End Silent Entity Systems-----\n";

   returnString += "-----End Aggregate State PDU-----\n";
   return returnString;
}

const DisEntityId& DisAggregateState::GetAggregateId() const
{
   return mAggregateId;
}

DisEnum8 DisAggregateState::GetForceId() const
{
   return mForceId;
}

DisEnum8 DisAggregateState::GetAggregateState() const
{
   return mAggregateState;
}

const DisAggregateTypeRecord& DisAggregateState::GetAggregateTypeRecord() const
{
   return mAggregateType;
}

DisEnum32 DisAggregateState::GetFormation() const
{
   return mFormation;
}

const DisEnum8* DisAggregateState::GetAggregateMarking() const
{
   return mAggregateMarking;
}

void DisAggregateState::GetDimensions(DisFloat32& aDimensionX, DisFloat32& aDimensionY, DisFloat32& aDimensionZ) const
{
   aDimensionX = mDimensions[0];
   aDimensionY = mDimensions[1];
   aDimensionZ = mDimensions[2];
}

void DisAggregateState::GetOrientation(DisFloat32& aOrientationX, DisFloat32& aOrientationY, DisFloat32& aOrientationZ) const
{
   aOrientationX = mOrientation[0];
   aOrientationY = mOrientation[1];
   aOrientationZ = mOrientation[2];
}

void DisAggregateState::GetCenterOfMass(DisFloat64& aCenterOfMassX, DisFloat64& aCenterOfMassY, DisFloat64& aCenterOfMassZ) const
{
   aCenterOfMassX = mCenterOfMass[0];
   aCenterOfMassY = mCenterOfMass[1];
   aCenterOfMassZ = mCenterOfMass[2];
}

void DisAggregateState::GetVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const
{
   aVelocityX = mVelocity[0];
   aVelocityY = mVelocity[1];
   aVelocityZ = mVelocity[2];
}

DisUint16 DisAggregateState::GetNumberOfAggregateIds() const
{
   return mNumberOfAggregateIds;
}

DisUint16 DisAggregateState::GetNumberOfEntityIds() const
{
   return mNumberOfEntityIds;
}

DisUint16 DisAggregateState::GetNumberOfSilentAggregateSystems() const
{
   return mNumberofSilentAggregateSystems;
}

DisUint16 DisAggregateState::GetNumberOfSilentEntitySystems() const
{
   return mNumberOfSilentEntitySystems;
}

const std::vector<DisEntityId> DisAggregateState::GetAggregateIds() const
{
   return mAggregateIds;
}

const std::vector<DisEntityId> DisAggregateState::GetEntityIds() const
{
   return mEntityIds;
}

DisUint32 DisAggregateState::GetNumberOfVariableDatumRecords() const
{
   return mNumberOfVariableDatumRecords;
}

const DisSilentAggregateSystem& DisAggregateState::GetSilentAggregateSystemAtIndex(DisUint16 aIndex) const
{
   return *mSilentAggregateSystems[aIndex];
}

const DisSilentEntitySystemRecord& DisAggregateState::GetSilentEntitySystemAtIndex(DisUint16 aIndex) const
{
   return *mSilentEntitySystems[aIndex];
}

const DisVariableDatum& DisAggregateState::GetVariableDatumAtIndex(DisUint16 aIndex) const
{
   return *mVariableDatumRecords[aIndex];
}

void DisAggregateState::SetAggregateId(const DisEntityId& aAggregateId)
{
   mAggregateId = aAggregateId;
}

void DisAggregateState::SetForceId(const DisEnum8 aForceId)
{
   mForceId = aForceId;
}

void DisAggregateState::SetAggregateState(const DisEnum8 aAggregateState)
{
   mAggregateState = aAggregateState;
}

void DisAggregateState::SetAggregateTypeRecord(const DisAggregateTypeRecord& aAggregateTypeRecord)
{
   mAggregateType = aAggregateTypeRecord;
}

void DisAggregateState::SetFormation(const DisEnum32 aFormation)
{
   mFormation = aFormation;
}

void DisAggregateState::SetAggregateMarking(const DisEnum8* aAggregateMarking)
{
   for (int i = 0; i < 32; i++)
   {
      mAggregateMarking[i] = aAggregateMarking[i];
   }
}

void DisAggregateState::SetDimensions(DisFloat32 aDimensionX, DisFloat32 aDimensionY, DisFloat32 aDimensionZ)
{
   mDimensions[0] = aDimensionX;
   mDimensions[1] = aDimensionY;
   mDimensions[2] = aDimensionZ;
}

void DisAggregateState::SetOrientation(DisFloat32 aOrientationX, DisFloat32 aOrientationY, DisFloat32 aOrientationZ)
{
   mOrientation[0] = aOrientationX;
   mOrientation[1] = aOrientationY;
   mOrientation[2] = aOrientationZ;
}

void DisAggregateState::SetCenterOfMass(DisFloat64 aCenterOfMassX, DisFloat64 aCenterOfMassY, DisFloat64 aCenterOfMassZ)
{
   mCenterOfMass[0] = aCenterOfMassX;
   mCenterOfMass[1] = aCenterOfMassY;
   mCenterOfMass[2] = aCenterOfMassZ;
}

void DisAggregateState::SetVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ)
{
   mVelocity[0] = aVelocityX;
   mVelocity[1] = aVelocityY;
   mVelocity[2] = aVelocityZ;
}

void DisAggregateState::SetNumberOfAggregateIds(DisUint16 aNumberOfAggregateIds)
{
   mNumberOfAggregateIds = aNumberOfAggregateIds;
}

void DisAggregateState::SetNumberOfEntityIds(DisUint16 aNumberOfEntityIds)
{
   mNumberOfEntityIds = aNumberOfEntityIds;
}

void DisAggregateState::SetNumberOfSilentAggregateSystems(DisUint16 aNumberOfSilentAggregateSystems)
{
   mNumberofSilentAggregateSystems = aNumberOfSilentAggregateSystems;
}

void DisAggregateState::SetNumberOfSilentEntitySystems(DisUint16 aNumberfSilentEntitySystems)
{
   mNumberOfSilentEntitySystems = aNumberfSilentEntitySystems;
}

void DisAggregateState::SetAggregateIds(const std::vector<DisEntityId>& aAggregateIds)
{
   mAggregateIds = aAggregateIds;
}

void DisAggregateState::SetEntityIds(const std::vector<DisEntityId>& aEntityIds)
{
   mEntityIds = aEntityIds;
}

void DisAggregateState::AddAggregateId(DisEntityId aAggregateId)
{
   mAggregateIds.push_back(aAggregateId);
}

void DisAggregateState::AddEntityId(DisEntityId aEntityId)
{
   mEntityIds.push_back(aEntityId);
}

void DisAggregateState::AddSilentAggregateSystem(DisSilentAggregateSystem* aSilentAggregateSystem)
{
   mSilentAggregateSystems.Add(aSilentAggregateSystem);
   mNumberofSilentAggregateSystems = mSilentAggregateSystems.GetSize();
}

void DisAggregateState::AddSilentEntitySystem(DisSilentEntitySystemRecord* aSilentEntitySystem)
{
   mSilentEntitySystems.Add(aSilentEntitySystem);
   mNumberOfSilentEntitySystems = mSilentEntitySystems.GetSize();
}

void DisAggregateState::AddVariableDatum(DisVariableDatum* aVariableDatum)
{
   mVariableDatumRecords.Add(aVariableDatum);
   mNumberOfVariableDatumRecords = mVariableDatumRecords.GetSize();
}

std::vector<DisSilentAggregateSystem*>::iterator DisAggregateState::GetSilentAggregateSystemBegin()
{
   return mSilentAggregateSystems.GetBegin();
}

std::vector<DisSilentAggregateSystem*>::const_iterator DisAggregateState::GetSilentAggregateSystemBegin() const
{
   return mSilentAggregateSystems.GetBegin();
}

std::vector<DisSilentAggregateSystem*>::iterator DisAggregateState::GetSilentAggregateSystemEnd()
{
   return mSilentAggregateSystems.GetEnd();
}

std::vector<DisSilentAggregateSystem*>::const_iterator DisAggregateState::GetSilentAggregateSystemEnd() const
{
   return mSilentAggregateSystems.GetEnd();
}

std::vector<DisSilentEntitySystemRecord*>::iterator DisAggregateState::GetSilentEntitySystemBegin()
{
   return mSilentEntitySystems.GetBegin();
}

std::vector<DisSilentEntitySystemRecord*>::const_iterator DisAggregateState::GetSilentEntitySystemBegin() const
{
   return mSilentEntitySystems.GetBegin();
}

std::vector<DisSilentEntitySystemRecord*>::iterator DisAggregateState::GetSilentEntitySystemEnd()
{
   return mSilentEntitySystems.GetEnd();
}

std::vector<DisSilentEntitySystemRecord*>::const_iterator DisAggregateState::GetSilentEntitySystemEnd() const
{
   return mSilentEntitySystems.GetEnd();
}

void DisAggregateState::GetRecordData(GenI& aGenI)
{
   for (int i = 0; i < mNumberofSilentAggregateSystems; i++)
   {
      DisSilentAggregateSystem* silentAggregateSystem = new DisSilentAggregateSystem(aGenI);
      if (silentAggregateSystem != nullptr)
      {
         mSilentAggregateSystems.Add(silentAggregateSystem);
      }
      else
      {
         throw UtException("Invalid Silent Aggregate System");
      }
   }

   for (int i = 0; i < mNumberOfSilentEntitySystems; i++)
   {
      DisSilentEntitySystemRecord* silentEntitySystem = new DisSilentEntitySystemRecord(aGenI);
      if (silentEntitySystem != nullptr)
      {
         mSilentEntitySystems.Add(silentEntitySystem);
      }
      else
      {
         throw UtException("Invalid Silent Entity System");
      }
   }

   aGenI >> mNumberOfVariableDatumRecords;
   for (int i = 0; i < static_cast<int>(mNumberOfVariableDatumRecords); i++)
   {
      DisVariableDatum* variableDatum = new DisVariableDatum(aGenI);
      if (variableDatum != nullptr)
      {
         mVariableDatumRecords.Add(variableDatum);
      }
      else
      {
         throw UtException("Invalid Variable Datum Record");
      }
   }
}

void DisAggregateState::PutRecordData(GenO& aGenO)
{
   for (auto it = mSilentAggregateSystems.GetBegin(); it != mSilentAggregateSystems.GetEnd(); it++)
   {
      (*it)->Put(aGenO);
   }

   for (auto it = mSilentEntitySystems.GetBegin(); it != mSilentEntitySystems.GetEnd(); it++)
   {
      (*it)->Put(aGenO);
   }

   aGenO << mNumberOfVariableDatumRecords;
   for (auto it = mVariableDatumRecords.GetBegin(); it != mVariableDatumRecords.GetEnd(); it++)
   {
      (*it)->Put(aGenO);
   }
}

void DisAggregateState::GetMemberData(GenI& aGenI)
{
   aGenI >> mAggregateId;
   aGenI >> mForceId;
   aGenI >> mAggregateState;
   mAggregateType.Get(aGenI);
   aGenI >> mFormation;
   for (int i = 0; i < 32; i++)
   {
      aGenI >> mAggregateMarking[i];
   }
   aGenI >> mDimensions[0] >> mDimensions[1] >> mDimensions[2];
   aGenI >> mOrientation[0] >> mOrientation[1] >> mOrientation[2];
   aGenI >> mCenterOfMass[0] >> mCenterOfMass[1] >> mCenterOfMass[2];
   aGenI >> mVelocity[0] >> mVelocity[1] >> mVelocity[2];
   aGenI >> mNumberOfAggregateIds;
   aGenI >> mNumberOfEntityIds;
   aGenI >> mNumberofSilentAggregateSystems;
   aGenI >> mNumberOfSilentEntitySystems;

   if (mNumberOfAggregateIds)
   {
      for (int i = 0; i < mNumberOfAggregateIds; i++)
      {
         DisEntityId currentEntity;
         currentEntity.Get(aGenI);
         mAggregateIds.push_back(currentEntity);
      }
   }

   if (mNumberOfEntityIds)
   {
      for (int i = 0; i < mNumberOfEntityIds; i++)
      {
         DisEntityId currentEntity;
         currentEntity.Get(aGenI);
         mEntityIds.push_back(currentEntity);
      }
   }

   // Pad up to 32 bits
   // Padding size is calculated from the equation given on page 427 of the IEEE 1278.1-2012 spec
   // number of padding octets: ceiling((W + J)/4) * 4 - (W + J) (octets)
   int numberOfPaddingOctets = static_cast<int>(
      (std::ceil((mNumberOfAggregateIds + mNumberOfEntityIds) / 4) * 4 - (mNumberOfAggregateIds + mNumberOfEntityIds)));
   DisUint8 padding = 0;
   for (int i = 0; i < numberOfPaddingOctets; i++)
   {
      aGenI >> padding; // padding size
   }

   GetRecordData(aGenI);
   GetLength();
}
