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

#include "DisArealObjectState.hpp"

#include "DisPduEnums.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisArealObjectState::DisArealObjectState()
   : DisPdu()
   , mObjectType()
{
   SetProtocolFamily(DisEnum::Pdu::Family::SyntheticEnvironment);
   SetPduType(DisEnum::Pdu::Type::ArealObjectState);
   SetLength(DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS);
}

DisArealObjectState::DisArealObjectState(const DisArealObjectState& aSrc)
   : DisPdu(aSrc)
   , mObjectId(aSrc.mObjectId)
   , mReferencedObjectId(aSrc.mReferencedObjectId)
   , mUpdateNumber(aSrc.mUpdateNumber)
   , mForceId(aSrc.mForceId)
   , mModifications(aSrc.mModifications)
   , mObjectType(aSrc.mObjectType)
   , mSpecificObjectAppearance(aSrc.mSpecificObjectAppearance)
   , mGeneralObjectAppearance(aSrc.mGeneralObjectAppearance)
   , mNumberOfPoints(aSrc.mNumberOfPoints)
   , mRequesterSimulationId(aSrc.mRequesterSimulationId)
   , mReceivingSimulationId(aSrc.mReceivingSimulationId)
   , mPoints(aSrc.mPoints)
{
   SetProtocolFamily(DisEnum::Pdu::Family::SyntheticEnvironment);
   SetPduType(DisEnum::Pdu::Type::ArealObjectState);
   GetLength();
}

DisArealObjectState::DisArealObjectState(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisArealObjectState::~DisArealObjectState()
{
   mPoints.RemoveAll();
}

DisArealObjectState* DisArealObjectState::Clone() const
{
   return new DisArealObjectState(*this);
}

int DisArealObjectState::GetClass() const
{
   return DisEnum::Pdu::Type::ArealObjectState;
}

int DisArealObjectState::GetFamily() const
{
   return DisEnum::Pdu::Family::SyntheticEnvironment;
}

void DisArealObjectState::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisArealObjectState::GetLength()
{
   DisUint16 length = DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS;
   for (auto it = mPoints.GetBegin(); it != mPoints.GetEnd(); it++)
   {
      length += (**it).GetLength();
   }

   SetLength(length);

   return length;
}

void DisArealObjectState::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);
   mObjectId.Put(aGenO);
   mReferencedObjectId.Put(aGenO);
   aGenO << mUpdateNumber;
   aGenO << mForceId;
   aGenO << mModifications;
   mObjectType.Put(aGenO);
   mSpecificObjectAppearance.Put(aGenO);
   mGeneralObjectAppearance.Put(aGenO);
   aGenO << mNumberOfPoints;
   mRequesterSimulationId.Put(aGenO);
   mReceivingSimulationId.Put(aGenO);
   if (mNumberOfPoints > 0)
   {
      for (auto it = mPoints.GetBegin(); it != mPoints.GetEnd(); it++)
      {
         (**it).Put(aGenO);
      }
   }
}

const DisEntityId DisArealObjectState::GetObjectId() const
{
   return mObjectId;
}

const DisEntityId DisArealObjectState::GetReferencedObjectId() const
{
   return mReferencedObjectId;
}

DisUint16 DisArealObjectState::GetUpdateNumber() const
{
   return mUpdateNumber;
}

DisEnum8 DisArealObjectState::GetForceId() const
{
   return mForceId;
}

DisEnum8 DisArealObjectState::GetModifications() const
{
   return mModifications;
}

DisUint16 DisArealObjectState::GetNumberOfPoints() const
{
   return mNumberOfPoints;
}

const DisObjectType DisArealObjectState::GetObjectType() const
{
   return mObjectType;
}

const DisEntityAppearance DisArealObjectState::GetSpecificObjectAppearance() const
{
   return mSpecificObjectAppearance;
}

const DisGeneralAppearanceRecord DisArealObjectState::GetGeneralObjectAppearance() const
{
   return mGeneralObjectAppearance;
}

const DisSimulationAddressRecord DisArealObjectState::GetRequesterSimulationId() const
{
   return mRequesterSimulationId;
}

const DisSimulationAddressRecord DisArealObjectState::GetReceivingSimulationId() const
{
   return mReceivingSimulationId;
}

DisPtrContainer<DisWorldCoordinateRecord>* DisArealObjectState::GetPoints()
{
   return &mPoints;
}

void DisArealObjectState::SetObjectId(const DisEntityId& aObjectId)
{
   mObjectId = aObjectId;
}

void DisArealObjectState::SetReferenceObjectId(const DisEntityId& aObjectReferenceId)
{
   mReferencedObjectId = aObjectReferenceId;
}

void DisArealObjectState::SetUpdateNumber(DisUint16 aUpdateNumber)
{
   mUpdateNumber = aUpdateNumber;
}

void DisArealObjectState::SetForceId(DisEnum8 aForceId)
{
   mForceId = aForceId;
}

void DisArealObjectState::SetModifications(DisEnum8 aModifications)
{
   mModifications = aModifications;
}

void DisArealObjectState::SetNumberOfPoints(DisUint16 aNumberOfPoints)
{
   mNumberOfPoints = aNumberOfPoints;
}

void DisArealObjectState::SetObjectType(const DisObjectType& aObjectType)
{
   mObjectType = aObjectType;
}

void DisArealObjectState::SetSpecificObjectAppearance(const DisEntityAppearance& aSpecificObjectAppearance)
{
   mSpecificObjectAppearance = aSpecificObjectAppearance;
}

void DisArealObjectState::SetGeneralObjectAppearance(const DisGeneralAppearanceRecord& aGeneralObjectAppearance)
{
   mGeneralObjectAppearance = aGeneralObjectAppearance;
}

void DisArealObjectState::SetRequesterSimulationId(const DisSimulationAddressRecord& aRequesterSimulationId)
{
   mRequesterSimulationId = aRequesterSimulationId;
}

void DisArealObjectState::SetReceivingSimulationId(const DisSimulationAddressRecord& aRecievingSimulationId)
{
   mReceivingSimulationId = aRecievingSimulationId;
}

bool DisArealObjectState::IsValid() const
{
   return DisPdu::IsValid() && mObjectId.IsValid() && mReferencedObjectId.IsValid() && mObjectType.IsValid() &&
          mSpecificObjectAppearance.IsValid() && mRequesterSimulationId.IsValid() && mReceivingSimulationId.IsValid() &&
          mPoints.IsValid();
}

std::string DisArealObjectState::GetStringId() const
{
   return "WsfArealObjectState";
}

std::string DisArealObjectState::ToString() const
{
   std::string returnString = "-------Areal Object State-------\n";
   returnString += "Object Id: \n";
   returnString += mObjectId.ToString();
   returnString += "Object Reference Id: \n";
   returnString += mReferencedObjectId.ToString();
   returnString += "-----End Areal Object State-----\n";
   returnString += "Update Number: " + std::to_string(mUpdateNumber) + '\n';
   returnString += "Force Id: " + std::to_string(mForceId) + '\n';
   returnString += "Modifications: " + std::to_string(mModifications) + '\n';
   returnString += "Object Type: \n";
   returnString += "Specific Object Appearance: \n";
   returnString += mSpecificObjectAppearance.ToString(GetObjectType().GetObjectKind(), GetObjectType().GetDomain());
   returnString += "General Object Appearance: \n";
   returnString += mGeneralObjectAppearance.ToString();
   returnString += "Number of Points: " + std::to_string(mNumberOfPoints) + '\n';
   returnString += "Requesting Simulation Id: \n";
   returnString += mRequesterSimulationId.ToString();
   returnString += "Receiving Simulation Id: \n";
   returnString += mReceivingSimulationId.ToString();

   int index = 0;
   for (auto it = mPoints.GetBegin(); it != mPoints.GetEnd(); it++)
   {
      returnString += "Point " + std::to_string(index) + ": " + (**it).ToString() + '\n';
      index++;
   }

   return returnString;
}

void DisArealObjectState::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

void DisArealObjectState::GetMemberData(GenI& aGenI)
{
   mObjectId.Get(aGenI);
   mReferencedObjectId.Get(aGenI);
   aGenI >> mUpdateNumber;
   aGenI >> mForceId;
   aGenI >> mModifications;
   mObjectType.Get(aGenI);
   mSpecificObjectAppearance.Get(aGenI);
   mGeneralObjectAppearance.Get(aGenI);
   aGenI >> mNumberOfPoints;
   mRequesterSimulationId.Get(aGenI);
   mReceivingSimulationId.Get(aGenI);

   if (mNumberOfPoints > 0)
   {
      for (int i = 0; i < mNumberOfPoints; i++)
      {
         DisWorldCoordinateRecord* worldCoordinateRecord = new DisWorldCoordinateRecord(aGenI);
         mPoints.Add(worldCoordinateRecord);
      }
   }
   GetLength();
}
