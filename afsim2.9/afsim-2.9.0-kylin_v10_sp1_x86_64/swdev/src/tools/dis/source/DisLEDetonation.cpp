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

#include "DisLEDetonation.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisLEDetonation::DisLEDetonation()
   : DisPdu()
{
   SetPduType(DisEnum::Pdu::Type::LEDetonation);
   SetProtocolFamily(DisEnum::Pdu::Family::LiveEntity);
   SetLength(DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS);
}

DisLEDetonation::DisLEDetonation(const DisLEDetonation& aSrc)
   : DisPdu(aSrc)
   , mFiringEntityID(aSrc.mFiringEntityID)
   , mDetonationFlag1(aSrc.mDetonationFlag1)
   , mDetonationFlag2(aSrc.mDetonationFlag2)
   , mTargetEntityID(aSrc.mTargetEntityID)
   , mMunitionEntityID(aSrc.mMunitionEntityID)
   , mEventID(aSrc.mEventID)
   , mReferencePoint(aSrc.mReferencePoint)
   , mLocationWC(aSrc.mLocationWC)
   , mVelocity(aSrc.mVelocity)
   , mMunitionOrientation(aSrc.mMunitionOrientation)
   , mMunitionDescriptorRecord(aSrc.mMunitionDescriptorRecord)
   , mLocationEC(aSrc.mLocationEC)
   , mDetonationResult(aSrc.mDetonationResult)
{
   GetLength();
}

DisLEDetonation::DisLEDetonation(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisLEDetonation::DisLEDetonation(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisLEDetonation* DisLEDetonation::Clone() const
{
   return new DisLEDetonation(*this);
}

bool DisLEDetonation::IsValid() const
{
   return DisPdu::IsValid() && mFiringEntityID.IsValid() && mTargetEntityID.IsValid() && mMunitionEntityID.IsValid() &&
          mEventID.IsValid() && mMunitionDescriptorRecord.IsValid();
}

void DisLEDetonation::Get(GenI& aGenI)
{
   // Fill header fields from buffer first
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisLEDetonation::GetLength()
{
   DisUint16 octets = DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS;
   DisUint8  mask   = 1;

   if ((mask << 7) & mDetonationFlag1) // Detonation Flag 2
   {
      octets += 1;
   }

   if (mask & mDetonationFlag1) // Target Entity ID
   {
      octets += 4;
   }

   if ((mask << 2) & mDetonationFlag1) // Munition ID
   {
      if ((mask << 1) & mDetonationFlag1) // Site Number and Application Number (Munition ID)
      {
         octets += 2;
      }
      octets += 2;
   }
   if ((mask << 1) & mDetonationFlag2) // Event Number
   {
      if ((mask << 3) & mDetonationFlag1) // Site Number and Application Number (Event ID)
      {
         octets += 2;
      }
      octets += 2;
   }

   if (!((mask << 6) & mDetonationFlag1)) // Location in World Coordinates and Reference Point
   {
      octets += 8;
   }

   if (mask & mDetonationFlag2) // Munition Orientation
   {
      octets += 6;
   }

   if ((mask << 4) & mDetonationFlag1) // Warhead and Fuse
   {
      octets += 4;
   }
   if ((mask << 5) & mDetonationFlag1) // Quantity and Rate
   {
      octets += 4;
   }

   if ((mask << 6) & mDetonationFlag1) // Location in Entity Coordinates
   {
      octets += 6;
   }

   SetLength(octets);

   return octets;
}

void DisLEDetonation::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);

   DisUint8 mask = 1;

   // site and application are 8-bit for live entity id's
   aGenO << static_cast<DisUint8>(mFiringEntityID.GetSite());
   aGenO << static_cast<DisUint8>(mFiringEntityID.GetApplication());
   aGenO << mFiringEntityID.GetEntity();
   aGenO << mDetonationFlag1;
   if ((mask << 7) & mDetonationFlag1) // Detonation Flag 2
   {
      aGenO << mDetonationFlag2;
   }

   if (mask & mDetonationFlag1) // Target Entity ID
   {
      aGenO << static_cast<DisUint8>(mTargetEntityID.GetSite());
      aGenO << static_cast<DisUint8>(mTargetEntityID.GetApplication());
      aGenO << mTargetEntityID.GetEntity();
   }

   if ((mask << 2) & mDetonationFlag1) // Munition ID
   {
      if ((mask << 1) & mDetonationFlag1) // Site Number and Application Number (Munition ID)
      {
         aGenO << static_cast<DisUint8>(mMunitionEntityID.GetSite());
         aGenO << static_cast<DisUint8>(mMunitionEntityID.GetApplication());
      }
      aGenO << mMunitionEntityID.GetEntity();
   }
   if ((mask << 1) & mDetonationFlag2) // Event Number
   {
      if ((mask << 3) & mDetonationFlag1) // Site Number and Application Number (Event ID)
      {
         aGenO << static_cast<DisUint8>(mEventID.GetSite());
         aGenO << static_cast<DisUint8>(mEventID.GetApplication());
      }
      aGenO << mEventID.GetNumber();
   }

   if (!((mask << 6) & mDetonationFlag1)) // Location in World Coordinates and Reference Point
   {
      aGenO << mReferencePoint;
      aGenO << mLocationWC[0] << mLocationWC[1] << mLocationWC[2];
   }

   aGenO << mVelocity[0] << mVelocity[1] << mVelocity[2];

   if (mask & mDetonationFlag2) // Munition Orientation
   {
      aGenO << mMunitionOrientation[0] << mMunitionOrientation[1] << mMunitionOrientation[2];
   }

   // Munition Descriptor record - Munition
   aGenO << mMunitionDescriptorRecord.GetEntityType();

   if ((mask << 4) & mDetonationFlag1) // Warhead and Fuse
   {
      aGenO << mMunitionDescriptorRecord.GetWarhead();
      aGenO << mMunitionDescriptorRecord.GetFuse();
   }
   if ((mask << 5) & mDetonationFlag1) // Quantity and Rate
   {
      aGenO << mMunitionDescriptorRecord.GetQuantity();
      aGenO << mMunitionDescriptorRecord.GetRate();
   }

   if ((mask << 6) & mDetonationFlag1) // Location in Entity Coordinates
   {
      aGenO << mLocationEC[0] << mLocationEC[1] << mLocationEC[2];
   }

   aGenO << mDetonationResult;
}

void DisLEDetonation::GetMemberData(GenI& aGenI)
{
   DisUint8  mask        = 1;
   DisUint8  site        = 0;
   DisUint8  application = 0;
   DisUint16 entity;

   aGenI >> site >> application >> entity;
   mFiringEntityID.Set(site, application, entity);

   aGenI >> mDetonationFlag1;
   if ((mask << 7) & mDetonationFlag1) // Detonation Flag 2
   {
      aGenI >> mDetonationFlag2;
   }

   if (mask & mDetonationFlag1) // Target Entity ID
   {
      aGenI >> site >> application >> entity;
      mTargetEntityID.Set(site, application, entity);
   }

   if ((mask << 2) & mDetonationFlag1) // Munition ID
   {
      if ((mask << 1) & mDetonationFlag1) // Site Number and Application Number (Munition ID)
      {
         aGenI >> site >> application;
         mMunitionEntityID.SetSite(site);
         mMunitionEntityID.SetApplication(application);
      }
      aGenI >> entity;
      mMunitionEntityID.SetEntity(entity);
   }
   if ((mask << 1) & mDetonationFlag2) // Event Number
   {
      if ((mask << 3) & mDetonationFlag1) // Site Number and Application Number (Event ID)
      {
         DisUint8 eventSite = 0, eventApp = 0;
         aGenI >> eventSite >> eventApp;
         mEventID.SetSite(eventSite);
         mEventID.SetApplication(eventApp);
      }
      aGenI >> entity;
      mEventID.SetNumber(entity);
   }

   if (!((mask << 6) & mDetonationFlag1)) // Location in World Coordinates and Reference Point
   {
      aGenI >> mReferencePoint;
      aGenI >> mLocationWC[0] >> mLocationWC[1] >> mLocationWC[2];
   }

   aGenI >> mVelocity[0] >> mVelocity[1] >> mVelocity[2];

   if (mask & mDetonationFlag2) // Munition Orientation
   {
      aGenI >> mMunitionOrientation[0] >> mMunitionOrientation[1] >> mMunitionOrientation[2];
   }

   // Munition Descriptor record - Munition
   DisEntityType munition;
   aGenI >> munition;
   mMunitionDescriptorRecord.SetEntityType(munition);

   if ((mask << 4) & mDetonationFlag1) // Warhead and Fuse
   {
      DisUint16 warhead, fuse;
      aGenI >> warhead >> fuse;
      mMunitionDescriptorRecord.SetWarhead(warhead);
      mMunitionDescriptorRecord.SetFuse(fuse);
   }
   if ((mask << 5) & mDetonationFlag1) // Quantity and Rate
   {
      DisUint16 quantity, rate;
      aGenI >> quantity >> rate;
      mMunitionDescriptorRecord.SetQuantity(quantity);
      mMunitionDescriptorRecord.SetRate(rate);
   }

   if ((mask << 6) & mDetonationFlag1) // Location in Entity Coordinates
   {
      aGenI >> mLocationEC[0] >> mLocationEC[1] >> mLocationEC[2];
   }

   aGenI >> mDetonationResult;
}

std::string DisLEDetonation::ToString() const
{
   DisUint8    mask         = 1;
   std::string returnString = DisPdu::ToString();
   returnString += "---------DisLEDetonation Information---------\n";
   returnString += ": " + mFiringEntityID.ToString() + '\n';

   returnString += ": " + std::to_string(mDetonationFlag1) + '\n';
   if ((mask << 7) & mDetonationFlag1) // Detonation Flag 2
   {
      returnString += ": " + std::to_string(mDetonationFlag2) + '\n';
   }

   if (mask & mDetonationFlag1) // Target Entity ID
   {
      returnString += ": " + mTargetEntityID.ToString() + '\n';
   }

   if ((mask << 2) & mDetonationFlag1) // Munition ID
   {
      returnString += ": " + mMunitionEntityID.ToString() + '\n';
   }
   if ((mask << 1) & mDetonationFlag2) // Event Number
   {
      returnString += ": " + mEventID.ToString() + '\n';
   }

   if (!((mask << 6) & mDetonationFlag1)) // Location in World Coordinates and Reference Point
   {
      returnString += ": " + std::to_string(mReferencePoint) + '\n';
      returnString += ": " + std::to_string(mLocationWC[0]) + ", " + std::to_string(mLocationWC[1]) + ", " +
                      std::to_string(mLocationWC[2]) + '\n';
   }

   returnString += ": " + std::to_string(mVelocity[0]) + ", " + std::to_string(mVelocity[1]) + ", " +
                   std::to_string(mVelocity[2]) + '\n';

   if (mask & mDetonationFlag2) // Munition Orientation
   {
      returnString += ": " + std::to_string(mMunitionOrientation[0]) + ", " + std::to_string(mMunitionOrientation[1]) +
                      ", " + std::to_string(mMunitionOrientation[2]) + '\n';
   }

   returnString += ": " + mMunitionDescriptorRecord.ToString() + '\n';

   if ((mask << 6) & mDetonationFlag1) // Location in Entity Coordinates
   {
      returnString += ": " + std::to_string(mLocationEC[0]) + ", " + std::to_string(mLocationEC[1]) + ", " +
                      std::to_string(mLocationEC[2]) + '\n';
   }

   returnString += ": " + std::to_string(mDetonationResult) + '\n';
   returnString += "-------End DisLEDetonation Information-------\n";
   return returnString;
}

void DisLEDetonation::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}
