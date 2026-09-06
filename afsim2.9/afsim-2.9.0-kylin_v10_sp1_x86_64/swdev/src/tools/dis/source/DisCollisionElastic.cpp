//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#include "DisCollisionElastic.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisCollisionElastic::DisCollisionElastic()
   : DisPdu()
   , mIssuingEntityId()
   , mCollidingEntityId()
   , mEventId()
   , mContactVelocity()
   , mImpactLocation()
   , mCollisionIntermediateResults()
   , mSurfaceNormal()
{
   SetPduType(DisEnum::Pdu::Type::CollisionElastic);
   SetProtocolFamily(DisEnum::Pdu::Family::EntityInformationInteraction);
   SetLength(DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS);
}

DisCollisionElastic::DisCollisionElastic(const DisCollisionElastic& aSrc)
   : DisPdu(aSrc)
   , mIssuingEntityId(aSrc.mIssuingEntityId)
   , mCollidingEntityId(aSrc.mCollidingEntityId)
   , mEventId(aSrc.mEventId)
   , mContactVelocity(aSrc.mContactVelocity)
   , mMass(aSrc.mMass)
   , mImpactLocation(aSrc.mImpactLocation)
   , mCollisionIntermediateResults(aSrc.mCollisionIntermediateResults)
   , mSurfaceNormal(aSrc.mSurfaceNormal)
   , mRestitutionCoefficient(aSrc.mRestitutionCoefficient)
{
   GetLength();
}

DisCollisionElastic::DisCollisionElastic(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisCollisionElastic::DisCollisionElastic(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

std::string DisCollisionElastic::ToString() const
{
   std::string returnString = DisPdu::ToString();
   returnString += "PDU Type = COLLISION-ELASTIC\n";
   returnString += "---------Collision-Elastic PDU Information---------\n";
   returnString += "IssuingEntity ID                   = \n" + mIssuingEntityId.ToString() + '\n';
   returnString += "CollidingEntity ID                 = \n" + mCollidingEntityId.ToString() + '\n';
   returnString += "EventID                            = \n" + mEventId.ToString() + '\n';
   returnString += "Contact Velocity Vector (x,y,z)    = \n";
   returnString += "                                 x = " + std::to_string(static_cast<float>(mContactVelocity[0])) + '\n';
   returnString += "                                 y = " + std::to_string(static_cast<float>(mContactVelocity[1])) + '\n';
   returnString += "                                 z = " + std::to_string(static_cast<float>(mContactVelocity[2])) + '\n';
   returnString += "Mass                               = " + std::to_string(static_cast<float>(mMass)) + '\n';
   returnString += "Impact Location Vector (x,y,z)     = \n";
   returnString += "                                 x = " + std::to_string(static_cast<float>(mImpactLocation[0])) + '\n';
   returnString += "                                 y = " + std::to_string(static_cast<float>(mImpactLocation[1])) + '\n';
   returnString += "                                 z = " + std::to_string(static_cast<float>(mImpactLocation[2])) + '\n';

   returnString +=
      "Collision Intermediate Result-XX   = " + std::to_string(static_cast<float>(mCollisionIntermediateResults[0])) + '\n';
   returnString +=
      "Collision Intermediate Result-XY   = " + std::to_string(static_cast<float>(mCollisionIntermediateResults[1])) + '\n';
   returnString +=
      "Collision Intermediate Result-XZ   = " + std::to_string(static_cast<float>(mCollisionIntermediateResults[2])) + '\n';
   returnString +=
      "Collision Intermediate Result-YY   = " + std::to_string(static_cast<float>(mCollisionIntermediateResults[3])) + '\n';
   returnString +=
      "Collision Intermediate Result-YZ   = " + std::to_string(static_cast<float>(mCollisionIntermediateResults[4])) + '\n';
   returnString +=
      "Collision Intermediate Result-ZZ   = " + std::to_string(static_cast<float>(mCollisionIntermediateResults[5])) + '\n';

   returnString += "Surface Normal Vector (x,y,z)      = \n";
   returnString += "                                 x = " + std::to_string(static_cast<float>(mSurfaceNormal[0])) + '\n';
   returnString += "                                 y = " + std::to_string(static_cast<float>(mSurfaceNormal[1])) + '\n';
   returnString += "                                 z = " + std::to_string(static_cast<float>(mSurfaceNormal[2])) + '\n';
   returnString +=
      "Coefficient of Restitution         = " + std::to_string(static_cast<float>(mRestitutionCoefficient)) + '\n';
   returnString += "-------End Collision-ELASTIC PDU Information-------\n";
   return returnString;
}

void DisCollisionElastic::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisCollisionElastic::GetLength()
{
   DisUint16 length = DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS;
   SetLength(length);
   return length;
}

void DisCollisionElastic::Put(GenO& aGenO)
{
   // Ensure length is up-to-date
   GetLength();
   DisPdu::Put(aGenO);

   aGenO << mIssuingEntityId;
   aGenO << mCollidingEntityId;
   aGenO << mEventId;
   aGenO << mPadding;
   aGenO << mContactVelocity[0] << mContactVelocity[1] << mContactVelocity[2];
   aGenO << mMass;
   aGenO << mImpactLocation[0] << mImpactLocation[1] << mImpactLocation[2];
   for (auto it = mCollisionIntermediateResults.begin(); it != mCollisionIntermediateResults.end(); it++)
   {
      aGenO << *it;
   }
   aGenO << mSurfaceNormal[0] << mSurfaceNormal[1] << mSurfaceNormal[2];
   aGenO << mRestitutionCoefficient;
}

bool DisCollisionElastic::IsValid() const
{
   return DisPdu::IsValid() && mIssuingEntityId.IsValid() && mCollidingEntityId.IsValid() && mEventId.IsValid() &&
          ValidateScalar(mContactVelocity[0]) && ValidateScalar(mContactVelocity[1]) &&
          ValidateScalar(mContactVelocity[2]) && ValidateScalar(mMass) && ValidateScalar(mImpactLocation[0]) &&
          ValidateScalar(mImpactLocation[1]) && ValidateScalar(mImpactLocation[2]) &&
          ValidateScalarArray(mCollisionIntermediateResults, 6) && ValidateScalar(mSurfaceNormal[0]) &&
          ValidateScalar(mSurfaceNormal[1]) && ValidateScalar(mSurfaceNormal[2]) &&
          ValidateScalar(mRestitutionCoefficient);
}

void DisCollisionElastic::GetMemberData(GenI& aGenI)
{
   aGenI >> mIssuingEntityId;
   aGenI >> mCollidingEntityId;
   aGenI >> mEventId;
   aGenI >> mPadding;
   aGenI >> mContactVelocity[0] >> mContactVelocity[1] >> mContactVelocity[2];
   aGenI >> mMass;
   aGenI >> mImpactLocation[0] >> mImpactLocation[1] >> mImpactLocation[2];
   for (int i = 0; i < static_cast<int>(mCollisionIntermediateResults.size()); i++)
   {
      DisFloat32 result;
      aGenI >> result;
      mCollisionIntermediateResults[i] = result;
   }
   aGenI >> mSurfaceNormal[0] >> mSurfaceNormal[1] >> mSurfaceNormal[2];
   aGenI >> mRestitutionCoefficient;
   GetLength();
}
