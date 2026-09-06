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

#include "DisLEFire.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisLEFire::DisLEFire()
   : DisPdu()
{
   SetPduType(DisEnum::Pdu::Type::LEFire);
   SetProtocolFamily(DisEnum::Pdu::Family::LiveEntity);
   SetLength(DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS);
}

DisLEFire::DisLEFire(const DisLEFire& aSrc)
   : DisPdu(aSrc)
   , mFiringEntityID(aSrc.mFiringEntityID)
   , mFlags(aSrc.mFlags)
   , mTargetEntityID(aSrc.mTargetEntityID)
   , mMunitionEntityID(aSrc.mMunitionEntityID)
   , mEventID(aSrc.mEventID)
   , mReferencePoint(aSrc.mReferencePoint)
   , mEntityLocation(aSrc.mEntityLocation)
   , mMunitionDescriptorRecord(aSrc.mMunitionDescriptorRecord)
   , mVelocity(aSrc.mVelocity)
   , mRange(aSrc.mRange)
{
   GetLength();
}

DisLEFire::DisLEFire(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisLEFire::DisLEFire(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisLEFire::~DisLEFire() {}

DisLEFire* DisLEFire::Clone() const
{
   return new DisLEFire(*this);
}

bool DisLEFire::IsValid() const
{
   return DisPdu::IsValid() && mFiringEntityID.IsValid() && mTargetEntityID.IsValid() && mMunitionEntityID.IsValid() &&
          mEventID.IsValid() && mMunitionDescriptorRecord.IsValid();
}

void DisLEFire::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisLEFire::GetLength()
{
   DisUint16 octets = DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS;
   DisUint8  mask   = 1;

   if (mask & mFlags) // Target Entity ID
   {
      octets += 4;
   }

   if ((mask << 2) & mFlags) // Munition ID
   {
      if ((mask << 1) & mFlags) // Site Number and Application Number (Munition ID)
      {
         octets += 2;
      }
      octets += 2;
   }
   if ((mask << 3) & mFlags) // Site Number and Application Number (Event ID)
   {
      octets += 2;
   }
   if ((mask << 4) & mFlags) // Warhead and Fuse
   {
      octets += 4;
   }
   if ((mask << 5) & mFlags) // Quantity and Rate
   {
      octets += 4;
   }
   if ((mask << 6) & mFlags) // Reference point and Location
   {
      octets += 8;
   }

   SetLength(octets);

   return octets;
}

void DisLEFire::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);

   DisUint8 mask = 1;

   aGenO << static_cast<DisUint8>(mFiringEntityID.GetSite());
   aGenO << static_cast<DisUint8>(mFiringEntityID.GetApplication());
   aGenO << mFiringEntityID.GetEntity();

   aGenO << mFlags;

   if (mask & mFlags) // Target Entity ID
   {
      aGenO << static_cast<DisUint8>(mTargetEntityID.GetSite());
      aGenO << static_cast<DisUint8>(mTargetEntityID.GetApplication());
      aGenO << mTargetEntityID.GetEntity();
   }

   if ((mask << 2) & mFlags) // Munition ID
   {
      if ((mask << 1) & mFlags) // Site Number and Application Number (Munition ID)
      {
         aGenO << static_cast<DisUint8>(mMunitionEntityID.GetSite());
         aGenO << static_cast<DisUint8>(mMunitionEntityID.GetApplication());
      }
      aGenO << mMunitionEntityID.GetEntity();
   }
   if ((mask << 3) & mFlags) // Site Number and Application Number (Event ID)
   {
      aGenO << static_cast<DisUint8>(mEventID.GetSite());
      aGenO << static_cast<DisUint8>(mEventID.GetApplication());
   }
   aGenO << mEventID.GetNumber();

   if ((mask << 6) & mFlags) // Reference point and Location
   {
      aGenO << mReferencePoint;
      aGenO << mEntityLocation[0] << mEntityLocation[1] << mEntityLocation[2];
   }

   aGenO << mMunitionDescriptorRecord.GetEntityType();

   if ((mask << 4) & mFlags) // Warhead and Fuse
   {
      aGenO << mMunitionDescriptorRecord.GetWarhead();
      aGenO << mMunitionDescriptorRecord.GetFuse();
   }
   if ((mask << 5) & mFlags) // Quantity and Rate
   {
      aGenO << mMunitionDescriptorRecord.GetQuantity();
      aGenO << mMunitionDescriptorRecord.GetRate();
   }
   aGenO << mVelocity[0] << mVelocity[1] << mVelocity[2];
   aGenO << mRange;
}

void DisLEFire::GetMemberData(GenI& aGenI)
{
   DisUint8  mask        = 1;
   DisUint8  site        = 0;
   DisUint8  application = 0;
   DisUint16 entity;
   aGenI >> site >> application >> entity;

   mFiringEntityID.Set(site, application, entity);
   aGenI >> mFlags;

   if (mask & mFlags) // Target Entity ID
   {
      aGenI >> site >> application >> entity;
      mTargetEntityID.Set(site, application, entity);
   }

   if ((mask << 2) & mFlags) // Munition ID
   {
      if ((mask << 1) & mFlags) // Site Number and Application Number (Munition ID)
      {
         aGenI >> site >> application;
         mMunitionEntityID.SetSite(site);
         mMunitionEntityID.SetApplication(application);
      }
      aGenI >> entity;
      mMunitionEntityID.SetEntity(entity);
   }
   if ((mask << 3) & mFlags) // Site Number and Application Number (Event ID)
   {
      DisUint8 eventSite = 0, eventApp = 0;
      aGenI >> eventSite >> eventApp;
      mEventID.SetSite(eventSite);
      mEventID.SetApplication(eventApp);
   }
   aGenI >> entity;
   mEventID.SetNumber(entity);

   if ((mask << 6) & mFlags) // Reference point and Location
   {
      aGenI >> mReferencePoint;
      aGenI >> mEntityLocation[0] >> mEntityLocation[1] >> mEntityLocation[2];
   }

   DisEntityType munition;
   aGenI >> munition;
   mMunitionDescriptorRecord.SetEntityType(munition);

   if ((mask << 4) & mFlags) // Warhead and Fuse
   {
      DisUint16 warhead, fuse;
      aGenI >> warhead >> fuse;
      mMunitionDescriptorRecord.SetWarhead(warhead);
      mMunitionDescriptorRecord.SetFuse(fuse);
   }
   if ((mask << 5) & mFlags) // Quantity and Rate
   {
      DisUint16 quantity, rate;
      aGenI >> quantity >> rate;
      mMunitionDescriptorRecord.SetQuantity(quantity);
      mMunitionDescriptorRecord.SetRate(rate);
   }
   aGenI >> mVelocity[0] >> mVelocity[1] >> mVelocity[2];
   aGenI >> mRange;
}

std::string DisLEFire::ToString() const
{
   DisUint8    mask         = 1;
   std::string returnString = DisPdu::ToString();
   returnString += "---------DisLEFire Information---------\n";
   returnString += "miring Entity ID: " + mFiringEntityID.ToString() + "\n";
   returnString += "Flags: " + std::to_string(mFlags);
   returnString += "Target Entity ID: " + mTargetEntityID.ToString() + "\n";
   returnString += "Munition Entity ID: " + mMunitionEntityID.ToString() + "\n";
   returnString += "Event ID: " + mEventID.ToString() + "\n";
   if ((mask << 6) & mFlags) // Reference point and Location
   {
      returnString += "Reference Point: " + std::to_string(mReferencePoint) + "\n";
      returnString += "Entity Location: " + std::to_string(mEntityLocation[0]) + ", " +
                      std::to_string(mEntityLocation[1]) + ", " + std::to_string(mEntityLocation[2]) + "\n";
   }

   returnString += "Munition Descriptor Record: " + mMunitionDescriptorRecord.ToString() + "\n";
   returnString += "Velocity: " + std::to_string(mVelocity[0]) + ", " + std::to_string(mVelocity[1]) + ", " +
                   std::to_string(mVelocity[2]) + "\n";
   returnString += "Range: " + std::to_string(mRange) + "\n";
   returnString += "-------End DisLEFire Information-------\n";
   return returnString;
}

void DisLEFire::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}
