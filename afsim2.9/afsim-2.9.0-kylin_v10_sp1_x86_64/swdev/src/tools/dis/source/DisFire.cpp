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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "DisFire.hpp"

#include <sstream>

#include "DisPduEnums.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

static const DisUint16 sDisFirePduSize = 96; // 768 bits

DisFire::DisFire()
   : DisPdu()
   , mFiringEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mTargetEntity(DisEntityId::TARGET_ID_UNKNOWN)
   , mWeaponEntity(DisEntityId::MUNITION_NOT_TRACKED)
   , mEvent()
   , mFireMissionIndex(0)
   , mWeaponType()
   , mWarhead(0)
   , mFuse(0)
   , mQuantity(1)
   , mRate(0)
   , mRange(0.0F)
{
   SetPduType(DisEnum::Pdu::Type::Fire);
   SetProtocolFamily(DisEnum::Pdu::Family::Warfare);
   SetLength(sDisFirePduSize);

   mLocation[0] = 0.0;
   mLocation[1] = 0.0;
   mLocation[2] = 0.0;
   mVelocity[0] = 0.0F;
   mVelocity[1] = 0.0F;
   mVelocity[2] = 0.0F;
}

DisFire::DisFire(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisFire::DisFire(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

// virtual
DisPdu* DisFire::Clone() const
{
   return new DisFire(*this);
}

// Virtual
int DisFire::GetClass() const
{
   return (int)DisEnum::Pdu::Type::Fire;
}

// Virtual
const DisEntityId& DisFire::GetOriginatingEntity() const
{
   return mFiringEntity;
}

void DisFire::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   DisFire::GetMemberData(aGenI);
}

void DisFire::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   aGenI >> mFiringEntity;
   aGenI >> mTargetEntity;
   aGenI >> mWeaponEntity;
   aGenI >> mEvent;
   aGenI >> mFireMissionIndex;
   aGenI >> mLocation[0] >> mLocation[1] >> mLocation[2];
   aGenI >> mWeaponType;
   aGenI >> mWarhead;
   aGenI >> mFuse;
   aGenI >> mQuantity;
   aGenI >> mRate;
   aGenI >> mVelocity[0] >> mVelocity[1] >> mVelocity[2];
   aGenI >> mRange;

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

void DisFire::Put(GenO& aGenO)
{
   DisPdu::Put(aGenO);

   aGenO << mFiringEntity;
   aGenO << mTargetEntity;
   aGenO << mWeaponEntity;
   aGenO << mEvent;
   aGenO << mFireMissionIndex;
   aGenO << mLocation[0] << mLocation[1] << mLocation[2];
   aGenO << mWeaponType;
   aGenO << mWarhead;
   aGenO << mFuse;
   aGenO << mQuantity;
   aGenO << mRate;
   aGenO << mVelocity[0] << mVelocity[1] << mVelocity[2];
   aGenO << mRange;
}

bool DisFire::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!DisPdu::IsValid())
   {
      isValid = false;
   }
   if (!mFiringEntity.IsValid())
   {
      isValid = false;
   }
   if (!mTargetEntity.IsValid())
   {
      isValid = false;
   }
   if (!mWeaponEntity.IsValid())
   {
      isValid = false;
   }
   if (!mEvent.IsValid())
   {
      isValid = false;
   }
   if (!mWeaponType.IsValid())
   {
      isValid = false;
   }
   if (!ValidateScalar(mLocation[0]))
   {
      isValid = false;
   }
   if (!ValidateScalar(mLocation[1]))
   {
      isValid = false;
   }
   if (!ValidateScalar(mLocation[2]))
   {
      isValid = false;
   }
   if (!ValidateScalar(mVelocity[0]))
   {
      isValid = false;
   }
   if (!ValidateScalar(mVelocity[1]))
   {
      isValid = false;
   }
   if (!ValidateScalar(mVelocity[2]))
   {
      isValid = false;
   }
   if (!ValidateScalar(mRange))
   {
      isValid = false;
   }
   return isValid;
}

void DisFire::Stream(std::ostream& aStream) const
{
   aStream << ToString();
}

std::string DisFire::ToString() const
{
   using namespace std;
   std::ostringstream ss("");

   DisFloat64 locationX = 0.0;
   DisFloat64 locationY = 0.0;
   DisFloat64 locationZ = 0.0;

   GetLocation(locationX, locationY, locationZ);

   DisFloat64 lat = 0.0;
   DisFloat64 lon = 0.0;
   DisFloat64 alt = 0.0;

   double positionECEF[3] = {locationX, locationY, locationZ};
   UtEllipsoidalEarth::ConvertECEFToLLA(positionECEF, lat, lon, alt);
   alt *= UtMath::cFT_PER_M;

   DisFloat32 velocityX = 0.0f;
   DisFloat32 velocityY = 0.0f;
   DisFloat32 velocityZ = 0.0f;

   GetVelocity(velocityX, velocityY, velocityZ);

   ss << setiosflags(ios::fixed) << DisPdu::ToString() << '\n';
   locale tmp = ss.getloc();
   ss.imbue(locale(""));
   ss << "Firing Entity:   " << GetFiringEntity().ToString() << '\n'
      << "Target Entity:   " << GetTargetEntity().ToString() << '\n'
      << "Weapon Entity:   " << GetWeaponEntity().ToString() << '\n'
      << "Event Id:        " << GetEvent().ToString() << '\n'
      << "Mission Index:   " << static_cast<unsigned int>(GetFireMissionIndex()) << '\n'
      << "Location:        " << setprecision(0) << locationX << ", " << locationY << ", " << locationZ << " m ECEF\n"
      << "                 " << setprecision(6) << lat << "N, " << lon << "E, " << setprecision(0) << alt << " ft MSL\n"
      << "Weapon Type:     " << GetWeaponType().ToString() << '\n'
      << "Warhead:         " << static_cast<unsigned int>(GetWarhead()) << '\n'
      << "Fuse:            " << static_cast<unsigned int>(GetFuse()) << '\n'
      << "Quantity:        " << static_cast<unsigned int>(GetQuantity()) << '\n'
      << "Rate:            " << static_cast<unsigned int>(GetRate()) << '\n'
      << "Velocity:        " << velocityX << ", " << velocityY << ", " << velocityZ << " (m/s)\n"
      << "Range:           " << GetRange() << " m (" << GetRange() / UtMath::cM_PER_NM << " nmile)\n";
   ss.imbue(tmp);
   return ss.str();
}
