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

#include "DisCollision.hpp"

#include <fstream>

#include "DisPduEnums.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"

DisCollision::DisCollision()
   : DisPdu()
   , mIssuingEntityId()
   , mCollidingEntityId()
   , mEventId()
{
   SetPduType(DisEnum::Pdu::Type::Collision);
   SetProtocolFamily(DisEnum::Pdu::Family::EntityInformationInteraction);
   SetLength(DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS); // 480 bits
}

DisCollision::DisCollision(const DisCollision& aSrc)
   : DisPdu(aSrc)
   , mIssuingEntityId(aSrc.mIssuingEntityId)
   , mCollidingEntityId(aSrc.mCollidingEntityId)
   , mEventId(aSrc.mEventId)
   , mCollisionType(aSrc.mCollisionType)
   , mMass(aSrc.mMass)
{
   for (int i = 0; i < 3; ++i)
   {
      mVelocity[i] = aSrc.mVelocity[i];
      mLocation[i] = aSrc.mLocation[i];
   }
   GetLength(); // Ensure consistent length
}

DisCollision::DisCollision(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisCollision::DisCollision(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

//! Method to clone this PDU
DisCollision* DisCollision::Clone() const
{
   return new DisCollision(*this);
}

//! Returns the PDU type
int DisCollision::GetClass() const
{
   return static_cast<int>(DisEnum::Pdu::Type::Collision);
}

//! Returns the PDU family
int DisCollision::GetFamily() const
{
   return static_cast<int>(DisEnum::Pdu::Family::EntityInformationInteraction);
}

//! Returns the entity ID that originated this PDU
const DisEntityId& DisCollision::GetOriginatingEntity() const
{
   return mIssuingEntityId;
}

//! Reads this PDU into class member variables
void DisCollision::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

//! Returns the length of this PDU in bytes
DisUint16 DisCollision::GetLength()
{
   DisUint16 octetLength = DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS;
   SetLength(octetLength);
   return octetLength;
}

//! Reads this PDU's non-header data into class member variables
void DisCollision::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   aGenI >> mIssuingEntityId;
   aGenI >> mCollidingEntityId;
   aGenI >> mEventId;
   aGenI >> mCollisionType;
   aGenI >> mPadding;
   aGenI >> mVelocity[0] >> mVelocity[1] >> mVelocity[2];
   aGenI >> mMass;
   aGenI >> mLocation[0] >> mLocation[1] >> mLocation[2];

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

//! Writes this PDU from class member variables
void DisCollision::Put(GenO& aGenO)
{
   GetLength(); // Ensure length is up-to-date
   DisPdu::Put(aGenO);
   aGenO << mIssuingEntityId;
   aGenO << mCollidingEntityId;
   aGenO << mEventId;
   aGenO << mCollisionType;
   aGenO << mPadding;
   aGenO << mVelocity[0] << mVelocity[1] << mVelocity[2];
   aGenO << mMass;
   aGenO << mLocation[0] << mLocation[1] << mLocation[2];
}

//! Returns the string representation of this PDU based based on class member variables
std::string DisCollision::ToString() const
{
   std::string returnString = DisPdu::ToString();
   returnString += "PDU Type                   = COLLISION\n";
   returnString += "---------Collision PDU Information---------\n";
   returnString += "IssuingEntity ID           = \n" + mIssuingEntityId.ToString() + '\n';
   returnString += "CollidingEntity ID         = \n" + mCollidingEntityId.ToString() + '\n';
   returnString += "Event ID                   = \n" + mEventId.ToString() + '\n';
   returnString += "Collision Type             = " + std::to_string(static_cast<unsigned int>(mCollisionType)) + '\n';
   returnString += "Velocity Vector (x,y,z)    = \n";
   returnString += "                         x = " + std::to_string(static_cast<float>(mVelocity[0])) + '\n';
   returnString += "                         y = " + std::to_string(static_cast<float>(mVelocity[1])) + '\n';
   returnString += "                         z = " + std::to_string(static_cast<float>(mVelocity[2])) + '\n';
   returnString += "Mass                       = " + std::to_string(static_cast<float>(mMass)) + '\n';
   returnString += "Location Vector (x,y,z)    = \n";
   returnString += "                         x = " + std::to_string(static_cast<float>(mLocation[0])) + '\n';
   returnString += "                         y = " + std::to_string(static_cast<float>(mLocation[1])) + '\n';
   returnString += "                         z = " + std::to_string(static_cast<float>(mLocation[2])) + '\n';
   returnString += "-------End Collision PDU Information-------\n";
   return returnString;
}

//! Returns true if this PDU contains valid data, else returns false
bool DisCollision::IsValid() const
{
   return DisPdu::IsValid() && mIssuingEntityId.IsValid() && mCollidingEntityId.IsValid() && mEventId.IsValid() &&
          ValidateScalarArray(mVelocity, 3) && ValidateScalar(mMass) && ValidateScalarArray(mLocation, 3) &&

          // Verify that collision type is a member of the enum (TODO-better way)
          !(mCollisionType != cINELASTIC && mCollisionType != cELASTIC && mCollisionType != cREFUELING_BOOM_NOZZLE_CLEAR);
}
