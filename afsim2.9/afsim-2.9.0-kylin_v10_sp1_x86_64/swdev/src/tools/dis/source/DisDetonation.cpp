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

#include "DisDetonation.hpp"

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisDetonation::DisDetonation()
   : DisPdu()
   , mFiringEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mTargetEntity(DisEntityId::TARGET_ID_UNKNOWN)
   , mWeaponEntity(DisEntityId::MUNITION_NOT_TRACKED)
   , mEvent()
   , mWeaponType()
   , mWarhead(0)
   , mFuse(0)
   , mQuantity(1)
   , mRate(0)
   , mDetonationResult(0)
   , mEntityPartCount(0)
{
   SetPduType(DisEnum::Pdu::Type::Detonation);
   SetProtocolFamily(DisEnum::Pdu::Family::Warfare);
   SetLength(104); // 832 bits

   mLocation[0]       = 0.0;
   mLocation[1]       = 0.0;
   mLocation[2]       = 0.0;
   mVelocity[0]       = 0.0F;
   mVelocity[1]       = 0.0F;
   mVelocity[2]       = 0.0F;
   mImpactLocation[0] = 0.0F;
   mImpactLocation[1] = 0.0F;
   mImpactLocation[2] = 0.0F;
}

DisDetonation::DisDetonation(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisDetonation::DisDetonation(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisDetonation::~DisDetonation()
{
   RemoveAllEntityParts();
}

// virtual
DisPdu* DisDetonation::Clone() const
{
   return new DisDetonation(*this);
}

// Virtual
int DisDetonation::GetClass() const
{
   return (int)DisEnum::Pdu::Type::Detonation;
}

// Virtual
const DisEntityId& DisDetonation::GetOriginatingEntity() const
{
   return mFiringEntity;
}

void DisDetonation::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   DisDetonation::GetMemberData(aGenI);
}

DisUint16 DisDetonation::GetLength()
{
   DisUint16 octetLength = 104 + (16 * mEntityPartCount);
   SetLength(octetLength);
   return octetLength;
}

void DisDetonation::GetMemberData(GenI& aGenI)
{
   DisUint16 pad16;
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   RemoveAllEntityParts();

   aGenI >> mFiringEntity;
   aGenI >> mTargetEntity;
   aGenI >> mWeaponEntity;
   aGenI >> mEvent;
   aGenI >> mVelocity[0] >> mVelocity[1] >> mVelocity[2];
   aGenI >> mLocation[0] >> mLocation[1] >> mLocation[2];
   aGenI >> mWeaponType;
   aGenI >> mWarhead;
   aGenI >> mFuse;
   aGenI >> mQuantity;
   aGenI >> mRate;
   aGenI >> mImpactLocation[0] >> mImpactLocation[1] >> mImpactLocation[2];
   aGenI >> mDetonationResult;
   aGenI >> mEntityPartCount;
   aGenI >> pad16;
   if (mEntityPartCount != 0)
   {
      mEntityPart.Resize(mEntityPartCount);
      for (int i = 0; i < mEntityPartCount; ++i)
      {
         mEntityPart[i] = new DisEntityPart();
         aGenI >> *mEntityPart[i];
      }
   }

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

void DisDetonation::Put(GenO& aGenO)
{
   DisUint16 pad16(0);

   GetLength(); // Ensure length is up-to-date
   DisPdu::Put(aGenO);
   aGenO << mFiringEntity;
   aGenO << mTargetEntity;
   aGenO << mWeaponEntity;
   aGenO << mEvent;
   aGenO << mVelocity[0] << mVelocity[1] << mVelocity[2];
   aGenO << mLocation[0] << mLocation[1] << mLocation[2];
   aGenO << mWeaponType;
   aGenO << mWarhead;
   aGenO << mFuse;
   aGenO << mQuantity;
   aGenO << mRate;
   aGenO << mImpactLocation[0] << mImpactLocation[1] << mImpactLocation[2];
   aGenO << mDetonationResult;
   aGenO << mEntityPartCount;
   aGenO << pad16;
   for (int i = 0; i < mEntityPartCount; ++i)
   {
      aGenO << *mEntityPart[i];
   }
}

void DisDetonation::Stream(std::ostream& aStream) const
{
   aStream << ToString();
}

std::string DisDetonation::ToString() const
{
   using namespace std;
   using namespace DisEnum;

   std::ostringstream ss("");

   DisFloat32 velocityX = 0.0f;
   DisFloat32 velocityY = 0.0f;
   DisFloat32 velocityZ = 0.0f;

   GetVelocity(velocityX, velocityY, velocityZ);

   DisFloat64 locationX = 0.0;
   DisFloat64 locationY = 0.0;
   DisFloat64 locationZ = 0.0;

   GetLocation(locationX, locationY, locationZ);

   DisFloat64 lat = 0.0;
   DisFloat64 lon = 0.0;
   DisFloat64 alt = 0.0;

   double positionECEF1[3] = {locationX, locationY, locationZ};
   UtEllipsoidalEarth::ConvertECEFToLLA(positionECEF1, lat, lon, alt);
   alt *= UtMath::cFT_PER_M;

   DisFloat32 impactLocationX = 0.0;
   DisFloat32 impactLocationY = 0.0;
   DisFloat32 impactLocationZ = 0.0;

   GetImpactLocation(impactLocationX, impactLocationY, impactLocationZ);

   ss << setiosflags(ios::fixed) << DisPdu::ToString() << '\n';
   locale tmp = ss.getloc();
   ss.imbue(locale(""));
   ss << "Firing Entity: " << GetFiringEntity().ToString() << '\n'
      << "Target Entity: " << GetTargetEntity().ToString() << '\n'
      << "Weapon Entity: " << GetWeaponEntity().ToString() << '\n'
      << "Event Id:      " << GetEvent().ToString() << '\n'
      << "Velocity:      " << velocityX << ", " << velocityY << ", " << velocityZ << " (m/s)\n"
      << "Location:      " << setprecision(0) << locationX << ", " << locationY << ", " << locationZ << " m ECEF\n"
      << "               " << setprecision(6) << lat << "N, " << lon << "E, " << setprecision(0) << alt << " ft MSL\n"
      << "Weapon Type:   " << GetWeaponType().ToString() << '\n'
      << "Warhead:       " << static_cast<unsigned int>(GetWarhead()) << '\n'
      << "Fuse:          " << static_cast<unsigned int>(GetFuse()) << '\n'
      << "Quantity:      " << static_cast<unsigned int>(GetQuantity()) << '\n'
      << "Rate:          " << static_cast<unsigned int>(GetRate()) << '\n'
      << "Result:        " << static_cast<unsigned int>(GetDetonationResult()) << '\n'
      << "Imp. Location: " << impactLocationX << ", " << impactLocationY << ", " << impactLocationZ
      << " m ECS (Target)\n";
   ss.imbue(tmp);
   return ss.str();
}

void DisDetonation::AddEntityPart(const DisEntityPart& aPart)
{
   mEntityPart.Add(new DisEntityPart(aPart));
   ++mEntityPartCount;
}

const DisEntityPart* DisDetonation::GetEntityPart(int aIndex) const
{
   return mEntityPart[aIndex];
}

void DisDetonation::RemoveAllEntityParts()
{
   mEntityPart.RemoveAll();
   mEntityPartCount = 0;
}

void DisDetonation::RemoveEntityPart(int aIndex)
{
   mEntityPart.Remove(aIndex);
   mEntityPartCount = static_cast<DisUint8>(mEntityPart.GetSize());
}
