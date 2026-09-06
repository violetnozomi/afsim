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

#include "DisSpotted.hpp"

#include <cassert>
#include <iostream> // Only for debugging

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

// static
const DisInt8 DisSpotted::sType   = -71;  // 185 "Spotted"
const DisInt8 DisSpotted::sFamily = -106; // 150 "Experimental"

namespace
{
const DisUint16 sDisSpottedSize = 100;
}

DisSpotted::DisSpotted()
   : DisPdu()
   , mEntityId()
   , mTargetId()
   , mGuise()
   , mAlive(0)
   , mMobilityDead(0)
   , mFirepowerDead(0)
   , mCatastropicDead(0)
   , mAppearance(0)
   , mSensorTypes(0)
   , mCount(0)
   , mNewLevel(0)
   , mForce(0)
   , mLocationAccuracy(0)
   , mAgeInSecs(0)
   , mNumComments(0)
{
   SetPduType(DisEnum::Pdu::Type::Spotted);
   SetProtocolFamily(DisEnum::Pdu::Family::Experimental);
   SetLength(DisPdu::GetBaseLength() + sDisSpottedSize);
}

DisSpotted::DisSpotted(const DisSpotted& aSrc)
   : DisPdu(aSrc)
{
   Copy(aSrc);
}

DisSpotted::DisSpotted(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisSpotted::DisSpotted(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

// virtual
DisSpotted::~DisSpotted() {}

// virtual
DisPdu* DisSpotted::Clone() const
{
   return new DisSpotted(*this);
}

// virtual
int DisSpotted::GetClass() const
{
   return DisSpotted::sType;
}

// Copy the data portion from one DisSpotted to another.
// The new data is always owned by the class, even if
// the source was owned by the user.

// private
void DisSpotted::Copy(const DisSpotted& aSrc)
{
   mEntityId = aSrc.mEntityId;
   mTargetId = aSrc.mTargetId;
   memcpy(mSensorName, aSrc.mSensorName, 24);
   mLocation[0]      = aSrc.mLocation[0];
   mLocation[1]      = aSrc.mLocation[1];
   mLocation[2]      = aSrc.mLocation[2];
   mVelocity[0]      = aSrc.mVelocity[0];
   mVelocity[1]      = aSrc.mVelocity[1];
   mVelocity[2]      = aSrc.mVelocity[2];
   mGuise            = aSrc.mGuise;
   mAlive            = aSrc.mAlive;
   mMobilityDead     = aSrc.mMobilityDead;
   mFirepowerDead    = aSrc.mFirepowerDead;
   mCatastropicDead  = aSrc.mCatastropicDead;
   mAppearance       = aSrc.mAppearance;
   mSensorTypes      = aSrc.mSensorTypes;
   mCount            = aSrc.mCount;
   mNewLevel         = aSrc.mNewLevel;
   mForce            = aSrc.mForce;
   mLocationAccuracy = aSrc.mLocationAccuracy;
   mAgeInSecs        = aSrc.mAgeInSecs;
   mNumComments      = aSrc.mNumComments;
   mComments[0]      = aSrc.mComments[0];
   mComments[1]      = aSrc.mComments[1];
}

// virtual
void DisSpotted::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   DisSpotted::GetMemberData(aGenI);
}

// virtual
DisUint16 DisSpotted::GetLength()
{
   DisUint16 byteCount = DisPdu::GetBaseLength() + sDisSpottedSize;
   SetLength(byteCount);
   return byteCount;
}

// private
void DisSpotted::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   aGenI >> mEntityId;
   aGenI >> mTargetId;
   for (int i = 0; i < 24; ++i)
   {
      aGenI >> mSensorName[i];
   }
   aGenI >> mLocation[0];
   aGenI >> mLocation[1];
   aGenI >> mLocation[2];
   aGenI >> mVelocity[0];
   aGenI >> mVelocity[1];
   aGenI >> mVelocity[2];
   aGenI >> mGuise;
   aGenI >> mAlive;
   aGenI >> mMobilityDead;
   aGenI >> mFirepowerDead;
   aGenI >> mCatastropicDead;
   aGenI >> mAppearance;
   aGenI >> mSensorTypes;
   aGenI >> mCount;
   aGenI >> mNewLevel;
   aGenI >> mForce;
   aGenI >> mLocationAccuracy;
   aGenI >> mAgeInSecs;
   aGenI >> mNumComments;
   aGenI >> mComments[0];
   aGenI >> mComments[1];

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();

   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

// virtual
void DisSpotted::Put(GenO& aGenO)
{
   GetLength(); // Assure length is up to date.

   DisPdu::Put(aGenO);

   aGenO << mEntityId;
   aGenO << mTargetId;
   for (int i = 0; i < 24; ++i)
   {
      aGenO << mSensorName[i];
   }
   aGenO << mLocation[0];
   aGenO << mLocation[1];
   aGenO << mLocation[2];
   aGenO << mVelocity[0];
   aGenO << mVelocity[1];
   aGenO << mVelocity[2];
   aGenO << mGuise;
   aGenO << mAlive;
   aGenO << mMobilityDead;
   aGenO << mFirepowerDead;
   aGenO << mCatastropicDead;
   aGenO << mAppearance;
   aGenO << mSensorTypes;
   aGenO << mCount;
   aGenO << mNewLevel;
   aGenO << mForce;
   aGenO << mLocationAccuracy;
   aGenO << mAgeInSecs;
   aGenO << mNumComments;
   aGenO << mComments[0];
   aGenO << mComments[1];
}
