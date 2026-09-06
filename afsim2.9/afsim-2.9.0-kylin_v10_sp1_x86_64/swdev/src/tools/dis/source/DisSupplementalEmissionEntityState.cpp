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

#include "DisSupplementalEmissionEntityState.hpp"

#include <cassert>

#include "DisPduEnums.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisSupplementalEmissionEntityState::DisSupplementalEmissionEntityState()
   : DisPdu()
{
   SetPduType(DisEnum::Pdu::Type::SupplementalEmissionEntityState);
   SetProtocolFamily(DisEnum::Pdu::Family::DistributedEmissionRegeneration);
   GetLength(); // 224 bits -> size with no Propulsion and vector records
}

DisSupplementalEmissionEntityState::DisSupplementalEmissionEntityState(const DisSupplementalEmissionEntityState& aSrc)
   : DisPdu(aSrc)
   , mOriginatingEntity(aSrc.mOriginatingEntity)
   , mInfraredSignatureRepresentationIndex(aSrc.mInfraredSignatureRepresentationIndex)
   , mAcousticSignatureRepresentationIndex(aSrc.mAcousticSignatureRepresentationIndex)
   , mRadarCrossSectionRepresentationIndex(aSrc.mRadarCrossSectionRepresentationIndex)
{
   // IMPORTANT: See documentation in .hpp file before changing
   CopyRecords(aSrc);
}

DisSupplementalEmissionEntityState::DisSupplementalEmissionEntityState(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisSupplementalEmissionEntityState::DisSupplementalEmissionEntityState(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisSupplementalEmissionEntityState* DisSupplementalEmissionEntityState::Clone() const
{
   return new DisSupplementalEmissionEntityState(*this);
}

int DisSupplementalEmissionEntityState::GetClass() const
{
   return static_cast<int>(DisEnum::Pdu::Type::SupplementalEmissionEntityState);
}

int DisSupplementalEmissionEntityState::GetFamily() const
{
   return static_cast<int>(DisEnum::Pdu::Family::DistributedEmissionRegeneration);
}

const DisEntityId& DisSupplementalEmissionEntityState::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

const DisEntityId& DisSupplementalEmissionEntityState::GetEntityId() const
{
   return mOriginatingEntity;
}

DisUint16 DisSupplementalEmissionEntityState::GetInfraredSignatureRepresentationIndex() const
{
   return mInfraredSignatureRepresentationIndex;
}

DisUint16 DisSupplementalEmissionEntityState::GetAcousticSignatureRepresentationIndex() const
{
   return mAcousticSignatureRepresentationIndex;
}

DisUint16 DisSupplementalEmissionEntityState::GetRadarCrossSectionRepresentationIndex() const
{
   return mRadarCrossSectionRepresentationIndex;
}

DisUint16 DisSupplementalEmissionEntityState::GetNumberOfPropulsionSystems() const
{
   return mNumberOfPropulsionSystemRecords;
}

DisUint16 DisSupplementalEmissionEntityState::GetNumberOfVectoringNozzleSystems() const
{
   return mNumberOfVectoringNozzleSystemRecords;
}

std::vector<DisPropulsionSystemDataRecord*>::iterator DisSupplementalEmissionEntityState::GetPropulsionSystemBegin()
{
   return mPropulsionSystemsRecordList.GetBegin();
}

std::vector<DisPropulsionSystemDataRecord*>::const_iterator DisSupplementalEmissionEntityState::GetPropulsionSystemBegin() const
{
   return mPropulsionSystemsRecordList.GetBegin();
}

std::vector<DisVectoringNozzleSystemDataRecord*>::iterator DisSupplementalEmissionEntityState::GetVectoringNozzleSystemBegin()
{
   return mVectoringNozzleSystemRecordList.GetBegin();
}

std::vector<DisVectoringNozzleSystemDataRecord*>::const_iterator
DisSupplementalEmissionEntityState::GetVectoringNozzleSystemBegin() const
{
   return mVectoringNozzleSystemRecordList.GetBegin();
}

void DisSupplementalEmissionEntityState::SetEntityId(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}

void DisSupplementalEmissionEntityState::SetInfraredSignatureRepresentationIndex(DisUint16 aInfraredSignatureRepresentationIndex)
{
   mInfraredSignatureRepresentationIndex = aInfraredSignatureRepresentationIndex;
}

void DisSupplementalEmissionEntityState::SetAcousticSignatureRepresentationIndex(DisUint16 aAcousticSignatureRepresentationIndex)
{
   mAcousticSignatureRepresentationIndex = aAcousticSignatureRepresentationIndex;
}

void DisSupplementalEmissionEntityState::SetRadarCrossSectionRepresentationIndex(DisUint16 aRadarCrossSectionRepresentationIndex)
{
   mRadarCrossSectionRepresentationIndex = aRadarCrossSectionRepresentationIndex;
}

void DisSupplementalEmissionEntityState::SetNumberOfPropulsionSystems(DisUint16 aNumberOfPropulsionSystems)
{
   mNumberOfPropulsionSystemRecords = aNumberOfPropulsionSystems;
}

void DisSupplementalEmissionEntityState::SetNumberOfVectoringNozzleSystems(DisUint16 aNumberOfVectoringNozzleSystems)
{
   mNumberOfVectoringNozzleSystemRecords = aNumberOfVectoringNozzleSystems;
}

void DisSupplementalEmissionEntityState::AddPropulsionSystemRecord(DisPropulsionSystemDataRecord* aPropulsionSystemRecord)
{
   mPropulsionSystemsRecordList.Add(aPropulsionSystemRecord);
}

unsigned int DisSupplementalEmissionEntityState::RemovePropulsionSystemRecord(DisPropulsionSystemDataRecord* aPropulsionSystemRecord)
{
   return mPropulsionSystemsRecordList.Remove(aPropulsionSystemRecord);
}

void DisSupplementalEmissionEntityState::RemoveAllPropulsionSystemRecords()
{
   mPropulsionSystemsRecordList.RemoveAll();
}

unsigned int DisSupplementalEmissionEntityState::RemovePropulsionSystemRecordWithNoDelete(
   DisPropulsionSystemDataRecord* aPropulsionSystemRecord)
{
   return mPropulsionSystemsRecordList.RemoveWithNoDelete(aPropulsionSystemRecord);
}

void DisSupplementalEmissionEntityState::RemoveAllPropulsionSystemRecordsWithNoDelete()
{
   mPropulsionSystemsRecordList.RemoveAllWithNoDelete();
}

const DisPropulsionSystemDataRecord* DisSupplementalEmissionEntityState::GetPropulsionSystemDataRecordAtIndex(int aIndex) const
{
   const DisPropulsionSystemDataRecord* recordPtr{nullptr};
   if (aIndex >= 0 && aIndex < static_cast<int>(mPropulsionSystemsRecordList.GetSize()))
   {
      return mPropulsionSystemsRecordList[aIndex];
   }

   return recordPtr;
}

void DisSupplementalEmissionEntityState::AddVectoringNozzleSystemRecord(DisVectoringNozzleSystemDataRecord* aVectorNozzleSystemRecord)
{
   mVectoringNozzleSystemRecordList.Add(aVectorNozzleSystemRecord);
}

unsigned int DisSupplementalEmissionEntityState::RemoveVectoringNozzleSystemRecord(
   DisVectoringNozzleSystemDataRecord* aVectorNozzleSystemRecord)
{
   return mVectoringNozzleSystemRecordList.Remove(aVectorNozzleSystemRecord);
}

void DisSupplementalEmissionEntityState::RemoveAllVectoringNozzleSystemRecords()
{
   mVectoringNozzleSystemRecordList.RemoveAll();
}

unsigned int DisSupplementalEmissionEntityState::RemoveVectoringNozzleSystemRecordWithNoDelete(
   DisVectoringNozzleSystemDataRecord* aVectorNozzleSystemRecord)
{
   return mVectoringNozzleSystemRecordList.RemoveWithNoDelete(aVectorNozzleSystemRecord);
}

void DisSupplementalEmissionEntityState::RemoveAllVectoringNozzleSystemRecordsWithNoDelete()
{
   mVectoringNozzleSystemRecordList.RemoveAllWithNoDelete();
}

const DisVectoringNozzleSystemDataRecord*
DisSupplementalEmissionEntityState::GetVectoringNozzleSystemDataRecordAtIndex(int aIndex) const
{
   const DisVectoringNozzleSystemDataRecord* recordPtr{nullptr};
   if (aIndex >= 0 && aIndex < static_cast<int>(mVectoringNozzleSystemRecordList.GetSize()))
   {
      return mVectoringNozzleSystemRecordList[aIndex];
   }

   return recordPtr;
}

std::string DisSupplementalEmissionEntityState::GetStringId() const
{
   return "DisSupplementalEmissionEntityState";
}

std::string DisSupplementalEmissionEntityState::ToString() const
{
   std::string returnString = DisPdu::ToString();
   returnString += "PDU Type = SUPPLEMENTAL_EMISSION_ENTITY_STATE\n";
   returnString += "---------Supplemental Emission Entity State PDU Information---------\n";
   returnString += "\nEntityID    = \n" + mOriginatingEntity.ToString() + "\n";
   returnString +=
      "   Infrared Signature Representation Index  = " + std::to_string(mInfraredSignatureRepresentationIndex) + "\n";
   returnString +=
      "   Acoustic Signature Representation Index  = " + std::to_string(mAcousticSignatureRepresentationIndex) + "\n";
   returnString +=
      "   Radar Cross Section Representation Index = " + std::to_string(mRadarCrossSectionRepresentationIndex) + "\n";
   returnString +=
      "   Number of propulsion system records      = " + std::to_string(mNumberOfPropulsionSystemRecords) + "\n";
   returnString +=
      "   Number of vector nozzle system records   = " + std::to_string(mNumberOfVectoringNozzleSystemRecords) + "\n";

   returnString += "PROPULSION SYSTEM RECORDS:                \n";
   for (auto it = mPropulsionSystemsRecordList.GetBegin(); it != mPropulsionSystemsRecordList.GetEnd(); it++)
   {
      returnString += (**it).ToString();
   }
   returnString += "END PROPULSION SYSTEM RECORDS:            \n";

   returnString += "VECTOR NOZZLE SYSTEM RECORDS:             \n";
   for (auto it = mVectoringNozzleSystemRecordList.GetBegin(); it != mVectoringNozzleSystemRecordList.GetEnd(); it++)
   {
      returnString += (**it).ToString();
   }
   returnString += "END VECTOR NOZZLE SYSTEM RECORDS:         \n";
   returnString += "-------End Supplemental Emission Entity State PDU Information-------\n";
   return returnString;
}

void DisSupplementalEmissionEntityState::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

void DisSupplementalEmissionEntityState::CopyRecords(const DisSupplementalEmissionEntityState& aSrc)
{
   RemoveAllPropulsionSystemRecords();

   for (auto it = aSrc.mPropulsionSystemsRecordList.GetBegin(); it != aSrc.mPropulsionSystemsRecordList.GetEnd(); it++)
   {
      mPropulsionSystemsRecordList.Add((**it).Clone());
   }

   UpdatePropulsionRecordCount();

   RemoveAllVectoringNozzleSystemRecords();

   for (auto it = aSrc.mVectoringNozzleSystemRecordList.GetBegin(); it != aSrc.mVectoringNozzleSystemRecordList.GetEnd();
        it++)
   {
      mVectoringNozzleSystemRecordList.Add((**it).Clone());
   }

   UpdateVectorNozzleRecordCount();
}

void DisSupplementalEmissionEntityState::GetMemberData(GenI& aGenI)
{
   DisInt32 lengthToRead = GetLengthToRead();

   mOriginatingEntity.Get(aGenI);
   aGenI >> mInfraredSignatureRepresentationIndex;
   aGenI >> mAcousticSignatureRepresentationIndex;
   aGenI >> mRadarCrossSectionRepresentationIndex;
   aGenI >> mNumberOfPropulsionSystemRecords;
   aGenI >> mNumberOfVectoringNozzleSystemRecords;

   lengthToRead -= cPDU_MIN_LENGTH_OCTETS;

   RemoveAllPropulsionSystemRecords();
   for (int i = 0; i < mNumberOfPropulsionSystemRecords; i++)
   {
      DisPropulsionSystemDataRecord* propulsionRecordPtr = new DisPropulsionSystemDataRecord(aGenI);
      if (propulsionRecordPtr != nullptr)
      {
         mPropulsionSystemsRecordList.Add(propulsionRecordPtr);
         lengthToRead -= propulsionRecordPtr->GetLength();
      }
   }
   UpdatePropulsionRecordCount();

   RemoveAllVectoringNozzleSystemRecords();

   for (int i = 0; i < mNumberOfVectoringNozzleSystemRecords; i++)
   {
      DisVectoringNozzleSystemDataRecord* vectoringNozzleRecordPtr = new DisVectoringNozzleSystemDataRecord(aGenI);
      if (vectoringNozzleRecordPtr != nullptr)
      {
         mVectoringNozzleSystemRecordList.Add(vectoringNozzleRecordPtr);
         lengthToRead -= vectoringNozzleRecordPtr->GetLength();
      }
   }
   UpdateVectorNozzleRecordCount();

   assert(lengthToRead >= 0);
   ReadExtraData(aGenI, static_cast<DisUint16>(lengthToRead));
   GetLength();
}

void DisSupplementalEmissionEntityState::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisSupplementalEmissionEntityState::GetLength()
{
   UpdatePropulsionRecordCount();
   UpdateVectorNozzleRecordCount();
   DisUint16 length = DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS;

   std::vector<DisPropulsionSystemDataRecord*>::const_iterator propIter(mPropulsionSystemsRecordList.GetBegin());
   while (propIter != mPropulsionSystemsRecordList.GetEnd())
   {
      length += (*propIter++)->GetLength();
   }

   std::vector<DisVectoringNozzleSystemDataRecord*>::const_iterator vecIter(mVectoringNozzleSystemRecordList.GetBegin());
   while (vecIter != mVectoringNozzleSystemRecordList.GetEnd())
   {
      length += (*vecIter++)->GetLength();
   }

   SetLength(length);

   return length;
}

void DisSupplementalEmissionEntityState::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);
   mOriginatingEntity.Put(aGenO);
   aGenO << mInfraredSignatureRepresentationIndex;
   aGenO << mAcousticSignatureRepresentationIndex;
   aGenO << mRadarCrossSectionRepresentationIndex;
   aGenO << mNumberOfPropulsionSystemRecords;
   aGenO << mNumberOfVectoringNozzleSystemRecords;

   for (auto it = mPropulsionSystemsRecordList.GetBegin(); it != mPropulsionSystemsRecordList.GetEnd(); it++)
   {
      (*it)->Put(aGenO);
   }

   for (auto it = mVectoringNozzleSystemRecordList.GetBegin(); it != mVectoringNozzleSystemRecordList.GetEnd(); it++)
   {
      (*it)->Put(aGenO);
   }
}

bool DisSupplementalEmissionEntityState::IsValid() const
{
   return DisPdu::IsValid() && mOriginatingEntity.IsValid() && mPropulsionSystemsRecordList.IsValid() &&
          mVectoringNozzleSystemRecordList.IsValid();
}

void DisSupplementalEmissionEntityState::UpdatePropulsionRecordCount() const
{
   mNumberOfPropulsionSystemRecords = static_cast<DisUint16>(mPropulsionSystemsRecordList.GetSize());
}

void DisSupplementalEmissionEntityState::UpdateVectorNozzleRecordCount() const
{
   mNumberOfVectoringNozzleSystemRecords = static_cast<DisUint16>(mVectoringNozzleSystemRecordList.GetSize());
}
