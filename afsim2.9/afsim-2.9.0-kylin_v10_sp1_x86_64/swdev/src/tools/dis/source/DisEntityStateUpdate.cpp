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

#include "DisEntityStateUpdate.hpp"

#include "DisValidationUtils.hpp"
#include "DisVariableParameterRecordEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"

DisEntityStateUpdate::DisEntityStateUpdate()
   : DisPdu()
   , mEntityId()
   , mLinearVelocity()
   , mOrientation()
   , mAppearance()
{
   SetPduType(DisEnum::Pdu::Type::EntityStateUpdate);
   SetProtocolFamily(DisEnum::Pdu::Family::EntityInformationInteraction);
   GetLength();
}

DisEntityStateUpdate::DisEntityStateUpdate(const DisEntityStateUpdate& aSrc)
   : DisPdu(aSrc)
   , mEntityId(aSrc.mEntityId)
   , mVariableParameterRecordsCount(aSrc.mVariableParameterRecordsCount)
   , mLinearVelocity(aSrc.mLinearVelocity)
   , mLocation(aSrc.mLocation)
   , mOrientation(aSrc.mOrientation)
   , mAppearance(aSrc.mAppearance)
{
   mVariableParameterRecords.Reserve(mVariableParameterRecordsCount);
   for (auto it = aSrc.mVariableParameterRecords.GetBegin(); it != aSrc.mVariableParameterRecords.GetEnd(); it++)
   {
      mVariableParameterRecords.Add((*it)->Clone());
   }
   GetLength();
}

DisEntityStateUpdate::DisEntityStateUpdate(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisEntityStateUpdate::DisEntityStateUpdate(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisEntityStateUpdate* DisEntityStateUpdate::Clone() const
{
   return new DisEntityStateUpdate(*this);
}

std::string DisEntityStateUpdate::ToString() const
{
   std::string returnString = DisPdu::ToString();
   returnString += "PDU Type = ENTITY STATE UPDATE\n";
   returnString += "---------Entity State Update PDU Information---------\n";
   returnString += "   Entity ID                      = \n" + mEntityId.ToString() + '\n';
   returnString += "   Variable Parameter Records     = " + std::to_string(mVariableParameterRecordsCount) + '\n';
   returnString += "   Linear Velocity Vector (x,y,z) = \n";
   returnString += "                                x = " + std::to_string(static_cast<float>(mLinearVelocity[0])) + '\n';
   returnString += "                                y = " + std::to_string(static_cast<float>(mLinearVelocity[1])) + '\n';
   returnString += "                                z = " + std::to_string(static_cast<float>(mLinearVelocity[2])) + '\n';
   returnString += "   Location Vector (x,y,z)        = \n";
   returnString += "                                x = " + std::to_string(static_cast<float>(mLocation[0])) + '\n';
   returnString += "                                y = " + std::to_string(static_cast<float>(mLocation[1])) + '\n';
   returnString += "                                z = " + std::to_string(static_cast<float>(mLocation[2])) + '\n';
   returnString += "   Orientation Vector (x,y,z)     = \n";
   returnString += "                                x = " + std::to_string(static_cast<float>(mOrientation[0])) + '\n';
   returnString += "                                y = " + std::to_string(static_cast<float>(mOrientation[1])) + '\n';
   returnString += "                                z = " + std::to_string(static_cast<float>(mOrientation[2])) + '\n';
   returnString += "   Appearance                     = \n" + std::to_string(mAppearance.Value()) + '\n';

   for (auto it = mVariableParameterRecords.GetBegin(); it != mVariableParameterRecords.GetEnd(); it++)
   {
      returnString += (*it)->ToString() + '\n';
   }

   returnString += "-------End Entity State Update PDU Information-------\n";
   return returnString;
}

void DisEntityStateUpdate::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisEntityStateUpdate::GetLength()
{
   DisUint16 lengthInOctets       = DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS;
   mVariableParameterRecordsCount = static_cast<DisUint8>(mVariableParameterRecords.GetSize());
   lengthInOctets += (16 * mVariableParameterRecordsCount);
   SetLength(lengthInOctets);
   return lengthInOctets;
}

void DisEntityStateUpdate::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);

   mEntityId.Put(aGenO);
   aGenO << mPadding;
   aGenO << mVariableParameterRecordsCount;
   aGenO << mLinearVelocity[0] << mLinearVelocity[1] << mLinearVelocity[2];
   aGenO << mLocation[0] << mLocation[1] << mLocation[2];
   aGenO << mOrientation[0] << mOrientation[1] << mOrientation[2];
   mAppearance.Put(aGenO);

   PutRecords(aGenO);
}

bool DisEntityStateUpdate::IsValid() const
{
   return DisPdu::IsValid() && mEntityId.IsValid() && ValidateScalar(mLinearVelocity[0]) &&
          ValidateScalar(mLinearVelocity[1]) && ValidateScalar(mLinearVelocity[2]) && ValidateScalar(mLocation[0]) &&
          ValidateScalar(mLocation[1]) && ValidateScalar(mLocation[2]) && ValidateScalar(mOrientation[0]) &&
          (fabs(mOrientation[0]) > UtMath::cTWO_PI) && ValidateScalar(mOrientation[1]) &&
          !(fabs(mOrientation[1]) > UtMath::cTWO_PI) && ValidateScalar(mOrientation[2]) &&
          !(fabs(mOrientation[2]) > UtMath::cTWO_PI) && mVariableParameterRecords.IsValid();
}

void DisEntityStateUpdate::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();
   RemoveAllVariableParameterRecords();

   mEntityId.Get(aGenI);
   aGenI >> mPadding;
   aGenI >> mVariableParameterRecordsCount;
   aGenI >> mLinearVelocity[0] >> mLinearVelocity[1] >> mLinearVelocity[2];
   aGenI >> mLocation[0] >> mLocation[1] >> mLocation[2];
   aGenI >> mOrientation[0] >> mOrientation[1] >> mOrientation[2];
   mAppearance.Get(aGenI);

   GetRecords(aGenI);

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

void DisEntityStateUpdate::GetRecords(GenI& aGenI)
{
   for (int i = 0; i < mVariableParameterRecordsCount; i++)
   {
      DisVariableParameterRecord record(aGenI);

      if (record.GetTypeDesignator() == static_cast<int>(DisVariableParameterRecordTypes::cATTACHED) ||
          record.GetTypeDesignator() == static_cast<int>(DisVariableParameterRecordTypes::cARTICULATED))
      {
         DisEntityPart temp(record, aGenI);
         mVariableParameterRecords.Add(temp.Clone());
      }
   }
}

void DisEntityStateUpdate::PutRecords(GenO& aGenO)
{
   for (int i = 0; i < mVariableParameterRecordsCount; i++)
   {
      mVariableParameterRecords[i]->Put(aGenO);
   }
}
